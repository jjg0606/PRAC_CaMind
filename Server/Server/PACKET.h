#pragma once

#define MAX_ARR_SIZE 20

enum PACKET_TYPE
{
	PACKET_TYPE_SYSTEM,
	PACKET_TYPE_POINT,
};

enum SYSTEM_MSG
{

};

#pragma pack(1)
typedef struct 
{
	int type;
	int size;
}
PACKET_HEADER;

typedef struct
{
	PACKET_HEADER header;
	int msg;
}
PACKET_SYSTEM;


typedef struct 
{
	PACKET_HEADER header;
	int num;
	int xarr[MAX_ARR_SIZE];
	int yarr[MAX_ARR_SIZE];
}
PACKET_POINT;

#pragma pack()