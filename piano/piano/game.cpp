#include"game.h"
std::string g_ServeID;
Clock g_gametime, g_connecttime;
extern CButton button[20];
extern CInputBox inputbox[5];
extern char IP[20];
extern std::queue<Msg>g_Send, g_Recv; //队列中的消息
extern SOCKET g_connSocket;
EXTERN_INPUT_DATA()
inline void CGame::SetGameState(CGame::EGameState eGameStateCurrent)
{
    m_eGameState = eGameStateCurrent;
}

inline void CGame::SetWindowHandle(HWND hwnd)
{
    m_hWnd = hwnd;
}
void CGame::GameInit()
{
    SetGameState(MAINMENU);
    //load buttons in main menu
    button[ISINGLE_MODE].Create(ISINGLE_MODE, 271, 63, 41, 550, "single");
    button[IMULTI_MODE].Create(IMULTI_MODE, 271, 63, 454, 550, "multi");
    button[IHELP].Create(IHELP, 271, 63, 847, 550, "help");
    button[ISERVER].Create(ISERVER, 271, 63, 1240, 550, "server");
    button[IRETURN].Create(IRETURN, 80, 80, 0, 0, "return");
    button[IOK].Create(IOK, 271, 63, 635, 500, "OK");
    inputbox[IIPBOX].Create(IIPBOX, 250, 25, 645, 300, false);
    inputbox[IROOMBOX].Create(IROOMBOX, 250, 25, 120, 300, false);
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);
    CreateThread(NULL, 0, CreateCilent, 0, 0, NULL);
    piano.Init();
    DInput_Init();
    DInput_Init_Keyboard();
    DInput_Init_Mouse();
    
}
void CGame::GameMain()
{
    switch (m_eGameState)
    {
    case MAINMENU:
        ShowMenu();
        break;
    case SINGLE_PLAY:
        SinglePlay();
        break;
    case HELP:
        Help();
        break;
    case JOIN_ROOM:
        JoinRoom();
        break;
    case SERVER:
        Server();
        break;
    case MULTIPLAYER:
        MultiPlayer();
        break;
    default:
        break;
    }
    HWND hw;
    hw = GetForegroundWindow();
    if (hw == main_window_handle)
    {
        GetCurMsg();//include mouse and keyboard.understand "cur" as "current",not "cursor"
        ProcessButtonMsg();
        ProcessKeyMsg();
        ProcessSerMessage();
    }
}

void CGame::GetCurMsg()
{
    GetCursorPos(&pos);
    ScreenToClient(main_window_handle, &pos);
    DInput_Read_Mouse();
    DInput_Read_Keyboard();
    return;
}

bool CGame::ButtonReturn() {
    button[IRETURN].Check();
    if (button[IRETURN].m_state == BSTATEUP) {
        mciSendString("play .\\sound\\click\\0.wav", NULL, 0, NULL);
        button[IRETURN].m_state = BSTATENORMAL;
        return true;
    }
    return false;
}

