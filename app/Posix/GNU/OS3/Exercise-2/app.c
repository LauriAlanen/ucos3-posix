/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
#define TASK_STK_SIZE 4096       /* Size of each task's stacks (# of WORDs)            */
#define RANDOM_POS

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static OS_TCB App_TaskStartTCB;
static CPU_STK_SIZE App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];

#define TASK_A_PRIORITY 35
static OS_TCB App_TaskATCB;
static CPU_STK_SIZE App_TaskASTK[TASK_STK_SIZE];

#define TASK_B_PRIORITY 33
static OS_TCB App_TaskBTCB;
static CPU_STK_SIZE App_TaskBSTK[TASK_STK_SIZE];

static OS_SEM RandPrintSem;


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void App_TaskStart(void *p_arg);
static void App_TaskA(void *p_arg);
static void App_TaskB(void *p_arg);

int  main (void)
{
    OS_ERR err;

    PC_DispClrScr();      /* Clear the screen                         */

    OSInit(&err);                                               /* Initialize "uC/OS-III, The Real-Time Kernel"         */

    OSTaskCreate((OS_TCB     *)&App_TaskStartTCB,               /* Create the start task                                */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR ) App_TaskStart,
                 (void       *) 0,
                 (OS_PRIO     ) APP_CFG_TASK_START_PRIO,
                 (CPU_STK    *)&App_TaskStartStk[0],
                 (CPU_STK     )(APP_CFG_TASK_START_STK_SIZE / 10u),
                 (CPU_STK_SIZE) APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 0,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */

    /* Should Never Get Here							    */
    while(DEF_ON) {};
}

static  void  App_TaskStart (void *p_arg)
{
    OS_ERR err;

    (void)p_arg;                                                /* See Note #1                                          */

    CPU_Init();
    Mem_Init();                                                 /* Initialize the Memory Management Module              */
    Math_Init();                                                /* Initialize the Mathematical Module                   */
    OS_CPU_SysTickInit();

    OSTaskCreate((OS_TCB     *)&App_TaskATCB,
                 (CPU_CHAR   *)"App Task A",
                 (OS_TASK_PTR ) App_TaskA,
                 (void       *) 0,
                 (OS_PRIO     ) TASK_A_PRIORITY,
                 (CPU_STK    *)&App_TaskASTK[0],
                 (CPU_STK     )(TASK_STK_SIZE / 10u),
                 (CPU_STK_SIZE) TASK_STK_SIZE,
                 (OS_MSG_QTY  ) 0,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    OSTaskCreate((OS_TCB     *)&App_TaskBTCB,
                 (CPU_CHAR   *)"App Task B",
                 (OS_TASK_PTR ) App_TaskB,
                 (void       *) 0,
                 (OS_PRIO     ) TASK_B_PRIORITY,
                 (CPU_STK    *)&App_TaskBSTK[0],
                 (CPU_STK     )(TASK_STK_SIZE / 10u),
                 (CPU_STK_SIZE) TASK_STK_SIZE,
                 (OS_MSG_QTY  ) 0,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);


    while (DEF_TRUE) 
    {
        OSTimeDlyHMSM(0u, 0u, 1u, 0u,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
    }
}

static void App_TaskA(void *p_arg)
{
    OS_ERR err;
    const char *msg = "Hello from task A";

    while (DEF_ON)
    {
        Print_to_Screen(msg);
        OSTimeDlyHMSM(0u, 0u, 1u, 0u,
                OS_OPT_TIME_HMSM_STRICT,
                &err);
    }
}

static void App_TaskB(void *p_arg)
{
    OS_ERR err;
    const char *msg = "Hello from task B";

    while (DEF_ON)
    {
        Print_to_Screen(msg);
        OSTimeDlyHMSM(0u, 0u, 1u, 0u,
                OS_OPT_TIME_HMSM_STRICT,
                &err);
    }
}