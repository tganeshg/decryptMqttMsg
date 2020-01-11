#--------------------------------------------------------------------------------------#
#	Project : Esp32 Sample
#
#	Author : Ganesh
#--------------------------------------------------------------------------------------#

CC = gcc
CFLAGS = -c -Wall -I../libsodium-master/src/libsodium/include -I../mosquitto-1.6.8/lib
#LDLAGS = -L../mosquitto-1.6.8/lib -L../libsodium-master/src/libsodium/.libs -lmosquitto -lsodium
LDLAGS = -lmosquitto -lsodium
TARGET = mqttClient

all: $(TARGET)

mqttClient: main.o
	$(CC) main.o $(LDLAGS) -o mqttClient

main.o: main.c
	$(CC) $(CFLAGS) main.c
	
clean:
	rm -rf *.o $(TARGET)
