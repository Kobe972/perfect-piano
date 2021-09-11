#include"CPiano.h"
char name[85][4];
int now[85];
int single;
int pressed;
extern std::queue<Msg>g_Send, g_Recv; //队列中的消息
#define TEST_KBD(id,index) if ((keyboard_state[id] & 0x80) && avail[index]==0&&index+LeftKey<85)\
{\
status_mine[index+LeftKey]=1;\
now[index+LeftKey]=1;\
avail[index]=1;\
pressed=1;\
}\
else if(!(keyboard_state[id] & 0x80)&&index+LeftKey<85)\
{\
status_mine[index+LeftKey]=0;\
avail[index]=0;\
}
 void CPiano::Init()
{
	LeftKey = 36;
	keyimg[0].Load_File(".\\keys\\white.bmp");
	keyimg[1].Load_File(".\\keys\\white_down.bmp");
	keyimg[2].Load_File(".\\keys\\black.bmp");
	keyimg[3].Load_File(".\\keys\\black_down.bmp");
	FILE* fp;
	fp = fopen("keys.txt", "r");
	for (int i = 0; i < 30; i++)
		fscanf(fp, "%d", &keys[i]);
	fclose(fp);
	for (int i = 0; i < 7; i++)
	{
		strcpy(name[12 * i], ("c" + to_string(i)).c_str());
		strcpy(name[12 * i + 1], ("cs" + to_string(i)).c_str());
		strcpy(name[12 * i + 2], ("d" + to_string(i)).c_str());
		strcpy(name[12 * i + 3], ("ds" + to_string(i)).c_str());
		strcpy(name[12 * i + 4], ("e" + to_string(i)).c_str());
		strcpy(name[12 * i + 5], ("f" + to_string(i)).c_str());
		strcpy(name[12 * i + 6], ("fs" + to_string(i)).c_str());
		strcpy(name[12 * i + 7], ("g" + to_string(i)).c_str());
		strcpy(name[12 * i + 8], ("gs" + to_string(i)).c_str());
		strcpy(name[12 * i + 9], ("a" + to_string(i)).c_str());
		strcpy(name[12 * i + 10], ("as" + to_string(i)).c_str());
		strcpy(name[12 * i + 11], ("b" + to_string(i)).c_str());
	}
	strcpy(name[84], "c7");
}
 CPiano::~CPiano()
 {
	 for (int i = 0; i < 4; i++)
		 keyimg[i].Unload_File();
 }
DWORD WINAPI ParallelPlay(LPVOID param)
{
	if (!pressed) return 0;
	static int cookie = 0;
	Msg message;
	message.ID = SYLLABLES;
	message.string[0] = 0;
	for (int i = 0; i < 85; i++)
	{
		if (now[i])
		{
			mciSendString(("open .\\sound\\syllables\\" + string(name[i]) + ".mp3 alias "+to_string(cookie)).c_str(), NULL, 0, NULL);
			mciSendString(("play "+to_string(cookie++)).c_str(), NULL, 0, NULL);
			if (!single)
			{
				if (strlen(message.string)) strcat(message.string, "|");
				strcat(message.string, to_string(i).c_str());
			}
		}
	}
	if (!single) g_Send.push(message);
	return 0;
}
void CPiano::Check()
{
	memset(now, 0, sizeof(now));
	pressed = 0;
	static int move_left = 1,move_right=1;
	if ((keyboard_state[DIK_A] & 0x80) && move_right)
	{
		move_right = 0;
		if(LeftKey>=12) LeftKey -= 12;
	}
	else if(!(keyboard_state[DIK_A] & 0x80))
		move_right = 1;
	if ((keyboard_state[DIK_D] & 0x80) && move_left)
	{
		move_left = 0;
		if (LeftKey <= 48) LeftKey += 12;
	}
	else if (!(keyboard_state[DIK_D] & 0x80))
		move_left = 1;
	for (int i = 0; i < 30; i++)
		TEST_KBD(keys[i], i);
	Play();
}
void CPiano::PlayString(char* syllables)
{
	static int cookie = 0;
	char* syllable;
	syllable = strtok(syllables, "|");
	mciSendString(("open .\\sound\\syllables\\" + string(name[atoi(syllable)]) + ".mp3 alias " + to_string(cookie)).c_str(), NULL, 0, NULL);
	mciSendString(("play " + to_string(cookie++)).c_str(), NULL, 0, NULL);
	status_others[atoi(syllable)] = clock();
	syllable = strtok(NULL, "|");
	while (syllable != NULL)
	{
		mciSendString(("open .\\sound\\syllables\\" + string(name[atoi(syllable)]) + ".mp3 alias " + to_string(cookie)).c_str(), NULL, 0, NULL);
		mciSendString(("play " + to_string(cookie++)).c_str(), NULL, 0, NULL);
		status_others[atoi(syllable)] = clock();
		syllable = strtok(NULL, "|");
	}
}
void CPiano::Play()
{
	CreateThread(NULL, 0, ParallelPlay, NULL, 0, NULL);
}

void CPiano::Draw()
{
	POINT coor_white = { 0,250 };
	POINT coor_black = { 60,250 };
	int white[29] = { 1,0,1,0,1,1,0,1,0,1,0,1,1,0,1,0,1,1,0,1,0,1,0,1,1,0,1,0,1 };
	for (int i = 0; i < 29; i++)
	{
		if (white[i])
		{
			if (i + LeftKey > 84) break;
			if (status_mine[i + LeftKey] == 0&&( status_others[i + LeftKey] == 0 || clock() - status_others[i + LeftKey] > 300))
				DDraw_Draw_Bitmap(&keyimg[0], lpddsback, coor_white);
			else
				DDraw_Draw_Bitmap(&keyimg[1], lpddsback, coor_white);
			coor_white.x += 90;
		}
	}
	for (int i = 0; i < 29; i++)
	{
		if (i + LeftKey > 84) break;
		if (!white[i])
		{
			if (status_mine[i + LeftKey] == 0 && (status_others[i + LeftKey]==0 || clock() - status_others[i + LeftKey] > 300))
				DDraw_Draw_Bitmap(&keyimg[2], lpddsback, coor_black);
			else
				DDraw_Draw_Bitmap(&keyimg[3], lpddsback, coor_black);
			coor_black.x += ((i == 3 || i==10||i == 15||i==22) ? 180 : 90);
		}
	}
	clock_t tmp = clock();
	for (int i = 0; i < 85; i++)
		if (tmp - status_others[i] > 300)
			status_others[i] = 0;
	CFont CurText;
	lpddsback->GetDC(&CurText.hdc);
	CurText.SetType(25,8,10);
	CurText.Uself();
	SetTextColor(CurText.hdc, RGB(0, 0, 0));
	SetBkMode(CurText.hdc, TRANSPARENT);
	TextOut(CurText.hdc,35 , 650,name[LeftKey], strlen(name[LeftKey]));
	lpddsback->ReleaseDC(CurText.hdc);
}