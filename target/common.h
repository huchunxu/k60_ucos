/*
 * =====================================================================================
 *        COPYRIGHT NOTICE
 *        Copyright (c) 2012  HUST-Renesas Lab
 *        ALL rights reserved.
 *//**        
 *        @file     common.h
 *
 *        @brief    common functions for k60
 *
 *        @version  0.1
 *        @date     2011/12/30 16:39:45
 *
 *        @author   Ren Wei , renweihust@gmail.com
 *//* ==================================================================================
 *  @0.1    Ren Wei 2011/12/30  create orignal file
 * =====================================================================================
 */

#ifndef  COMMON_H
#define  COMMON_H

#include "MK60N512VMD100.h"   /* 瀵勫瓨鍣ㄦ槧鍍忓ご鏂囦欢 */
#include "t_stddef.h"

#define BSET(bit,Register)  ((Register)|= (1<<(bit)))    /**< 璁剧疆瀵勫瓨鍣ㄤ腑鏌愪竴浣嶄负1 */
#define BCLR(bit,Register)  ((Register) &= ~(1<<(bit)))  /**< 璁剧疆瀵勫瓨鍣ㄤ腑鏌愪竴浣嶄负0 */
#define BGET(bit,Register)  (((Register) >> (bit)) & 1)  /**< 寰楀埌瀵勫瓨鍣ㄤ腑鏌愪竴浣嶇姸鎬�*/

#define EnableInterrupts  asm(" CPSIE i");    /**< 寮��涓柇 */
#define DisableInterrupts asm(" CPSID i");    /**< 鍏虫�涓柇 */


#define Inline static __inline__
#define Asm asm __volatile__

extern void stop(void);
extern void wait(void);
#endif   /* ----- #ifndef COMMON_H  ----- */


