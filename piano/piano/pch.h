#pragma once

/*
	这里放头文件和全局变量
*/

#include <cassert>
#include <cmath>
#include <algorithm>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <ios>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <numeric>
#include <ostream>
#include <queue>
#include <random>
#include <regex>
#include <set>
#include <stack>
#include <string>
#include <strstream>
#include <tuple>
#include <vector>


#include <ddraw.h>    // directX includes
#include<dinput.h>
#include"T3DLIB2.h"
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib,"winmm.lib")

typedef unsigned short USHORT;
typedef unsigned short WORD;
typedef unsigned char  BYTE;
typedef unsigned int   QUAD;
typedef unsigned int   UINT;

#define EXTERN_BOB_OBJECTS()																\
extern LPDIRECTDRAW7        lpdd;														\
extern LPDIRECTDRAWSURFACE7 lpddsprimary;  												\
extern LPDIRECTDRAWSURFACE7 lpddsback;  												\
extern LPDIRECTDRAWCLIPPER  lpddclipper; 												\
extern LPDIRECTDRAWCLIPPER  lpddclipperwin; 											\
extern DDSURFACEDESC2       ddsd;               										\
extern DDBLTFX              ddbltfx;          								 			\
extern DDSCAPS2             ddscaps;        											\
extern HRESULT              ddrval;             										\
extern UINT* primary_buffer;															\
extern UINT* back_buffer; 																\
extern HWND main_window_handle; 														\
extern HINSTANCE main_instance; 														\
extern int screen_width, screen_height;													\
extern int min_clip_x,  max_clip_x, min_clip_y, max_clip_y;  							\
extern int window_client_x0;   															\
extern int window_client_y0;   															\


#define CREATE_BOB_OBJECTS()													\
LPDIRECTDRAW7        lpdd;														\
LPDIRECTDRAWSURFACE7 lpddsprimary;  											\
LPDIRECTDRAWSURFACE7 lpddsback;  												\
LPDIRECTDRAWCLIPPER  lpddclipper; 												\
LPDIRECTDRAWCLIPPER  lpddclipperwin; 											\
DDSURFACEDESC2       ddsd;               										\
DDBLTFX              ddbltfx;          								 			\
DDSCAPS2             ddscaps;        											\
HRESULT              ddrval;             										\
UINT* primary_buffer;															\
UINT* back_buffer; 																\
HWND main_window_handle; 														\
HINSTANCE main_instance; 														\
int screen_width, screen_height;												\
int min_clip_x,  max_clip_x, min_clip_y, max_clip_y;  							\
int window_client_x0;   														\
int window_client_y0;   														\

#define EXTERN_INPUT_DATA()\
extern LPDIRECTINPUT8       lpdi;\
extern LPDIRECTINPUTDEVICE8 lpdikey;\
extern LPDIRECTINPUTDEVICE8 lpdimouse;\
extern UCHAR keyboard_state[256]; \
extern DIMOUSESTATE mouse_state; \
extern POINT pos;\
// these read the keyboard asynchronously
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

#define SCREEN_WIDTH        1540  // size of screen
#define SCREEN_HEIGHT       700

#define DDRAW_INIT_STRUCT(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }

// bit manipulation macros
#define SET_BIT(word,bit_flag)   ((word)=((word) | (bit_flag)))
#define RESET_BIT(word,bit_flag) ((word)=((word) & (~bit_flag)))
