#ifndef _OTU_4U_
#define _OTU_4U_

/**********Definition for usart************************/
#define DI_GPIO      GPIOA
#define DI_Pin       GPIO_Pin_9

#define RO_GPIO      GPIOA
#define RO_Pin       GPIO_Pin_10

#define nRE_GPIO      GPIOA
#define nRE_Pin       GPIO_Pin_11

#define DE_GPIO      GPIOA
#define DE_Pin       GPIO_Pin_12

#define USART        USART1
#define USART_CLK   RCC_APB2Periph_USART1

#define nExist_GPIO      GPIOA
#define nExist_Pin       GPIO_Pin_15

#define INT_GPIO      GPIOC
#define INT_Pin       GPIO_Pin_10

#define ADD0_GPIO      GPIOC
#define ADD0_Pin       GPIO_Pin_11

#define ADD1_GPIO      GPIOD
#define ADD1_Pin       GPIO_Pin_0

#define ADD2_GPIO      GPIOC
#define ADD2_Pin       GPIO_Pin_12

#define ADD3_GPIO      GPIOD
#define ADD3_Pin       GPIO_Pin_1




/**********Definition for CS4323 signals************************/
#define CS4323_SPI_SCK_GPIO GPIOA
#define CS4323_SPI_SCK_Pin GPIO_Pin_5

#define CS4323_SPI_MISO_GPIO GPIOA
#define CS4323_SPI_MISO_Pin GPIO_Pin_6

#define CS4323_SPI_MOSI_GPIO GPIOA
#define CS4323_SPI_MOSI_Pin GPIO_Pin_7

#define CS4323_SPI SPI1
#define CS4323_SPI_CLK RCC_APB2Periph_SPI1


#define CS4323_SPI_PHY0_GPIO GPIOB
#define CS4323_SPI_PHY0_Pin GPIO_Pin_0

#define CS4323_SPI_PHY1_GPIO GPIOE
#define CS4323_SPI_PHY1_Pin GPIO_Pin_14

#define CS4323_SPI_PHY2_GPIO GPIOC
#define CS4323_SPI_PHY2_Pin GPIO_Pin_15

#define CS4323_SPI_PHY3_GPIO GPIOB
#define CS4323_SPI_PHY3_Pin GPIO_Pin_2

#define CS4323_GPIO1_0_GPIO GPIOE
#define CS4323_GPIO1_0_Pin GPIO_Pin_10

#define CS4323_GPIO1_1_GPIO GPIOE
#define CS4323_GPIO1_1_Pin GPIO_Pin_8

#define CS4323_GPIO1_2_GPIO GPIOE
#define CS4323_GPIO1_2_Pin GPIO_Pin_9

#define CS4323_GPIO1_3_GPIO GPIOE
#define CS4323_GPIO1_3_Pin GPIO_Pin_7

#define CS4323_GPIO2_0_GPIO GPIOE
#define CS4323_GPIO2_0_Pin GPIO_Pin_12

#define CS4323_GPIO2_1_GPIO GPIOE
#define CS4323_GPIO2_1_Pin GPIO_Pin_11

#define CS4323_GPIO2_2_GPIO GPIOE
#define CS4323_GPIO2_2_Pin GPIO_Pin_13

#define CS4323_GPIO2_3_GPIO GPIOB
#define CS4323_GPIO2_3_Pin GPIO_Pin_1


#define CS4323_GPIO_INT_GPIO GPIOB
#define CS4323_GPIO_INT_Pin GPIO_Pin_12

#define CS4323_nRESET_GPIO GPIOE
#define CS4323_nRESET_Pin GPIO_Pin_15


/**********Definition for CLK CONTROL and EEPROM SIGNALS************************/
#define EEP_SI514_SCL_GPIO GPIOB
#define EEP_SI514_SCL_Pin GPIO_Pin_10

#define EEP_SI514_SDA_GPIO GPIOB
#define EEP_SI514_SDA_Pin GPIO_Pin_11

#define EEP_SI514_SCL_CLK RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO
#define EEP_SI514_SDA_CLK RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO

#define EEP_SI514_I2C_DMA_TX_IRQn DMA1_Channel4_IRQn
#define EEP_SI514_I2C_DMA_RX_IRQn DMA1_Channel5_IRQn

#define EEP_SI514_I2C_TX_DMA_Channel DMA1_Channel4
#define EEP_SI514_I2C_RX_DMA_Channel DMA1_Channel5

