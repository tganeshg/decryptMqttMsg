#ifndef _GENERAL_H_
#define _GENERAL_H_

/*include Headers serial server and client file*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <unistd.h>  /* UNIX Standard Definitions 	   		*/ 
#include "mosquitto.h"
#include "sodium.h"

/*Macros*/
#define	APP_VERSION		" V 1.0.0 29_12_2019"
#define	CONFIG_PATH		"./Config.conf"

#define	DEBUG_LOG		0
#define	FALSE			0
#define	TRUE			1

#define	RET_OK			0
#define	RET_FAILURE		-1

//Enums
// Size constants
typedef enum
{
	SIZE_2=2,
	SIZE_4=4,
	SIZE_6=6,
	SIZE_8=8,
	SIZE_16=16,
	SIZE_32=32,
	SIZE_64=64,
	SIZE_128=128,
	SIZE_256=256,
	SIZE_512=512,
	SIZE_640=640,
	SIZE_1024=1024,
	SIZE_2048=2048,
	SIZE_6144=6144
}BUFFER_SIZES;

/*Structures*/
#pragma pack(push)
#pragma pack(1)

#pragma pack(pop)

#endif
/* EOF */
