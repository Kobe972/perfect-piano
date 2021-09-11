#include "client.h"
char IP[20] = "118.195.140.195";
SOCKET g_connSocket;
int connected;
std::queue<Msg>g_Send, g_Recv; //队列中的消息

char* GetIP()
{
	return IP;
}
void SetIP(char* newIP)
{
	strcpy(IP, newIP);
}
bool Connectivity()
{
	return connected;
}
bool SendData(const char* buf, int buf_length)
{
	int sent_bytes = 0;
	int ret = 0;
	while (true)
	{
		ret = send(g_connSocket, buf + sent_bytes, buf_length - sent_bytes, 0);
		if (ret == -1)
		{
			if (errno == EWOULDBLOCK)
			{
				break;
			}
			else if (errno == EINTR)
				continue;
			else
				return false;
		}
		else if (ret == 0)
		{
			return false;
		}

		sent_bytes += ret;
		if (sent_bytes == buf_length)
			break;
		Sleep(1);
	}

	return true;
}


DWORD WINAPI CreateCilent(LPVOID)
{
	char Prev_IP[20];
	strcpy(Prev_IP, IP);
	sockaddr_in addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(IP);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(8001);
conn:;
	connected = 0;
	int mode = 1;
	g_connSocket = socket(AF_INET, SOCK_STREAM, 0);
	ioctlsocket(g_connSocket, FIONBIO, (u_long FAR*) & mode);
	while (!connected)
	{
		if (!strcmp(IP, Prev_IP))
		{
			strcpy(Prev_IP, IP);
			addrSrv.sin_addr.S_un.S_addr = inet_addr(IP);
		}
		if (connect(g_connSocket, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) != SOCKET_ERROR)
		{
			connected = 1;
			break;
		}
		else
		{
			int err = WSAGetLastError();
			if (err == WSAEISCONN)
			{
				connected = 1;
				break;
			}
		}
	}
	char buf[300];
	clock_t last_bump_time = clock();
	clock_t last_check_time = clock();
	while (1)
	{
		if (strcmp(IP, Prev_IP)!=0)
		{
			closesocket(g_connSocket);
			goto conn;
		}
		if (clock() - last_bump_time >= 2000)
		{
			last_bump_time = clock();
			Msg Tmesssage;
			Tmesssage.ID = BUMP;
			g_Send.push(Tmesssage);
		}
		Send();
		if (recv(g_connSocket, buf, sizeof(buf), 0) > 0)
			g_Recv.push(StringToMsg(buf));
		last_check_time = clock();
		WSASetLastError(0);
		recv(g_connSocket, buf, 1, MSG_PEEK);
		bool is_ok = (WSAECONNRESET != WSAGetLastError());
		if (!is_ok)
		{
			closesocket(g_connSocket);
			goto conn;
		}
	}
	return 0;
}

int send_message(Msg* message)
{
	char buf[300];
	MsgToString(*message, buf);
	if (SendData(buf, strlen(buf) + 1)) return 1;
	return 0;
}

void Send()
{
	while (!g_Send.empty())
	{
		send_message(&g_Send.front());
		g_Send.pop();
	}
}

void MsgToString(Msg message, char* ans)
{
	std::string tmp;
	tmp = std::to_string(message.ID) + " " + std::to_string(message.num) + " " ;
	strcpy(ans, tmp.c_str());
	strcat(ans, message.string);
	return;
}

Msg StringToMsg(char* str)
{
	Msg message;
	char* tmp;
	tmp = strtok(str, " ");
	message.ID = atoi(tmp);
	tmp = strtok(NULL, " ");
	message.num = atoi(tmp);
	tmp = strtok(NULL, " ");
	strcpy(message.string, tmp);
	return message;
}
