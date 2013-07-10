/*
 * =====================================================================================
 *        COPYRIGHT NOTICE
 *        Copyright (c) 2012  HUST-Renesas Lab
 *        ALL rights reserved.
 *//**        
 *        @file     sysinit.c
 *
 *        @brief   
 *
 *        @version  0.1
 *        @date     2012/3/9 11:50:06
 *
 *        @author   Ren Wei , renweihust@gmail.com
 *//* ==================================================================================
 *  @0.1    Ren Wei 2012/3/9    create orignal file
 * =====================================================================================
 */

#include "sysinit.h"


static void trace_clk_init(void);
static void fb_clk_init(void);
static int32_t pll_init(uint8_t clk_option, uint8_t crystal_val);
static void set_sys_dividers(uint8_t mcg, uint8_t sys_bus, uint8_t flex_bus, uint8_t flash);


/**
 * @brief 纭欢鍒濆鍖栵紝璋冪敤璇ュ嚱鏁颁箣鍓嶅簲鍑嗗濂藉爢鏍�
 */
void hardware_init_hook(void) {
    /* Disable the WDOG module */
    /* WDOG_UNLOCK: WDOGUNLOCK=0xC520 */
    WDOG_UNLOCK = (uint16_t)0xC520U;     /* Key 1 */
    /* WDOG_UNLOCK : WDOGUNLOCK=0xD928 */
    WDOG_UNLOCK  = (uint16_t)0xD928U;    /* Key 2 */
    /* WDOG_STCTRLH: ??=0,DISTESTWDOG=0,BYTESEL=0,TESTSEL=0,TESTWDOG=0,??=0,STNDBYEN=1,
     * WAITEN=1,STOPEN=1,DBGEN=0,ALLOWUPDATE=1,WINEN=0,IRQRSTEN=0,CLKSRC=1,WDOGEN=0 
     */
    WDOG_STCTRLH = (uint16_t)0x01D2U;

    /* 浣胯兘IO绔彛鏃堕挓 */
    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
                | SIM_SCGC5_PORTB_MASK
                | SIM_SCGC5_PORTC_MASK
                | SIM_SCGC5_PORTD_MASK
                | SIM_SCGC5_PORTE_MASK);

    /* 鍒濆鍖栨椂閽�*/
    pll_init(PLL_100MHZ, XTAL_8MHZ);

    /* 璋冭瘯鏃堕挓 */
    trace_clk_init();

    /* flexbus鏃堕挓浣胯兘 */
    fb_clk_init();
}

/**
 * @brief 鍒濆鍖朠LL鏃堕挓锛岀郴缁熷惎鍔ㄦ椂锛岀敱FLL鎻愪緵鏃堕挓锛屽闇�鍒囨崲鍒扮敱PLL鎻�
 * 渚涙椂閽燂紝闇�鍐欏垏鎹㈠埌FBE妯″紡锛堝仠姝LL锛岀敱澶栭儴鏃堕挓鎻愪緵绯荤粺鏃堕挓锛夛紝鐒跺悗
 * 鍒囨崲鍒癙BE妯″紡锛堜娇鑳絇LL锛夛紝鏈�悗鍒囨崲鍒癙EE妯″紡锛堢敱PLL鎻愪緵绯荤粺鏃堕挓锛�
 *
 * @param clk_option    鏃堕挓閫夐」锛�0 Mhz,48 Mhz,96 Mhz, 100 Mhz
 * @param crystal_val   鏅舵尟閫夐」锛�-32 Mhz
 *
 * @return >0 绯荤粺鏃堕挓棰戠巼 <0 澶辫触
 */
