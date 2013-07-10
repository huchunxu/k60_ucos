/*
 * =====================================================================================
 *                                 COPYRIGHT NOTICE
 *                     Copyright (c) 2012  HUST-Renesas Lab
 *                               ALL rights reserved.
 *//**
 *        @file     test.h
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
#ifndef __TEST_H__
#define __TEST_H__

//define the priority of task
#define  APP_CFG_TASK_START_PRIO                           2u
#define  APP_TASK1_PRIO                                    6u
#define  APP_TASK2_PRIO                                    5u

//define the STK size of task
#define  APP_CFG_TASK_START_STK_SIZE           256u
#define  APP_TASK1_STK_SIZE                    128u
#define  APP_TASK2_STK_SIZE                    128u


#endif
