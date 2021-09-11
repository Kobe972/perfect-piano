#pragma once

// DEFINES ////////////////////////////////////////////////

// MACROS /////////////////////////////////////////////////



// TYPES //////////////////////////////////////////////////



// PROTOTYPES /////////////////////////////////////////////

// input
int DInput_Init(void);
void DInput_Shutdown(void);

int DInput_Init_Mouse(void);
int DInput_Init_Keyboard(void);
int DInput_Read_Mouse(void);
int DInput_Read_Keyboard(void);
void DInput_Release_Mouse(void);
void DInput_Release_Keyboard(void);

// GLOBALS ////////////////////////////////////////////////


// EXTERNALS //////////////////////////////////////////////

extern HWND main_window_handle; // save the window handle
extern HINSTANCE main_instance; // save the instance

// directinput globals
extern LPDIRECTINPUT8       lpdi;       // dinput object
extern LPDIRECTINPUTDEVICE8 lpdikey;    // dinput keyboard
extern LPDIRECTINPUTDEVICE8 lpdimouse;  // dinput mouse

// these contain the target records for all di input packets
extern UCHAR keyboard_state[256]; // contains keyboard state table
extern DIMOUSESTATE mouse_state;  // contains state of mouse
extern POINT pos;

#define MOUSE_LEFT_BUTTON 0