#define EEP_SI514_I2C_DMA_BaseAddr  0x40005810

#define EEP_SI514_I2C_TX_DMA_FLAG_TC DMA1_FLAG_TC4
#define EEP_SI514_I2C_RX_DMA_FLAG_TC DMA1_FLAG_TC5

#define EEP_SI514_I2C_TX_DMA_FLAG_GL DMA1_FLAG_GL4
#define EEP_SI514_I2C_RX_DMA_FLAG_GL DMA1_FLAG_GL5



#define EEP_SI514_I2C I2C2
#define EEP_SI514_I2C_CLK RCC_APB1Periph_I2C2
#define EEP_SI514_DMA_CLK RCC_AHBPeriph_DMA1

#define CLK_SEL0_GPIO GPIOC
#define CLK_SEL0_Pin GPIO_Pin_4

#define CLK_SEL1_GPIO GPIOA
#define CLK_SEL1_Pin GPIO_Pin_1

#define CLK_EN0_GPIO GPIOC
#define CLK_EN0_Pin GPIO_Pin_3

#define CLK_EN1_GPIO GPIOA
#define CLK_EN1_Pin GPIO_Pin_0

/**********Definition for XFP SIGNALS************************/
#define XFP_SCL_GPIO GPIOB
#define XFP_SCL_Pin GPIO_Pin_6

#define XFP_SDA_GPIO GPIOB
#define XFP_SDA_Pin GPIO_Pin_7

#define XFP_SCL_CLK RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO
#define XFP_SDA_CLK RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO

#define XFP_I2C_DMA_TX_IRQn DMA1_Channel6_IRQn
#define XFP_I2C_DMA_RX_IRQn DMA1_Channel7_IRQn

#define XFP_I2C_TX_DMA_Channel DMA1_Channel6
#define XFP_I2C_RX_DMA_Channel DMA1_Channel7

#define XFP_I2C_DMA_BaseAddr  0x40005410

#define XFP_I2C_TX_DMA_FLAG_TC DMA1_FLAG_TC6
#define XFP_I2C_RX_DMA_FLAG_TC DMA1_FLAG_TC7

#define XFP_I2C_TX_DMA_FLAG_GL DMA1_FLAG_GL6
#define XFP_I2C_RX_DMA_FLAG_GL DMA1_FLAG_GL7


#define XFP_I2C I2C1
#define XFP_I2C_CLK RCC_APB1Periph_I2C1
#define XFP_DMA_CLK RCC_AHBPeriph_DMA1

#define XFP1_SEL_GPIO GPIOC
#define XFP1_SEL_Pin GPIO_Pin_13

#define XFP1_nINTER_GPIO GPIOC
#define XFP1_nINTER_Pin GPIO_Pin_14

#define XFP1_TX_DIS_GPIO GPIOC
#define XFP1_TX_DIS_Pin GPIO_Pin_15

#define XFP1_nExist_GPIO GPIOC
#define XFP1_nExist_Pin GPIO_Pin_0

#define XFP1_MD_NR_GPIO GPIOC
#define XFP1_MD_NR_Pin GPIO_Pin_1

#define XFP1_RX_LOS_GPIO GPIOC
#define XFP1_RX_LOS_Pin GPIO_Pin_2


#define XFP2_SEL_GPIO GPIOE
#define XFP2_SEL_Pin GPIO_Pin_1

#define XFP2_nINTER_GPIO GPIOE
#define XFP2_nINTER_Pin GPIO_Pin_2

#define XFP2_TX_DIS_GPIO GPIOE
#define XFP2_TX_DIS_Pin GPIO_Pin_3

#define XFP2_nExist_GPIO GPIOE
#define XFP2_nExist_Pin GPIO_Pin_4

#define XFP2_MD_NR_GPIO GPIOE
#define XFP2_MD_NR_Pin GPIO_Pin_5

#define XFP2_RX_LOS_GPIO GPIOE
#define XFP2_RX_LOS_Pin GPIO_Pin_6


#define XFP3_SEL_GPIO GPIOB
#define XFP3_SEL_Pin GPIO_Pin_3

#define XFP3_nINTER_GPIO GPIOB
#define XFP3_nINTER_Pin GPIO_Pin_4

#define XFP3_TX_DIS_GPIO GPIOB
#define XFP3_TX_DIS_Pin GPIO_Pin_5

#define XFP3_nExist_GPIO GPIOB
#define XFP3_nExist_Pin GPIO_Pin_8

