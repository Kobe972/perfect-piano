#pragma once
#include"pch.h"
#include "GUI.h"
#include"BobClass.h"
#include"client.h"
using namespace std;

DWORD WINAPI ParallelPlay(LPVOID param);
extern int single;
class CPiano
{
public:
	int status_mine[85];
	clock_t status_others[85] = { 0 };
	int avail[30] = { 1 };
	int keys[30];
	int LeftKey;
	void Init();
	void PlayString(char* syllables);
	~CPiano();
	BITMAP_FILE keyimg[4];
	void Play();
	void Check();
	void Draw();
};

