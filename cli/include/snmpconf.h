/*
 *  unit.h -- Vispace Unit Management public header
 *
 *  Copyright (c) Visint Inc., 2002-2012. All Rights Reserved.
 *
 *  See the file "license.txt" for information on usage and redistribution
 *
 */

#ifndef __SNMP_CONF_H
#define __SNMP_CONF_H 1

/******************************** Description *********************************/

/*
 *  Vispace Unit header. This defines the Unit Management
 *  public APIs.  Include this header for files that contain access to
 *  unit inquiry or management.
 */

/********************************* Includes ***********************************/

//#include  <typedefs.h>

/********************************** Defines ***********************************/
#define MAX_DEST_HOST_COUNT 10	/*最大目标主机数*/

//int snmpdConf();
int insertCommunity(const char *ipadd,const char *rocommunity,const char *rwcommunity,int port,int read_only,int enable,char isRestart);
int updateCommunity(int id,const char *ipadd,const char *rocommunity,const char *rwcommunity,int port,int read_only,int enable,char isRestart);// isRestart = 1 重启；isRestart = 0 不重启
int deleteCommunityById(int id,char isRestart);
int deleteCommunityByIp(char * ip,char isRestart);
//根据数据库里的Community配置生成snmpd.conf文件
int updateSnmpdConf(char isRestart); // 最后执行重启

/**************************
添加告警目标主机IP
目标主机名称不能为空且必须唯一,成功返回0，失败返回-1
*********************/
int insertDestHost(const char *target_name,const char *dest_ipadd,int udp_port,int enable);

int deleteDestHost(const char *dest_ipadd,int udp_port);
int deleteTarget(const char *target_name);
int restoreSnmpdConf(char isRestart);

#endif /* __SNMP_CONF_H */
/******************************************************************************/
