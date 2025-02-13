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
static CPU_STK App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];

#define TASK_GENERIC 32
static CPU_STK App_TaskGenericPrintStk_A[TASK_STK_SIZE];
static CPU_STK App_TaskGenericPrintStk_B[TASK_STK_SIZE];
static OS_TCB App_TaskGenericPrintTCB_A;
static OS_TCB App_TaskGenericPrintTCB_B;


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void App_TaskStart(void *p_arg);
static void App_TaskGenericPrint(void *p_arg);

OS_SEM PrintSem;

int  main (void)
{
    OS_ERR err;

    PC_DispClrScr();      /* Clear the screen                         */

    OSInit(&err);                                               /* Initialize "uC/OS-III, The Real-Time Kernel"         */

    OSSemCreate(&DispStrSem, "DispStrSem", 1, &err);
    OSSemCreate(&RandomSem, "RandomSem", 1, &err);
    OSSemCreate(&PrintSem, "PrintSem", 1, &err);

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

    OSTaskCreate((OS_TCB     *)&App_TaskGenericPrintTCB_A,
                 (CPU_CHAR   *)"App Task Generic A",
                 (OS_TASK_PTR ) App_TaskGenericPrint,
                 (void       *) "Hi, task A here",
                 (OS_PRIO     ) TASK_GENERIC,
                 (CPU_STK    *)&App_TaskGenericPrintStk_A[0],
                 (CPU_STK     )(TASK_STK_SIZE / 10u),
                 (CPU_STK_SIZE) TASK_STK_SIZE,
                 (OS_MSG_QTY  ) 0,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    OSTaskCreate((OS_TCB     *)&App_TaskGenericPrintTCB_B,
                 (CPU_CHAR   *)"App Task Generic B",
                 (OS_TASK_PTR ) App_TaskGenericPrint,
                 (void       *) "Hello from task B",
                 (OS_PRIO     ) TASK_GENERIC - 1,
                 (CPU_STK    *)&App_TaskGenericPrintStk_B[0],
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


static void App_TaskGenericPrint(void *p_arg)
{
    OS_ERR err;

    const char* msg = (char*)(p_arg); 
    
    while (DEF_ON)
    {
        OSSemPend(&PrintSem, 0, OS_OPT_PEND_BLOCKING, DEF_NULL, &err);
        Print_to_Screen(msg);
        OSSemPost(&PrintSem, OS_OPT_POST_1, &err);
        OSTimeDlyHMSM(0u, 0u, 1u, 0u,
            OS_OPT_TIME_HMSM_STRICT,
            &err);
        }
}
