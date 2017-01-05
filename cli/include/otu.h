#ifndef __OTU_H
#define __OTU_H 1

/*OTU单元盘最大通道数*/
#define OTU_MAX_CHANNEL_COUNT 2
#define OTU_EVERY_CHANNEL_PORT_COUNT 2

typedef struct
{
  unsigned short  distance;
  unsigned short  max_speed;
  char  wave_length[7];
}otuChannelProperty;

typedef struct
{
  unitProperty_t unit_property;
  //uchar_t otu_type;
  uchar_t channel_count;
  uchar_t channel_port_count;
  uchar_t port_type;
  otuChannelProperty channel_property[OTU_MAX_CHANNEL_COUNT];
}otuProperty_t;


typedef struct
{
  unsigned  short distance;//传输距离
  unsigned  short max_speed;//最大速率
  unsigned  short wave_lenght;//端口1波长
  unsigned  short current_speed;//速率
}otuPortProperty_t;

typedef struct
{
 uchar_t port_status; //工作状态 1—正常 2—不正常
 uchar_t on_module; //在位状态
 uchar_t tx_enable; //TX关断状态 1—enable 2—disable
}otuPortStatus_t;

typedef struct
{
 unsigned short current_speed;
 otuPortStatus_t  port_status[OTU_EVERY_CHANNEL_PORT_COUNT];
}otuChannelStatus_t;

typedef struct
{
 char wave_length[8];
 char distance[6];
 char max_speed[6];
 char current_speed[6];
 uchar_t port_count;
 uchar_t rx_status[2];
 uchar_t tx_status[2];
 uchar_t tx_enable[2];
}channel_t;

typedef struct
{
  uchar_t channel_count;
  channel_t channel[OTU_MAX_CHANNEL_COUNT];
}otuInfo_t;

typedef struct
{
  uchar_t reserve1;
  uchar_t reserve2;
  uchar_t work_mode;
  uchar_t run_status; 
  otuPortStatus_t portStatus[8];
}otuUnitStatus_t;

typedef struct
{
  uchar_t loopback;
  uchar_t fec;
  uchar_t reserve1;
  uchar_t reserve2;
}otuPortFEC_t;

//OTU单元盘SFP端口属性(报文)信息集 UNIT_SFP_PROPERTY_INFOSET_ID
otuPortProperty_t *getOtuPortProperty(int chassis,int slot,int port);
//OTU单元盘(报文)信息集OTU_STATUS_INFOSET_ID
otuUnitStatus_t *getOtuStatus(int chassis,int slot);
//OTU单元盘(报文)信息集 OTU_SFP_FEC_INFOSET_ID
otuPortFEC_t *getOtuFec(int chassis,int slot,int port);

int insertOtuInfoSets(uchar_t chassis,uchar_t slot,otuProperty_t otu);
//bool_t getOtuInfo(uchar_t chassis,uchar_t slot,uchar_t channel,otuInfo_t *info);
otuInfo_t *getOtuInfo(uchar_t chassis,uchar_t slot,uchar_t channel);
otuProperty_t *getOtuProperty(uchar_t chassis,uchar_t slot,uchar_t index);
otuChannelStatus_t *getOtuChannelStatus(uchar_t chassis,uchar_t slot,uchar_t group);

/*设置OTU工作模式*/
int setOtuWorMode(int chassis,int slot,uchar_t mode);
/*设置OTU光模块速率*/
int setOtuPortSpeed(int chassis,int slot,int port,unsigned short speed);
/*设置OTU光模块发送端关断状态*/
int setOtuPortTx(int chassis,int slot,int port,uchar_t enale);
/*设置OTU光模块 发送端关断状态 自环及FEC ，loopback或fec为0时则不设置该项*/
int setOtuLoopBackAndFec(int chassis,int slot,int port,uchar_t loopback,uchar_t fec);

void setStartUpService(char service_id,char boot,char watchdog);
int setConfValue(char *conf_path,char *conf_name,char *config_buff,char *config_sign);
int getConfValue(char *conf_path,char *conf_name,char *config_buff,char *config_sign);
int addConfItem(char *conf_path,char *conf_name,char *config_buff);
int getConfItems(char *conf_path,char *item_name,char items_buff[][20],int item_count,char  *item_delim);
int removeConfItem(char *conf_path,char *conf_name,char *config_buff);
#endif /* __OTU_H */
