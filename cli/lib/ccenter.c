/******************************** Description *********************************
* 文件名称： ccenter.c
* 功能描述： 
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/9/23        V1.0      李文峰        创建
*
* 编译命令:gcc -o /usr/local/bin/ccenter ccenter.c -I../include -L=./
*
*Copyright (c) Visint Inc., 2002-2012. All Rights Reserved.
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <malloc.h>
#include "sys/types.h"
#include "sys/stat.h"
#include "stdarg.h"
#include "termios.h"
#include "linux/serial.h"

#include <sys/ipc.h>    
#include <sys/sem.h>

#include <sysinfo.h>
#include <visipc.h>
#include <chassis.h>
#include <olp.h>
#include <otu.h>
#include <ddm.h>
#include <public.h>
#include<time.h>

#define TIOCSRS485 0x542F
#define MODIFY_BY_LIWF 1

//arm-linux-gcc -o ioacc  ioacc.c -I../include -L=./ -L/home/visint/src/arm/lib -lvispace -lsqlite3 -liconv
/****************************************************************
 * Constants
 ****************************************************************/
#define SEMKEY 1235L 
#define SHMKEY_SET 1236L

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define POLL_TIMEOUT (1 * 1000) /* 3 seconds */
#define MAX_BUF 64

#define own_addr_0     32+22   /*PB22*/
#define own_addr_1     32+23   /*PB23*/
#define own_addr_2     32+24   /*PB24*/
#define own_addr_3     32+25    /*PB25*/

#define bp_int_line    64+15   /*PC15*/
#define reset_bp_cpu   32+20   /*PB20*/

#define led_pwr1        32+1   /*PB1*/
#define led_pwr2        32+3   /*Pb3*/
#define led_fan_green   32     /*PB0*/
#define led_fan_red     32+2   /*PB2*/
#define led_run_green   28     /*PA28*/
#define led_run_red     29     /*PA29*/
#define led_board       25     /*PA25*/

#define PORT_READ_COUNT  5
#define sfp1_exist_pin        7   //PA7
#define sfp2_exist_pin        6   //PA6

#define port_lan2_link_up_down 11   //port 2
#define port_lan1_link_up_down 8  //port 3

#define port_sfp2_link_up_down   10
#define port_sfp1_link_up_down   9



#define setup_require    4     /*PA4*/
#define setup_respone    5     /*PA5*/

//low level communication command
#define cmd_get_fix_info      0x01  //获取板卡固定信息
#define CMD_GET_RUN_TIME       0x02  //获取板卡动态信息
#define cmd_get_alarm         0x03  //获取板卡告警信息

#define cmd_bp_valid_info     0x04

#define cmd_set_runtime_1 0x05  //设置信息集，预留7个
#define cmd_set_runtime_2 0x06
#define cmd_set_runtime_3 0x07
#define cmd_set_runtime_4 0x08
#define cmd_set_runtime_5 0x09
#define cmd_set_runtime_6 0x0a
#define cmd_set_runtime_7 0x0b

#define cmd_set_fix_para1 0x20  //设置固定信息，预留11条
#define cmd_set_fix_para2 0x21
#define cmd_set_fix_para3 0x22
#define cmd_set_fix_para4 0x23
#define cmd_set_fix_para5 0x24
#define cmd_set_fix_para6 0x25
#define cmd_set_fix_para7 0x26
#define cmd_set_fix_para8 0x27
#define cmd_set_fix_para9 0x28
#define cmd_set_fix_para10 0x29
#define cmd_set_fix_para11 0x2a


#define cmd_update_start 0x80
#define cmd_update_proc  0x81
#define cmd_update_end   0x82

#define BP_ADDR             17 //背板CPU  RS485地址
#define master_nmu_addr     0
#define slave_nmu_addr      15

#define RS485_TX_LEN  512    //发送长度
#define RS485_RX_LEN  512    //接收长度

#define led_green_state    1 //LED 绿色
#define led_red_state      2 //LED 红色
#define led_off_state      3 //LED 熄灭

#define led_pwr1_num   1  //LED 序号
#define led_pwr2_num   2
#define led_fan_num    3
#define led_run_num    4

//#define DEBUG_4U

backplane_info_t bp_runtime_info_temp;
chassis_all_t chassis_info;

//unsigned char nmu_own_addr=0xff;
unsigned char first_power_on=1; //识别初次上电
int rs485_comm_error=0;
key_t semkey_ttys1;    
int semid_ttys1;
int shmid_set,shmid_get;

int global_uart_fd=0;

 int sfp1_exist,sfp2_exist;//1=exist,  2=not exist  
 int lan1_updown,lan2_updown,sfp1_updown,sfp2_updown;  //1=linkup  , 2=linkdown  
static char is_debug=0;
/****************************************************************
 * gpio_export
 ****************************************************************/
int gpio_export(unsigned int gpio)
{
    int fd, len;
    char buf[MAX_BUF];

    fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
    if (fd < 0)
    {
        perror("gpio/export");
        return fd;
    }

    len = snprintf(buf, sizeof(buf), "%d", gpio);
    write(fd, buf, len);
    close(fd);

    return 0;
}

/****************************************************************
 * gpio_unexport
 ****************************************************************/
int gpio_unexport(unsigned int gpio)
{
    int fd, len;
    char buf[MAX_BUF];

    fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
    if (fd < 0)
    {
        perror("gpio/export");
        return fd;
    }

    len = snprintf(buf, sizeof(buf), "%d", gpio);
    write(fd, buf, len);
    close(fd);
    return 0;
}

/****************************************************************
 * gpio_set_dir
 ****************************************************************/
int gpio_set_dir(unsigned int gpio, unsigned int out_flag)
{
    int fd, len;
    char buf[MAX_BUF];

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);

    fd = open(buf, O_WRONLY);
    if (fd < 0)
    {
        perror("gpio/direction");
        return fd;
    }

    if (out_flag)
        write(fd, "out", 4);
    else
        write(fd, "in", 3);

    close(fd);
    return 0;
}

/****************************************************************
 * gpio_set_value
 ****************************************************************/
/*
int gpio_set_value(unsigned int gpio, unsigned int value)
{
    int fd, len;
    char buf[MAX_BUF];

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

    fd = open(buf, O_WRONLY);
    if (fd < 0)
    {
        perror("gpio/set-value");
        return fd;
    }

    if (value)
        write(fd, "1", 2);
    else
        write(fd, "0", 2);

    close(fd);
    return 0;
}
*/
/****************************************************************
 * gpio_get_value
 ****************************************************************/
/*int gpio_get_value(unsigned int gpio, unsigned int *value)
{
    int fd, len;
    char buf[MAX_BUF];
    char ch;

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

    fd = open(buf, O_RDONLY);
    if (fd < 0)
    {
        perror("gpio/get-value");
        return fd;
    }

    read(fd, &ch, 1);

    if (ch != '0')
    {
        *value = 1;
    }
    else
    {
        *value = 0;
    }

    close(fd);
    return 0;
}
*/
/****************************************************************
 * gpio_set_edge
 ****************************************************************/

int gpio_set_edge(unsigned int gpio, char *edge)
{
    int fd, len;
    char buf[MAX_BUF];

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", gpio);

    fd = open(buf, O_WRONLY);
    if (fd < 0)
    {
        perror("gpio/set-edge");
        return fd;
    }

    write(fd, edge, strlen(edge) + 1);
    close(fd);
    return 0;
}

/****************************************************************
 * gpio_fd_open
 ****************************************************************/

int gpio_fd_open(unsigned int gpio)
{
    int fd, len;
    char buf[MAX_BUF];

    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

    fd = open(buf, O_RDONLY | O_NONBLOCK );
    if (fd < 0)
    {
        perror("gpio/fd_open");
    }
    return fd;
}

/****************************************************************
 * gpio_fd_close
 ****************************************************************/

int gpio_fd_close(int fd)
{
    return close(fd);
}

/****************************************************************
 * 获取网管卡本身地址
 ****************************************************************/
