#include "GUI.h"
#define TEST_KBD(id,ch) else if ((keyboard_state[id] & 0x80) && next_available[id]&&strlen(m_input)<21)\
{\
next_available[id] = 0;\
last_inputs.push_back(id);\
m_input[strlen(m_input) - 1] = (keyboard_state[DIK_LSHIFT]&0x80|keyboard_state[DIK_RSHIFT]&0x80)?toupper(ch):ch;\
}
EXTERN_BOB_OBJECTS()
CButton button[20];
CInputBox inputbox[5];
CButton::~CButton()
{
	for (int i = 0; i < 3; i++)
	{
		if (m_ButtonSur[i])
		{
			m_ButtonSur[i]->Release();
			m_ButtonSur[i] = NULL;
			m_bitmap[i].Unload_File();
		}
	}
}
void CButton::Create(int ID, int width, int height, int x, int y, std::string filename)
{
	m_ID = ID;
	POINT position_in_offscreen;
	position_in_offscreen.x = 0;
	position_in_offscreen.y = 0;
	BITMAP_FILE T_bitmap[3];
	for (int i = 0; i < 3; i++)
		m_ButtonSur[i] = DDraw_Create_Surface(width, height, 0, 0);
	m_Width = width;
	m_Height = height;
	boarder.left = x;
	boarder.top = y;
	boarder.bottom = boarder.top + m_Height;
	boarder.right = boarder.left + m_Width;
	T_bitmap[0].Load_File((".\\GUI\\button\\" + filename + ".bmp").c_str());
	T_bitmap[1].Load_File((".\\GUI\\button\\" + filename + "_on.bmp").c_str());
	T_bitmap[2].Load_File((".\\GUI\\button\\" + filename + "_down.bmp").c_str());
	for (int i = 0; i < 3; i++)
		DDraw_Draw_Bitmap(&T_bitmap[i], m_ButtonSur[i], position_in_offscreen);
	m_state = BSTATENORMAL;
	for (int i = 0; i < 3; ++i)
		T_bitmap[i].Unload_File();
}
void CButton::Draw()//在后备缓冲表面绘图
{
	lpddsback->Blt(&boarder, m_ButtonSur[m_state % 3], NULL, DDBLT_WAIT | DDBLT_KEYSRC, NULL);
}

void CButton::Check()
{
	if (m_clipped == false && m_state != BSTATEUP) m_state = BSTATENORMAL;
	if (pos.x >= boarder.left && pos.x <= boarder.right && pos.y >= boarder.top && pos.y <= boarder.bottom)
	{
		if (m_clipped == false) m_state = BSTATEON;
		else m_state = BSTATEUP;
		if (mouse_state.rgbButtons[MOUSE_LEFT_BUTTON] & 0x80)
		{
			m_state = BSTATEDOWN;
			m_clipped = true;
		}
		else m_clipped = false;
	}
	else
	{
		m_clipped = false;
	}
}

void CInputBox::Create(int ID, int width, int height, int x, int y, bool password)
{
	m_ID = ID;
	m_Width = width;
	m_Height = height;
	memset(m_input, 0, sizeof(m_input));
	m_input[0] = '|';
	m_password = password;
	boarder.left = x;
	boarder.top = y;
	boarder.bottom = boarder.top + m_Height;
	boarder.right = boarder.left + m_Width;
	for (int i = 0; i < 256; i++) next_available[i] = 1;
}

void CInputBox::Load(char* buf)
{
	strcpy(buf, m_input);
	buf[strlen(buf) - 1] = 0;
}