#define XFP3_MD_NR_GPIO GPIOB
#define XFP3_MD_NR_Pin GPIO_Pin_9

#define XFP3_RX_LOS_GPIO GPIOE
#define XFP3_RX_LOS_Pin GPIO_Pin_0


#define XFP4_SEL_GPIO GPIOD
#define XFP4_SEL_Pin GPIO_Pin_2

#define XFP4_nINTER_GPIO GPIOD
#define XFP4_nINTER_Pin GPIO_Pin_3

#define XFP4_TX_DIS_GPIO GPIOD
#define XFP4_TX_DIS_Pin GPIO_Pin_4

#define XFP4_nExist_GPIO GPIOD
#define XFP4_nExist_Pin GPIO_Pin_5

#define XFP4_MD_NR_GPIO GPIOD
#define XFP4_MD_NR_Pin GPIO_Pin_6

#define XFP4_RX_LOS_GPIO GPIOD
#define XFP4_RX_LOS_Pin GPIO_Pin_7

/**********Definition for POWER CONTROL and EEPROM SIGNALS************************/
#define EN_1V_GPIO GPIOD
#define EN_1V_Pin GPIO_Pin_13

#define EN_1_8V_GPIO GPIOD
#define EN_1_8V_Pin GPIO_Pin_12

#define EN_2_5V_GPIO GPIOD
#define EN_2_5V_Pin GPIO_Pin_11


/***********************led signals**************************************/
#define LED_L1_GPIO      GPIOB
#define LED_L1_Pin       GPIO_Pin_15


#define LED_C1_GPIO      GPIOB
#define LED_C1_Pin       GPIO_Pin_13


#define LED_L2_GPIO      GPIOD
#define LED_L2_Pin       GPIO_Pin_8


#define LED_C2_GPIO      GPIOB
#define LED_C2_Pin       GPIO_Pin_14



#define LED_Alarm_GREEN_GPIO      GPIOD
#define LED_Alarm_GREEN_Pin       GPIO_Pin_10


#define LED_Alarm_RED_GPIO      GPIOD
#define LED_Alarm_RED_Pin       GPIO_Pin_9

#define LED_Mode_GREEN_GPIO LED_Alarm_GREEN_GPIO
#define LED_Mode_GREEN_Pin LED_Alarm_GREEN_Pin


#define LED_Mode_RED_GPIO LED_Alarm_RED_GPIO
#define LED_Mode_RED_Pin LED_Alarm_RED_Pin

/**********Definition for  Miscellaneous SIGNALS************************/
#define SPEED_MOD1_GPIO GPIOA
#define SPEED_MOD1_Pin GPIO_Pin_8

#define SPEED_MOD2_GPIO GPIOC
#define SPEED_MOD2_Pin GPIO_Pin_9

#define SPEED_MOD3_GPIO GPIOC
#define SPEED_MOD3_Pin GPIO_Pin_8

#define SPEED_MOD4_GPIO GPIOC
#define SPEED_MOD4_Pin GPIO_Pin_7

#define SPEED_MOD5_GPIO GPIOC
#define SPEED_MOD5_Pin GPIO_Pin_6

#define SPEED_MOD6_GPIO GPIOD
#define SPEED_MOD6_Pin GPIO_Pin_15

#define SPEED_MOD7_GPIO GPIOD
#define SPEED_MOD7_Pin GPIO_Pin_14

#define SPEED_MOD8_GPIO GPIOA
#define SPEED_MOD8_Pin GPIO_Pin_4




#define flash_addr 0x801ff00 //0x801fc00   
#define cs4323_phy0_addr 0x78
#define cs4323_phy1_addr 0x7a
#define cs4323_phy2_addr 0x7c
#define cs4323_phy3_addr 0x7e

/**********Definition for  variable types************************/

typedef enum
{CLKSource1=0,CLKSource2=!CLKSource1} CLKSourceType;

typedef enum
{PHY0=0,PHY1,PHY2,PHY3,AllPHY} CS4323_PHY_Type;

typedef enum
{XFP1=0,XFP2,XFP3,XFP4} XFP_Type;