unsigned char get_nmu_slot(void)
{
    int tmp,val=0;

    gpio_export(own_addr_0);
    gpio_export(own_addr_1);
    gpio_export(own_addr_2);
    gpio_export(own_addr_3);
//set input
    gpio_set_dir(own_addr_0, 0);
    gpio_set_dir(own_addr_1, 0);
    gpio_set_dir(own_addr_2, 0);
    gpio_set_dir(own_addr_3, 0);
//get own  card number
    gpio_get_value(own_addr_0,&tmp);
    val|=tmp;
    gpio_get_value(own_addr_1,&tmp);
    val|=(tmp<<1);
    gpio_get_value(own_addr_2,&tmp);
    val|=(tmp<<2);
    gpio_get_value(own_addr_3,&tmp);
    val|=(tmp<<3);
    
    //printf ("get_nmu_slot tmp=%d\n",tmp);
    //printf ("get_nmu_slot val=%d\n",val);
    if (val<1 || val>16)
       val=16;
	printf ("\nget_nmu_slot val=%d\n",val);
    return (unsigned char)val;
}

void port_up_down_init(void)
{
    int tmp,val;

		gpio_export(sfp1_exist_pin);
		gpio_export(sfp2_exist_pin);
		gpio_export(port_lan2_link_up_down);
		gpio_export(port_lan1_link_up_down);
		gpio_export(port_sfp2_link_up_down);
		gpio_export(port_sfp1_link_up_down);

		
	//set input
	          gpio_set_dir(sfp1_exist_pin, 0);
		 gpio_set_dir(sfp2_exist_pin, 0);
		 gpio_set_dir(port_lan1_link_up_down, 0);
	          gpio_set_dir(port_lan2_link_up_down, 0);
		gpio_set_dir(port_sfp2_link_up_down, 0);
	         gpio_set_dir(port_sfp1_link_up_down, 0);

//gpio_get_value(port_lan1_link_up_down,&tmp);
//printf("sfp1=%d\n",tmp);



}


/****************************************************************
 * LED 初始化
 ****************************************************************/

void led_init(void)
{
    gpio_export(led_pwr1);
    gpio_export(led_pwr2);
    gpio_export(led_fan_green);
    gpio_export(led_fan_red);
    gpio_export(led_run_green);
    gpio_export(led_run_red);
    gpio_export(setup_require);

    gpio_set_dir(led_pwr1, 1);
    gpio_set_dir(led_pwr2, 1);
    gpio_set_dir(led_fan_green, 1);
    gpio_set_dir(led_fan_red, 1);
    gpio_set_dir(led_run_green, 1);
    gpio_set_dir(led_run_red, 1);
    gpio_set_dir(setup_require, 1);

//all led off
    gpio_set_value(led_pwr1,1);
    gpio_set_value(led_pwr2,1);
    gpio_set_value(led_fan_green,0);
    gpio_set_value(led_fan_red,0);
    gpio_set_value(led_run_green,0);
    gpio_set_value(led_run_red,0);

    gpio_set_value(setup_require,0);

}


/****************************************************************
 * LED 操作，参数1为LED序号，参数2为LED颜色状态
 ****************************************************************/

void led_control(unsigned char led_num ,unsigned char state)
{
    switch(led_num)
    {
    case led_pwr1_num:

        switch(state)
        {
        case  led_green_state:
            gpio_set_value(led_pwr1,0);
            break;
        case led_off_state:
            gpio_set_value(led_pwr1,1);
            break;
        default:
            break;
        }
        break;
    case led_pwr2_num:

        switch(state)
        {
        case  led_green_state:
            gpio_set_value(led_pwr2,0);
            break;
        case led_off_state:
            gpio_set_value(led_pwr2,1);
            break;
        default:
            break;
        }
        break;
    case led_fan_num:
        switch(state)
        {
        case  led_green_state:
            gpio_set_value(led_fan_green,1);
            gpio_set_value(led_fan_red,0);

            break;
        case  led_red_state:
            gpio_set_value(led_fan_green,0);
            gpio_set_value(led_fan_red,1);

            break;

        case led_off_state:
            gpio_set_value(led_fan_green,0);
            gpio_set_value(led_fan_red,0);

            break;
        default:
            break;
        }
        break;
    case led_run_num:
        switch(state)
        {
        case  led_green_state:
            gpio_set_value(led_run_green,1);
            gpio_set_value(led_run_red,0);

            break;
        case  led_red_state:
            gpio_set_value(led_run_green,0);
            gpio_set_value(led_run_red,1);

            break;

        case led_off_state:
            gpio_set_value(led_run_green,0);
            gpio_set_value(led_run_red,0);

            break;
        default:
            break;
        }
        break;

    default:
        break;
    }
}


/****************************************************************
 * RS485发送校验
 ****************************************************************/
/*
unsigned char tx_crc_calc(char *p_buff,int len)
{
    unsigned char result=0;
    int i;
    for(i=0; i<len; i++)result+=p_buff[i];
    return (0-result);
}
*/

/****************************************************************
 * RS485接收校验
 ****************************************************************/
/*unsigned char rx_crc_calc(char *p_buff,int len)
{
    unsigned char result=0;
    int i;
    for(i=0; i<len; i++)result+=p_buff[i];
    return result;
}
*/
void shm_set_init(void)
{ //creat
    shmid_set=shmget(SHMKEY_SET,1024,0666|IPC_CREAT);    
    if(shmid_set==-1)
      printf("creat shmset is fail\n");
    //else printf("\ncreate share mem good\n");
//    shmid_get=shmget(SHMKEY_GET,1024,0666|IPC_CREAT);    
//    if(shmid_set==-1)printf("creat shmget is fail\n");  
}

void sem_tty_init(void)
{
    semkey_ttys1=ftok("/proc",101); 
    semid_ttys1=semget(SEMKEY,1,0666|IPC_CREAT);    
    if(semid_ttys1==-1)   printf("creat sem is fail\n");	

union semun{	
	int val;	
	struct semid_ds *buf;	 
	ushort *array;	  
}sem_u;    
	
sem_u.val=1;	
semctl(semid_ttys1,0,SETVAL,sem_u);  

}
/*
void p()	
   {	
	   struct sembuf sem_p;    
	   sem_p.sem_num=0;    
	   sem_p.sem_op=-1;    
//	      printf("serve_p_op\n"); 	
	   if(semop(semid_ttys1,&sem_p,1)==-1)	
	   printf("p operation is fail\n"); 		   
   }	
 */  
   /*\u4fe1\u53f7\u91cf\u7684V\u64cd\u4f5c*/
/*
   void v()    
   {	
	   struct sembuf sem_v;    
	   sem_v.sem_num=0;    
	   sem_v.sem_op=1;	 
	   
	//   printf("serve_v_op\n");	 
	   if(semop(semid_ttys1,&sem_v,1)==-1)	
	   printf("v operation is fail\n");    
   }
*/

/****************************************************************
 * 板卡数据读写函数:
 参数1，card_addr:      板卡地址
 参数2，access_cmd:   访问命令
 参数3，send_buff       发送缓冲区地址
 参数4，send_len         发送缓冲区长度
 参数5，receive_buff     接收缓冲区地址
 参数6，receive_len       接收缓冲区长度
 ****************************************************************/
