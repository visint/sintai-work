#ifndef __EDFA_H
#define __EDFA_H 1

//EDFA������Ϣ
typedef struct
{
  short working_current;//���ֹ�������
  short power; //���ֹ���
  short temperature;//�����¶�
  short cool_current;//�����������
  char   reserve1;
  char   reserve2; 
}edfaPumpStatus_t;

//EDFA״̬��Ϣ 
typedef struct
{
  char   run_led;
  char   in_led;
  char   out_led;
  char   pta_led;
  char   mta_led;
  char   pump1_status;//����1�ض�״̬,1���� 2:�ر�
  char   pump2_status;//����2�ض�״̬,1���� 2:�ر�
  char   work_mode;    //����ģʽ,1��AGC  2��APC
  short inputPower;  //ģ������⹦��
  short outputPower; //ģ������⹦��
  short temperature; //ģ���¶�
  short agc_or_apc_value;//����ģʽΪAGCʱΪ���棨0-200����ΪAPCʱ����⹦�ʣ���λΪ0.1dBm
  edfaPumpStatus_t pump_status[2];
}edfaInfo_t;

//���ָ澯��ֵ
typedef struct
{
  char   minTemperature;//ģ����С�¶�
  char   maxTemperature;//ģ������¶�
  short  minCurrent;    //������С����
  short  maxCurrent;    //����������
}pumpThreshold_t;

//EDFA�澯��ֵ
typedef struct
{
  char  minInputPower;//ģ����С����⹦��
  char  minOutputPower;//ģ����С����⹦��
  char  minTemperature;//ģ����С�¶�
  char  maxTemperature;//ģ����С�¶�
  pumpThreshold_t pump[2];
}edfaThreshold_t;

//��ȡEDFA��Ϣ
edfaInfo_t *getEdfaInfo(uchar_t chassis,uchar_t slot);
//��ȡEDFA�澯��ֵ
edfaThreshold_t *getEdfaThreshold(uchar_t chassis,uchar_t slot);
int insertEdfaInfoSets(uchar_t chassis,uchar_t slot);
#endif /* __EDFA_H */
