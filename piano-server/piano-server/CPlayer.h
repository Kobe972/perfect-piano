#pragma once
#include<Windows.h>
#include<string>
#include<ctime>
#include<queue>
#include<conio.h>
#pragma comment(lib,"User32.lib")
using namespace std;
//ID of structure MSG
#define JOIN 0 //加入房间,服务器发送给客户端时，num为1表示成功
#define QUIT 1 //退出，客户端发给服务器
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
	queue<Msg>messages; //队列中的消息
	queue<int> Dest; //各消息要发送给哪个玩家，如Dest=1表示发给player[1]
	void add(Msg message, int dest); //向消息列表添加消息
	void Send(); //发送列表中所有消息
};
void MsgToString(Msg message, char* ans);//把Msg型数据表示成字符串格式，以便发送
void StringToMsg(Msg* message, char* str);//把字符串解码回Msg型数据格式
class CPlayer
{
public:
	clock_t last_bump_time = clock();
	int ID;
	int room=0;
	SOCKET conn;//用于和客户端交流的套接字
	bool connected = false; //conn是否连上客户端
	sockaddr_in clientAddr; //客户端地址信息（只是函数参数需要，暂无实际用处）
	int state = LOBBY;//该玩家状态，默认休息
	int connect(SOCKET listening_sock);//检测conn有没有客户端可连，若有则连上
	int receive(Msg* message);//接收信息并存入message，接受成功返回1，否则返回0
	int send_message(Msg* message);//发送message信息给客户端
	void check();//判断连接是否断开，如断开connected置零
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