static int32_t pll_init(uint8_t clk_option, uint8_t crystal_val)
{
    int32_t pll_freq;
    /* 鍙傛暟妫�煡 */
    if (clk_option > PLL_100MHZ) 
        return -1;
    if (crystal_val > XTAL_32MHZ)
        return -1;
    
    /* SIM_SOPT2: PLLFLLSEL=0 */
    /* Select FLL as a clock source for various peripherals */
    SIM_SOPT2 &= (uint32_t)~0x00010000UL; 
    /* SIM_SOPT1: OSC32KSEL=0 */
    /* System oscillator drives 32 kHz clock for various peripherals */
    SIM_SOPT1 &= (uint32_t)~0x00080000UL;
       
    /* 浣胯兘澶栭儴鏅舵尟锛屽鏋滀娇鐢ㄤ簡鏃犳簮鏅舵尟鍙兘杩橀渶瑕佽瀹氱數瀹瑰尮閰�*/
    OSC_CR = (uint8_t)0x80U; 
    /* SIM_SOPT2: MCGCLKSEL=0 */
    SIM_SOPT2 &= (uint32_t)~0x01UL;  
    /* Very high frequency range selected for the crystal oscillator */
    MCG_C2 = MCG_C2_RANGE(2);
    
    /* Select ext oscillator, reference divider and clear IREFS to start ext osc */
    /* CLKS=2, select the external clock source */
    /* FRDIV=3, set the FLL ref divider to keep the ref clock in range */
    /* (even if FLL is not being used) 8 MHz / 256 = 31.25 kHz */
    /* IREFS=0, select the external clock */
    /* IRCLKEN=0, disable IRCLK (can enable it if desired) */
    /* IREFSTEN=0, disable IRC in stop mode (can keep it enabled in stop if desired) */
    MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);
    /* MCG_C4: DMX32=0,DRST_DRS=0 */
    MCG_C4 &= (uint8_t)~(uint8_t)0xE0U;                           
    /* MCG_C5: ??=0,PLLCLKEN=0,PLLSTEN=0,PRDIV=1 */
    MCG_C5 = (uint8_t)0x01U;                             
    /* MCG_C6: LOLIE=0,PLLS=0,CME=0,VDIV=0 */
    MCG_C6 = (uint8_t)0x00U; 

    /* 绛夊緟鏅舵尟绋冲畾    */
    while (!(MCG_S & MCG_S_OSCINIT_MASK));  
    /* 绛夊緟鍙傝�鏃堕挓鐘舵�浣嶆竻闆�*/
    while (MCG_S & MCG_S_IREFST_MASK);
    /* 绛夊緟鏃堕挓鐘舵�浣嶆樉绀烘椂閽熸簮鏉ヨ嚜澶栭儴鍙傝�鏃堕挓 */
    while ((MCG_S & 0x0CU) != 0x08U); 
    /* FBE mode */
        
    /* MCG_C6: LOLIE=0,PLLS=1,CME=0,VDIV=0 */
    MCG_C6 = (uint8_t)0x40U;
    /* Wait until external reference clock is selected as MCG output */ 
    while((MCG_S & 0x0CU) != 0x08U);
    /* PBE mode */

    /* switch to PEE mode */    
    /* 閰嶇疆PLL鍒嗛鍣ㄦ潵鍖归厤鎵�敤鐨勬櫠鎸�PLL鏉ユ簮涓� Mhz */
    MCG_C5 = MCG_C5_PRDIV(crystal_val);  

    /* 閫夋嫨PLL VCO鍒嗛鍣紝绯荤粺鏃堕挓鍒嗛鍣ㄥ彇鍐充簬鏃堕挓閫夐」 */
    switch (clk_option) {
        case PLL_48MHZ:
            /* MCG=PLL, core = MCG, bus = MCG, FlexBus = MCG, Flash clock= MCG/2 */
            set_sys_dividers(0,0,0,1);
            /* 璁剧疆VCO鍒嗛鍣紝浣胯兘PLL涓�8MHz, LOLIE=0, PLLS=1, CME=0, VDIV=0 */
            MCG_C6 = MCG_C6_PLLS_MASK; /* VDIV = 0 (x24) */
            pll_freq = 48000000;
            break;
        case PLL_50MHZ:
            /* MCG=PLL, core = MCG, bus = MCG, FlexBus = MCG, Flash clock= MCG/2 */
            set_sys_dividers(0,0,0,1);
            /* 璁剧疆VCO鍒嗛鍣紝浣胯兘PLL涓�0MHz, LOLIE=0, PLLS=1, CME=0, VDIV=1 */
            MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(1); /* VDIV = 1 (x25) */
            pll_freq = 50000000;
            break;
        case PLL_96MHZ:
            /* MCG=PLL, core = MCG, bus = MCG/2, FlexBus = MCG/2, Flash clock= MCG/4 */
            set_sys_dividers(0,1,1,3);
            /* 璁剧疆VCO鍒嗛鍣紝浣胯兘PLL涓�6MHz, LOLIE=0, PLLS=1, CME=0, VDIV=24 */
            MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(24); /* VDIV = 24 (x48) */
            pll_freq = 96000000;
            break;
        case PLL_100MHZ:
            /* MCG=PLL, core = MCG, bus = MCG/2, FlexBus = MCG/2, Flash clock= MCG/4 */
            set_sys_dividers(0,1,1,3);
            /* 璁剧疆VCO鍒嗛鍣紝浣胯兘PLL涓�00MHz, LOLIE=0, PLLS=1, CME=0, VDIV=26 */
            MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(26); /* VDIV = 26 (x50) */
            pll_freq = 100000000;
            break;
        default:
            break;
    }
    /* wait for PLL status bit to set */
    while (!(MCG_S & MCG_S_PLLST_MASK));
    /* Wait for LOCK bit to set */
    while (!(MCG_S & MCG_S_LOCK_MASK)); 

    /* 閫氳繃娓呴浂CLKS浣嶆潵杩涘叆PEE妯″紡 */
    /* CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0 */
    MCG_C1 &= ~MCG_C1_CLKS_MASK;
    
    /* 绛夊緟鏃堕挓鐘舵�浣嶆洿鏂�*/
    /* Wait until output of the PLL is selected */
    while((MCG_S & 0x0CU) != 0x0CU);  
  
    /* 寮�杩涘叆PEE妯″紡 */
    
    return pll_freq;
}