typedef enum
{LED_ERR=0,LED_L1,LED_C1,LED_L2,LED_C2} LED_Type;
/*********************************AlarmID*******************************************/
typedef enum
{
    Rx_Power_AlarmID=31,
    Rx_Power_ClearAlarmID=32,

    Tx_Power_AlarmID=33,
    Tx_Power_ClearAlarmID=34,

    Bias_AlarmID=35,
    Bias_ClearAlarmID=36,

    Voltage_AlarmID=37,
    Voltage_ClearAlarmID=38,

    Tem_AlarmID=39,
    Tem_ClearAlarmID=40,

    SFP_XFP_Unplugged_AlarmID=71,
    SFP_XFP_Unplugged_ClearAlarmID=72,
} AlarmID_t;

typedef enum
{MODE_Normal=1,MODE_Protect} CS4323_Mode_Type;

typedef enum
{RATE_10G=10312,RATE_8_5G=850,RATE_6_25G=625,RATE_4_25G=425,RATE_2_xG=250} RATE_Type;

typedef enum
{Port1=0,Port2,Port3,Port4} Port_Type;

typedef enum
{
    CMD_MODE_Normal=1,
    CMD_MODE_Protect,
    CMD_Loopback,
    CMD_PBERT,CMD_FEC,
    CMD_Fix,   //设置固定信息
    CMD_OTU_Property,//设置单元盘属性
    CMD_Threshold,//设置光模块阈值
    CMD_XFP_Property//设置光模块属性(最大传输距离、波长、最大传输速率)
} Set_CMD_Type;


/**********Definition for  variable types************************/




/**********************************************
*单元盘属性
*/
/*
typedef struct
{
    char InfoSetID;
    char Index;
    short len;

    char unit_class;     //单元盘大类
    char unit_type;      //单元盘小类
    char hardware_type;  //单元盘硬件类型
    char firmware_type;  //单元盘固件类型
    char struct_type;    //单元盘结构类型
    char port_num;
    char channel_num;
    char port_per_channel_num;
    char port_type;      //端口类型

} Unit_Property_InfoSet_t;
*/

typedef struct
{
    //u8 InfoSetID;
    //u8 Index;
    //u16 len;

    char unit_class;     //单元盘大类
    char unit_type;      //单元盘小类
    char hardware_type;  //单元盘硬件类型
    char firmware_type;  //单元盘固件类型
    char struct_type;    //单元盘结构类型
    char port_type;      //1-SFP 2SFP+ 3XFP
    char port_num;
    char xfp_property_source;//1:from card,others :from XFP module
    //u8 channel_num;
    //u8 port_per_channel_num;
       
} Unit_Property_InfoSet_t;


typedef struct
{
    //u8 InfoSetID;
    //u8 Index;
    //u16 len;

    char unit_class;     //单元盘大类
    char unit_type;      //单元盘小类
    char hardware_type;  //单元盘硬件类型
    char firmware_type;  //单元盘固件类型
    char struct_type;    //单元盘结构类型
    char card_func1;      // 1:OTU 2:OEO
    char card_func2;
    char wave_src;        // 1:光模块 2:板卡
    char distance_src;        // 1:光模块 2:板卡
    char port_num;
    char port_type;      //1-SFP 2SFP+ 3XFP
    char vendor[10];
    //u8 channel_num;
    //u8 port_per_channel_num;

} Unit_Otu_InfoSet_t;


typedef struct
{

    char InfoSetID;
    char Index;
    short len;


    short port1_distance; //传输距离(单位:10米)
    short port1_max_rate; //最大速率(单位:10M)
    short port1_wave_length;//波长(单位:1/20 nm)
    short port1_rate; //最大速率(单位:10M)    

    short port2_distance; //传输距离(单位:10米)
    short port2_max_rate; //最大速率(单位:10M)
    short port2_wave_length;//波长(单位:1/20 nm)
    short port2_rate; //最大速率(单位:10M)

    short port3_distance; //传输距离(单位:10米)
    short port3_max_rate; //最大速率(单位:10M)
    short port3_wave_length;//波长(单位:1/20 nm)
    short port3_rate; //最大速率(单位:10M)    

    short port4_distance; //传输距离(单位:10米)
    short port4_max_rate; //最大速率(单位:10M)

    short port4_wave_length;//波长(单位:1/20 nm)
    short port4_rate; //最大速率(单位:10M)    
}  XFP_Property_InfoSet_t;


typedef struct
{
    char Card_Type;
    char Card_SubType;
    char Card_hardware;
    char Card_software;
    char Card_mech;
    char property[30];
    char sn[30];
    char model[30];
    char creation[30];
    char fwver[30];
    char hwver[30];
    char Card_parm7[30];
} fix_Info_t;



#endif