/*
int card_access(unsigned char card_addr,int access_cmd, void *send_buff, int send_len,void * receive_buff,int receive_len)
{
    char txBuffer[1024];
    char rxBuffer[1024];
    int fd;
    int i;
    int rev_count;
    struct termios tty_attributes;
    struct serial_rs485 rs485conf;
    txBuffer[0]=card_addr;
    txBuffer[1]=access_cmd;
    memcpy(txBuffer+2,send_buff,send_len);
 // printf("haha-bre-p\n");
 
    p();
//	printf("server-p\n");
	
    if ((fd = open("/dev/ttyS1",O_RDWR|O_NOCTTY|O_NONBLOCK))<0)
    {
        fprintf (stderr,"Open error on %s\n", strerror(errno));
	v();
        return EXIT_FAILURE;
    }
    else
    {
        tcgetattr(fd,&tty_attributes);

        // c_cflag
        // Enable receiver
        tty_attributes.c_cflag |= CREAD;

        // 8 data bit
        tty_attributes.c_cflag |= CS8;

        // c_iflag
        // Ignore framing errors and parity errors.
        tty_attributes.c_iflag |= IGNPAR;
        tty_attributes.c_iflag &=~(INLCR|ICRNL|IGNCR|IXON);

        tty_attributes.c_oflag&=~(ONLCR|OCRNL|ONOCR|ONLRET);

        // c_lflag
        // DISABLE canonical mode.
        // Disables the special characters EOF, EOL, EOL2,
        // ERASE, KILL, LNEXT, REPRINT, STATUS, and WERASE, and buffers by lines.

        // DISABLE this: Echo input characters.
        tty_attributes.c_lflag &= ~(ICANON);

        tty_attributes.c_lflag &= ~(ECHO);

        // DISABLE this: If ICANON is also set, the ERASE character erases the preceding input
        // character, and WERASE erases the preceding word.
        tty_attributes.c_lflag &= ~(ECHOE);

        // DISABLE this: When any of the characters INTR, QUIT, SUSP, or DSUSP are received, generate the corresponding signal.
        tty_attributes.c_lflag &= ~(ISIG);

        // Minimum number of characters for non-canonical read.
        tty_attributes.c_cc[VMIN]=1;

        // Timeout in deciseconds for non-canonical read.
        tty_attributes.c_cc[VTIME]=0;

        // Set the baud rate
        cfsetospeed(&tty_attributes,B115200);
        cfsetispeed(&tty_attributes,B115200);

        tcsetattr(fd, TCSANOW, &tty_attributes);
        // Set RS485 mode:
        rs485conf.flags |= SER_RS485_ENABLED;
        if (ioctl (fd, TIOCSRS485, &rs485conf) < 0)
        {
            printf("ioctl error\n");
        }
        txBuffer[RS485_TX_LEN-1]=tx_crc_calc(txBuffer,RS485_TX_LEN-1);
        usleep(10000);
        write(fd,txBuffer,RS485_TX_LEN);
        usleep(220000);
        rev_count=read(fd,rxBuffer,RS485_RX_LEN);
        if((rev_count==(RS485_RX_LEN))   )
        {

            if(rx_crc_calc(rxBuffer,(RS485_RX_LEN))==0)
            {
                memcpy(receive_buff,(rxBuffer+1),receive_len);
            }
            else
            {
                close(fd);
	        v();
                return EXIT_FAILURE;
            }

        }
        else
        {
            close(fd);
	     v();
            return EXIT_FAILURE;
        }

    }

    close(fd);
     v();
    return EXIT_SUCCESS;
}
*/

/****************************************************************
 * 中断 初始化
 ****************************************************************/

int interrupt_init(int pin_num)
{
    int int_fd;
    gpio_export(pin_num);
    gpio_set_dir(pin_num, 0);
    gpio_set_edge(pin_num, "both");
    int_fd = gpio_fd_open(pin_num);
    return int_fd;
}

/**************************************************************
【功能】      插入单元盘
【参数说明】
            chassis      机箱号（0-MAX_CHASSIS_COUNT）,0为主机箱
            uchar_t      slot 槽位号（0-MAX_UNIT_COUNT）,0为背板（0-MAX_UNIT_COUNT）
【返回值说明】
【提示】
***************************************************************/
static void insertUnit(uchar_t chassis,uchar_t slot,enum UnitClass uclass,uchar_t utype,unit_base_info_t bi,char *uproperty,char send_trap)
{
    struct timeval tpstart;
    unit_base_info_t *pbi;
    uchar_t *us_shm;
    char property[32]= {0},*propertyMap;
    char unit_class_name[8]="";
    //olp1P1Info_t *olp;
    char *values;
    if (uclass<1)
       return;
    us_shm=getUnitStatusMap(0);
    if (NULL==us_shm)
        return;

    pbi=getBaseInfoMap(0,slot);
    if (NULL==pbi)
        return;
        
    //memcpy(pbi,&bi,sizeof(unit_base_info_t));
    gettimeofday(&tpstart,NULL);
    pbi->uptime=tpstart.tv_sec;
    #ifdef _DEBUG_
      printf("Unit #%d Uptime:%d\n",slot,tpstart.tv_sec);
    #endif
    /*if (slot==6)
       printf("property:%d %d %d\n",uproperty[0],uproperty[1],uproperty[2]);
    */
    /*if (uclass!=5)
       property[0]=uclass;
    else
       property[0]=9;
    */
/*
    property[0]=uclass;
    property[1]=1;
    property[2]=1;
    property[3]=1;
    property[4]=1;
    */
    //loadInfosets(0,slot,property);
    *(us_shm+slot)=uclass;
    if (utype<1)
      utype=1;
    loadInfosets(0,slot,uclass,utype,uproperty);
    *(us_shm +MAX_UNIT_COUNT+slot)=utype;
    //getUnitClassName(uclass,unit_class_name);
    getUnitLabelName(uclass,utype,unit_class_name);
    propertyMap=getUnitPropertyMap(0,slot);
    if (propertyMap!=NULL)
    {
       /*if (slot==12)
       {
          char n;
          for (n=0;n<18;n++)
           printf("%d ",uproperty[n]);
           printf("\n");
       }
       */
       memcpy(propertyMap+4,uproperty,48);
       if (UC_PASSIVE!=uclass)
       {
         if (UC_OTU!=uclass)
         {
          if (uproperty[2]>1)
             *(us_shm +MAX_UNIT_COUNT*2+slot)=uproperty[2];
           else
             *(us_shm +MAX_UNIT_COUNT*2+slot)=1;
         }
         else
             *(us_shm +MAX_UNIT_COUNT*2+slot)=1;
       }
       else
           *(us_shm +MAX_UNIT_COUNT*2+slot)=1;
    }
    else
        printf("property of slot %d is null\n",slot);
    if (send_trap)
    {
      sendSnmpTrap(52,0,slot,0,0);
      printf("%s on slot #%d inserted\n",unit_class_name,slot);
    }
    else
        printf("%s on slot #%d exist\n",unit_class_name,slot);
    /*values=getInfoSetValues(chassis,slot,DDM_INFOSET_ID,1);
    if (NULL==values)
    {
      printf("ddm is null\n");
      return;
    }
    else
        printf("ddm is OK\n");
    */
   /*
    olp=getOlp1P1Info(0,slot);
    if (NULL==olp)
    {
      printf("Olp is null\n");
      return;
    }
    
    olp->mode=1;
    olp->line=1;
    olp->rule=1;
    olp->ret_mode=1;
    olp->ret_time=330;
    olp->rx1_power=-280;
    olp->rx2_power=-280;
    olp->tx_power=-180;
    olp->l1_power=-120;
    olp->l2_power=-110;
    olp->tx_alm_power=-280;
    */
    //printf("ret_mode=%d\n",olp->ret_mode);
    //printf("\nSlot:%d mode=%d line=%d rule=%d ret_mode=%d ret_time=%d rx1=%d rx2=%d tx=%d l1=%d l2=%d alm=%d\n",slot,olp->mode,olp->line,olp->rule,olp->ret_mode,olp->ret_time,olp->rx1_power,olp->rx2_power,olp->tx_power,olp->l1_power,olp->l2_power,olp->tx_alm_power);
}

static void removeUnit(char chassis,char slot)
{
    uchar_t *us_shm;
    //char property[32]= {0};

    us_shm=getUnitStatusMap(0);
    if (NULL!=us_shm)
    {
      *(us_shm+slot)=0;
      //printf("Unit %d removed\n",slot);
      sendSnmpTrap(51,0,slot,0,0);
    }
}

void initSys()
{
    //unsigned char slot=16;
    clearShm(0);
    insertBpuInfoSet(0);
    
    uchar_t *us_shm;
    us_shm=getUnitStatusMap(0);
    
    if (NULL!=us_shm)
    {
      unsigned char slot=16;
      slot=get_nmu_slot();
      *(us_shm+slot)=1;
    }

     //initNmuBaseInfo(0,slot);
     //printf("NMU on slot #%d\n",slot);
     //insertNmuInfoSet(0,slot);
    //insertNmuInfoSet(0);
}

//#define	ALARM_INFOSET_ID  99

