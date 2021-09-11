#pragma once
#include"client.h"
#include"CPiano.h"
#include <Windows.h>
#include<direct.h>
#include<math.h>
//then define game states
#define MAINMENU (EGameState::eGameStateMainMenu)
#define SINGLE_PLAY (EGameState::eGameStateSinglePlayer)
#define SINGLE_END (EGameState::eGameStateSingleEnd)
#define HELP (EGameState::eGameStateHelp)
#define SERVER (EGameState::eGameStateServer)
#define MULTIPLAYER (EGameState::eGameStateMultiplePlayer) //������Ϸ��ʽ��ʼ
#define CREATE_ROOM (EGameState::eGameStateCreateRoom) //��������
#define JOIN_ROOM (EGameState::eGameStateJoin) //���뷿��
#define CHOOSE_MODE (EGameState::eGameStateChooseMode) //ѡ�񴴽����仹�Ǽ��뷿��


#pragma warning
EXTERN_BOB_OBJECTS()
EXTERN_INPUT_DATA()


class CGame
{
public:
	void GameInit();
	void GameMain();
	~CGame();
	CPiano piano;
private:
	int is_single;
public:
	enum EGameState {
		eGameStateMainMenu = 0,eGameStateSinglePlayer, eGameStateSingleEnd,
		eGameStateHelp,eGameStateJoin, eGameStateMultiplePlayer,eGameStateServer
	};
	//GameState ��������
private:
	EGameState m_eGameState{ CGame::EGameState::eGameStateMainMenu };
public:
	inline void SetGameState(EGameState eGameStateCurrent);
	inline void SetWindowHandle(HWND hwnd);
	void GetCurMsg();
	void ProcessKeyMsg();
	void ProcessButtonMsg();
	void ShowMenu();
	void SinglePlay();
	void JoinRoom();
	void MultiPlayer();
	void Help();
	void Server();
	bool ButtonReturn();
private:
	HWND m_hWnd;
	CPiano Piano;
public: // serve and client
	void ProcessSerMessage();//�����ź�
};