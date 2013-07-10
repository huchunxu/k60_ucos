/*
 * =====================================================================================
 *                                 COPYRIGHT NOTICE
 *                     Copyright (c) 2012  HUST-Renesas Lab
 *                               ALL rights reserved.
 *//**
 *        @file     test.c
 *
 *        @brief    ucos移植测试
 *
 *        @version  0.1
 *        @date     2012/12/29    15:17
 *
 *        @author   Hu Chunxu, huchunxu@hust.edu.cn
 *//* ==================================================================================
 *  @0.1    Hu Chunxu  2012/12/29  create orignal file
 * =====================================================================================
 */
#include <stdio.h>

#include  "ucos_ii.h"
#include  "os_cfg.h"
#include "os_cpu.h"
#include "exception.h"
#include "test.h"

/* 设置任务堆栈 */
static  OS_STK       task1_stk[APP_TASK1_STK_SIZE];
static  OS_STK       task2_stk[APP_TASK2_STK_SIZE];

void hardware_init(void);     //硬件初始化
static void task1 (void *p_arg);
static void task2 (void *p_arg);

int main (void)
{
	hardware_init();

	OSInit();

	OSTaskCreate(task1,NULL,&task1_stk[APP_TASK1_STK_SIZE - 1],1);
	OSTaskCreate(task2,NULL,&task2_stk[APP_TASK2_STK_SIZE - 1],2);

	OSStart();

    return 0;
}

static void task1(void *p_arg)
{
	int i = 0;

	while (1)
	{
		i++;

		OSTimeDlyHMSM(0, 0, 1, 0);             //delay
	}
}

static void task2(void *p_arg)
{
	int i = 0;

	while (1)
	{
		i++;

		OSTimeDlyHMSM(0, 0, 0, 100);             //delay
	}
}

void hardware_init(void)
{
    exc_init(); /* 设置中断 */
    OS_CPU_SysTickInit(100000); /* 1ms tick */

	/* install exception handler */
	exc_install(INT_PendableSrvReq, OS_CPU_PendSVHandler);
	exc_set_pri(INT_PendableSrvReq, 0);
}
