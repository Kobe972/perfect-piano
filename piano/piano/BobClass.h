#pragma once
#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>
#include <iostream> // include important C/C++ stuff
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <sys/timeb.h>
#include <time.h>

#include "pch.h"
EXTERN_BOB_OBJECTS()

#define BITMAP_ID            0x4D42 // universal id for a bitmap
#define BITMAP_STATE_DEAD    0
#define BITMAP_STATE_ALIVE   1
#define BITMAP_STATE_DYING   2 
#define BITMAP_ATTR_LOADED   128

#define BITMAP_EXTRACT_MODE_CELL  0
#define BITMAP_EXTRACT_MODE_ABS   1

#define BOB_STATE_DEAD         0    // this is a dead bob
#define BOB_STATE_ALIVE        1    // this is a live bob
#define BOB_STATE_DYING        2    // this bob is dying
#define BOB_STATE_ANIM_DONE    1    // done animation state
#define MAX_BOB_FRAMES         64   // maximum number of bob frames
#define MAX_BOB_ANIMATIONS     16   // maximum number of animation sequeces

#define BOB_ATTR_SINGLE_FRAME   1   // bob has single frame
#define BOB_ATTR_MULTI_FRAME    2   // bob has multiple frames
#define BOB_ATTR_MULTI_ANIM     4   // bob has multiple animations
#define BOB_ATTR_ANIM_ONE_SHOT  8   // bob will perform the animation once
#define BOB_ATTR_VISIBLE        16  // bob is visible
#define BOB_ATTR_BOUNCE         32  // bob bounces off edges
#define BOB_ATTR_WRAPAROUND     64  // bob wraps around edges
#define BOB_ATTR_LOADED         128 // the bob has been loaded
#define BOB_ATTR_CLONE          256 // the bob is a clone

#define WINDOW_WIDTH      1540   // size of window
#define WINDOW_HEIGHT     700

UINT RGBBIT(UCHAR a, UCHAR r, UCHAR g, UCHAR b);
int DDraw_Init(int width, int height);
int DDraw_Shutdown(void);
LPDIRECTDRAWCLIPPER DDraw_Attach_Clipper(LPDIRECTDRAWSURFACE7 lpdds, int num_rects, LPRECT clip_list);
LPDIRECTDRAWSURFACE7 DDraw_Create_Surface(int width, int height, int mem_flags = 0,
    UINT color_key_low = RGBBIT(0, 0, 0, 0),
    UINT color_key_high = RGBBIT(0, 0, 0, 0));
int DDraw_Fill_Surface(LPDIRECTDRAWSURFACE7 lpdds, USHORT color, RECT* client = NULL);
int DDraw_Flip(void);
int Draw_Text_GDI(char* text, int x, int y, COLORREF color, LPDIRECTDRAWSURFACE7 lpdds);
void Change_To_Client_Rect(LPRECT rect);
UINT Gradual_Change(UINT A, UINT B, int alpha);

typedef class BITMAP_FILE_TAG
{
public:
    BITMAPFILEHEADER bitmapfileheader;  // this contains the bitmapfile header
    BITMAPINFOHEADER bitmapinfoheader;  // this is all the info including the palette
    UINT* buffer;           // this is a pointer to the data
    int Flip_Bitmap(UINT* image, int width, int height);
    int Load_File(const char* filename);
    int Unload_File();
} BITMAP_FILE, * BITMAP_FILE_PTR;
void DDraw_Draw_Bitmap(BITMAP_FILE_PTR bitmap, LPDIRECTDRAWSURFACE7 lpdds, POINT coor);
void DDraw_SetColorKey(LPDIRECTDRAWSURFACE7 lpdds, UINT color_key_low = RGBBIT(0, 0, 0, 0),
    UINT color_key_high = RGBBIT(0, 0, 0, 0));
typedef class BOB_TYP
{
private:
    int state;          // the state of the object (general)
    int anim_state;     // an animation state variable, up to you
    int attr;           // attributes pertaining to the object (general)
    float x, y;            // position bitmap will be displayed at
    float xv, yv;          // velocity of object
    int width, height;  // the width and height of the bob
    int counter_1;      // general counters
    int counter_2;
    int max_count_1;    // general threshold values;
    int max_count_2;
    int varsI[16];      // stack of 16 integers
    float varsF[16];    // stack of 16 floats
    int curr_frame;     // current animation frame
    int num_frames;     // total number of animation frames
    int curr_animation; // index of current animation
    int anim_counter;   // used to time animation transitions
    int anim_index;     // animation element index
    int anim_count_max; // number of cycles before animation
    int* animations[MAX_BOB_ANIMATIONS]; // animation sequences
    int next_animation[MAX_BOB_ANIMATIONS];

    UINT color_key_value; // default color key
    LPDIRECTDRAWSURFACE7 images[MAX_BOB_FRAMES]; // the bitmap images DD surfaces
public:
    friend class CPlayer;
    void Set_Anim_Speed(int speed) { anim_count_max = speed; }
    void Set_Animation(int _anim_index) { curr_animation = _anim_index; anim_index = 0; }
    void Set_Vel(int _xv, int _yv) { xv = _xv; yv = _yv; }
    void Set_Pos(int _x, int _y) { x = _x; y = _y; }
    void Hide() { RESET_BIT(attr, BOB_ATTR_VISIBLE); }
    void Show() { SET_BIT(attr, BOB_ATTR_VISIBLE); }
    int Create(int x, int y, int width, int height, int num_frames, int attr,
        int mem_flags = 0, UINT color_key_low = RGBBIT(0, 0, 0, 0),
        UINT color_key_high = RGBBIT(0, 0, 0, 0));
    int Destroy();
    int Draw(LPDIRECTDRAWSURFACE7 dest);
    int Draw_Scaled(int swidth, int sheight, LPDIRECTDRAWSURFACE7 dest);
    int Load_Frame(BITMAP_FILE_PTR bitmap, int frame, int cx, int cy, int mode);
    void Load_Gradual_Frame(BITMAP_FILE_PTR bitmap1, BITMAP_FILE_PTR bitmap2, int cx, int cy, int step);
    int Animate();
    int Move();
    int Load_Animation(int anim_index, int num_frames, int* sequence, int next_animation);
    friend int Clone(BOB_TYP* source, BOB_TYP* dest);
    friend int Collision(BOB_TYP* bob1, BOB_TYP* bob2);
} BOB, * BOB_PTR;

class Clock {
private:
    ULONGLONG m_dwStartTime;
public:
    void Start_Clock() { m_dwStartTime = GetTickCount64(); }
    void Wait_Clock(DWORD dwcount) { while (GetTickCount64() - m_dwStartTime <= dwcount); }
    int Get_Time() { return (GetTickCount64() - m_dwStartTime) / 1000; }
};
