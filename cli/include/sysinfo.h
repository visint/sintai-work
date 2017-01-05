/*
 *  unit.h -- Vispace Unit Management public header
 *
 *  Copyright (c) Visint Inc., 2002-2012. All Rights Reserved.
 *
 *  See the file "license.txt" for information on usage and redistribution
 *
 */

#ifndef __SYSINFO_H
#define __SYSINFO_H 1
#include  <sys/types.h>
#include  <unit.h>
/******************************** Description *********************************/

/*
 *  Vispace Unit header. This defines the Unit Management
 *  public APIs.  Include this header for files that contain access to
 *  unit inquiry or management.
 */

/********************************* Includes ***********************************/

//#include  <typedefs.h>
/********************************** Defines ***********************************/
#define MAX_POWER_SUPPLY_COUNT 4	/*最大保存日志条数*/
#define MAX_POWER_FAN_COUNT 4

#define VENDOR_CN_NAME 1
#define VENDOR_EN_NAME 2
#define VENDOR_URL 3
#define WEB_NAME 4
#define VENDOR_LOGO 5
//#define NMU_FW_VERSION "1.0.0.1"
//#define NMU_HW_VERSION "1.0.0.1"

/*
enum PowerSupplyType
{
  unkownType=0,
  psAC=1,
  psDC=2
};

typedef struct
{
  enum PowerSupplyType type;
  short volt;
  short max_vlot;
  short min_vlot;
}powerSupply_t;

typedef struct
{
  short temperature;
  short max_temperature;
  short min_temperature;
}temperature_t;

typedef struct
{
  char status;
}fan_t;

typedef struct
{
  powerSupply_t power_supply[MAX_POWER_SUPPLY_COUNT];
  temperature_t temperature;
  fan_t fan[MAX_POWER_FAN_COUNT];
}sysInfo_t;
*/
struct cpu_usage_t
{
	unsigned long cpu_user;
	unsigned long cpu_sys;
	unsigned long cpu_nice;
	unsigned long cpu_idle;
};

struct mem_usage_t
{
	unsigned long total;
	unsigned long used;
	unsigned long free;
	unsigned long shared;
        unsigned long buffers ;
        unsigned long cached;
};
//恢复单元盘出厂设置
void restoreUnitConf(int chassis,int slot);
//恢复NMU出厂设置
void restoreNmuConf(int chassis,char isRestoreIp,char isReboot);// isRestoreIp = 0 不恢复ip； isRestoreIp =1 恢复ip 
char getCpuUsage(float *sys_usage,float *user_usage);
float  getMemUsage(struct mem_usage_t *usage);
//unsigned long getMemUsage(struct mem_usage_t *usage);
//unsigned long getCpuUsage(struct cpu_usage_t *usage);
int getMac(char* mac);
int getDeviceIp(char *ip_add,char *mask,char *gateway);
int setDeviceIp(char *ip_addr,char *mask,char *gateway);
pid_t getPidByName ( char *name);
int doSystem(const char *fmt, ...);
/*获取供应商中英文名称、URL、WEB名称、logo图片文件名*/
int getVendorInfo(int item_id,char *info);
/*设置供应商中英文名称、URL、WEB名称、logo图片文件名*/
int setVendorInfo(int item_id,char *info);

int getNmuBaseInfo(int chassis,unit_base_info_t *nmu_bi);
int setNmuBaseInfo(int chassis,char *sn,char *hw_ver,char *creation,char * model);
//int getSysInfo(sysInfo_t *sys_info);
#endif /* _H_SYSINFO */
/******************************************************************************/
