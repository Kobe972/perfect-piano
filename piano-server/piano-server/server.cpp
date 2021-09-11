#include<io.h>
#include"CPlayer.h"
#pragma comment(lib, "Ws2_32.lib")
using namespace std;
extern CPlayer player[1000];
extern CRoom rooms[10000];
extern MsgList msglist;
SOCKET serverSocket;
void Join(int person,int room); //第person号玩家状态设为等待游戏开始
void Process(Msg message, int src); //处理消息
void Show_IP(); //打印IP地址
int state = 0;//0表示等待玩家加入游戏，1表示已经开始游戏
const int g_cnt[3] = { 10, 6, 9 };
clock_t clk;
int main()
{
	srand(time(0));
	int mode = 1;
	clk = clock();
	sockaddr_in addrSrv;
	WSADATA wsadata;
	//初始化套接字库
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	ioctlsocket(serverSocket, FIONBIO, (u_long FAR*) & mode);
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(8001);
	bind(serverSocket, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	listen(serverSocket, 100);
	bool display_players;
	while (1)
	{
		display_players = (clock() - clk >= 300);
		if (display_players)
		{
			clk = clock();
			system("cls");
			Show_IP();
		}
		for (int i = 0; i < 1000; i++) //依次检测每个玩家，看是否连接
		{
			player[i].check();
			if (!player[i].connected&&player[i].connect(serverSocket)) player[i].ID=i;
		}
		Msg mes;
		if (display_players) printf("Player information below:\n");
		for (int i = 0; i < 1000; i++)
		{
			player[i].check();
			if (player[i].connected)
			{
				if (player[i].receive(&mes)) Process(mes, i); //接受玩家信息并进行处理
				msglist.Send(); //将消息列表中的信息发送
				//服务器界面显示玩家基本信息
				if (display_players)
				{
					printf("player[%d].ID=%d\nplayer[%d].room=%d\n", i, player[i].ID, i, player[i].room, i);
					switch (player[i].state)
					{
					case LOBBY:
						printf("player[%d].state=LOBBY\n", i);
						break;
					case WAITING:
						printf("player[%d].state=WAITING\n", i);
						break;
					case PLAYING:
						printf("player[%d].state=PLAYING\n", i);
						break;
					default:
						break;
					}
				}
			}
		}
	}
	closesocket(serverSocket);
	return 0;
}

void MsgList::add(Msg message, int dest)
{
	messages.push(message);
	Dest.push(dest);
}

void MsgList::Send()
{
	while (!Dest.empty())
	{
		player[Dest.front()].check();
		if (player[Dest.front()].connected)
		{
			player[Dest.front()].send_message(&messages.front());
		}
		messages.pop();
		Dest.pop();
	}
}

void Join(int person,int room)
{
	player[person].state = PLAYING;
	player[person].room = room;
	Msg mes;
	mes.ID = JOIN;
	mes.num = rooms[player[person].room].m_players.size()+1;
	msglist.add(mes, person);
	rooms[player[person].room].m_players.push_back(player[person]);
}

void Process(Msg message, int src)
{
	Msg mes;
	switch (message.ID)
	{
	case JOIN:
		Join(src,message.num);
		break;
	case QUIT:
		rooms[player[src].room].del(player[src]);
		player[src].state = LOBBY;
		player[src].room = 0;
	break;
	case BUMP:
		player[src].last_bump_time = clock();
		break;
	case SYLLABLES:
		rooms[player[src].room].inform(message,src);
		break;
	default:
		break;
	}
}

void Show_IP()
{
	char ip4[20];
	// 获得本机主机名
	char hostname[MAX_PATH];
	memset(hostname, 0, MAX_PATH);
	gethostname(hostname, MAX_PATH);
	struct hostent FAR* lpHostEnt = gethostbyname(hostname);
	if (lpHostEnt == NULL)
	{
		return;
	}
	// 取得IP地址列表中的第一个为返回的IP, 即有线网卡IP(因为一台主机可能会绑定多个IP)
	LPSTR lpAddr = lpHostEnt->h_addr_list[0];
	// 将IP地址转化成字符串形式
	struct in_addr inAddr;
	memmove(&inAddr, lpAddr, 4);
	strcpy(ip4, inet_ntoa(inAddr));
	printf("IP address:%s\n", ip4);
	return;
}
