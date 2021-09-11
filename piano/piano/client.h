#pragma once
#include<Windows.h>
#include<string>
#include<ctime>
#include<queue>
//ID of structure MSG
//ID of structure MSG
#define JOIN 0 //���뷿��,���������͸��ͻ���ʱ��numΪ���䵱ǰ����
#define QUIT 1 //�˳����ͻ��˷���������
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
int send_message(Msg* message);//����message��Ϣ�������
void Send(); //�����б���������Ϣ

void MsgToString(Msg message, char* ans);//��Msg�����ݱ�ʾ���ַ�����ʽ���Ա㷢��
Msg StringToMsg(char* str);//���ַ��������Msg�����ݸ�ʽ
