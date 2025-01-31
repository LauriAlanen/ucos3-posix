/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                         uC/OS-III on POSIX
*
*
* Filename      : main.c
* Version       : V1.00
* Programmer(s) : DC
*                 SB
*********************************************************************************************************
*/

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

#define TASK_FIXED_PRINT_POS_PRIO 35
static OS_TCB App_TaskRandomPrintTCB;
static CPU_STK_SIZE App_TaskRandomPrintStk[TASK_STK_SIZE];

static OS_SEM RandPrintSem;

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void App_TaskStart(void *p_arg);
static void App_TaskRandomPrint(void *p_arg);


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This the main standard entry point.
*
* Note(s)     : none.
*********************************************************************************************************
*/

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

/*
*********************************************************************************************************
*                                          App_TaskStart()
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Notes       : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  App_TaskStart (void *p_arg)
{
    OS_ERR err;

    CPU_INT08U key;
    CPU_INT08U symbol;
    symbol = 'X';

    (void)p_arg;                                                /* See Note #1                                          */

    CPU_Init();
    Mem_Init();                                                 /* Initialize the Memory Management Module              */
    Math_Init();                                                /* Initialize the Mathematical Module                   */

    OS_CPU_SysTickInit();

    OSTaskCreate((OS_TCB     *)&App_TaskRandomPrintTCB,               /* Create the start task                                */
                 (CPU_CHAR   *)"App Print Random Position",
                 (OS_TASK_PTR ) App_TaskRandomPrint,
                 (void       *)&symbol,
                 (OS_PRIO     ) TASK_FIXED_PRINT_POS_PRIO,
                 (CPU_STK    *)&App_TaskRandomPrintStk[0],
                 (CPU_STK     )(TASK_STK_SIZE / 10u),
                 (CPU_STK_SIZE) TASK_STK_SIZE,
                 (OS_MSG_QTY  ) 0,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    while (DEF_TRUE) 
    {                
        OSSemPend(&RandPrintSem, 0, OS_OPT_PEND_BLOCKING, NULL, &err);
        if (PC_GetKey(&key)) 
        {             
            if (key == 0x1B) 
            {                             /* Yes, see if it's the ESCAPE key          */
                exit(0);  	                           /* End program                              */
            }
        }
        OSTimeDlyHMSM(0u, 0u, 1u, 0u,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
        OSSemPost(&RandPrintSem, OS_OPT_POST_1, &err);
    }
}

void App_TaskRandomPrint(void *p_arg)
{
    OS_ERR err;

    CPU_INT08U  x;
    CPU_INT08U  y;  

    OSSemCreate(&DispStrSem, "DispStr Semaphore", 1, &err);
    OSSemCreate(&RandPrintSem, "Random Print Semaphore", 1, &err);

    if (err != OS_ERR_NONE) 
    {
        printf("Semaphore creation failed: %d\n", err);
    }

    srand((unsigned int)pthread_self());

    OSSemPend(&RandPrintSem, 0, OS_OPT_PEND_BLOCKING, NULL, &err);
    for (int i = 0; i < 10; i++)
    {
        #ifdef RANDOM_POS
        x = rand() % 80;
        y = rand() % 25;
        #else
        x = 15;
        y = 15;   
        #endif          
        PC_DispStr(50, 0, "Lauri Alanen", COLOR_BLACK, COLOR_LIGHT_GRAY);

        PC_DispChar(x, y, *((CPU_INT08U *)p_arg), COLOR_BLACK, COLOR_LIGHT_GRAY);

        OSTimeDlyHMSM(0u, 0u, 1u, 0u,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
        PC_DispClrScr();
    }
    OSSemPost(&RandPrintSem, OS_OPT_POST_1, &err);

    OSTaskDel((OS_TCB*)0, &err);
}