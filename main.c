/* Include Headers */
#include "general.h"

/* Extern */

/* Globals */
unsigned char decrypted[4096];

struct mosquitto *mosq = NULL;

/* Functions */
#if 0
bool readConfig(void)
{
	char line[SIZE_1024];
	char tmpBuff[SIZE_512];
	int i = 0;
	char *cfline=NULL;
	
	memset(&serialConfig,0,sizeof(SERIAL_CONFIG));
	FILE *file = fopen (CONFIG_PATH, "r");
	if (file != NULL)
	{
		while(fgets(line, sizeof(line), file) != NULL)
        {
			if( (cfline = strstr((const char *)line,(const char *)"SERIALFILE")) != NULL )
			{
				cfline +=strlen("SERIALFILE") + 2;
				i=0; memset(tmpBuff,0,sizeof(tmpBuff));
				while( (*cfline != '"') && (*cfline != '\r') && (*cfline != '\0') && (*cfline != '\n') )
				{
					tmpBuff[i++] = *cfline;	
					cfline++;
				}
				strcpy(serialConfig.serialFile,tmpBuff);
			}
			else if( (cfline = strstr((const char *)line,(const char *)"SERBAUDRATE")) != NULL )
			{
				cfline +=strlen("SERBAUDRATE") + 2;
				i=0; memset(tmpBuff,0,sizeof(tmpBuff));
				while( (*cfline != '"') && (*cfline != '\r') && (*cfline != '\0') && (*cfline != '\n') )
				{
					tmpBuff[i++] = *cfline;	
					cfline++;
				}
				serialConfig.baudRate = (unsigned int)atoi(tmpBuff);
			}
			else if( (cfline = strstr((const char *)line,(const char *)"SERDATABITS")) != NULL )
			{
				cfline +=strlen("SERDATABITS") + 2;
				i=0; memset(tmpBuff,0,sizeof(tmpBuff));
				while( (*cfline != '"') && (*cfline != '\r') && (*cfline != '\0') && (*cfline != '\n') )
				{
					tmpBuff[i++] = *cfline;	
					cfline++;
				}
				serialConfig.databits = (unsigned char)atoi(tmpBuff);
			}
			else if( (cfline = strstr((const char *)line,(const char *)"SERSTOPBITS")) != NULL )
			{
				cfline +=strlen("SERSTOPBITS") + 2;
				i=0; memset(tmpBuff,0,sizeof(tmpBuff));
				while( (*cfline != '"') && (*cfline != '\r') && (*cfline != '\0') && (*cfline != '\n') )
				{
					tmpBuff[i++] = *cfline;	
					cfline++;
				}
				serialConfig.stopbits = (unsigned char)atoi(tmpBuff);
			}
			else if( (cfline = strstr((const char *)line,(const char *)"SERPARITY")) != NULL )
			{
				cfline +=strlen("SERPARITY") + 2;
				i=0; memset(tmpBuff,0,sizeof(tmpBuff));
				while( (*cfline != '"') && (*cfline != '\r') && (*cfline != '\0') && (*cfline != '\n') )
				{
					tmpBuff[i++] = *cfline;	
					cfline++;
				}
				serialConfig.parity = (char)tmpBuff[0];
			}
        } // End while
        fclose(file);
    } 
	else
		return RET_FAILURE;

	return RET_OK;
}
#endif

unsigned char nonce[crypto_secretbox_NONCEBYTES]="nonce";
unsigned char key[crypto_secretbox_KEYBYTES]="key";
	
void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{

	/* Generate a secure random key and nonce */
	/* randombytes_buf(nonce, sizeof(nonce));
	randombytes_buf(key, sizeof(key)); */

	if(message->payloadlen)
	{
		//printf("Topic:%s \nMessage:%s\nMsg Length:%d\n", message->topic, (char *)message->payload,message->payloadlen);
		printf("Topic:%s \nMsg Length:%d\n", message->topic,message->payloadlen);
		
		if (crypto_secretbox_open_easy(decrypted,(unsigned char *)message->payload, (4096 + crypto_secretbox_MACBYTES), nonce, key) != 0) 
		{
			/* If we get here, the Message was a forgery. This means someone (or the network) somehow tried to tamper with the message*/
			printf(">>>>>>> Error\n");
		}
		else
		{
			printf(">>>>>>> %s\n",decrypted);
		}
	}
	else
	{
		printf("Empty Msg:%s \n", message->topic);
	}
	fflush(stdout);
}

void my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
	if(!result)
	{
		/* Subscribe to broker information topics on successful connect. */
		mosquitto_subscribe(mosq, NULL, "mosquittoTest_esp32/#", 0);
	}
	else
	{
		fprintf(stderr, "Connect failed\n");
	}
}

void my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
	int i=0;
	printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
	for(i=1; i<qos_count; i++){
		printf(", %d", granted_qos[i]);
	}
	printf("\n");
}

void my_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
	/* Pring all log messages regardless of level. */
	
	printf("Log : %s\n", str);
}

void decrypt(void)
{
	#define MESSAGE (const unsigned char *) "test"
	#define MESSAGE_LEN 4
	#define CIPHERTEXT_LEN (crypto_box_MACBYTES + MESSAGE_LEN)

	unsigned char alice_publickey[crypto_box_PUBLICKEYBYTES];
	unsigned char alice_secretkey[crypto_box_SECRETKEYBYTES];
	crypto_box_keypair(alice_publickey, alice_secretkey);

	unsigned char bob_publickey[crypto_box_PUBLICKEYBYTES];
	unsigned char bob_secretkey[crypto_box_SECRETKEYBYTES];
	crypto_box_keypair(bob_publickey, bob_secretkey);

	unsigned char nonce[crypto_box_NONCEBYTES];
	unsigned char ciphertext[CIPHERTEXT_LEN];
	randombytes_buf(nonce, sizeof(nonce));
	if (crypto_box_easy(ciphertext, MESSAGE, MESSAGE_LEN, nonce,
						bob_publickey, alice_secretkey) != 0) {
		/* error */
	}

	unsigned char decrypted[MESSAGE_LEN];
	if (crypto_box_open_easy(decrypted, ciphertext, CIPHERTEXT_LEN, nonce,
							 alice_publickey, bob_secretkey) != 0) {
		/* message for Bob pretending to be from Alice has been forged! */
	}
}

/* Main */
int main(void)
{
	char id[30]="sub_0001";
	char *host = "test.mosquitto.org";
	int port = 1883;
	int keepalive = 600;
	bool clean_session = true;

	printf("\n<<< MQTT Client - LibSodium Encryption Example : %s >>>\n",APP_VERSION);

	/* TODO : Signal handling */

	/* Read Config */
	//readConfig();

	mosquitto_lib_init();
	mosq = mosquitto_new(id, clean_session, NULL);
	if(!mosq){
		fprintf(stderr, "Error: Out of memory.\n");
		return 1;
	}
	mosquitto_log_callback_set(mosq, my_log_callback);
	mosquitto_connect_callback_set(mosq, my_connect_callback);
	mosquitto_message_callback_set(mosq, my_message_callback);
	mosquitto_subscribe_callback_set(mosq, my_subscribe_callback);
	
	int ret = mosquitto_connect(mosq, host, port, keepalive);
	if(ret != MOSQ_ERR_SUCCESS)
	{
		fprintf(stderr, "Unable to connect.%s\n",mosquitto_strerror(ret));
		return 1;
	}

	/* Main Loop */
	while(!(mosquitto_loop(mosq, -1,1000)));
	
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();

	return TRUE;
}

/* EOF */