/**
 * @brief 璁剧疆绯荤郴缁熷垎棰戝櫒锛屾鍑芥暟蹇呴』鏀惧湪RAM閲屾墽琛岋紝鍚﹀垯浼氫骇鐢熼敊璇痚2448銆�
 *        褰揊LASH鏃堕挓鍒嗛鏀瑰彉鏃讹紝蹇呴』绂佹FLASH鐨勯鍙栧姛鑳姐�鍦ㄦ椂閽熷垎棰戞敼鍙樹箣鍚庯紝
 *        蹇呴』寤舵椂涓�皬娈垫椂闂存墠鍙互浠庢柊浣胯兘棰勫彇鍔熻兘銆�
 *
 * @param mcg       鍐呮牳鏃堕挓
 * @param sys_bus   绯荤粺鎬荤嚎鏃堕挓
 * @param flex_bus  澶栭儴鎬荤嚎鏃堕挓
 * @param flash     鍐呴儴flash鏃堕挓
 */
static void set_sys_dividers(uint8_t mcg, uint8_t sys_bus, uint8_t flex_bus, uint8_t flash)
{
    uint32_t temp_reg;
    volatile uint8_t i;
    /* 淇濆瓨FMC_PFAPR褰撳墠鐨勫� */
    temp_reg = FMC_PFAPR;
    
    /* 閫氳繃M&PFD缃綅M0PFD鏉ョ姝㈤鍙栧姛鑳�*/
    FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK | FMC_PFAPR_M5PFD_MASK
             | FMC_PFAPR_M4PFD_MASK | FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK
             | FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;
    
    /* 缁欐椂閽熷垎棰戝櫒璁剧疆鏈熸湜鍊�  */
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(mcg) | SIM_CLKDIV1_OUTDIV2(sys_bus) 
              | SIM_CLKDIV1_OUTDIV3(flex_bus) | SIM_CLKDIV1_OUTDIV4(flash);
    
    /* 绛夊緟鍒嗛鍣ㄦ敼鍙�*/
    for (i = 0 ; i < flash ; i++);
    
    /* 浠庢柊瀛楩MC_PFAPR鐨勫師濮嬪� */
    FMC_PFAPR = temp_reg;

    return;
}


/**
 * @brief 璺熻釜鏃堕挓鍒濆鍖�
 */
static void trace_clk_init(void)
{
    /* 璁剧疆璺熻釜鏃堕挓涓哄唴鏍告椂閽�*/
    SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;
    /* 鍦≒TA6寮曡剼涓婁娇鑳絋RACE_CLKOU鍔熻兘 */
    PORTA_PCR6 = ( PORT_PCR_MUX(0x7));
}


/**
 * @brief FlexBus鏃堕挓鍒濆鍖�
 */
static void fb_clk_init(void)
{
    /* 浣胯兘FlexBus妯″潡鏃堕挓 */
    SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
    /* 鍦≒TA6寮曡剼涓婁娇鑳紽B_CLKOUT鍔熻兘 */
    PORTC_PCR3 = ( PORT_PCR_MUX(0x5));
}