void CGame::ProcessButtonMsg()
{
    Msg message;
    switch (m_eGameState)
    {
    case MAINMENU:
        for (int i = ISINGLE_MODE; i <= ISERVER; i++)
        {
            button[i].Check();
            if (button[i].m_state == BSTATEUP)
            {
                mciSendString("play .\\sound\\click\\0.wav", NULL, 0, NULL);
                switch (i)
                {
                case ISINGLE_MODE:
                    single = 1;
                    SetGameState(SINGLE_PLAY);
                    break;
                case IMULTI_MODE:
                    SetGameState(JOIN_ROOM);
                    break;
                case IHELP:
                    SetGameState(HELP);
                    break;
                case ISERVER:
                    strcpy(inputbox[IIPBOX].m_input, IP);
                    strcat(inputbox[IIPBOX].m_input, "|");
                    SetGameState(SERVER);
                    break;
                default:
                    break;
                }
                button[i].m_state = BSTATENORMAL;
            }
        }
        break;
    case SINGLE_PLAY:
        if (ButtonReturn())
            SetGameState(MAINMENU);
        break;
    case HELP:
        if (ButtonReturn())
            SetGameState(MAINMENU);
        break;
    case JOIN_ROOM:
        button[IOK].Check();
        if (button[IOK].m_state == BSTATEUP)
        {
             mciSendString("play .\\sound\\click\\0.wav", NULL, 0, NULL);
             if (atoi(inputbox[IROOMBOX].m_input) > 0 && atoi(inputbox[IROOMBOX].m_input) < 10000)
             {
                 Msg Tmessage;
                 char tmp[10];
                 Tmessage.ID = JOIN;
                 inputbox[IROOMBOX].Load(tmp);
                 Tmessage.num = atoi(tmp);
                 g_Send.push(Tmessage);
                 memset(inputbox[IROOMBOX].m_input, 0, sizeof(inputbox[IROOMBOX].m_input));
                 inputbox[IROOMBOX].m_input[0] = '|';
             }
             else
             {
                 MessageBox(main_window_handle, "Your room number must be 1 to 9999!", "Error", MB_OK);
             }
             button[IOK].m_state = BSTATENORMAL;
         }
        if (ButtonReturn()) {
            memset(inputbox[IROOMBOX].m_input, 0, sizeof(inputbox[IROOMBOX].m_input));
            inputbox[IROOMBOX].m_input[0] = '|';
            SetGameState(MAINMENU);
        }
        break;
    case SERVER:
        button[IOK].Check();
        if (button[IOK].m_state == BSTATEUP)
        {
            mciSendString("play .\\sound\\click\\0.wav", NULL, 0, NULL);
            char tmp[20];
            inputbox[IIPBOX].Load(tmp);
            strcpy(IP, tmp);
            button[IOK].m_state = BSTATENORMAL;
        }
        if (ButtonReturn())
            SetGameState(MAINMENU);
        break;
    case MULTIPLAYER:
        if (ButtonReturn()) {
            Msg Tmessage;
            Tmessage.ID = QUIT;
            g_Send.push(Tmessage);
            SetGameState(MAINMENU);
        }
        break;
    default:
        break;
    }
    return;
}

void CGame::ProcessKeyMsg()
{
    switch (m_eGameState)
    {
    case SINGLE_PLAY:case MULTIPLAYER:
        piano.Check();
        break;
    case JOIN_ROOM:
        inputbox[IROOMBOX].Check();
        break;
    case SERVER:
        inputbox[IIPBOX].Check();
        break;
    default:
        break;
    }
    return;
}

void CGame::ShowMenu()
{
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\MainMenu.bmp");
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    button[ISINGLE_MODE].Draw();
    button[IMULTI_MODE].Draw();
    button[IHELP].Draw();
    button[ISERVER].Draw();
    return;
}

void CGame::Server()
{
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\server.bmp");
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    inputbox[IIPBOX].Draw();
    button[IOK].Draw();
    button[IRETURN].Draw();
}

void CGame::SinglePlay()
{
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\play.bmp");
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    button[IRETURN].Draw();
    piano.Draw();
}

void CGame::Help()
{
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\Help.bmp");
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    button[IRETURN].Draw();
}

CGame::~CGame()
{
    DInput_Release_Keyboard();
    DInput_Read_Mouse();
    DInput_Shutdown();
    DDraw_Shutdown();
}

void CGame::ProcessSerMessage()
{
    if (!Connectivity()) return;
    for (; !g_Recv.empty();)
    {
        Msg Tmessage = g_Recv.front(); g_Recv.pop();
        switch (Tmessage.ID)
        {
        case SYLLABLES:
            piano.PlayString(Tmessage.string);
            break;
        case JOIN:
            MessageBox(main_window_handle, ("There are now " + to_string(Tmessage.num) + " people in the room").c_str(),"Attention",MB_OK);
            single = 0;
            SetGameState(MULTIPLAYER);
            break;
        default:
            break;
        }
    }
}

void CGame::JoinRoom()
{
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\selroom.bmp");
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    inputbox[IROOMBOX].Draw();
    button[IOK].Draw();
    button[IRETURN].Draw();
}

void CGame::MultiPlayer()
{
    BITMAP_FILE_PTR bitmap = new BITMAP_FILE;
    bitmap->Load_File(".\\background\\play.bmp");
    DDraw_Draw_Bitmap(bitmap, lpddsback, { 0,0 });
    bitmap->Unload_File();
    button[IRETURN].Draw();
    piano.Draw();
}



