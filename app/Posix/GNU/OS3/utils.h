#ifndef UTILS_H
#define UTILS_H

#include "includes.h"
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#define  COLOR_BLACK     0         /* VT102 Color Codes  */
#define  COLOR_RED       1
#define  COLOR_GREEN     2
#define  COLOR_BROWN     3
#define  COLOR_BLUE      4
#define  COLOR_MAGENTA   5
#define  COLOR_CYAN      6
#define  COLOR_WHITE     7
#define  COLOR_LIGHT_GRAY 7
#define  COLOR_YELLOW	0

extern OS_SEM DispStrSem;
extern OS_SEM RandomSem;

int PC_GetKey(void* x);
void PC_DOSSaveReturn();
void PC_DOSReturn();
void PC_ElapsedInit();
int PC_ElapsedStop();
void PC_ElapsedStart();
void PC_GetDateTime(char* s);
void OSTaskStkInit_FPE_x86();
void PC_VectSet(int x, void* f);
void PC_SetTickRate(int ticks);
void PutChar (char c);
void PutString (const char *s);
void SPrintDec(char *s, CPU_INT16U x, CPU_INT08U n);
void PutDec (CPU_INT08U x2);
void PC_Attribute (CPU_INT08U fgcolor, CPU_INT08U bgcolor);
void PC_DispClrScr (void);
void PC_DispChar (CPU_INT08U x, CPU_INT08U y, char c, CPU_INT08U fgcolor, CPU_INT08U bgcolor);
void PC_DispStr (CPU_INT08U x, CPU_INT08U y, const char *s, CPU_INT08U fgcolor, CPU_INT08U bgcolor);
void Print_to_Screen(const char *text_ptr);

#endif