void CInputBox::Draw()
{
	DDBLTFX ddbltfx;
	static clock_t last_draw_time = clock();
	static bool character = false;//should buf's last character be '|'
	if (clock() - last_draw_time >= 600)
	{
		character = !character;
		last_draw_time = clock();
	}
	DDRAW_INIT_STRUCT(ddbltfx);
	ddbltfx.dwFillColor = RGBBIT(0, 220, 220, 220);
	lpddsback->Blt(&boarder, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
	POINT beginning;
	beginning.x = boarder.left + 5;
	beginning.y = boarder.top + 5;
	if (!m_password)
	{
		char buf[50];
		memcpy(buf, m_input, sizeof(m_input));
		if (!m_activated || !character) buf[strlen(buf) - 1] = 0;
		Draw_Text_GDI(buf, beginning.x, beginning.y, RGB(0, 0, 0), lpddsback);
	}
	else
	{
		char buf[50];
		memset(buf, 0, sizeof(buf));
		for (int i = 0; i < strlen(m_input); i++) buf[i] = '*';
		if (m_activated && character) buf[strlen(buf) - 1] = '|';
		else buf[strlen(m_input) - 1] = 0;
		Draw_Text_GDI(buf, beginning.x, beginning.y, RGB(0, 0, 0), lpddsback);
	}
}

void CInputBox::Check()
{
	if (mouse_state.rgbButtons[MOUSE_LEFT_BUTTON] & 0x80)
	{
		if (pos.x >= boarder.left && pos.x <= boarder.right && pos.y >= boarder.top && pos.y <= boarder.bottom)
		{
			m_activated = true;
		}
		else
		{
			m_activated = false;
		}
	}
	if (!m_activated) return;
	int tmp = 0;
	for (vector<int>::iterator ite = last_inputs.begin(); ite < last_inputs.end();ite++)
	{
		if (!(keyboard_state[*ite] & 0x80))
		{
			next_available[*ite] = 1;
			last_inputs.erase(ite);
			ite = last_inputs.begin();
			if (ite == last_inputs.end()) break;
		}
	}
	if (keyboard_state[DIK_0] & 0x80 && next_available[DIK_0])
	{
		next_available[DIK_0] = 0;
		last_inputs.push_back(DIK_0);
		m_input[strlen(m_input) - 1] = '0';
	}
	TEST_KBD(DIK_1, '1')
		TEST_KBD(DIK_2, '2')
		TEST_KBD(DIK_3, '3')
		TEST_KBD(DIK_4, '4')
		TEST_KBD(DIK_5, '5')
		TEST_KBD(DIK_6, '6')
		TEST_KBD(DIK_7, '7')
		TEST_KBD(DIK_8, '8')
		TEST_KBD(DIK_9, '9')
		TEST_KBD(DIK_NUMPAD0, '0')
		TEST_KBD(DIK_NUMPAD1, '1')
		TEST_KBD(DIK_NUMPAD2, '2')
		TEST_KBD(DIK_NUMPAD3, '3')
		TEST_KBD(DIK_NUMPAD4, '4')
		TEST_KBD(DIK_NUMPAD5, '5')
		TEST_KBD(DIK_NUMPAD6, '6')
		TEST_KBD(DIK_NUMPAD7, '7')
		TEST_KBD(DIK_NUMPAD8, '8')
		TEST_KBD(DIK_NUMPAD9, '9')
		TEST_KBD(DIK_PERIOD, '.')
		TEST_KBD(DIK_NUMPADPERIOD, '.')
		TEST_KBD(DIK_A, 'a')
		TEST_KBD(DIK_B, 'b')
		TEST_KBD(DIK_C, 'c')
		TEST_KBD(DIK_D, 'd')
		TEST_KBD(DIK_E, 'e')
		TEST_KBD(DIK_F, 'f')
		TEST_KBD(DIK_G, 'g')
		TEST_KBD(DIK_H, 'h')
		TEST_KBD(DIK_I, 'i')
		TEST_KBD(DIK_J, 'j')
		TEST_KBD(DIK_K, 'k')
		TEST_KBD(DIK_L, 'l')
		TEST_KBD(DIK_M, 'm')
		TEST_KBD(DIK_N, 'n')
		TEST_KBD(DIK_O, 'o')
		TEST_KBD(DIK_P, 'p')
		TEST_KBD(DIK_Q, 'q')
		TEST_KBD(DIK_R, 'r')
		TEST_KBD(DIK_S, 's')
		TEST_KBD(DIK_T, 't')
		TEST_KBD(DIK_U, 'u')
		TEST_KBD(DIK_V, 'v')
		TEST_KBD(DIK_W, 'w')
		TEST_KBD(DIK_X, 'x')
		TEST_KBD(DIK_Y, 'y')
		TEST_KBD(DIK_Z, 'z')
	else if (keyboard_state[DIK_BACKSPACE] & 0x80 && next_available[DIK_BACKSPACE] && strlen(m_input) > 1)
	{
		next_available[DIK_BACKSPACE] = 0;
		last_inputs.push_back(DIK_BACKSPACE);
		m_input[strlen(m_input) - 1] = 0;
		m_input[strlen(m_input) - 1] = 0;
	}
	else return;
	m_input[strlen(m_input)] = '|';
	return;
}
CFont::CFont() {
	lf.lfHeight = 20;
	lf.lfWidth = 10;
	lf.lfEscapement = 0;
	lf.lfUnderline = false;
	lf.lfStrikeOut = false;
	lf.lfOrientation = 0;
	lf.lfWeight = 1000;
	lf.lfItalic = false;
	lf.lfCharSet = 0;
	lf.lfQuality = 0;
	lf.lfClipPrecision = 0;
	lf.lfPitchAndFamily = 0;
	lf.lfOutPrecision = 0;
	strcpy(lf.lfFaceName, "Comic Sans");
}

void CFont::SetType(int height, int width, int weight)
{
	lf.lfHeight = height;
	lf.lfWidth = width;
	lf.lfWeight = weight;
}

void CFont::Uself()
{
	hf = CreateFontIndirect(&lf);
	SelectObject(hdc, hf);
	DeleteObject(hf);
}