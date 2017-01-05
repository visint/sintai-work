#ifndef __EDFA_H
#define __EDFA_H 1

//EDFA泵浦信息
typedef struct
{
  short working_current;//泵浦工作电流
  short power; //泵浦功率
  short temperature;//泵浦温度
  short cool_current;//泵浦制冷电流
  char   reserve1;
  char   reserve2; 
}edfaPumpStatus_t;

//EDFA状态信息 
typedef struct
{
  char   run_led;
  char   in_led;
  char   out_led;
  char   pta_led;
  char   mta_led;
  char   pump1_status;//泵浦1关断状态,1：打开 2:关闭
  char   pump2_status;//泵浦2关断状态,1：打开 2:关闭
  char   work_mode;    //工作模式,1：AGC  2：APC
  short inputPower;  //模块输入光功率
  short outputPower; //模块输出光功率
  short temperature; //模块温度
  short agc_or_apc_value;//工作模式为AGC时为增益（0-200），为APC时代表光功率，定位为0.1dBm
  edfaPumpStatus_t pump_status[2];
}edfaInfo_t;

//泵浦告警阈值
typedef struct
{
  char   minTemperature;//模块最小温度
  char   maxTemperature;//模块最大温度
  short  minCurrent;    //泵浦最小电流
  short  maxCurrent;    //泵浦最大电流
}pumpThreshold_t;

//EDFA告警阈值
typedef struct
{
  char  minInputPower;//模块最小输入光功率
  char  minOutputPower;//模块最小输出光功率
  char  minTemperature;//模块最小温度
  char  maxTemperature;//模块最小温度
  pumpThreshold_t pump[2];
}edfaThreshold_t;

//获取EDFA信息
edfaInfo_t *getEdfaInfo(uchar_t chassis,uchar_t slot);
//获取EDFA告警阈值
edfaThreshold_t *getEdfaThreshold(uchar_t chassis,uchar_t slot);
int insertEdfaInfoSets(uchar_t chassis,uchar_t slot);
#endif /* __EDFA_H */
