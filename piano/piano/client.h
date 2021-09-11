#pragma once
#include<Windows.h>
#include<string>
#include<ctime>
#include<queue>
//ID of structure MSG
//ID of structure MSG
#define JOIN 0 //加入房间,服务器发送给客户端时，num为房间当前人数
#define QUIT 1 //退出，客户端发给服务器
#define DISCONNECT 2
#define SYLLABLES 3
#define BUMP 4

//player state
#define LOBBY 0
#define WAITING 1 //waiting to play
#define PLAYING 2
#pragma comment(lib, "ws2_32.lib") 
struct Msg
{
	int ID;
	int num=0;
	char string[100] = "?";
};

DWORD WINAPI CreateCilent(LPVOID);
char* GetIP();
void SetIP(char* newIP);
bool Connectivity();
bool SendData(const char* buf, int buf_length);
int send_message(Msg* message);//发送message信息给服务端
void Send(); //发送列表中所有消息

void MsgToString(Msg message, char* ans);//把Msg型数据表示成字符串格式，以便发送
Msg StringToMsg(char* str);//把字符串解码回Msg型数据格式