int get_dataset_length(uchar_t infoset_id)
{
    switch (infoset_id)
    {
    case OLP_STATUS_INFOSET_ID:
        return sizeof(olp1P1Info_t);
        break;
    case  ALARM_INFOSET_ID :
        //return (sizeof(alarm_item)*10);
       return 40;
        break;
    default :
        return 0;
        break;
    }
}
/*
|count  2Byte|id |index | len 2Byte | data set |   id   | index | len |data set |
*/
#define MAX_DATASET_COUNT 10
int get_parseMessage(char *message,uchar_t slot)
{ 
    uchar_t infoset_count,infoset_id,infoset_index,infoset_len;
    uchar_t *pDataSet;
    char test_copyto[MAX_DATASET_COUNT][500];//for test only  
    int m=0;
    int test=0;
    infoset_count=(short )(*message); //get message count
    pDataSet=message+6;  //first message position
    for(; m < infoset_count;m++)
    {
        memcpy(	test_copyto[m],pDataSet,get_dataset_length(*(pDataSet-4))); 
        pDataSet =pDataSet+ ((short)*(pDataSet-2))+4; //next message start position
    }
   //for debug 
             olp1P1Info_t  *p_olp=(olp1P1Info_t  *)test_copyto[0];
             printf("\nL1_sw= %d",p_olp->l1_power);	
  //           printf("\nrx1= %d",p_olp->rx1_power);	
//	 for(;test<4;test++)
//	 	 printf("\n%2x,",test_copyto[1][test]);	
}
/*
static void saveMessageToShm1(uchar_t chassis,uchar_t slot,char *message)
{
  char msg[32],*values;
  int n;
  //uchar_t slot=11;
  for (n=0;n<32;n++)
    msg[n]=n;
 
  values=getInfoSetValues(0,slot,50,1);
  if (values==NULL)
  {
    return;
  }
  printf("Before 50\n");
  values=getInfoSetValues(0,slot,99,1);
  printf("\n");
  values=getInfoSetValues(0,slot,50,1);
  if (values!=NULL)
    memcpy(values,msg,32);
  printf("\n");
  values=getInfoSetValues(0,slot,99,1);
  
  printf("After memcpy 50\n");
  
  if (values==NULL)
  {
    return;
  }
  //memcpy(values,msg,32);
  
  values=getInfoSetValues(0,slot,99,1);
  printf("After memcpy 51\n");
}
*/
//|count  2Byte|id |index | len 2Byte | message |   id   | index | len |message ...
static void saveMessageToShm(uchar_t chassis,uchar_t slot,char *message)
{
  uchar_t index;
  uchar_t *pInfoSet;
  uchar_t infoset_id;
  short infoset_count;
  short pInfoSet_size;
  int n,m;
  char *values;
  //char buf[512];
  
  infoset_count=(short )(*message);
  if (infoset_count>13)
      infoset_count=13;
  //infoset_count++;
  pInfoSet=message+2;  //first message position
  //memcpy(buf,pInfoSet,480);
  //olp1P1Info_t *olp,*olp1,*olp2;
  /*
  if (slot==1 && *(pInfoSet+341)!=0)
  {
   //for (n=0;n<MAX_MESSAGE_SIZE*8;n++)
   printf("\n");
   for (n=340;n<360;n++)
   {
    if (0==*(pInfoSet+n))
    {
      //printf("n=%d\n",n);
      break;
    }
    
    printf("%d:%d|",n,*(pInfoSet+n));
    //if (n % MAX_MESSAGE_SIZE==0)
    if (n % 20==0)
     printf("\n");
   }
  }
  if (slot==12 && *(pInfoSet+340)==99 && *(pInfoSet+341)!=0)
  {
    //printf("Buf:%d:%d:%d:%d\n",buf[340],buf[341],buf[342],buf[343]);
    printf("Infoset count:%d\n",infoset_count);
    for (n=0;n<360;n++)
    {
      if (n%20==0)
          printf("\n");
      printf("D[%d]:%d;",n,*(pInfoSet+n));
    }
    printf("\n");
  }
  */
 //if (slot==1)
 //printf("#%d infoset_count=%d\n",slot,infoset_count);
 unsigned char alarm_size=4;
 for(m=0; m < infoset_count; m++)
  {
     //memcpy(test_copyto[m],n,get_dataset_length(*(n-4)));
     infoset_id=*(pInfoSet);
    
     index=*(pInfoSet+1);
     if (index<1)
        index=1;
     pInfoSet_size=(short)(*(pInfoSet+2));//信息集字节长度
 
     //if (slot==1)
     //   printf("#%d pInfoSet_size=%d MAX_MESSAGE_SIZE:%d\n",m,pInfoSet_size,MAX_MESSAGE_SIZE);
     if (pInfoSet_size>MAX_MESSAGE_SIZE)
       pInfoSet_size=MAX_MESSAGE_SIZE;
     /*
     if (slot==1 && 99==infoset_id)
       printf("Slot=%d infoset_id=%d index=%d size=%d\n",slot,infoset_id,index,pInfoSet_size);
     */
     values=getInfoSetValues(0,slot,infoset_id,index);
     if (infoset_id!=99 && values==NULL)
     {
         if (is_debug)
           printf("slot=%d infoset_id=%d index=%d NOT found!\n",slot,infoset_id,index);
         break;
     }

     if (values!=NULL)
     {
        //char buf[36],buf2[36];
        //printf("infoset_id=%d index=%d found!\n",infoset_id,index);
        if (pInfoSet_size>MAX_MESSAGE_SIZE)
           pInfoSet_size=MAX_MESSAGE_SIZE;
        memcpy(values,pInfoSet+4,pInfoSet_size);
        ///printf("After memcpy\n");
        //getInfoSetValues(0,slot,infoset_id,index);
     }
       /************************************************************
       告警格式:
       |99|等于36-单个告警字节长度为6,否则为告警条数|告警总字节长度 2字节 {[告警号 2字节| index | 告警值(1字节 or 3字节)]...}
      **************************************************************/
       if (ALARM_INFOSET_ID==infoset_id)
       {
          unsigned short alarm_id;
          short alarm_value=0;
          /*
         short k,l;
          if (slot==1)
          {
              printf("BUF:%d:%d:%d:%d\n",buf[340],buf[341],buf[342],buf[343]);
          }
          for (k=340;k<460;k++)
          {
            if (buf[k]==99)
             break;
          }
          if (k<460)
          {
             for (l=k;l<k+10;l++)
             printf("B[%d]=%d ",l,buf[l]);
          }
          */
          //  printf("D[%d]=%d ",k,*(pInfoSet+k));
          // printf("\n");
          
          //printf("\n");
          /*for (k=342;k<362;k++)
             printf("D[%d]==%d ",k,*(message+k));
           printf("\n");
          */
           //printf("D0:%d D2:%d D3:%d...\n",*(pInfoSet),*(pInfoSet+2),*(pInfoSet+3));
          infoset_count=(short )(*(pInfoSet+2));//告警总字节长度
          //if (slot==1)
          //printf("infoset count==%d\n",infoset_count);
          //infoset_count=*(pInfoSet+2);
          /*infoset_count=infoset_count<<8;
          infoset_count=infoset_count | *(pInfoSet+3);
          */
          //printf("infoset_count=%d\n",infoset_count);
          //alarm_size=*(pInfoSet+1)-30;
          if (36==*(pInfoSet+1))
             alarm_size=6;
          //printf("infoset_count=%d alarm_size=%d pInfoSet_size=%d\n",infoset_count,alarm_size,pInfoSet_size);
          //if (alarm_size<6 || alarm_size>10)
           //  alarm_size=4;
          //printf("D0:%d D2:%d D3:%d\n",*(pInfoSet),*(pInfoSet+2),*(pInfoSet+3));
          //if (alarm_size!=4)
            infoset_count/=alarm_size;//告警条数
          //if (infoset_count>0)
          //printf("infoset_count =%d alarm len=%d on slot %d\n",infoset_count,*(pInfoSet+1),slot);
          
          if (infoset_count>20)
            infoset_count=20;
          if (infoset_count<1)
            break;
          /*printf("Unit #%d alarm count=%d\n",slot,infoset_count);
          for (m=0;m<40;m++)
            printf("%x ",*(pInfoSet+m));
          printf("\n");
          */
          values=pInfoSet+4;
 
          for (n=0;n<infoset_count;n++)
          {
             alarm_id=*(short *)(values);
             if (alarm_size!=4)
               alarm_value=*(short *)(values+4);
             //printf("v0:%d v1:%d v2:%d v3:%d v4:%d v5:%d\n",values[0],values[1],values[2],values[3],values[4],values[5]);
             //printf("alarm_size=%d alarm_id=%d alarm value=%d on slot %d\n",alarm_size,alarm_id,alarm_value,slot);
             //printf("alarm_id=%0d index=%d\n",alarm_id,values[2]);
             if (alarm_id<1)
               break;
             //printf("Alarm_id=%0d index=%d\n",alarm_id,values[2]);
             //printf("%0x %0x %0x %0x\n",values[0],values[1],values[2],values[3]);
             //printf("\nsendSnmpTrap...alarm_id=%0d index=%d\n",alarm_id,values[2]);
             sendSnmpTrap(alarm_id,0,slot,values[2],alarm_value);
             values+=alarm_size;
          }
          //printf("\nsendSnmpTrap...\n");
       }
     //pInfoSet =pInfoSet+ ((short)*(pInfoSet-2))+4; //next message start position
       //printf("\nsendSnmpTrap pInfoSet_size=%d,alarm_size=%d...\n",pInfoSet_size,alarm_size);
       pInfoSet =pInfoSet+pInfoSet_size+4;//alarm_size;
  }
}

