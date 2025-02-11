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
OS_SEM RandomSem;

static struct timeval _t;

/**
 * @brief Reads a key press from the keyboard in a non-blocking manner.
 * 
 * This function temporarily configures the terminal to operate in non-canonical mode,
 * allowing single-character input without waiting for the Enter key. The terminal is
 * restored to its original settings after the function completes.
 * 
 * @param x A pointer to a variable where the detected key will be stored. If NULL, the key value is not stored.
 * @return int Returns 1 if a key was pressed, or 0 if no key was pressed.
 * 
 * @note This function is specific to Linux/Unix environments and requires <termios.h>, <fcntl.h>, and <unistd.h>.
 */
int PC_GetKey(void *x) 
{
    struct termios oldt, newt;
    int oldf;
    int ch;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) 
    {
        if (x != NULL) 
        {
            *(char *)x = (char)ch;
        }

        return 1;
    }

    return 0;
}

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

    OSSemPend(&DispStrSem, 0, OS_OPT_PEND_NON_BLOCKING, NULL, &err);                     /* Acquire semaphore to display string              */
    PC_Attribute(fgcolor, bgcolor);
    PutChar(0x1B);
    PutChar('[');
    PutDec(y);
    PutChar(';');
    PutDec(x);
    PutChar('H');
    PutChar(c);
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

void Print_to_Screen(const char *text_ptr)
{
    OS_ERR err;
    CPU_INT08U delay;
    CPU_INT08U symbols[80]= {' '};
    delay = -1;

    OSSemPend(&RandomSem, 0, OS_OPT_PEND_BLOCKING, DEF_NULL, &err);

    while(delay <= 1 || delay >= 3)
        delay = rand();
    OSSemPost(&RandomSem, OS_OPT_POST_1, &err);
    PC_DispClrScr(); 
    OSTimeDlyHMSM(0, 0, delay, 0, OS_OPT_TIME_HMSM_STRICT, &err);
    PC_DispStr(33, 15, text_ptr, COLOR_WHITE, COLOR_BLACK);
    OSTimeDlyHMSM(0, 0, delay, 0, OS_OPT_TIME_HMSM_STRICT, &err);
    PC_DispStr(33, 0, symbols, COLOR_RED, COLOR_BLACK);
}