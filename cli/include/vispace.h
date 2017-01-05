#ifndef _VISPACE_H_
#define _VISPACE_H_


/**************************DDM_INFOSET***************************************/
#define DDM_InfoSet_ID 11
#define Optical1_RX_Power_Item_ID 1
#define Optical1_TX_Power_Item_ID 2
#define Optical1_Voltage_Item_ID 3
#define Optical1_Ibias_Item_ID 4
#define Optical1_Temperature_Item_ID 5

#define Optical2_RX_Power_Item_ID 6
#define Optical2_TX_Power_Item_ID 7
#define Optical2_Voltage_Item_ID 8
#define Optical2_Ibias_Item_ID 9
#define Optical2_Temperature_Item_ID 10

/**************************DDM_Threshold_InfoSet***************************************/
#define DDM_Threshlod_InfoSet_ID 12
#define Optical1_RX_Power_Low_Threshold_Item_ID 1
#define Optical1_RX_Power_High_Threshold_Item_ID 2

#define Optical1_TX_Power_Low_Threshold_Item_ID 3
#define Optical1_TX_Power_High_Threshold_Item_ID 4

#define Optical1_Voltage_Low_Threshold_Item_ID 5
#define Optical1_Voltage_High_Threshold_Item_ID 6

#define Optical1_Ibias_Low_Threshold_Item_ID 7
#define Optical1_Ibias_High_Threshold_Item_ID 8

#define Optical1_Temperature_Low_Threshold_Item_ID 9
#define Optical1_Temperature_High_Threshold_Item_ID 10

#define Optical2_RX_Power_Low_Threshold_Item_ID 11
#define Optical2_RX_Power_High_Threshold_Item_ID 12

#define Optical2_TX_Power_Low_Threshold_Item_ID 13
#define Optical2_TX_Power_High_Threshold_Item_ID 14

#define Optical2_Voltage_Low_Threshold_Item_ID 15
#define Optical2_Voltage_High_Threshold_Item_ID 16

#define Optical2_Ibias_Low_Threshold_Item_ID 17
#define Optical2_Ibias_High_Threshold_Item_ID 18

#define Optical2_Temperature_Low_Threshold_Item_ID 19
#define Optical2_Temperature_High_Threshold_Item_ID 20


/**************************UNIT_Property_InfoSet***************************************/
#define UNIT_Property_InfoSet_ID 10
#define unit_class_Item_ID	 1  //单元盘大类
#define unit_type_Item_ID	2   //单元盘小类
#define hardware_type_Item_ID  3//单元盘硬件类型
#define firmware_type_Item_ID  4//单元盘固件类型
#define struct_type_Item_ID   5 //单元盘结构类型
#define port_num_Item_ID     6
#define channel_num_Item_ID  7
#define port_per_channel_num_Item_ID 8
#define port_type_Item_ID	   9//端口类型

/**************************OTU_Status_InfoSet***************************************/
#define OTU_Status_InfoSet_ID 51
/**************************OTU_Status_InfoSet***************************************/
#define OTU_Port_Func_InfoSet_ID 52
/**************************XFP_Property_InfoSet_t***************************************/
#define XFP_Property_InfoSet_ID 50

/**************************Misc_Set_InfoSet_ID***************************************/
#define Misc_Set_InfoSet_ID 54
#define Mode_Set_Item_ID 1
#define Rate_Set_Item_ID 2
#define Loopback_Set_Item_ID 3
#define PRBS_Set_Item_ID 4
#define FEC_Set_Item_ID 5
/**************************otu_fix_Info_ID***************************************/
#define otu_fix_Info_ID 100
/**************************DDM_Threshlod_InfoSet_ID***************************************/
#define otu_fix_Set_ID 100
#define UNIT_Property_Set_ID 10
#define XFP_Property_Set_ID 50
#define UNIT_Otu_Set_ID 12

#define Unit_Base_InfoSet_Update_Flag 1
#define Unit_Property_InfoSet_Update_Flag 2
#define XFP_Property_InfoSet_Update_Flag 3
#define OTU_Status_InfoSet_Update_Flag 4
#define OTU_Port_Func_InfoSet_Update_Flag 5
#define DDM_InfoSet1_Update_Flag 6
#define DDM_InfoSet2_Update_Flag 7

void init_infoset_alarm();
void Reflesh_Data_Alarm();
int  parseMessage(char *message);
void PackageDataAlarm();


#endif

