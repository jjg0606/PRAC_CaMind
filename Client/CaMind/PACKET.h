#pragma once

#define MAX_LOBBY 10
#define MAX_PLAYERS_IN_ROOM 8
#define MAX_CHAT_LENGTH 50
#define MAX_NAME_LENGTH 12

enum PACKET_TYPE
{
	PACKET_TYPE_SYSTEM,
	PACKET_TYPE_LOGIN_INFO,
	PACKET_TYPE_LOBBY_INFO,
	PACKET_TYPE_LOBBY_IN,
	PACKET_TYPE_LOBBY_CHAT,
	PACKET_TYPE_LOBBY_PLAYERS,
	PACKET_TYPE_GAME_READY,
	PACKET_TYPE_GAME_TURN_MSG,
	PACKET_TYPE_GAME_CHAT_MSG,
};

enum SYSTEM_MSG
{
	SYSTEM_MSG_CLIENT_READY,
	SYSTEM_MSG_SERVER_READY,
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
	int system_msg;
}
PACKET_SYSTEM;

typedef struct
{
	PACKET_HEADER header;
	int avatar;
	wchar_t name[MAX_NAME_LENGTH];
}
PACKET_LOGIN_INFO;

typedef struct
{
	PACKET_HEADER header;
	int roomNum;
	bool isPlaying[MAX_LOBBY];
	int playerNum[MAX_LOBBY];
}
PACKET_LOBBY_INFO;

typedef struct
{
	PACKET_HEADER header;
	int index;
}
PACKET_LOBBY_IN;

typedef struct
{
	PACKET_HEADER header;
	int playerNum;
	int myNum;
	int avatar[MAX_PLAYERS_IN_ROOM];
	bool isReady[MAX_PLAYERS_IN_ROOM];
	wchar_t nameArr[MAX_PLAYERS_IN_ROOM][MAX_NAME_LENGTH];
}
PACKET_LOBBY_PLAYERS;

typedef struct
{
	PACKET_HEADER header;
	int turnp;
	int command;
}
PACKET_GAME_TURN_MSG;

typedef struct
{
	PACKET_HEADER header;
	int playerIdx;
	int chatLength;
	wchar_t msg[MAX_CHAT_LENGTH];
}
PACKET_GAME_CHAT_MSG;

typedef struct
{
	PACKET_HEADER header;
	wchar_t playerName[MAX_NAME_LENGTH];
	int chatLength;
	wchar_t msg[MAX_CHAT_LENGTH];
}
PACKET_LOBBY_CHAT;

#pragma pack()