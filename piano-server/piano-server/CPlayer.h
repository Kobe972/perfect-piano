#pragma once
#include<Windows.h>
#include<string>
#include<ctime>
#include<queue>
#include<conio.h>
#pragma comment(lib,"User32.lib")
using namespace std;
//ID of structure MSG
#define JOIN 0 //���뷿��,���������͸��ͻ���ʱ��numΪ1��ʾ�ɹ�
#define QUIT 1 //�˳����ͻ��˷���������
#define DISCONNECT 2
#define SYLLABLES 3
#define BUMP 4

//player state
#define LOBBY 0
#define WAITING 1 //waiting to play
#define PLAYING 2
struct Msg
{
	int ID = 0;
	int num =  0;
	char string[100] = "?";
};
class MsgList
{
public:
	queue<Msg>messages; //�����е���Ϣ
	queue<int> Dest; //����ϢҪ���͸��ĸ���ң���Dest=1��ʾ����player[1]
	void add(Msg message, int dest); //����Ϣ�б������Ϣ
	void Send(); //�����б���������Ϣ
};
void MsgToString(Msg message, char* ans);//��Msg�����ݱ�ʾ���ַ�����ʽ���Ա㷢��
void StringToMsg(Msg* message, char* str);//���ַ��������Msg�����ݸ�ʽ
class CPlayer
{
public:
	clock_t last_bump_time = clock();
	int ID;
	int room=0;
	SOCKET conn;//���ںͿͻ��˽������׽���
	bool connected = false; //conn�Ƿ����Ͽͻ���
	sockaddr_in clientAddr; //�ͻ��˵�ַ��Ϣ��ֻ�Ǻ���������Ҫ������ʵ���ô���
	int state = LOBBY;//�����״̬��Ĭ����Ϣ
	int connect(SOCKET listening_sock);//���conn��û�пͻ��˿���������������
	int receive(Msg* message);//������Ϣ������message�����ܳɹ�����1�����򷵻�0
	int send_message(Msg* message);//����message��Ϣ���ͻ���
	void check();//�ж������Ƿ�Ͽ�����Ͽ�connected����
};
class CRoom
{
public:
	int occupied = 0;
	vector<CPlayer> m_players;
	CPlayer host;
	void del(CPlayer player);
	void inform(Msg message,int src);
	void clear();
};
bool SendData(SOCKET m_hSocket, const char* buf, int buf_length);


