#ifndef __CHASSIS_H
#define __CHASSIS_H 1
#include <typedefs.h>
#include <unit.h>
#define MAX_CHASSIS_COUNT 1	/*最大机箱数*/
#define MAX_PWR_COUNT 2	/*最大机箱数*/
#define MAX_FAN_COUNT 4	/*最大机箱数*/

#define MIN_CHASSIS_TEMP 1
#define MAX_CHASSIS_TEMP 2
#define MIN_PWR_VOLT     3
#define MAX_PWR_VOLT     4
 
typedef struct
{
 char  status;
 char  type;
 short volt;
 char  fan_status;
 char  exist;
}powerSupply_t;

typedef struct
{
  short speed;
  char  status;
  char  rev;
}fan_t;

typedef struct
{
  short         temp;
  powerSupply_t powersupply[MAX_PWR_COUNT];
  fan_t         fan[MAX_FAN_COUNT];
  unsigned char pwr1_led;
  unsigned char pwr2_led;
  unsigned char fan_led;
  unsigned char rev;
}chassis_t;

typedef struct
{
  unitProperty_t unit_property;
  uchar_t max_pwr_count;
  uchar_t monitor_pwr_run_speed;//支持电源风扇状态监控	1-支持 2-不支持
  uchar_t max_fan_count;
  uchar_t monitor_fan_run_speed;//支持风扇转速监控	1-支持 2-不支持
}chassisProperty_t;

typedef struct
{
 signed char  min_temperature;
 signed char  max_temperature;
 char  min_volt;
 char  max_volt;
 short  ps_fan_min_speed;
 short  ps_fan_max_speed;
 short  chassis_fan_min_speed;
 short  chassis_fan_max_speed;
}chassisThreshold_t;


typedef struct
{
  uchar_t Card_Type;
  uchar_t Card_SubType;
  uchar_t Card_hardware;
  uchar_t Card_software;
  uchar_t Card_mech;
  char property[30];
  char sn[30];  //单元盘序列号
  char model[30]; //单元型号
  char creation[30]; //单元盘创建日期，格式为：YYYY-MM-DD
  char fwver[30];    //单元盘固件版本号
  char hwver[30];
  char Card_parm7[30];
}card_fix_Info_t;

typedef struct
{
short    alarm_code;
uchar_t  sub_index;
uchar_t  sub_val;
}alarm_item1;

typedef struct
{
uchar_t data_set_id;
uchar_t data_set_count;
short   data_set_length;
alarm_item1 bp_alarm[10];
}bpP1Alarm_t;

typedef struct
{
uint8_t bp_exist[MAX_UNIT_COUNT];
uint8_t bp_data_valid[MAX_UNIT_COUNT];

short	data_set_count;
uchar_t data_set_id1;
uchar_t data_set_index1;
short data_set_length1;
chassis_t monitor_info;

uchar_t data_set_id2;
uchar_t data_set_index2;
short data_set_length2;
chassisThreshold_t  Threshold;
bpP1Alarm_t  bp_sys_alarm;

}backplane_info_t;

typedef struct {
backplane_info_t 	bp_runtime_info;
card_fix_Info_t        card_fix_info[MAX_UNIT_COUNT];
//void * runtime_info[MAX_UNIT_COUNT];
char                    runtime_info[MAX_UNIT_COUNT][500];
}chassis_all_t;

int setChassisThreshold(setInfoSet_t infoset);
chassis_t *getChassisInfo(uchar_t chassis);
chassisThreshold_t *getChassisThreshold(uchar_t chassis);
#endif /* __CHASSIS_H */
