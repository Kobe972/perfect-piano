#include<io.h>
#include"CPlayer.h"
#pragma comment(lib, "Ws2_32.lib")
using namespace std;
extern CPlayer player[1000];
extern CRoom rooms[10000];
extern MsgList msglist;
SOCKET serverSocket;
void Join(int person,int room); //��person�����״̬��Ϊ�ȴ���Ϸ��ʼ
void Process(Msg message, int src); //������Ϣ
void Show_IP(); //��ӡIP��ַ
int state = 0;//0��ʾ�ȴ���Ҽ�����Ϸ��1��ʾ�Ѿ���ʼ��Ϸ
const int g_cnt[3] = { 10, 6, 9 };
clock_t clk;
int main()
{
	srand(time(0));
	int mode = 1;
	clk = clock();
	sockaddr_in addrSrv;
	WSADATA wsadata;
	//��ʼ���׽��ֿ�
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
		for (int i = 0; i < 1000; i++) //���μ��ÿ����ң����Ƿ�����
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
				if (player[i].receive(&mes)) Process(mes, i); //���������Ϣ�����д���
				msglist.Send(); //����Ϣ�б��е���Ϣ����
				//������������ʾ��һ�����Ϣ
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
	// ��ñ���������
	char hostname[MAX_PATH];
	memset(hostname, 0, MAX_PATH);
	gethostname(hostname, MAX_PATH);
	struct hostent FAR* lpHostEnt = gethostbyname(hostname);
	if (lpHostEnt == NULL)
	{
		return;
	}
	// ȡ��IP��ַ�б��еĵ�һ��Ϊ���ص�IP, ����������IP(��Ϊһ̨�������ܻ�󶨶��IP)
	LPSTR lpAddr = lpHostEnt->h_addr_list[0];
	// ��IP��ַת�����ַ�����ʽ
	struct in_addr inAddr;
	memmove(&inAddr, lpAddr, 4);
	strcpy(ip4, inet_ntoa(inAddr));
	printf("IP address:%s\n", ip4);
	return;
}