/****************************************************************
 * 
 ****************************************************************/
static unsigned int max_error_count=0,max_error_count_save=0,error_count_save=3;
int io_get(uchar_t slot,uchar_t *pData_get, uchar_t op_cmd)
{
#define MAX_ERR_COUNT 5
int access_result,error_count=0;
#ifdef MODIFY_BY_LIWF
static unsigned char io_get_error_count=0;
#endif
unsigned char tx_tmp[1024];
//printf("Run into io_get #%d\n",slot);
io_read_retry:	
     access_result=card_access(slot,op_cmd,tx_tmp,500,pData_get,500);
     if(access_result==EXIT_FAILURE)
     {  
        error_count++;
	if(error_count>MAX_ERR_COUNT)
        {
          
          #ifdef MODIFY_BY_LIWF
          if (is_debug)
          {
           if (slot<17)
           {
            if (io_get_error_count>error_count_save)
            {
                //printf("ccenter shut down!\n",slot);
                if (max_error_count_save>0 && io_get_error_count>error_count_save)
                {
                 int count;
                 char value[30];
                 getConfValue(SYS_CONF_FILE,"errcount",value," ");
                 count=atoi(value)+error_count_save;
                 if (count<max_error_count_save)
                 {
                   sprintf(value,"%d",count);
                   setConfValue(SYS_CONF_FILE,"errcount",value," ");
                 }
               }
              io_get_error_count=0;
              //exit(0);
            }
            else
              io_get_error_count++;
            }
          }
          #endif
          
          return EXIT_FAILURE;
        }
        usleep(220000);
        /*
        #ifdef  _DEBUG_
	  printf("Unit #%d io_get failed  retry\n",slot);
        #endif
        */
        //printf("Unit #%d io_get failed  retry\n",slot);
        #ifdef MODIFY_BY_LIWF
        if (is_debug)
           printf("Unit #%d io_get failed!retry\n",slot);
        #endif
        goto io_read_retry;
     }
    #ifdef MODIFY_BY_LIWF
    else if (io_get_error_count>0)
      io_get_error_count=0;
    #endif
    return access_result;
}

int io_set(uchar_t slot,uchar_t *unit_class, uchar_t op_cmd)
{
      #define MAX_ERRSET_COUNT 3
	
	int access_result,error_count=0;
	unsigned char rev_tmp[1024];
	//printf("\nRun into io_set slot #%d accur",slot); 
	io_set_retry:	
		  access_result=card_access(slot,op_cmd,unit_class,500,rev_tmp,500);
	         if(access_result==EXIT_FAILURE)
		 {	
                   error_count++;
		   if(error_count>MAX_ERRSET_COUNT)return EXIT_FAILURE;
						usleep(220000);
                   if (is_debug)
		    printf("Slot #%d io_set in ccenter failed  retry\n",slot);
		   goto io_set_retry;
		 }
		  return access_result;

}

