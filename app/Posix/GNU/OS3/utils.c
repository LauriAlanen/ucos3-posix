#include "utils.h"

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

CPU_INT32U          RndNext;          /* Used by random generator */
OS_SEM DispStrSem;

static struct timeval _t;
int PC_GetKey(void* x) { return 0; }
void PC_DOSSaveReturn() {}
void PC_DOSReturn() {}
void PC_ElapsedInit() {}
int PC_ElapsedStop() { 
	struct timeval x; 
	gettimeofday(&x, NULL); 
	return (1000000*(x.tv_sec - _t.tv_sec) + (x.tv_usec - _t.tv_usec)); 
}
void PC_ElapsedStart() { 
	gettimeofday(&_t, NULL); 
}
void PC_GetDateTime(char* s) {
	struct timeval x;
	gettimeofday(&x, NULL);
	ctime_r(&x.tv_sec, s);
}
void OSTaskStkInit_FPE_x86() {}

void PC_SetTickRate(int ticks) {
	ualarm(1000000/ticks, 1000000/ticks);
}


void PutChar (char c)
{
    write(1, &c, 1);
}

void PutString (const char *s)
{
    while (*s != '\0') {
        PutChar(*s++);
    }
}

void SPrintDec(char *s, CPU_INT16U x, CPU_INT08U n)
{
    CPU_INT08U i;

    s[n] = '\0';
    for (i = 0; i < n; i++) {
        s[n - i - 1] = '0' + (x % 10);
        x /= 10;
    }
    for (i = 0; i < (n - 1); i++) {
        if (s[i] == '0') {
            s[i] = ' ';
        } else {
            break;
        }
    }
}

CPU_INT08U random (CPU_INT08U n)
{
    RndNext = RndNext * 1103515245 + 12345;
    return ((CPU_INT08U)(RndNext / 256) % (n + 1));
}

void PutDec (CPU_INT08U x2)
{
    CPU_INT08U x0;
    CPU_INT08U x1;

    x0  = (x2 % 10);
    x2 /= 10;
    x1  = (x2 % 10);
    x2 /= 10;
    if (x2) {
        PutChar(x2 + '0');
    }
    if (x1 || x2) {
        PutChar(x1 + '0');
    }
    PutChar(x0 + '0');
}

void PC_Attribute (CPU_INT08U fgcolor, CPU_INT08U bgcolor)
{/*
    PutChar(0x1b);
    PutChar('[');
    PutDec(30 + fgcolor);
    PutChar(';');
    PutDec(40 + bgcolor);
    PutChar('m');*/
}

void PC_DispClrScr (void)
{
    PC_Attribute(COLOR_WHITE, COLOR_BLACK);
    PutString("\x1B[2J");
}

void PC_DispChar (CPU_INT08U x, CPU_INT08U y, char c, CPU_INT08U fgcolor, CPU_INT08U bgcolor)
{
    OS_ERR err;

    printf("1\n");
    OSSemPend(&DispStrSem, 0, OS_OPT_PEND_NON_BLOCKING, NULL, &err);                     /* Acquire semaphore to display string              */
    printf("2\n");
    PC_Attribute(fgcolor, bgcolor);
    PutChar(0x1B);
    PutChar('[');
    PutDec(y);
    PutChar(';');
    PutDec(x);
    PutChar('H');
    PutChar(c);
    printf("3\n");
    OSSemPost(&DispStrSem, OS_OPT_POST_1, &err);                              /* Release semaphore                                */
}

void PC_DispStr (CPU_INT08U x, CPU_INT08U y, const char *s, CPU_INT08U fgcolor, CPU_INT08U bgcolor)
{
    OS_ERR err;

    OSSemPend(&DispStrSem, 0, OS_OPT_PEND_BLOCKING, NULL,&err);                     /* Acquire semaphore to display string              */
    PC_Attribute(fgcolor, bgcolor);
    PutChar(0x1B);
    PutChar('[');
    PutDec(y);
    PutChar(';');
    PutDec(x);
    PutChar('H');
    PutString(s);
    OSSemPost(&DispStrSem, OS_OPT_POST_1 ,&err);                              /* Release semaphore                                */
}