int low_uart_init(void)
{   
    int uart_fd;
    struct termios tty_attributes,tty_old_attrib;
    struct serial_rs485 rs485conf;
    memset(&rs485conf, 0x0, sizeof(struct serial_rs485));
    if ((uart_fd = open("/dev/ttyS1",O_RDWR|O_NOCTTY|O_NONBLOCK))<0)
    {
        fprintf (stderr,"Open error on %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    else
    {
        tcgetattr(uart_fd,&tty_attributes);
        tcgetattr(uart_fd,&tty_old_attrib);
	

        // c_cflag
        // Enable receiver
        tty_attributes.c_cflag |= CREAD;

        // 8 data bit
        tty_attributes.c_cflag |= CS8;

        // c_iflag
        // Ignore framing errors and parity errors.
        tty_attributes.c_iflag |= IGNPAR;
        tty_attributes.c_iflag &=~(INLCR|ICRNL|IGNCR|IXON);

        tty_attributes.c_oflag&=~(ONLCR|OCRNL|ONOCR|ONLRET);

        // c_lflag
        // DISABLE canonical mode.
        // Disables the special characters EOF, EOL, EOL2,
        // ERASE, KILL, LNEXT, REPRINT, STATUS, and WERASE, and buffers by lines.

        // DISABLE this: Echo input characters.
        tty_attributes.c_lflag &= ~(ICANON);

        tty_attributes.c_lflag &= ~(ECHO);

        // DISABLE this: If ICANON is also set, the ERASE character erases the preceding input
        // character, and WERASE erases the preceding word.
        tty_attributes.c_lflag &= ~(ECHOE);

        // DISABLE this: When any of the characters INTR, QUIT, SUSP, or DSUSP are received, generate the corresponding signal.
        tty_attributes.c_lflag &= ~(ISIG);

        // Minimum number of characters for non-canonical read.
        tty_attributes.c_cc[VMIN]=1;

        // Timeout in deciseconds for non-canonical read.
        tty_attributes.c_cc[VTIME]=0;

        // Set the baud rate
        cfsetospeed(&tty_attributes,B115200);
        cfsetispeed(&tty_attributes,B115200);

        tcsetattr(uart_fd, TCSANOW, &tty_attributes);
        // Set RS485 mode:
       rs485conf.flags |= SER_RS485_ENABLED;
        if (ioctl (uart_fd, TIOCSRS485, &rs485conf) < 0)
       {
           printf("ioctl error\n");
       }
    	}

return uart_fd;

}

static void insertNmu(uchar_t chassiss ,uchar_t slot)
{
  
  insertNmuInfoSet(0,slot);
  initNmuBaseInfo(0,slot);
  //printf("Nmu on slot:%d\n",slot);
  uchar_t *us_shm;
  us_shm=getUnitStatusMap(0);

  if (NULL!=us_shm)
  {
      unsigned char unit=16;
      unit=get_nmu_slot();
       
      *(us_shm+unit)=1;
   }
}

int main(int argc, char **argv)
{
   #define SHARE_EMTPY  0X55
   #define SHARE_FULL   0XAA
   #define SET_STATUS_GOOD 0x55
   #define SET_STATUS_BAD 0xAA
   //char is_debug=0;
   struct solt_set{    
   char setbuf[512];    
   unsigned char slot;
   unsigned char op_cmd;
   unsigned char set_flag;
   unsigned char stauts_flag;
   };  

   char *shm_addr;
   time_t timep;

    struct pollfd fdset[2];
    int nfds = 2;
    int int_bp_fd, int_setup_respone,timeout, rc;
    char buf[MAX_BUF],send_tmp_buff[500],rev_tmp_buff[500];
    int len,i,access_result;
    enum UnitClass dev_type;
    
    unit_base_info_t *bi,binfo;
    uchar_t *us_shm;
    uchar_t uclass=0,utype=1;
    unsigned short err_count=0;
     
     int       lan1_status=0,lan2_status=0,sfp1_stauts=0,sfp2_stauts=0,sfp1_exist_status=0,sfp2_exist_status=0;
     int port_read_count=0;
     int port_tmp;
     nmuStatus_t *nmu_status;
     unsigned char slotOfNum=16;
	 unsigned char dbg_i;
     shm_set_init();
    struct solt_set * addr;    
    addr=(struct solt_set*)shmat(shmid_set,0,0);    
    if(addr==(struct solt_set*)-1)
       printf("shm shmat is fail\n");  
    //else printf("get share mem good\n");


    #ifdef MODIFY_BY_LIWF
    while((rc = getopt(argc, argv, "c:dhs:")) != -1)
    {
     switch(rc)
     {
       case 'd':
         is_debug=1;
       break;
       case 's':
         //is_debug=1;
       break;
       case 'c':
         max_error_count=atoi(optarg);
       break;
       case 'h':
         printf("ver 1.0.26\nLast modifed:2014-4-08\n");
         printf("-d: printf error\n");
         printf("-s: printf info of slot(0:all slot)\n");
         printf("-c: restart ccenter when io get error over the count(0:all slot)\n");
       exit(0);
     }
    }
    if (is_debug!=1)
    {
     char value[30]="";
     getConfValue(SYS_CONF_FILE,"debug",value," ");
     if (value[0]=='Y')
     {
       is_debug=1;
       
       getConfValue(SYS_CONF_FILE,"max_count",value," ");
       max_error_count_save=atoi(value);
       if (max_error_count_save<0)
          max_error_count_save=0;
       getConfValue(SYS_CONF_FILE,"count",value," ");
       error_count_save=atoi(value);
       if (error_count_save<0)
          error_count_save=0;    
     }
    }
    initSys();
    us_shm=getUnitStatusMap(0);
    
    if (NULL==us_shm)
        return 0;
    *us_shm=0;
    /*
     *us_shm=4;
    *(us_shm+17)=0;
    */
    #endif

    /*\u8bbe\u7f6e\u4fe1\u53f7\u91cf\u7684\u521d\u59cb\u503c\uff0c\u5c31\u662f\u8d44\u6e90\u4e2a\u6570*/    


   // sem_tty_init();
    //nmu_own_addr=get_own_addr();
    led_init();                                 //初始化LED
    port_up_down_init();
	
    int_bp_fd = interrupt_init(bp_int_line);    //初始化背板CPU中断线
    int_setup_respone=interrupt_init(setup_respone); //初始化背板CPU中断线
    //global_uart_fd=low_uart_init();

    timeout = POLL_TIMEOUT;
    memset(&chassis_info,0,sizeof(chassis_all_t));

    //printf("NUM on slot #%d\n",get_nmu_slot());
    //insertNmuInfoSet(0,get_nmu_slot());
    //initNmuBaseInfo(0,slot);
    //insertNmu(0,get_nmu_slot());
    slotOfNum=get_nmu_slot();
	
    insertNmu(0,slotOfNum);

   // printf("\nslotOfNum=%d",slotOfNum);

    while (1)
    {
        memset((void*)fdset, 0, sizeof(fdset));
		
        fdset[0].fd = int_setup_respone;
        fdset[0].events = POLLPRI;

        fdset[1].fd = int_bp_fd;
        fdset[1].events = POLLPRI;

        rc = poll(fdset, nfds, timeout);
	

   if(port_read_count<PORT_READ_COUNT)
   {//read
     gpio_get_value(port_lan1_link_up_down,&port_tmp);
     lan1_status+=port_tmp;

     gpio_get_value(port_lan2_link_up_down,&port_tmp);
     lan2_status+=port_tmp;

     gpio_get_value(port_sfp1_link_up_down,&port_tmp);
     sfp1_stauts+=port_tmp;
	
     gpio_get_value(port_sfp2_link_up_down,&port_tmp);
     sfp2_stauts+=port_tmp;

	
     gpio_get_value(sfp1_exist_pin,&port_tmp);
     sfp1_exist_status+=port_tmp;
	
     gpio_get_value(sfp2_exist_pin,&port_tmp);
     sfp2_exist_status+=port_tmp;
     port_read_count++;

    }		
nmu_status=(nmuStatus_t*)getInfoSetValues(0,slotOfNum,8,1);
if (NULL!=nmu_status)
{
   if (nmu_status->sfp1_exist!=sfp1_exist)
	nmu_status->sfp1_exist=sfp1_exist;
   if (nmu_status->sfp2_exist!=sfp2_exist)
	nmu_status->sfp2_exist=sfp2_exist;
   if (nmu_status->sfp1_status!=sfp1_updown)
	nmu_status->sfp1_status=sfp1_updown;
   if (nmu_status->sfp2_status!=sfp2_updown)
	nmu_status->sfp2_status=sfp2_updown;
   if (nmu_status->lan1_status!=lan1_updown)
        nmu_status->lan1_status=lan1_updown;
   if (nmu_status->lan2_status!=lan2_updown)
        nmu_status->lan2_status=lan2_updown;
	//printf("NMU Status:sfp1_exist_status:%d sfp1_status=%d sfp2_exist_status:%d sfp2_status=%d lan1_updown:%d lan2_updown:%d\n",sfp1_exist,sfp1_updown,sfp2_exist,sfp2_updown,lan1_updown,lan2_updown);
}
  
 if (fdset[0].revents & POLLPRI)  //背板中断发生
 {
    len = read(fdset[0].fd, buf, MAX_BUF);//清除中断标志
	    //printf("requre int happen %d\n",addr->set_flag);
           // for(print_count=0;print_count<10;print_count++)
	     //       printf("setbuf[%d]=%d\n",print_count,addr->setbuf[print_count]);
           //  usleep(120000);
   if( addr->set_flag==SHARE_FULL)          
   {      
          if(( io_set(addr->slot,addr->setbuf, addr->op_cmd))==EXIT_SUCCESS)
               addr->stauts_flag = SET_STATUS_GOOD;
	  else 
               addr->stauts_flag = SET_STATUS_BAD;
        }	  
	       addr->set_flag=SHARE_EMTPY;	
		//printf("\ne_set done");   
   }


        if (fdset[1].revents & POLLPRI)  //背板中断发生
        {
            char message[512]={0};
            len = read(fdset[1].fd, buf, MAX_BUF);//清除中断标志

            if(first_power_on==1)//第一次上电读取全部信息
            {
                chassis_t *pChassis;
                first_power_on=0;
                io_get(BP_ADDR,rev_tmp_buff, CMD_GET_RUN_TIME);
		//printf("bp get \n");
		memcpy(&bp_runtime_info_temp,rev_tmp_buff,sizeof(backplane_info_t));		
                memcpy(&(chassis_info.bp_runtime_info),&bp_runtime_info_temp,sizeof(backplane_info_t));
               
               if(bp_runtime_info_temp.monitor_info.pwr1_led==1) 
                  led_control(led_pwr1_num,led_green_state);
	       else  
                  led_control(led_pwr1_num,led_off_state);
	        if(bp_runtime_info_temp.monitor_info.pwr2_led==1) 
                  led_control(led_pwr2_num,led_green_state);
		else 
                    led_control(led_pwr2_num,led_off_state);		
		if(bp_runtime_info_temp.monitor_info.fan_led==1) 
                    led_control(led_fan_num,led_green_state);
		else 
                    led_control(led_fan_num,led_red_state);
               led_control(led_run_num,led_green_state);

               #ifdef MODIFY_BY_LIWF
               if (*us_shm>4 ||*us_shm<1)
               {
                 if (bp_runtime_info_temp.monitor_info.chassis_type>0 && bp_runtime_info_temp.monitor_info.chassis_type<5)
                    *us_shm=bp_runtime_info_temp.monitor_info.chassis_type;
                 /*else
                 {
                  if (err_count<3)
                     err_count++;
                  else
                     exit(0);
                 }
                 */
                 if (is_debug)
                   printf("Chassis type(%d)!\n",*us_shm);
               }
               
               //printf("Chassis class:%dU\n",bp_runtime_info_temp.monitor_info.chassis_type);
               /*if (*us_shm>4 ||*us_shm<1)
                 *us_shm=4;
               else 
                 *us_shm=1;
               */
               pChassis=getChassisInfo(0);
               if (NULL!=pChassis)
               {
                  memcpy(message,rev_tmp_buff+32,300);
                  saveMessageToShm(0,0,message+32);
                  //memcpy(pChassis,rev_tmp_buff+32+5,sizeof(chassis_t));   //copy backplane datasets
                  //printf("temp=%d\n",pChassis->temp);
                  /*pChassis->temp=chassis_info.bp_runtime_info.monitor_info.temp;
                  pChassis->powersupply[0].status=chassis_info.bp_runtime_info.monitor_info.powersupply[0].status;
                  pChassis->powersupply[1].status=chassis_info.bp_runtime_info.monitor_info.powersupply[1].status;
                  pChassis->powersupply[0].type=chassis_info.bp_runtime_info.monitor_info.powersupply[0].type;
                  pChassis->powersupply[1].type=chassis_info.bp_runtime_info.monitor_info.powersupply[1].type;
                  pChassis->powersupply[0].volt=chassis_info.bp_runtime_info.monitor_info.powersupply[0].volt;
                  pChassis->powersupply[1].volt=chassis_info.bp_runtime_info.monitor_info.powersupply[1].volt;

                  pChassis->fan[0].status=chassis_info.bp_runtime_info.monitor_info.fan[0].status;
                  pChassis->fan[1].status=chassis_info.bp_runtime_info.monitor_info.fan[1].status;
                */
               }
               //printf("Chassis temperatur=%d PW1=%d PW2=%d type1=%d type2=%d volt1=%d volt2=%d \n",chassis_info.bp_runtime_info.monitor_info.temp,chassis_info.bp_runtime_info.monitor_info.powersupply[0].status,chassis_info.bp_runtime_info.monitor_info.powersupply[1].status,chassis_info.bp_runtime_info.monitor_info.powersupply[0].type,chassis_info.bp_runtime_info.monitor_info.powersupply[1].type,pChassis->powersupply[0].volt,pChassis->powersupply[1].volt);
             #endif
                for(i=0; i<15; i++) //read fix information
                {
                    if(bp_runtime_info_temp.bp_exist[i]==0)
                    {
                     //char *property;
                     utype=1;
	  	     io_get( i+1,rev_tmp_buff,  cmd_get_fix_info);
		     memcpy(&(chassis_info.card_fix_info[i]),rev_tmp_buff,sizeof(card_fix_Info_t));
	    	     //io_get( i+1,chassis_info.runtime_info[i],  CMD_GET_RUN_TIME);
		     //get_parseMessage(chassis_info.runtime_info[i],i+1);  //parse message and copy to share memory	
                     /*
                     if (i<8 && i>4)
                     {
                       int nn;
                       
                       printf("Unit property\n");
                       for (nn=256;nn<305;nn++)
                          printf("%d ",rev_tmp_buff[nn]);
                       printf("\n");
                       property=getUnitProperty(0,i+1);
                       if (property!=NULL)
                          memcpy(property+1,rev_tmp_buff+256,48);
                     }
                    
                     property=getUnitProperty(0,i+1);
                     if (property!=NULL)
                     {
                        int mm;
                        memcpy(property+1,rev_tmp_buff+256,48);
                        printf("Unit #%d property:\n",i+1);
                        for (mm=0;mm<32;mm++)
                          printf("%d ",property[mm]);
                        printf("\n");
                     }
                     else
                         printf("Unit #%d property is null\n",i+1);
                     printf("Get Slot #%d  data ok!\n",i+1);
                    */	 
                      #ifdef MODIFY_BY_LIWF
                        bi=getBaseInfoMap(0,i+1);
                        
                        if (NULL!=bi && EXIT_FAILURE!=io_get( i+1,message, CMD_GET_RUN_TIME))
                        {
                          uclass=chassis_info.card_fix_info[i].Card_Type;
                          utype=chassis_info.card_fix_info[i].Card_SubType;
                          //printf("\nUnit #%d inserted\n",i+1);
                          //printf("#%d uClass=%d uType=%d\n",i+1,uclass,utype);
                          /*if (UC_OLP==uclass)
                          {
                            sprintf(bi->property,"%s-%d","OLP11-1-1",i+1);
                          }
                          else if (UC_OTU==uclass)
                          {
                           sprintf(bi->property,"%s-%d","OTU-1-1",i+1);
                          }
                          else if (UC_OEO==uclass)
                          {
                           sprintf(bi->property,"%s-%d","OEO-1-1",i+1);
                          }
                          else if (UC_EDFA==uclass)
                          {
                             uclass=UC_EDFA;
                             sprintf(bi->property,"%s-%d","EDFA-1-1",i+1);
                          }
                          if(6==i)
                          {
                            //uclass=UC_PASSIVE;
                            //uclass=UC_OSS;
                            utype=1;
                            uclass=UC_EDFA;
                            sprintf(bi->property,"%s-%d","EDFA-1-1",i+1);
                          }
                          */
                          /*if(7==i)
                          {
                            uclass=UC_OTU;
                            utype=2;
                            sprintf(bi->property,"%s-%d","OTU-1-1",i+1);
                          }
                          if(11==i)
                          {
                            uclass=UC_PASSIVE;
                            utype=1;
                            sprintf(bi->property,"%s-%d","OSU-1-1",i+1);
                          }
                          
                          if(11==i)
                          {
                            uclass=UC_OSS;
                            utype=1;
                            sprintf(bi->property,"%s-%d","OSS-1-1",i+1);
                          }
                          
                          if(12==i)
                          {
                            uclass=UC_PASSIVE;
                            //uclass=UC_OSS;
                            utype=2;
                             //uclass=UC_EDFA;
                            sprintf(bi->property,"%s-%d","ODM-1-1",i+1);
                          }
                          */
                        strncpy(bi->model,chassis_info.card_fix_info[i].model,30);
                        strncpy(bi->sn,chassis_info.card_fix_info[i].sn,15);
                        strncpy(bi->fwver,chassis_info.card_fix_info[i].fwver,10);
                        strncpy(bi->hwver,chassis_info.card_fix_info[i].hwver,10);
                        strncpy(bi->creation,chassis_info.card_fix_info[i].creation,10);
                        printf("Unit #%d model:%s\n",i+1,chassis_info.card_fix_info[i].model);
                        //#ifdef MODIFY_BY_LIWF
                        insertUnit(0,i+1,uclass,utype,*bi,rev_tmp_buff+256,0);
                        //#endif
                        //insertUnit(0,4,UC_OLP,1,binfo,0);
                        //io_get( i+1,chassis_info.runtime_info[i],  CMD_GET_RUN_TIME);
                        //for (n=0;n<60;n++)
                        // printf("%d\n",  message[n]);
                        if (EXIT_FAILURE!=io_get( i+1,message, CMD_GET_RUN_TIME))
                         {
                           saveMessageToShm(0,i+1,message);
                           //printf("Get io data ok!\n");
                         }
                         else
                            printf("Get io data error!\n");
                        //saveMessageToShm(0,i+1,message);
                        }
                        #endif
                        printf("Unit #%d exists\n",i+1);
#ifdef DEBUG_4U
                        printf("\ncard= %s",chassis_info.card_fix_info[i].Card_parm1);
                        printf("\ncard %d= %s",i+1,chassis_info.card_fix_info[i].Card_parm1);
#endif
                    }
                }

            }
            else
            { //中断读取变化的板卡信息
	     io_get( BP_ADDR,rev_tmp_buff,  CMD_GET_RUN_TIME);
	
	    //printf("bp get22 \n");
	    memcpy(&bp_runtime_info_temp,rev_tmp_buff,sizeof(backplane_info_t));
		
     /*  printf debug info by cxz */


        time(&timep);
        printf("%s",ctime(&timep));
	    printf("\nlast_xx:");
       for(dbg_i=0;dbg_i<16;dbg_i++)
		printf("%2x,",chassis_info.bp_runtime_info.bp_exist[dbg_i]);
	   
		printf("\ncurrent:");
       for(dbg_i=0;dbg_i<16;dbg_i++)
		printf("%2x,",bp_runtime_info_temp.bp_exist[dbg_i]);

	  
//       printf("classis_info =%d PW1=%d PW2=%d type1=%d type2=%d  \n",bp_runtime_info_temp.monitor_info.temp,bp_runtime_info_temp.monitor_info.powersupply[0].status,bp_runtime_info_temp.monitor_info.powersupply[1].status,bp_runtime_info_temp.monitor_info.powersupply[0].type,bp_runtime_info_temp.monitor_info.powersupply[1].type);
     	
            //memcpy(imess,rev_tmp_buff+32,200);   //copy backplane datasets
	
            if(bp_runtime_info_temp.monitor_info.pwr1_led==1) 
                  led_control(led_pwr1_num,led_green_state);
	       else  
                  led_control(led_pwr1_num,led_off_state);
	        if(bp_runtime_info_temp.monitor_info.pwr2_led==1) 
                  led_control(led_pwr2_num,led_green_state);
		else 
                    led_control(led_pwr2_num,led_off_state);		
		if(bp_runtime_info_temp.monitor_info.fan_led==1) 
                      led_control(led_fan_num,led_green_state);
		else 
                     led_control(led_fan_num,led_red_state);


   if(port_read_count<PORT_READ_COUNT)
   {//read
   gpio_get_value(port_lan1_link_up_down,&port_tmp);
   lan1_status+=port_tmp;

   gpio_get_value(port_lan2_link_up_down,&port_tmp);
   lan2_status+=port_tmp;

	gpio_get_value(port_sfp1_link_up_down,&port_tmp);
	sfp1_stauts+=port_tmp;
	
	gpio_get_value(port_sfp2_link_up_down,&port_tmp);
	sfp2_stauts+=port_tmp;

	
	gpio_get_value(sfp1_exist_pin,&port_tmp);
	sfp1_exist_status+=port_tmp;
	
	gpio_get_value(sfp2_exist_pin,&port_tmp);
	sfp2_exist_status+=port_tmp;
	port_read_count++;

    }
else
{
//printf("\n all = %d,%d,%d,%d,%d,%d \n", lan1_status,lan2_status,sfp1_stauts,sfp2_stauts,sfp1_exist_status,sfp2_exist_status );

if(lan1_status==PORT_READ_COUNT)//down
    {  lan1_updown=2;  }
else { lan1_updown=1; }

if(lan2_status==PORT_READ_COUNT)//down
    {  lan2_updown=2;  }
else { lan2_updown=1; }

if(sfp1_stauts==PORT_READ_COUNT)//down
    {  sfp1_updown=2;  }
else { sfp1_updown=1; }

if(sfp2_stauts==PORT_READ_COUNT)//down
    {  sfp2_updown=2;  }
else { sfp2_updown=1; }

if(sfp1_exist_status==PORT_READ_COUNT)//no exist
    {  sfp1_exist=2;  }
else { sfp1_exist=1; }  

if(sfp2_exist_status==PORT_READ_COUNT)//no exist
    {  sfp2_exist=2;  }
else { sfp2_exist=1; }  
//printf("\n all = %d,%d,%d,%d,%d,%d \n", lan1_updown,lan2_updown,sfp1_updown,sfp2_updown,sfp1_exist,sfp2_exist );

lan1_status=0;
lan2_status=0;
sfp1_stauts=0;
sfp2_stauts=0;
sfp1_exist_status=0;
sfp2_exist_status=0;
port_read_count=0;

}
#ifdef MODIFY_BY_LIWF		
nmu_status=(nmuStatus_t*)getInfoSetValues(0,16,8,1);
if (NULL!=nmu_status)
{
   if (nmu_status->sfp1_exist!=sfp1_exist)
	nmu_status->sfp1_exist=sfp1_exist;
   if (nmu_status->sfp2_exist!=sfp2_exist)
	nmu_status->sfp2_exist=sfp2_exist;
   if (nmu_status->sfp1_status!=sfp1_updown)
	nmu_status->sfp1_status=sfp1_updown;
   if (nmu_status->sfp2_status!=sfp2_updown)
	nmu_status->sfp2_status=sfp2_updown;
   if (nmu_status->lan1_status!=lan1_updown)
        nmu_status->lan1_status=lan1_updown;
   if (nmu_status->lan2_status!=lan2_updown)
        nmu_status->lan2_status=lan2_updown;
   //	printf("NMU Status:sfp1_status=%d sfp2_status=%d",sfp1_updown,sfp2_updown);
}
                memcpy(message,rev_tmp_buff+32,300);
                /*int j;
                for (j=0;j<100;j++)
                    printf("%d,",message[j]);
                printf("\n");
                */
                saveMessageToShm(0,0,message);
#endif
               // printf("\nfan:%d %d %d %d\n",bp_runtime_info_temp.monitor_info.fan[0].status,bp_runtime_info_temp.monitor_info.fan[1].status,bp_runtime_info_temp.monitor_info.fan[2].status,bp_runtime_info_temp.monitor_info.fan[3].status);
                //printf("fan len=%d \n",bp_runtime_info_temp.bp_sys_alarm.data_set_length);
                for(i=0; i<15; i++)
                {
                    if(bp_runtime_info_temp.bp_exist[i]<chassis_info.bp_runtime_info.bp_exist[i])//判断是否有板卡插入
                    {
                        // 板卡插入:读取固定信息
                        if(EXIT_FAILURE != io_get( i+1,rev_tmp_buff,  cmd_get_fix_info))
			{
	         	  memcpy(&(chassis_info.card_fix_info[i]),rev_tmp_buff,sizeof(card_fix_Info_t));

                          //io_get( i+1,chassis_info.runtime_info[i],  CMD_GET_RUN_TIME);
                          //get_parseMessage(chassis_info.runtime_info[i],i+1);  //parse message and copy to share memory

                        printf("\nUnit #%d inserted\n",i+1);
                        //printf("\ncard %d= %s",i+1,chassis_info.card_fix_info[i].property);
                        #ifdef MODIFY_BY_LIWF
                        bi=getBaseInfoMap(0,i+1);
                        if (NULL!=bi )
                        {
                         olp1P1Info_t *olp;

                         uclass=chassis_info.card_fix_info[i].Card_Type;
                         utype=chassis_info.card_fix_info[i].Card_SubType;
                         //printf("#%d uclass=%d\n",i+1,uclass);
                         if (UC_OLP==uclass)
                         {
                            sprintf(bi->property,"%s-%d","OLP11-1-1",i+1);
                         }
                         else if (UC_OTU==uclass)
                         {
                           sprintf(bi->property,"%s-%d","OTU-1-1",i+1);
                         }
                         else if (UC_EDFA==uclass|| 5==uclass)
                         {
                           uclass=UC_EDFA;
                           sprintf(bi->property,"%s-%d","EDFA-1-1",i+1);
                         }
                         strncpy(bi->model,chassis_info.card_fix_info[i].model,30);
                         strncpy(bi->sn,chassis_info.card_fix_info[i].sn,15);
                         strncpy(bi->fwver,chassis_info.card_fix_info[i].fwver,10);
                         strncpy(bi->hwver,chassis_info.card_fix_info[i].hwver,10);
                         strncpy(bi->creation,chassis_info.card_fix_info[i].creation,10);
                         insertUnit(0,i+1,uclass,utype,*bi,rev_tmp_buff+256,1);
                         #endif
                         if (EXIT_FAILURE!=io_get( i+1,message, CMD_GET_RUN_TIME))
                         {
                           #ifdef MODIFY_BY_LIWF
                           saveMessageToShm(0,i+1,message);
                           #endif
                           //printf("Get Slot #%d io data ok!\n",i+1);
                         }
                         else
                            printf("Get io data error!\n");
                        }
                      
                     }
                    }
                    else if(bp_runtime_info_temp.bp_exist[i]>chassis_info.bp_runtime_info.bp_exist[i] )
                    {
                        //板卡拔出，清除固定信息和释放内存
                        memset(&(chassis_info.card_fix_info[i]),0,sizeof(card_fix_Info_t));
                        printf("Unit %d removed\n",i+1);
                        #ifdef MODIFY_BY_LIWF
                        removeUnit(0,i+1);
                        #endif
                    }
else if((bp_runtime_info_temp.bp_exist[i]==0) && (chassis_info.bp_runtime_info.bp_exist[i]==0) && (bp_runtime_info_temp.bp_data_valid[i]==0))
  {
  int ret;
  ret=io_get( i+1,message, CMD_GET_RUN_TIME);
//parse message and copy to share memory
#ifdef MODIFY_BY_LIWF
     if (EXIT_FAILURE!=ret)
       saveMessageToShm(0,i+1,message);
     //printf("saveMessageToShm\r\n");
#endif		
}

 }

   memcpy(&(chassis_info.bp_runtime_info),&bp_runtime_info_temp,sizeof(backplane_info_t));
   }
   // printf("\n");
   }
  }

    gpio_fd_close(int_bp_fd);
    gpio_fd_close(int_setup_respone);

    return 0;
}

