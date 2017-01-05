/*
 * shell_execute.c
 */
#include "private.h"
#include "lub/string.h"
#include "lub/argv.h"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <signal.h>
#include <fcntl.h>

#include <typedefs.h>
#include <sysinfo.h>

#include <unit.h>
#include <visipc.h>
#include <olp.h>
#include <chassis.h>
#include <otu.h>
#include <vispace.h>
#include <OTU_4U.h>
#include <ddm.h>
#include <edfa.h>


/*
 * These are the internal commands for this framework.
 */

static int CurrentSlot = 0 ;

int CurrentBloadNum  ;

extern char cliSaveName[128];

extern int   saveSlotid;
extern long  saveSlotType;
#define CLI_CONF_FILE "/vispace/sys.conf"

static clish_shell_builtin_fn_t
clish_close,
clish_overview,
clish_source,
clish_source_nostop,
clish_history,
clish_nested_up,
clish_nop,
clish_wdog;
// INSERT INTO "TableValue"('type','value') VALUES('company','jiang');
// INSERT INTO "TableValue"('type','value') VALUES('devType','1');

static bool_t
shell_dev_set_Company(const clish_context_t *context,
                      const lub_argv_t    *argv)
{
    const char    *company   = lub_argv__get_arg(argv,0);

    const char    *devType   = lub_argv__get_arg(argv,1);
	
    nvram_set("company",company);

	nvram_set("devType",devType);

}

static bool_t
shell_upgrade_config(const clish_context_t *shell,
                     const lub_argv_t    *argv)
{

    const char    *arg   = lub_argv__get_arg(argv,0);

//  printf("shell_upgrade_config  %s\n",arg);

    es_update_config( arg );

}


static bool_t
shell_upgrade_image(const clish_context_t *shell,
                    const lub_argv_t    *argv)
{

    const char    *arg   = lub_argv__get_arg(argv,0);

//    printf("shell_upgrade_image  %s\n",arg);

//   es_update_image( arg );
    es_download_config( arg );

}


static bool_t
shell_download_config(const clish_context_t *shell,
                      const lub_argv_t    *argv)
{

    const char     *arg   = lub_argv__get_arg(argv,0);

//   printf("shell_download_config  %s\n",arg);

    es_download_config( arg );

}



static bool_t
shell_show_module(const clish_context_t *shell,
                  const lub_argv_t    *argv)
{



}



static bool_t
shell_set_ipaddr(const clish_context_t *shell,
                 const lub_argv_t    *argv)
{
    long visChasissIndex = 1;
    long visModuleIndex = 1 ;
    long visPortIndex,visEnetPortType ;
    const char    *arg   = lub_argv__get_arg(argv,0);

    const char    *mask   = lub_argv__get_arg(argv,1);

    const char    *gate   = lub_argv__get_arg(argv,2);


//    printf("shell_set_ipaddr  %s\n",arg);

    setDeviceIp(arg,mask,gate);

}

static bool_t
shell_set_gateway(const clish_context_t *shell,
                  const lub_argv_t    *argv)
{
    long visChasissIndex = 1;
    long visModuleIndex = 1 ;
    long visPortIndex,visEnetPortType ;
    const char    *arg   = lub_argv__get_arg(argv,0);

    printf("shell_set_ipaddr  %s\n",arg);

    setDeviceIp(NULL,NULL,arg);

}



static bool_t
shell_show_snmp(const clish_context_t *shell,
                const lub_argv_t    *argv)
{

    show_snmp_com( );


}

static bool_t
shell_set_snmp_com(const clish_context_t *shell,
                   const lub_argv_t    *argv)
{
    long visChasissIndex = 1;
    long visModuleIndex = 1 ;
    long visPortIndex,visEnetPortType ;

    const char                   *ipaddr  = lub_argv__get_arg(argv,0);
    const char                   *rocomm = lub_argv__get_arg(argv,1);
    const char                   *rwcomm = lub_argv__get_arg(argv,2);
    const int                   readonly = atoi(lub_argv__get_arg(argv,3));
    const int                   enable = atoi(lub_argv__get_arg(argv,4));
    if(strcmp(ipaddr,"255.255.255.255") == 0)
    {
        insertCommunity(NULL,rocomm ,rwcomm ,161,readonly ,enable);
    }
    else
    {

        insertCommunity(ipaddr,rocomm ,rwcomm ,161,readonly ,enable);
    }

}

static bool_t
shell_delete_snmp_com(const clish_context_t *shell,
                      const lub_argv_t    *argv)
{
    const int                   index   = atoi(lub_argv__get_arg(argv,0));

    deleteCommunityById(index);

}


static bool_t
shell_set_snmp_port(const clish_context_t *shell,
                    const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);


}

static bool_t
shell_set_snmp_trap(const clish_context_t *shell,
                    const lub_argv_t    *argv)
{
    const char                   *name   = lub_argv__get_arg(argv,0);
    const char                   *arg   = lub_argv__get_arg(argv,1);
    const char                   *arg1 = lub_argv__get_arg(argv,2);
    const int                   enable = atoi(lub_argv__get_arg(argv,3));

    short port = atoi(arg1);


    if(insertDestHost(name,arg,port,enable) == 0)
    {

        printf("snmp add trap success \n");
    }
    else
    {

        printf("snmp add trap error \n");
    }


}

static bool_t
shell_delete_snmp_trap(const clish_context_t *shell,
                       const lub_argv_t    *argv)
{

    const char                   *arg   = lub_argv__get_arg(argv,0);

    const char                   *arg1   = lub_argv__get_arg(argv,1);

    short port = atoi(arg1);

    if(deleteDestHost(arg,port) == 0)
    {

        printf("snmp delete trap success \n");
    }
    else
    {

        printf("snmp delete trap error \n");
    }


}

static bool_t
clish_test_show_share_mem(const clish_context_t *shell,
                          const lub_argv_t    *argv)
{
    long visChasissIndex = 1;
    long visModuleIndex = 1 ;
    long visPortIndex ;
    const char                   *arg   = lub_argv__get_arg(argv,0);
    const char                   *arg1 = lub_argv__get_arg(argv,1);
    const char                   *arg2 = lub_argv__get_arg(argv,2);
    clish_shell_t *this = (clish_shell_t *)shell;

    visChasissIndex	 =  atoi(arg);
    visModuleIndex   = atoi(arg1);
    visPortIndex       = atoi(arg2);


}




static bool_t
clish_set_vispace_enetport_type(const clish_context_t *shell,
                                const lub_argv_t    *argv)
{
    long visChasissIndex = 1;
    long visModuleIndex = 1 ;
    long visPortIndex,visEnetPortType ;
    const char                   *arg   = lub_argv__get_arg(argv,0);
    const char                   *arg1 = lub_argv__get_arg(argv,1);
    const char                   *arg2 = lub_argv__get_arg(argv,2);
    const char                   *arg3 = lub_argv__get_arg(argv,3);


    printf("clish_set_vispace_enetport_type %s %s %s %s\n",arg,arg1,arg2,arg3);
    visChasissIndex	 =  atoi(arg);
    visModuleIndex   = atoi(arg1);
    visPortIndex       = atoi(arg2);
    visEnetPortType   = atoi(arg3);
    set_vispace_enetport_type( visChasissIndex,visModuleIndex,visPortIndex,visEnetPortType);

}
/*----------------------------------------------------------- */

static bool_t
clish_reset_bload(const clish_context_t *shell,
                  const lub_argv_t    *argv)
{
    long visChasissIndex = 1;
    long visModuleIndex = 1 ;
    long visPortIndex,visEnetPortType ;
    const char                   *arg   = lub_argv__get_arg(argv,0);
    clish_shell_t *this = (clish_shell_t *)shell->shell;

    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[atoi(arg)] == 0)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] doesn't exist\n",atoi(arg));
        return ;
    }


    restoreUnitConf(0,atoi(arg));

    printf("restore bload%d factory settings success\n",atoi(arg));

}
/*----------------------------------------------------------- */
int getOk(char* passwd, int size)
{
    int c;
    int n = 0;

    printf("Sure:");

    do
    {
        c=getchar();
        if (c != '\n'||c!='\r')
        {
            passwd[n++] = c;
        }
    }
    while(c != '\n' && c !='\r' && n < (size - 1));
    passwd[n] = '\0';
    return n;
}

static bool_t
clish_system_default(const clish_context_t *context,
                     const lub_argv_t    *argv)
{

    char *p,passwd[128],name[128];
    int role;

    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;

    const int                   withip = atoi(lub_argv__get_arg(argv,0));

    tinyrl_printf(this->tinyrl,"Are you sure to restore factory setting(y/n)\n");
    getOk(passwd, sizeof(passwd));
    p=passwd;
    while(*p!='\n')
        p++;
    *p='\0';
    printf("\n");
    if(strcmp(passwd,"y")!= 0)
    {

        tinyrl_printf(this->tinyrl,"you want to restore factory setting must input y\n");

        return BOOL_TRUE;
    }
    if(withip == 1)
    {
        restoreNmuConf(0,1,1);
    }
    else
    {

        restoreNmuConf(0,0,1);
    }
    printf("restore factory settings success\n");
    return BOOL_TRUE;


}

/*
#define  INFO_SET_MAX_COUNT 170
#define  INFO_SET_MAX_COUNT_ERVERY_UNIT 10
#define  STATUS_SHM_KEY 71
#define  BASE_INFO_SHM_KEY 78
#define  INFO_HEAD_SIZE 2

#define  CMD_MESSAGE_OFFSET 36  //�����
#define  DATA_MESSAGE_OFFSET 72 //���ݱ���

//#define  INFO_SET_HEAD_OFFSET 96 //��ŵ�Ԫ��Infoset�Ľڵ㿪ʼλ��
//#define  INFO_SET_COUNT_OFFSET 112  //��ŵ�Ԫ��Infoset�Ľڵ���

#define  INFO_UNIT_PROPERTY_OFFSET 108 //��ŵ�Ԫ�����ԣ��粨������ÿ��Ԫ�̵�����Ϊ4���ֽ�
#define  INFO_UNIT_PROPERTY_SIZE 68

#define  INFO_SET_HEAD_OFFSET 176 //��ŵ�Ԫ��Infoset�Ľڵ����Ϣ����ڵ���
#define  INFO_SET_OFFSET 208  //��ŵ�Ԫ��Infoset�Ľڵ�
void test111 ()
{
 //clearShm(0);
 char n;
 static int   usi_shm_id = -1;
 static char *usi_shm_add=NULL;
 olp1P1Info_t *olp;
 char *p,buf[32]={1,1,0xE8,0xFE,0x88,0xFF,0xE8,0xFE,0xE8,0xFE,0x92,0xFF,0x4C,0xFF,0x4D,0x01,1,1,1,1,1,1,1,1,1};
 char * values;
    usi_shm_id = shmget(STATUS_SHM_KEY,INFO_HEAD_SIZE+sizeof(status_infoset_t)*INFO_SET_MAX_COUNT, 001000 | 0666);
    //printf("usi_shm_id =%d\n",usi_shm_id );
    if(usi_shm_id==-1)
    {
        perror("shmget error");
        return FALSE;
    }
    usi_shm_add=(char*)shmat(usi_shm_id,NULL,0);
    if(usi_shm_add == (void *) -1)
    {
      perror("shmat");
      printf("map the shm error: %s.\n",strerror(errno) );
      return FALSE;
    }
     printf("map the shm error: %0x.\n", usi_shm_add);

    usi_shm_add=  usi_shm_add+INFO_HEAD_SIZE;
    values=usi_shm_add+6;
    memcpy(values,buf,32);
    olp=(olp1P1Info_t *)values;
     printf("map the shm error: %0x %0x  %d.\n",values, &olp->rx1_power,olp->rx1_power);



    printf("\nSlot:%d\nmode=%d line=%d rule=%d ret_mode=%d rx1=%d wave_len=%d ret_time=%d rx1=%d rx2=%d tx=%d l1=%d l2=%d alm=%d\n",1,olp->mode,olp->line,olp->rule,olp->ret_mode,olp->rx1_led,olp->tx_wave_len,olp->ret_time,olp->rx1_power,olp->rx2_power,olp->tx_power,olp->l1_power,olp->l2_power,olp->tx_alm_power);
}
*/

char strbuftime[128];
char *parseTimeSeconds(int t)
{

    memset(strbuftime,0,128);
    int day, hour, minute, second;
    if (t >= 86400) //��,
    {
        day = t / 86400;
        hour = (t % 86400) / 3600;
        minute = ((t % 86400) % 3600) / 60;
        second = t % 86400 % 3600 % 60;
        sprintf(strbuftime,"%d day(s) %d hour(s) %d minute(s) %d second(s)",day,hour,minute,second);
    }
    else if (t >= 3600)//ʱ,
    {
        hour = t / 3600;
        minute = (t % 3600) / 60;
        second =  t % 3600 % 60;
        sprintf(strbuftime,"%d hour(s) %d minute(s) %d second(s)",hour,minute,second);
    }
    else if (t >= 60)//��
    {
        minute = t / 60;
        second = t % 60;
        sprintf(strbuftime,"%d minute(s) %d second(s)",minute,second);
    }
    else
    {
        second = t;
        sprintf(strbuftime,"%d second(s)",second);

    }
    return strbuftime;
}

static bool_t
clish_getsystem(const clish_context_t *context,
                const lub_argv_t    *argv)
{
    time_t now, tTemp;
    char tempStr[5];

    char mac[30];
    char ip_add[20];
    char mask[20];
    char gateway[20];
    float sys_usage;
    float user_usage;
    float mem_usage;
    struct mem_usage_t usage;
    unit_user_data_t udata;
    char output[64];
    char *input = "��C";


//  test111();

    clish_shell_t *this = (clish_shell_t *)context->shell;

    chassisThreshold_t *pThreshold = getChassisThreshold(0);

    if(pThreshold == NULL)
    {

        tinyrl_printf(this->tinyrl,"System wrong,please check!!!!\n");

        return BOOL_TRUE;

    }

    chassis_t *pChassis = getChassisInfo(0);

    if(pChassis == NULL)
    {

        tinyrl_printf(this->tinyrl,"Chassis wrong,please check!!!!\n");

        return BOOL_TRUE;

    }

    unit_base_info_t *info ;
//getBaseInfoMap
    info  = getBaseInfoMap(0 , CurrentBloadNum);
    /*
    	if(info == NULL)
        {

            tinyrl_printf(this->tinyrl,"info wrong,please check!!!!\n");

            return BOOL_TRUE;

        }
    */

    getUserDataFromDB(0 , CurrentBloadNum ,&udata);

    getCpuUsage(&sys_usage,&user_usage);

    mem_usage = getMemUsage(&usage);

    memset(mac,0,sizeof(mac));
    getMac(mac);
    getDeviceIp(ip_add,mask,gateway);

    now = time(NULL);

    tTemp = difftime(now , info->uptime);



//    struct tm *tSave = gmtime(&tTemp);

    /* the exception proves the rule... */

    argv = argv; /* not used */

    tinyrl_printf(this->tinyrl,"System Type: %s\n","Network Management Platform");

    u2g(udata.name, strlen(udata.name), output,64);
    tinyrl_printf(this->tinyrl,"System Name: %s\n",output);
    u2g(udata.location, strlen(udata.location), output,64);
    tinyrl_printf(this->tinyrl,"System Location: %s\n",output);
    u2g(udata.contact, strlen(udata.contact), output,64);
    tinyrl_printf(this->tinyrl,"System Contact: %s\n",output);
    u2g(udata.updev, strlen(udata.updev), output,64);
    tinyrl_printf(this->tinyrl,"System Up device: %s\n",output);
    u2g(udata.userdata, strlen(udata.userdata), output,64);
    tinyrl_printf(this->tinyrl,"System User data: %s\n",output);
    u2g(udata.service, strlen(udata.service), output,64);
    tinyrl_printf(this->tinyrl,"System Service: %s\n",output);



    tinyrl_printf(this->tinyrl,"System Model: %s\n",info->model);

    tinyrl_printf(this->tinyrl,"System Creation: %s\n",info->creation);

    tinyrl_printf(this->tinyrl,"System Sn: %s\n",info->sn);

    tinyrl_printf(this->tinyrl,"Hardware  Version: %s\n",info->hwver);

    tinyrl_printf(this->tinyrl,"Software  Version: %s\n",info->fwver);

    tinyrl_printf(this->tinyrl,"\nPower :\n");

    tinyrl_printf(this->tinyrl,"Power    State    Type   Current volt  Alarm min volt    Alarm max volt\n");

    if(pChassis->powersupply[0].exist == 1)
    {
        if(pChassis->powersupply[0].status == 1)
        {
            tinyrl_printf(this->tinyrl,"Power1    on       AC       %2.1f(V)         %d(V)              %d(V)\n",
                          (float)pChassis->powersupply[0].volt/10,pThreshold->min_volt,pThreshold->max_volt);
        }
        else
        {
            tinyrl_printf(this->tinyrl,"Power1   off       AC       %2.1f(V)         %d(V)              %d(V)\n",
                          (float)pChassis->powersupply[0].volt/10,pThreshold->min_volt,pThreshold->max_volt);

        }
    }

    if(pChassis->powersupply[1].exist == 1)
    {
        if(pChassis->powersupply[1].status == 1)
        {
            tinyrl_printf(this->tinyrl,"Power2    on       AC       %2.1f(V)         %d(V)              %d(V)\n",
                          (float)pChassis->powersupply[1].volt/10,pThreshold->min_volt,pThreshold->max_volt);
        }
        else
        {
            tinyrl_printf(this->tinyrl,"Power2   off       AC       %2.1f(V)         %d(V)              %d(V)\n",
                          (float)pChassis->powersupply[1].volt/10,pThreshold->min_volt,pThreshold->max_volt);

        }
    }

    tinyrl_printf(this->tinyrl,"\nTemperature :\n");

    tinyrl_printf(this->tinyrl,"Current temp     Alarm min temp       Alarm max temp\n");

    tinyrl_printf(this->tinyrl,"%.1f%s               %d%s                 %d%s\n",
                  (float)pChassis->temp/10,input,pThreshold->min_temperature,input,pThreshold->max_temperature,input);

    tinyrl_printf(this->tinyrl,"\nNetWork configure:\n");
    tinyrl_printf(this->tinyrl,"Ip address            Network               Gateway\n");

    tinyrl_printf(this->tinyrl,"%s       %s        %s\n",ip_add,mask,gateway);

    tinyrl_printf(this->tinyrl,"System Mac : %s\n",mac);

    tinyrl_printf(this->tinyrl,"Cpu utilization   Mem utilization\n");

    tinyrl_printf(this->tinyrl,"%2.2f\%%            %2.2f\%%\n",sys_usage+user_usage,mem_usage);

    /*
        if(strcpy(nvram_get("systemBuzz"),"1")== 0)
        {
            tinyrl_printf(this->tinyrl,"System Buzz: ON\n");
        }
        else
        {
            tinyrl_printf(this->tinyrl,"System Buzz: OFF\n");
        }
    */

//    tinyrl_printf(this->tinyrl,"Software Last Time: %s",ctime(&(info.uptime)));

    tinyrl_printf(this->tinyrl,"System Running Time: %s\n",parseTimeSeconds(tTemp));

    tinyrl_printf(this->tinyrl,"System Current Time: %s",ctime(&now));



    return BOOL_TRUE;
}

bool_t    test_power_value(char *value,int *outValue)
{
    char tt[] = "-99.8";
    float  temp ;
    float  maxValue  =  -88.3;
    float  minValue  =  -21.33;

    temp =	 atof(tt);

    printf("System Current Time: %.1f\n",temp);

    if( temp > maxValue  || temp < minValue)
    {

        return BOOL_FALSE;
    }
    else
    {
        *outValue = (int)(temp*10);
        return BOOL_TRUE;
    }
}

static bool_t
clish_show_bload(const clish_context_t *context,
                 const lub_argv_t    *argv)
{
    clish_shell_t *this = (clish_shell_t *)context->shell;

    uchar_t *status;
    unit_base_info_t *info;
    int n;
    status = getUnitStatusMap(0);

    int temp = 0;

    tinyrl_printf(this->tinyrl,"Slot         Type      Sn                  Model              Creation         SoftVersion      Hard Version\n");

    if (NULL!= status)
    {
        for (n=1 ; n< CurrentBloadNum; n++)
        {
            if (status[n] == olp)
            {
                info  = getBaseInfoMap(0 , n);
                temp++;
				if(getUnitType(0,n)== SEESION_OBP)
                {
                    tinyrl_printf(this->tinyrl,"slot[%2d]     OBP       %-13s %-25s %-16s  %-16s %-16s\n",n,info->sn,info->model,info->creation,info->fwver,info->hwver);
				}
				else
				{
				    tinyrl_printf(this->tinyrl,"slot[%2d]     OLP       %-13s %-25s %-16s  %-16s %-16s\n",n,info->sn,info->model,info->creation,info->fwver,info->hwver);
				}
            }
            else if (status[n] == otu)
            {
                info  = getBaseInfoMap(0 , n);
                temp++;
                if(getUnitType(0,n)== OTU_FOUR)
                {
                    tinyrl_printf(this->tinyrl,"slot[%2d]     OTU       %-13s %-25s %-16s  %-16s %-16s\n",n,info->sn,info->model,info->creation,info->fwver,info->hwver);
                }
                else if(getUnitType(0,n)== OTU_EIGHT)
                {
                    tinyrl_printf(this->tinyrl,"slot[%2d]     OTUE      %-13s %-25s %-16s  %-16s %-16s\n",n,info->sn,info->model,info->creation,info->fwver,info->hwver);

                }
				else if(getUnitType(0,n)== OTU_LIU_FOUR)
                {
                    tinyrl_printf(this->tinyrl,"slot[%2d]     OTUF      %-13s %-25s %-16s  %-16s %-16s\n",n,info->sn,info->model,info->creation,info->fwver,info->hwver);

                }
            }
            else if (status[n] == oeo)
            {
                info  = getBaseInfoMap(0 , n);
                temp++;
                if(getUnitType(0,n)== OEO_FOUR)
                {
                    tinyrl_printf(this->tinyrl,"slot[%2d]     OEO       %-13s %-25s %-16s  %-16s %-16s\n",n,info->sn,info->model,info->creation,info->fwver,info->hwver);
                }
                else if(getUnitType(0,n)== OEO_EIGHT)
                {
                    tinyrl_printf(this->tinyrl,"slot[%2d]     OEOE      %-13s %-25s %-16s  %-16s %-16s\n",n,info->sn,info->model,info->creation,info->fwver,info->hwver);

                }
				else if(getUnitType(0,n)== OEO_BEIGHT)
                {
                    tinyrl_printf(this->tinyrl,"slot[%2d]     OEOBE     %-13s %-25s %-16s  %-16s %-16s\n",n,info->sn,info->model,info->creation,info->fwver,info->hwver);

                }
            }			
            else if (status[n] == BLOAD_EDFA)
            {
                info  = getBaseInfoMap(0 , n);
                temp++;
                tinyrl_printf(this->tinyrl,"slot[%2d]     EDFA      %-13s %-25s %-16s  %-16s %-16s\n",n,info->sn,info->model,info->creation,info->fwver,info->hwver);
            }
			else if (status[n] == BLOAD_ETU)
            {
                info  = getBaseInfoMap(0 , n);
                temp++;
                tinyrl_printf(this->tinyrl,"slot[%2d]     ETU       %-13s %-25s %-16s  %-16s %-16s\n",n,info->sn,info->model,info->creation,info->fwver,info->hwver);
            }
            else if (status[n] == BLOAD_OSS)
            {
                info  = getBaseInfoMap(0 , n);
                temp++;
                tinyrl_printf(this->tinyrl,"slot[%2d]     OSS       %-13s %-25s %-16s  %-16s %-16s\n",n,info->sn,info->model,info->creation,info->fwver,info->hwver);
            }
			else if (status[n] == BLOAD_DVOA)
            {
                info  = getBaseInfoMap(0 , n);
                temp++;
                tinyrl_printf(this->tinyrl,"slot[%2d]     DVOA      %-13s %-25s %-16s  %-16s %-16s\n",n,info->sn,info->model,info->creation,info->fwver,info->hwver);
            }
            else if (status[n] == BLOAD_PASSIVE)
            {
                info  = getBaseInfoMap(0 , n);
                temp++;
                if(getUnitType(0,n)== BLOAD_OSU)
                {
                    tinyrl_printf(this->tinyrl,"slot[%2d]     OSU       %-13s %-25s %-16s  %-16s %-16s\n",n,info->sn,info->model,info->creation,info->fwver,info->hwver);
                }
                else if(getUnitType(0,n)== BLOAD_ODM)
                {
                    tinyrl_printf(this->tinyrl,"slot[%2d]     ODM       %-13s %-25s %-16s  %-16s %-16s\n",n,info->sn,info->model,info->creation,info->fwver,info->hwver);
                }
                else if(getUnitType(0,n)== BLOAD_AWG)
                {
                    tinyrl_printf(this->tinyrl,"slot[%2d]     AWG       %-13s %-25s %-16s  %-16s %-16s\n",n,info->sn,info->model,info->creation,info->fwver,info->hwver);
                }
            }

        }
    }

    info  = getBaseInfoMap(0 , CurrentBloadNum);
    tinyrl_printf(this->tinyrl,"slot[%2d]     NMU       %-13s %-25s %-16s  %-16s %-16s\n",CurrentBloadNum,info->sn,info->model,info->creation,info->fwver,info->hwver);


    return BOOL_TRUE;
}




static bool_t
clish_exit(const clish_context_t *context,
           const lub_argv_t    *argv)
{
    exit(1);
    return BOOL_TRUE;
}

static bool_t
shell_session_bload(const clish_context_t *context,
                    const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;

    CurrentSlot = atoi(arg);

    tinyrl_printf(this->tinyrl,"reset bolad %d\n",CurrentSlot);

    return BOOL_TRUE;
}

int li_show_users()
{
    char	*userid,*usergrp;
    short uselevel;
    bool_t enable;
    int	row;
    printf("User name     User role \n");

    row = 0;
    umOpen();
    umRestore("/vispace/webs/umconfig.txt");
    //umRestore(UM_TXT_FILENAME);
    userid = umGetFirstUser();
    if (NULL!=userid)
    {
        uselevel= umGetUserLevel(userid);
        usergrp=umGetUserGroup(userid);
        enable=umGetUserEnabled(userid);
    }
    else
        printf("Not user found!\n");
    while (userid)
    {
        printf("%-15s ",userid);
        if(uselevel == 1)
        {
            printf("admin\n",uselevel);
        }
        else
        {
            printf("normal\n",uselevel);


        }

        userid = umGetNextUser(userid);
        if (NULL!=userid)
        {
            usergrp=umGetUserGroup(userid);
            uselevel= umGetUserLevel(userid);
            enable=umGetUserEnabled(userid);
        }

    }
    umClose();

}

static bool_t
shell_show_users(const clish_context_t *context,
                 const lub_argv_t    *argv)
{

    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;

    tinyrl_printf(this->tinyrl,"All users :\n");

    li_show_users();


    return BOOL_TRUE;
}

int li_show_deleteuser(char *name)
{
    umOpen();
    umRestore("/vispace/webs/umconfig.txt");
    umDeleteUser(name);
    umCommit("/vispace/webs/umconfig.txt");
    umClose();
    system("killall -9 webs");
    system("cd /vispace/webs && ./webs&");

    return 0;
}

static bool_t
shell_delete_user(const clish_context_t *context,
                  const lub_argv_t    *argv)
{
    const char  *arg   = lub_argv__get_arg(argv,0);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;

    if(	strcmp(arg,"admin") == 0)
    {
        tinyrl_printf(this->tinyrl,"admin can't delete\n");

        return BOOL_TRUE;
    }

    temp = li_show_getuser(arg);

    if(temp != 1)
    {

        tinyrl_printf(this->tinyrl,"no such user\n");

        return BOOL_TRUE;
    }


    temp =  li_show_deleteuser(arg);

    if(temp == 0)
    {

        tinyrl_printf(this->tinyrl,"delete success\n");
    }
    else
    {

        tinyrl_printf(this->tinyrl,"can't delete\n");
    }



    return BOOL_TRUE;
}

typedef struct t_setFixInfo
{
    short index;
    short portid;
    char  info[256];
} setFixInfo;

typedef struct
{
    char  if_property[20];
    char  osu_type;//OSU ���� 1-FBT 2-PLC
    char  mode; //OSU ����ģ.1--��ģ 3-��ģ
    char  band_width;//����.1-������ 2- ˫���� 3- ������ 4-ȫ����
    char  groups;//����.1-6��
    char  split_rate;//�ֹ��
    char  in_if_type; //����ӿ�����.1-LC/UPC 2-SC/UPC 3-LC/APC 4-SC/APC
    char  out_if_type;//����ӿ�����.1-LC/UPC 2-SC/UPC 3-LC/APC 4-SC/APC
    char  reserved[20];//Ԥ��
} osuProperty_t;



static bool_t
shell_setslot_info(const clish_context_t *context,
                   const lub_argv_t    *argv)
{
    clish_shell_t *this = (clish_shell_t *)context->shell;

    int   slotId  = atoi(lub_argv__get_arg(argv,0));
    const char                   *sn   = lub_argv__get_arg(argv,1);
    const char                   *model   = lub_argv__get_arg(argv,2);
    const char                   *creation   = lub_argv__get_arg(argv,3);
    const char                   *hwver   = lub_argv__get_arg(argv,4);
    setFixInfo   tSetFixInfo;
    memset(&tSetFixInfo,0,sizeof(tSetFixInfo));
    tSetFixInfo.index  = otu_fix_Set_ID;

    fix_Info_t  *slotInfo;

    unit_base_info_t *nmu_bi;

    slotInfo = (fix_Info_t  *)tSetFixInfo.info;

    sprintf(slotInfo->sn,"%s",sn);
    sprintf(slotInfo->model,"%s",model);
    sprintf(slotInfo->creation,"%s",creation);
    sprintf(slotInfo->hwver,"%s",hwver);



    if(slotId == CurrentBloadNum)
    {
        setNmuBaseInfo(0,sn,hwver,creation,model);
        return ;
    }

    nmu_bi =	getBaseInfoMap(0,slotId);

    sprintf(slotInfo->fwver,"%s",nmu_bi->fwver);


    if (EXIT_FAILURE != ioSet(0,(char)slotId , &tSetFixInfo,6 ))
    {
        tinyrl_printf(this->tinyrl,"set success\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"set error!\n");
    }


}

static bool_t
shell_setotu_info(const clish_context_t *context,
                  const lub_argv_t    *argv)
{
    clish_shell_t *this = (clish_shell_t *)context->shell;

    int   portId  = atoi(lub_argv__get_arg(argv,0));
    int   port_per_channel_num   = atoi(lub_argv__get_arg(argv,1));
    int   port_type   = atoi(lub_argv__get_arg(argv,2));

    setFixInfo   tSetFixInfo;
    memset(&tSetFixInfo,0,sizeof(tSetFixInfo));
    tSetFixInfo.index  = UNIT_Property_Set_ID;
    tSetFixInfo.portid = (short)portId;

    Unit_Property_InfoSet_t  *slotInfo;

    slotInfo = (Unit_Property_InfoSet_t  *)tSetFixInfo.info;

    slotInfo->port_num = (char)portId;
    slotInfo->xfp_property_source= (char)port_per_channel_num;
    slotInfo->port_type = port_type;

    if (EXIT_FAILURE != ioSet(0,(char)saveSlotid , &tSetFixInfo,6 ))
    {
        tinyrl_printf(this->tinyrl,"set success\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"set error!\n");
    }


}

static bool_t
shell_setotu_extern(const clish_context_t *context,
                    const lub_argv_t    *argv)
{
    clish_shell_t *this = (clish_shell_t *)context->shell;

    char   *externInfo  = lub_argv__get_arg(argv,0);


    setFixInfo   tSetFixInfo;
    memset(&tSetFixInfo,0,sizeof(tSetFixInfo));
    tSetFixInfo.index  = 107;
    tSetFixInfo.portid = (short)0;
    osuProperty_t  *slotInfo;

    slotInfo = (osuProperty_t  *)tSetFixInfo.info;
    if(externInfo[0]=='a')
    {
        slotInfo->osu_type=1;
        slotInfo->mode=1;
        slotInfo->band_width=1;
        slotInfo->groups=6;
        slotInfo->split_rate=1;
        slotInfo->in_if_type=1;
        slotInfo->out_if_type=1;
		memset(slotInfo->if_property,1,20);
        memset(slotInfo->reserved,1,20);
    }
    else if(externInfo[0]=='b')
    {
        slotInfo->osu_type=1;
        slotInfo->mode=1;
        slotInfo->band_width=1;
        slotInfo->groups=6;
        slotInfo->split_rate=1;
        slotInfo->in_if_type=1;
        slotInfo->out_if_type=1;   
    }
    else  if(externInfo[0]=='c')
    {
        slotInfo->osu_type=1;
        slotInfo->mode=1;
        slotInfo->band_width=1;
        slotInfo->groups=6;
        slotInfo->split_rate=1;
        slotInfo->in_if_type=1;
        slotInfo->out_if_type=1;   
    }else {

	    return;
    }


    if (EXIT_FAILURE != ioSet(0,(char)saveSlotid , &tSetFixInfo,6 ))
    {
        tinyrl_printf(this->tinyrl,"set success\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"set error!\n");
    }
}

typedef struct
{
    char pwr1_tune[16];
    char pwr2_tune[16];
    char pwr3_tune[16];
    char pwr4_tune[16];
} fiber_pwr_tune;

#define olp_Set_pwr_ID 105


static bool_t
shell_setolp_info(const clish_context_t *context,
                  const lub_argv_t    *argv)
{
    clish_shell_t *this = (clish_shell_t *)context->shell;

    char *  pwr1_tune   = lub_argv__get_arg(argv,0);
    char *  pwr2_tune   = lub_argv__get_arg(argv,1);
    char *  pwr3_tune   = lub_argv__get_arg(argv,2);

    setFixInfo   tSetFixInfo;
    memset(&tSetFixInfo,0,sizeof(tSetFixInfo));
    tSetFixInfo.index  = olp_Set_pwr_ID;
    tSetFixInfo.portid = 0;

    fiber_pwr_tune  *slotInfo;

    slotInfo = (fiber_pwr_tune  *)tSetFixInfo.info;

    sprintf(slotInfo->pwr1_tune ,"%s",pwr1_tune);
    sprintf(slotInfo->pwr2_tune ,"%s",pwr2_tune);
    sprintf(slotInfo->pwr3_tune ,"%s",pwr3_tune);

    if (EXIT_FAILURE != ioSet(0,(char)saveSlotid , &tSetFixInfo,6 ))
    {
        tinyrl_printf(this->tinyrl,"set success\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"set error!\n");
    }


}

static bool_t
shell_setoss_info(const clish_context_t *context,
                  const lub_argv_t    *argv)
{
    clish_shell_t *this = (clish_shell_t *)context->shell;

    char *  pwr1_tune   = lub_argv__get_arg(argv,0);
    char *  pwr2_tune   = lub_argv__get_arg(argv,1);
    char *  pwr3_tune   = lub_argv__get_arg(argv,2);
    char *  pwr4_tune   = lub_argv__get_arg(argv,3);

    setFixInfo   tSetFixInfo;
    memset(&tSetFixInfo,0,sizeof(tSetFixInfo));
    tSetFixInfo.index  = olp_Set_pwr_ID;
    tSetFixInfo.portid = 0;

    fiber_pwr_tune  *slotInfo;

    slotInfo = (fiber_pwr_tune  *)tSetFixInfo.info;

    sprintf(slotInfo->pwr1_tune ,"%s",pwr1_tune);
    sprintf(slotInfo->pwr2_tune ,"%s",pwr2_tune);
    sprintf(slotInfo->pwr3_tune ,"%s",pwr3_tune);
    sprintf(slotInfo->pwr4_tune ,"%s",pwr4_tune);

    if (EXIT_FAILURE != ioSet(0,(char)saveSlotid , &tSetFixInfo,6 ))
    {
        tinyrl_printf(this->tinyrl,"set success\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"set error!\n");
    }


}



static bool_t
shell_setotuport_info(const clish_context_t *context,
                      const lub_argv_t    *argv)
{
    clish_shell_t *this = (clish_shell_t *)context->shell;

    int   portId  = atoi(lub_argv__get_arg(argv,0));
    int   port_distance   = atoi(lub_argv__get_arg(argv,1));
    int   port_max_rate   = atoi(lub_argv__get_arg(argv,2));
    int   port_wave_length   = atoi(lub_argv__get_arg(argv,3));
    setFixInfo   tSetFixInfo;
    memset(&tSetFixInfo,0,sizeof(tSetFixInfo));
    tSetFixInfo.index  = XFP_Property_Set_ID;
    tSetFixInfo.portid = (short)portId;

    XFP_Property_InfoSet_t  *slotInfo;

    slotInfo = (XFP_Property_InfoSet_t  *)tSetFixInfo.info;

    if(portId == 1)
    {

        slotInfo->port1_distance=port_distance;
        slotInfo->port1_max_rate=port_max_rate;
        slotInfo->port1_wave_length=port_wave_length;
    }
    else    if(portId == 2)
    {
        slotInfo->port2_distance=port_distance;
        slotInfo->port2_max_rate=port_max_rate;
        slotInfo->port2_wave_length=port_wave_length;

    }
    else    if(portId == 3)
    {

        slotInfo->port3_distance=port_distance;
        slotInfo->port3_max_rate=port_max_rate;
        slotInfo->port3_wave_length=port_wave_length;

    }

    else    if(portId == 4)
    {

        slotInfo->port4_distance=port_distance;
        slotInfo->port4_max_rate=port_max_rate;
        slotInfo->port4_wave_length=port_wave_length;


    }




    if (EXIT_FAILURE != ioSet(0,(char)saveSlotid , &tSetFixInfo,6 ))
    {
        tinyrl_printf(this->tinyrl,"set success\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"set error!\n");
    }


}



static bool_t
shell_setotuview_info(const clish_context_t *context,
                      const lub_argv_t    *argv)
{
    clish_shell_t *this = (clish_shell_t *)context->shell;

    int   value1          = atoi(lub_argv__get_arg(argv,0));
    int   value2          = atoi(lub_argv__get_arg(argv,1));
    int   value3          = atoi(lub_argv__get_arg(argv,2));
    int   value4          = atoi(lub_argv__get_arg(argv,3));
    int   value5          = atoi(lub_argv__get_arg(argv,4));
    int   value6          = atoi(lub_argv__get_arg(argv,5));
    int   value7          = atoi(lub_argv__get_arg(argv,6));
    int   value8          = atoi(lub_argv__get_arg(argv,7));
    int   value9          = atoi(lub_argv__get_arg(argv,8));
    int   value10         = atoi(lub_argv__get_arg(argv,9));

    setFixInfo   tSetFixInfo;
    memset(&tSetFixInfo,0,sizeof(tSetFixInfo));
    tSetFixInfo.index  = UNIT_Otu_Set_ID;

    Unit_Otu_InfoSet_t  *slotInfo;

    slotInfo = (Unit_Otu_InfoSet_t  *)tSetFixInfo.info;

	slotInfo->vendor[0] = (char)value1;
	slotInfo->vendor[1] = (char)value2;
	slotInfo->vendor[2] = (char)value3;	
	slotInfo->vendor[3] = (char)value4;
	slotInfo->vendor[4] = (char)value5;
	slotInfo->vendor[5] = (char)value6;	
	slotInfo->vendor[6] = (char)value7;
	slotInfo->vendor[7] = (char)value8;
	slotInfo->vendor[8] = (char)value9;	
	slotInfo->vendor[9] = (char)value10;


    if (EXIT_FAILURE != ioSet(0,(char)saveSlotid , &tSetFixInfo,6 ))
    {
        tinyrl_printf(this->tinyrl,"set success\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"set error!\n");
    }
}




static bool_t
shell_setosuport_info(const clish_context_t *context,
                      const lub_argv_t    *argv)
{
    clish_shell_t *this = (clish_shell_t *)context->shell;

    int   portId  = atoi(lub_argv__get_arg(argv,0));
    int   port_per_channel_num   = atoi(lub_argv__get_arg(argv,1));
    int   port_type   = atoi(lub_argv__get_arg(argv,2));

    setFixInfo   tSetFixInfo;
    memset(&tSetFixInfo,0,sizeof(tSetFixInfo));
    tSetFixInfo.index  = UNIT_Property_Set_ID;
    tSetFixInfo.portid = (short)portId;

    Unit_Property_InfoSet_t  *slotInfo;

    slotInfo = (Unit_Property_InfoSet_t  *)tSetFixInfo.info;

    slotInfo->port_num = (char)portId;
    slotInfo->xfp_property_source= (char)port_per_channel_num;
    slotInfo->port_type = port_type;

    if (EXIT_FAILURE != ioSet(0,(char)saveSlotid , &tSetFixInfo,6 ))
    {
        tinyrl_printf(this->tinyrl,"set success\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"set error!\n");
    }
}

static bool_t
shell_setodmport_info(const clish_context_t *context,
                      const lub_argv_t    *argv)
{
    clish_shell_t *this = (clish_shell_t *)context->shell;

    int   portId  = atoi(lub_argv__get_arg(argv,0));
    int   port_per_channel_num   = atoi(lub_argv__get_arg(argv,1));
    int   port_type   = atoi(lub_argv__get_arg(argv,2));

    setFixInfo   tSetFixInfo;
    memset(&tSetFixInfo,0,sizeof(tSetFixInfo));
    tSetFixInfo.index  = UNIT_Property_Set_ID;
    tSetFixInfo.portid = (short)portId;

    Unit_Property_InfoSet_t  *slotInfo;

    slotInfo = (Unit_Property_InfoSet_t  *)tSetFixInfo.info;

    slotInfo->port_num = (char)portId;
    slotInfo->xfp_property_source= (char)port_per_channel_num;
    slotInfo->port_type = port_type;

    if (EXIT_FAILURE != ioSet(0,(char)saveSlotid , &tSetFixInfo,6 ))
    {
        tinyrl_printf(this->tinyrl,"set success\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"set error!\n");
    }


}



int li_show_adduser(char *name,char *pass,int role,int state)
{
    umOpen();
    umRestore("/vispace/webs/umconfig.txt");
    umAddUser( name, pass,"admin", 0 , state);
    umSetUserLevel(name,role);
    umCommit("/vispace/webs/umconfig.txt");
    umClose();
    system("killall -9 webs");
    system("cd /vispace/webs && ./webs&");
}

int li_show_getuser(char *name)
{
    int role;
    umOpen();
    umRestore("/vispace/webs/umconfig.txt");
    role = umUserExists(name);
    umClose();
    return role;
}

static bool_t
shell_add_user(const clish_context_t *context,
               const lub_argv_t    *argv)
{
    const char                   *sSol   = lub_argv__get_arg(argv,0);

    const char                   *arg   = lub_argv__get_arg(argv,1);

    int sState   = atoi(lub_argv__get_arg(argv,2));


    char *p,passwd[128],name[128];
    int role;

    if(	strcmp(sSol,"1") == 0)
    {

        role  = 1;

    }
    else
    {
        role  = 2;
    }


    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;


    if(	strlen(arg) > 100 )
    {
        tinyrl_printf(this->tinyrl,"user name is too long\n");

        return BOOL_TRUE;
    }


    if(	strcmp(arg,"admin") == 0)
    {
        tinyrl_printf(this->tinyrl,"admin can't delete\n");

        return BOOL_TRUE;
    }
    if(li_show_getuser(arg) == 1)
    {

        tinyrl_printf(this->tinyrl,"user already exists\n");

        return BOOL_TRUE;
    }

    //���ȹر�������ԣ�������������ʱ�Ͳ�����ʾ������ַ���Ϣ
    set_disp_mode(STDIN_FILENO,0);
    //����getpasswd��������û����������
    getpasswd(passwd, sizeof(passwd));
    p=passwd;
    while(*p!='\n')
        p++;
    *p='\0';
    printf("\n");

    set_disp_mode(STDIN_FILENO,1);

    li_show_adduser(arg,passwd,role,sState);

    tinyrl_printf(this->tinyrl,"add user success\n");

    system("killall -9 webs");
    system("cd /vispace/webs && ./webs&");

    return BOOL_TRUE;
}



int li_show_changepass(char *name,char *pass)
{

    umOpen();
    umRestore("/vispace/webs/umconfig.txt");
    umSetUserPassword(name, pass);
    umCommit("/vispace/webs/umconfig.txt");
    umClose();

    system("killall -9 webs");
    system("cd /vispace/webs && ./webs&");

}




static bool_t
shell_add_changepass(const clish_context_t *context,
                     const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    char *p,passwd[128],name[128];
    int role;

    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;

    if(	strlen(arg) > 100 )
    {
        tinyrl_printf(this->tinyrl,"user name is too long\n");

        return BOOL_TRUE;
    }

    if(li_show_getuser(arg)<1)
    {

        tinyrl_printf(this->tinyrl,"no such user\n");
        return BOOL_TRUE;

    }

    //���ȹر�������ԣ�������������ʱ�Ͳ�����ʾ������ַ���Ϣ
    set_disp_mode(STDIN_FILENO,0);
    //����getpasswd��������û����������
    getpasswd(passwd, sizeof(passwd));
    p=passwd;
    while(*p!='\n')
        p++;
    *p='\0';
    printf("\n");

    set_disp_mode(STDIN_FILENO,1);

    li_show_changepass(arg,passwd);

    tinyrl_printf(this->tinyrl,"change password success\n");

    return BOOL_TRUE;
}




static bool_t
shell_show_alarm(const clish_context_t *context,
                 const lub_argv_t    *argv)
{
    clish_shell_t *this = (clish_shell_t *)context->shell;

//    tinyrl_printf(this->tinyrl,"show alarm information\n");


    show_alarm_info();

    return BOOL_TRUE;
}

static bool_t
shell_delete_alarm(const clish_context_t *context,
                   const lub_argv_t    *argv)
{
    int alarmIndex;
    clish_shell_t *this = (clish_shell_t *)context->shell;
    alarmIndex   = atoi(lub_argv__get_arg(argv,0));

    tinyrl_printf(this->tinyrl,"delete alarm success\n");

    delete_alarm_info(alarmIndex);

    return BOOL_TRUE;
}



static bool_t
shell_show_alarmtestadd(const clish_context_t *context,
                        const lub_argv_t    *argv)
{
    clish_shell_t *this = (clish_shell_t *)context->shell;

    tinyrl_printf(this->tinyrl,"show alarm information\n");

    return BOOL_TRUE;
}

static bool_t
shell_set_time(const clish_context_t *context,
               const lub_argv_t    *argv)
{
    char buf[128];

    clish_shell_t *this = (clish_shell_t *)context->shell;

    const char                   *year   = lub_argv__get_arg(argv,0);

    const char                   *month   = lub_argv__get_arg(argv,1);

    const char                   *day   = lub_argv__get_arg(argv,2);

    const char                   *time   = lub_argv__get_arg(argv,3);

    sprintf(buf,"date -s \"%s-%s-%s %s\"&& hwclock --systohc",year,month,day,time);
    system(buf);

    tinyrl_printf(this->tinyrl,"set time success\n");

    return BOOL_TRUE;
}

static bool_t
shell_view_name(const clish_context_t *context,
                const lub_argv_t    *argv)
{
    const char *arg   = lub_argv__get_arg(argv,0);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;


    temp =  cli_edit_propom(cliSaveName,arg);

    if(temp == 1)
    {

        tinyrl_printf(this->tinyrl,"name change success\n");
    }
    else
    {

        tinyrl_printf(this->tinyrl,"can't change\n");
    }



    return BOOL_TRUE;
}

void nomarl_set_value(int index,short value)
{

    setInfoSet_t infoset;
    memset(&infoset,0 ,sizeof(setInfoSet_t));
    infoItem_t *pInfo = infoset.infoItems;
    infoset.chassis = 0;
    infoset.slot = saveSlotid;
    infoset.infoset_type = 'S';
    infoset.infoset_id = 65;
    infoset.index  = 1;

    pInfo->item_id  = index;
    pInfo->item_value = value;

    setOlpInfo(infoset);
}

void edfa_set_value(int index,short value)
{

    setInfoSet_t infoset;
    memset(&infoset,0 ,sizeof(setInfoSet_t));
    infoItem_t *pInfo = infoset.infoItems;
    infoset.chassis = 0;
    infoset.slot = saveSlotid;
    infoset.infoset_type = 'S';
    infoset.infoset_id = 58;
    infoset.index  = 1;

    pInfo->item_id  = index;
    pInfo->item_value = value;
    if(value == 1)
    {
        value = 0x100;
    }
    else
    {

        value = 0x200;
    }

    if (EXIT_FAILURE != setShortInfoItem(1,saveSlotid,58,1,6,value,0))
    {
        //   tinyrl_printf(this->tinyrl,"set success\n");
    }
    else
    {
        //  tinyrl_printf(this->tinyrl,"set error!\n");
    }
}

void oss_set_value(int channel,int index,short value)
{

    setInfoSet_t infoset;
    memset(&infoset,0 ,sizeof(setInfoSet_t));
    infoItem_t *pInfo = infoset.infoItems;
    infoset.chassis = 0;
    infoset.slot = saveSlotid;
    infoset.infoset_type = 'S';
    infoset.infoset_id = 65;
    infoset.index  = channel;

    pInfo->item_id  = index;
    pInfo->item_value = value;

//    printf("jiang %d %d %d\n",channel,index,value);
    setOlpInfo(infoset);
}

void otu_nomarl_set_value(short value)
{

    setOtuWorMode(0 , saveSlotid,value);
}


void otu_nomarl_set_setspeed(int index,short value)
{
    setOtuPortSpeed(0 ,saveSlotid , index,value);

}
void otu_nomarl_set_setloopback(int index,short value)
{

    setOtuLoopBackAndFec(0 , saveSlotid,index, value , 0 );
}
void otu_nomarl_set_setprbs(int index,short value)
{


}
void otu_nomarl_set_setfec(int index,short value)
{
    setOtuLoopBackAndFec(0 ,  saveSlotid,index, 0 , value);

}
void otu_nomarl_set_setfibersend(int index,short value)
{
    setOtuPortTx(0 , saveSlotid, index,value);
}

void system_set_value(int index,short value)
{
    setInfoSet_t infoset;
    memset(&infoset,0 ,sizeof(setInfoSet_t));
    infoItem_t *pInfo = infoset.infoItems;
    infoset.chassis = 0;
    infoset.slot = 18;
    infoset.infoset_type = 'S';
    infoset.infoset_id = 4;
    infoset.index  = 1;

//  printf("jiangyibo %d\n",index);
    pInfo->item_id  = index;
    pInfo->item_value = value;

    setChassisThreshold(infoset);
}


static bool_t
shell_otu_set_mode(const clish_context_t *context,
                   const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */

    clish_shell_t *this = (clish_shell_t *)context->shell;
    char*   status = getUnitStatusMap(0);

    if (NULL == status|| (status[saveSlotid] != otu &&status[saveSlotid] != oeo))
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not otu Or not exist\n",saveSlotid);
        return ;
    }

    if(strcmp(arg,"auto") == 0)
    {
        otu_nomarl_set_value(1);
    }
    else  if(strcmp(arg,"manual") == 0)
    {
        otu_nomarl_set_value(2);
    }

    return BOOL_TRUE;
}


static bool_t
shell_otu_setspeed(const clish_context_t *context,
                   const lub_argv_t    *argv)
{
    const int                   portId   = atoi(lub_argv__get_arg(argv,0));
    const char                   *arg   = lub_argv__get_arg(argv,1);
    /* the exception proves the rule... */
    int  temp;



    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;
    char*   status = getUnitStatusMap(0);
    if (NULL == status|| (status[saveSlotid] != otu &&status[saveSlotid] != oeo))
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not otu Or not exist\n",saveSlotid);
        return ;
    }

    otuUnitStatus_t  *pOtuStatus = getOtuStatus(0 ,saveSlotid);

    otuPortStatus_t *pPortStatus;


    pPortStatus   =  &pOtuStatus->portStatus[portId-1];
    if(pPortStatus->on_module != 1)
    {
        tinyrl_printf(this->tinyrl,"port %d doesn't exist\n",portId);
        return ;
    }
	 if(strcmp(arg,"1") == 0)
    {
        otu_nomarl_set_setspeed(portId,850);
    }
    else  if(strcmp(arg,"2") == 0)
    {
        otu_nomarl_set_setspeed(portId,1000);
    }
    else  
    {
        
    }
    return BOOL_TRUE;
	

    if(strcmp(arg,"1") == 0)
    {
        otu_nomarl_set_setspeed(portId,1000);
    }
    else  if(strcmp(arg,"2") == 0)
    {
        otu_nomarl_set_setspeed(portId,1104);
    }
    else  if(strcmp(arg,"3") == 0)
    {
        otu_nomarl_set_setspeed(portId,1070);
    }
    else  if(strcmp(arg,"4") == 0)
    {
        otu_nomarl_set_setspeed(portId,1109);
    }
    else  if(strcmp(arg,"5") == 0)
    {
        otu_nomarl_set_setspeed(portId,995);
    }
	else    if(strcmp(arg,"6") == 0)
    {
        otu_nomarl_set_setspeed(portId,1079);
    }
    else  if(strcmp(arg,"7") == 0)
    {
        otu_nomarl_set_setspeed(portId,249);
    }
    else  if(strcmp(arg,"8") == 0)
    {
        otu_nomarl_set_setspeed(portId,1031);
    }
    else  if(strcmp(arg,"9") == 0)
    {
        otu_nomarl_set_setspeed(portId,250);
    }
    else  if(strcmp(arg,"10") == 0)
    {
        otu_nomarl_set_setspeed(portId,1051);
    }
	else    if(strcmp(arg,"11") == 0)
    {
        otu_nomarl_set_setspeed(portId,1131);
    }
    else  if(strcmp(arg,"12") == 0)
    {
        otu_nomarl_set_setspeed(portId,850);
    }
    else  if(strcmp(arg,"13") == 0)
    {
        otu_nomarl_set_setspeed(portId,425);
    }
    else  if(strcmp(arg,"14") == 0)
    {
        otu_nomarl_set_setspeed(portId,212);
    }
    else  
    {
        
    }
    return BOOL_TRUE;
}





static bool_t
shell_otu_setddm(const clish_context_t *context,
                 const lub_argv_t    *argv)
{
    const int                   portId   = atoi(lub_argv__get_arg(argv,0));


    int rxPowerMin= atoi(lub_argv__get_arg(argv,1));
    int rxPowerMax= atoi(lub_argv__get_arg(argv,2));
    int txPowerMin= atoi(lub_argv__get_arg(argv,3));
    int txPowerMax= atoi(lub_argv__get_arg(argv,4));
    int tempMin   = atoi(lub_argv__get_arg(argv, 5));
    int tempMax   = atoi(lub_argv__get_arg(argv, 6));
    int voltMin = atoi(lub_argv__get_arg(argv, 7));
    int voltMax = atoi(lub_argv__get_arg(argv, 8));
    int rxBiasMin = atoi(lub_argv__get_arg(argv, 9));
    int rxBiasMax = atoi(lub_argv__get_arg(argv, 10));

    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;
    char*   status = getUnitStatusMap(0);
    if (NULL == status|| (status[saveSlotid] != otu &&status[saveSlotid] != oeo))
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not otu Or not exist\n",saveSlotid);
        return ;
    }

    otuUnitStatus_t  *pOtuStatus = getOtuStatus(0 ,saveSlotid);

    otuPortStatus_t *pPortStatus;


    pPortStatus   =  &pOtuStatus->portStatus[portId-1];
    if(pPortStatus->on_module != 1)
    {
        tinyrl_printf(this->tinyrl,"port %d doesn't exist\n",portId);
        return ;
    }
    setInfoSet_t infoset;
    memset(&infoset,0 ,sizeof(setInfoSet_t));

    infoItem_t *pInfo = infoset.infoItems;
    infoset.chassis = 0;
    infoset.slot = saveSlotid;
    infoset.infoset_type = 'S';
    infoset.infoset_id = 12;
    if(portId >=3 )
    {
        infoset.index  = 2;
    }
    else
    {
        infoset.index  = 1;
    }
    if(portId%2 == 1)
    {
        pInfo[0].item_id    = 1;
        pInfo[0].item_value = rxPowerMin;
        pInfo[1].item_id    = 2;
        pInfo[1].item_value = rxPowerMax;
        pInfo[2].item_id    = 3;
        pInfo[2].item_value = txPowerMin;
        pInfo[3].item_id    = 4;
        pInfo[3].item_value = txPowerMax;
        pInfo[4].item_id    = 5;
        pInfo[4].item_value = voltMin;
        pInfo[5].item_id    = 6;
        pInfo[5].item_value = voltMax;
        pInfo[6].item_id    = 7;
        pInfo[6].item_value = rxBiasMin;
        pInfo[7].item_id    = 8;
        pInfo[7].item_value = rxBiasMax;
        pInfo[8].item_id    = 9;
        pInfo[8].item_value = tempMin;
        pInfo[9].item_id    = 10;
        pInfo[9].item_value = tempMax;
    }
    else
    {
        pInfo[0].item_id = 11;
        pInfo[0].item_value = rxPowerMin;
        pInfo[1].item_id = 12;
        pInfo[1].item_value = rxPowerMax;
        pInfo[2].item_id = 13;
        pInfo[2].item_value = txPowerMin;
        pInfo[3].item_id = 14;
        pInfo[3].item_value = txPowerMax;
        pInfo[4].item_id = 15;
        pInfo[4].item_value = voltMin;
        pInfo[5].item_id = 16;
        pInfo[5].item_value = voltMax;
        pInfo[6].item_id = 17;
        pInfo[6].item_value = rxBiasMin;
        pInfo[7].item_id = 18;
        pInfo[7].item_value = rxBiasMax;
        pInfo[8].item_id = 19;
        pInfo[8].item_value = tempMin;
        pInfo[9].item_id = 20;
        pInfo[9].item_value = tempMax;

    }
    setDdmThreshold(infoset);

    return BOOL_TRUE;
}


static bool_t
shell_otu_setloopback(const clish_context_t *context,
                      const lub_argv_t    *argv)
{
    const int                   portId   = atoi(lub_argv__get_arg(argv,0));
    const char                   *arg   = lub_argv__get_arg(argv,1);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;
    char*   status = getUnitStatusMap(0);

    if (NULL == status|| (status[saveSlotid] != otu &&status[saveSlotid] != oeo))
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not otu Or not exist\n",saveSlotid);
        return ;
    }
    otuUnitStatus_t  *pOtuStatus = getOtuStatus(0 ,saveSlotid);

    otuPortStatus_t *pPortStatus;


    pPortStatus   =  &pOtuStatus->portStatus[portId-1];
    if(pPortStatus->on_module != 1)
    {
        tinyrl_printf(this->tinyrl,"port%d module doesn't exist\n",portId);
        return ;
    }

    if(strcmp(arg,"1") == 0)
    {
        otu_nomarl_set_setloopback(portId,1);
    }
    else  if(strcmp(arg,"2") == 0)
    {
        otu_nomarl_set_setloopback(portId,2);
    }

    return BOOL_TRUE;
}

static bool_t
shell_otu_setprbs(const clish_context_t *context,
                  const lub_argv_t    *argv)
{
    const int                   portId   = atoi(lub_argv__get_arg(argv,0));
    const char                   *arg   = lub_argv__get_arg(argv,1);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;
    char*   status = getUnitStatusMap(0);

    if (NULL == status|| (status[saveSlotid] != otu &&status[saveSlotid] != oeo))
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not otu Or not exist\n",saveSlotid);
        return ;
    }

    otuUnitStatus_t  *pOtuStatus = getOtuStatus(0 ,saveSlotid);

    otuPortStatus_t *pPortStatus;


    pPortStatus   =  &pOtuStatus->portStatus[portId-1];
    if(pPortStatus->on_module != 1)
    {
        tinyrl_printf(this->tinyrl,"port%d module doesn't exist\n",portId);
        return ;
    }

    if(strcmp(arg,"1") == 0)
    {
        otu_nomarl_set_setprbs(portId,1);
    }
    else  if(strcmp(arg,"2") == 0)
    {
        otu_nomarl_set_setprbs(portId,2);
    }

    return BOOL_TRUE;
}
static bool_t
shell_otu_setfec(const clish_context_t *context,
                 const lub_argv_t    *argv)
{
    const int                   portId   = atoi(lub_argv__get_arg(argv,0));
    const char                   *arg   = lub_argv__get_arg(argv,1);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;
    char*   status = getUnitStatusMap(0);

    if (NULL == status|| (status[saveSlotid] != otu &&status[saveSlotid] != oeo))
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not otu Or not exist\n",saveSlotid);
        return ;
    }
    otuUnitStatus_t  *pOtuStatus = getOtuStatus(0 ,saveSlotid);

    otuPortStatus_t *pPortStatus;


    pPortStatus   =  &pOtuStatus->portStatus[portId-1];
    if(pPortStatus->on_module != 1)
    {
        tinyrl_printf(this->tinyrl,"port%d module doesn't exist\n",portId);
        return ;
    }

    if(strcmp(arg,"1") == 0)
    {
        otu_nomarl_set_setfec(portId,1);
    }
    else  if(strcmp(arg,"2") == 0)
    {
        otu_nomarl_set_setfec(portId,2);
    }

    return BOOL_TRUE;
}

static bool_t
shell_otu_setfibersend(const clish_context_t *context,
                       const lub_argv_t    *argv)
{
    const int                   portId   = atoi(lub_argv__get_arg(argv,0));
    const char                   *arg   = lub_argv__get_arg(argv,1);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;
    char*   status = getUnitStatusMap(0);

    if (NULL == status|| (status[saveSlotid] != otu &&status[saveSlotid] != oeo))
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not otu Or not exist\n",saveSlotid);
        return ;
    }

    otuUnitStatus_t  *pOtuStatus = getOtuStatus(0 ,saveSlotid);

    otuPortStatus_t *pPortStatus;


    pPortStatus   =  &pOtuStatus->portStatus[portId-1];
    if(pPortStatus->on_module != 1)
    {
        tinyrl_printf(this->tinyrl,"port%d module doesn't exist\n",portId);
        return ;
    }

    if(strcmp(arg,"1") == 0)
    {
        otu_nomarl_set_setfibersend(portId,1);
    }
    else  if(strcmp(arg,"2") == 0)
    {
        otu_nomarl_set_setfibersend(portId,2);
    }
    else  if(strcmp(arg,"3") == 0)
    {
        otu_nomarl_set_setfibersend(portId,3);
    }
    return BOOL_TRUE;
}


static bool_t
shell_olp_set_mode(const clish_context_t *context,
                   const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;

    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != olp)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not olp Or not exist\n",saveSlotid);
        return ;
    }

    if(strcmp(arg,"auto") == 0)
    {
        nomarl_set_value(8,1);
    }
    else  if(strcmp(arg,"manual") == 0)
    {
        nomarl_set_value(8,2);
    }

    return BOOL_TRUE;
}



static bool_t
shell_olp_set_selectline(const clish_context_t *context,
                         const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;
    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != olp)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not olp Or not exist\n",saveSlotid);
        return ;
    }

    if(strcmp(arg,"L1") == 0)
    {
        nomarl_set_value(9,1);
    }
    else  if(strcmp(arg,"L2") == 0)
    {
        nomarl_set_value(9,2);
    }

    return BOOL_TRUE;
}


static bool_t
shell_olp_set_backmode(const clish_context_t *context,
                       const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;
    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != olp)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not olp Or not exist\n",saveSlotid);
        return ;
    }

    if(strcmp(arg,"auto") == 0)
    {
        nomarl_set_value(11,1);
    }
    else  if(strcmp(arg,"manual") == 0)
    {
        nomarl_set_value(11,2);
    }
    return BOOL_TRUE;
}


static bool_t
shell_olp_set_switchrule(const clish_context_t *context,
                         const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;
    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != olp)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not olp Or not exist\n",saveSlotid);
        return ;
    }

    if(strcmp(arg,"working") == 0)
    {
        nomarl_set_value(10,1);

    }
    else  if(strcmp(arg,"highpower") == 0)
    {
        nomarl_set_value(10,2);

    }
    return BOOL_TRUE;
}


static bool_t
shell_olp_set_wavelength(const clish_context_t *context,
                         const lub_argv_t    *argv)
{
    const int                  portId   = atoi(lub_argv__get_arg(argv,0));
    const char                   *arg   = lub_argv__get_arg(argv,1);

    /* the exception proves the rule... */
    int  temp,index;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;
    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != olp)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not olp Or not exist\n",saveSlotid);
        return ;
    }

    if(strcmp(arg,"working") == 0)
    {
        nomarl_set_value(14+portId,1);

    }
    else  if(strcmp(arg,"highpower") == 0)
    {
        nomarl_set_value(14+portId,2);

    }


    return BOOL_TRUE;
}


static bool_t
shell_olp_set_r1power(const clish_context_t *context,
                      const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    float value  =	atof(arg);



    clish_shell_t *this = (clish_shell_t *)context->shell;
    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != olp)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not olp Or not exist\n",saveSlotid);
        return ;
    }
    if(value < -50 || value > 10)
    {
        tinyrl_printf(this->tinyrl,"parameter error,value isn't in(-50.0  10.0)\n");
        return BOOL_TRUE;
    }

    short tt = value *10;

    nomarl_set_value(4,tt);

    return BOOL_TRUE;
}

static bool_t
shell_olp_set_r2power(const clish_context_t *context,
                      const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    float value  =	atof(arg);



    clish_shell_t *this = (clish_shell_t *)context->shell;

    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != olp)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not olp Or not exist\n",saveSlotid);
        return ;
    }
    if(value < -50 || value > 10)
    {
        tinyrl_printf(this->tinyrl,"parameter error,value isn't in(-50.0  10.0)\n");
        return BOOL_TRUE;
    }
    nomarl_set_value(5,value*10);

    return BOOL_TRUE;
}

static bool_t
shell_olp_set_txpower(const clish_context_t *context,
                      const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);

    float value  =	atof(arg);


    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;
    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != olp)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not olp Or not exist\n",saveSlotid);
        return ;
    }
    if(value < -50 || value > 10)
    {
        tinyrl_printf(this->tinyrl,"parameter error,value isn't in(-50.0  10.0)\n");
        return BOOL_TRUE;
    }
    nomarl_set_value(6,value*10);

    return BOOL_TRUE;
}

static bool_t
shell_olp_set_backtime(const clish_context_t *context,
                       const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);

    short value  =	atoi(arg);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;

    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != olp)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not olp Or not exist\n",saveSlotid);
        return ;
    }

    nomarl_set_value(7,value);


    return BOOL_TRUE;
}

static bool_t
shell_obp_set_heartbeat(const clish_context_t *context,
                       const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    const char                   *arg1   = lub_argv__get_arg(argv,1);

    short value  =	atoi(arg);
    short value1  =	atoi(arg1);

	
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;

    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != olp)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not obp Or not exist\n",saveSlotid);
        return ;
    }

    setStartUpService(11,value,value1);


    return BOOL_TRUE;
}

static bool_t
shell_obp_set_iplist(const clish_context_t *context,
                       const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    const char                   *arg1   = lub_argv__get_arg(argv,1);
    short value  =	atoi(arg);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;

    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != olp)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not obp Or not exist\n",saveSlotid);
        return ;
    }

    if(value == 1)
    {
      addConfItem(CLI_CONF_FILE,"accept_ip",arg1);
    }
	else 
	{
	   removeConfItem(CLI_CONF_FILE,"accept_ip",arg1);
	}


    return BOOL_TRUE;
}
static bool_t
shell_obp_set_parameters(const clish_context_t *context,
                       const lub_argv_t    *argv)
{
    const char                   *value   = lub_argv__get_arg(argv,0);
    const char                   *value1   = lub_argv__get_arg(argv,1);
    const char                   *value2   = lub_argv__get_arg(argv,2);
	const char                   *value3   = lub_argv__get_arg(argv,3);
	const char                   *value4   = lub_argv__get_arg(argv,4);
    const char                   *value5   = lub_argv__get_arg(argv,5);
    const char                   *value6   = lub_argv__get_arg(argv,6);

    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;

    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != olp)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not obp Or not exist\n",saveSlotid);
        return ;
    }

   setConfValue(CLI_CONF_FILE,"port",value," ");

   setConfValue(CLI_CONF_FILE,"slots",value1," ");

   setConfValue(CLI_CONF_FILE,"waitting",value2," ");

	setConfValue(CLI_CONF_FILE,"sec",value3," ");

	setConfValue(CLI_CONF_FILE,"usec",value4," ");

	setConfValue(CLI_CONF_FILE,"interval",value5," ");

	setConfValue(CLI_CONF_FILE,"retry",value6," ");




    return BOOL_TRUE;
}
static bool_t
clish_obp_show_heartbeat(const clish_context_t *context,
                       const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);

    short value  =	atoi(arg);
	char buf[128];
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;

    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != olp)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not obp Or not exist\n",saveSlotid);
        return ;
    }

   tinyrl_printf(this->tinyrl,"Obp heartbeat detection info\n",saveSlotid);
   tinyrl_printf(this->tinyrl,"Heartbeat detection IP list\n",saveSlotid);
   int index= 0;
   int source_count=0;
   char accept_ip_list[10][20];
   source_count=getConfItems(CLI_CONF_FILE,"accept_ip",accept_ip_list,10,";");
   for(index=0;index<source_count;index++)
   {
          tinyrl_printf(this->tinyrl,"      %s\n",accept_ip_list[index]);

   }
   

   getConfValue(CLI_CONF_FILE,"port",buf," ");
   tinyrl_printf(this->tinyrl,"port:  %s\n",buf);

   getConfValue(CLI_CONF_FILE,"slots",buf," ");
      tinyrl_printf(this->tinyrl,"slots:  %s\n",buf);

   getConfValue(CLI_CONF_FILE,"waitting",buf," ");
      tinyrl_printf(this->tinyrl,"waitting:  %s\n",buf);

	getConfValue(CLI_CONF_FILE,"sec",buf," ");
	   tinyrl_printf(this->tinyrl,"sec:  %s\n",buf);

	getConfValue(CLI_CONF_FILE,"usec",buf," ");
	   tinyrl_printf(this->tinyrl,"usec:  %s\n",buf);

	getConfValue(CLI_CONF_FILE,"interval",buf," ");
	   tinyrl_printf(this->tinyrl,"interval:  %s\n",buf);

	getConfValue(CLI_CONF_FILE,"retry",buf," ");
	   tinyrl_printf(this->tinyrl,"retry:  %s\n",buf);


    return BOOL_TRUE;
}






static bool_t
shell_olp_test_in(const clish_context_t *context,
                  const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;


    init_olp_on(atoi(arg));

    return BOOL_TRUE;
}



static bool_t
shell_olp_test_out(const clish_context_t *context,
                   const lub_argv_t    *argv)
{
    const char  *arg   = lub_argv__get_arg(argv,0);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;

    return BOOL_TRUE;
}





static bool_t
shell_olp_show_all(const clish_context_t *context,
                   const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;

    cli_show_olp_view( atoi(arg));

    return BOOL_TRUE;
}

static bool_t
shell_oss_set_mode(const clish_context_t *context,
                   const lub_argv_t    *argv)
{
    int channel =  atoi(lub_argv__get_arg(argv,0));
    const char                   *arg   = lub_argv__get_arg(argv,1);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;

    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != BLOAD_OSS)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not oss Or not exist\n",saveSlotid);
        return ;
    }

    if(strcmp(arg,"auto") == 0)
    {
        oss_set_value(channel,8,1);
    }
    else  if(strcmp(arg,"manual") == 0)
    {
        oss_set_value(channel,8,2);
    }

    return BOOL_TRUE;
}



static bool_t
shell_oss_set_selectline(const clish_context_t *context,
                         const lub_argv_t    *argv)
{
    int channel =  atoi(lub_argv__get_arg(argv,0));
    const char                   *arg   = lub_argv__get_arg(argv,1);

    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;
    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != BLOAD_OSS)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not oss Or not exist\n",saveSlotid);
        return ;
    }

    if(strcmp(arg,"L1") == 0)
    {
        oss_set_value(channel,9,1);
    }
    else  if(strcmp(arg,"L2") == 0)
    {
        oss_set_value(channel,9,2);
    }

    return BOOL_TRUE;
}


static bool_t
shell_oss_set_backmode(const clish_context_t *context,
                       const lub_argv_t    *argv)
{
    int channel =  atoi(lub_argv__get_arg(argv,0));
    const char                   *arg   = lub_argv__get_arg(argv,1);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;
    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != BLOAD_OSS)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not oss Or not exist\n",saveSlotid);
        return ;
    }

    if(strcmp(arg,"auto") == 0)
    {
        oss_set_value(channel,11,1);
    }
    else  if(strcmp(arg,"manual") == 0)
    {
        oss_set_value(channel,11,2);
    }
    return BOOL_TRUE;
}


static bool_t
shell_oss_set_switchrule(const clish_context_t *context,
                         const lub_argv_t    *argv)
{
    int channel =  atoi(lub_argv__get_arg(argv,0));
    const char                   *arg   = lub_argv__get_arg(argv,1);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;
    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != BLOAD_OSS)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not oss Or not exist\n",saveSlotid);
        return ;
    }

    if(strcmp(arg,"working") == 0)
    {
        oss_set_value(channel,10,1);

    }
    else  if(strcmp(arg,"highpower") == 0)
    {
        oss_set_value(channel,10,2);

    }
    return BOOL_TRUE;
}


static bool_t
shell_oss_set_wavelength(const clish_context_t *context,
                         const lub_argv_t    *argv)
{
    int channel =  atoi(lub_argv__get_arg(argv,0));

    const int                  portId   = atoi(lub_argv__get_arg(argv,1));
    const char                   *arg   = lub_argv__get_arg(argv,2);

    /* the exception proves the rule... */
    int  temp,index;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;
    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != BLOAD_OSS)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not oss Or not exist\n",saveSlotid);
        return ;
    }

    if(strcmp(arg,"working") == 0)
    {
        oss_set_value(channel,14+portId,1);

    }
    else  if(strcmp(arg,"highpower") == 0)
    {
        oss_set_value(channel,14+portId,2);

    }


    return BOOL_TRUE;
}


static bool_t
shell_oss_set_r1power(const clish_context_t *context,
                      const lub_argv_t    *argv)
{
    int channel =  atoi(lub_argv__get_arg(argv,0));

    const char                   *arg   = lub_argv__get_arg(argv,1);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    float value  =	atof(arg);



    clish_shell_t *this = (clish_shell_t *)context->shell;
    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != BLOAD_OSS)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not oss Or not exist\n",saveSlotid);
        return ;
    }
    if(value < -50 || value > 10)
    {
        tinyrl_printf(this->tinyrl,"parameter error,value isn't in(-50.0  10.0)\n");
        return BOOL_TRUE;
    }

    short tt = value *10;

    oss_set_value(channel,4,tt);

    return BOOL_TRUE;
}

static bool_t
shell_oss_set_r2power(const clish_context_t *context,
                      const lub_argv_t    *argv)
{
    int channel =  atoi(lub_argv__get_arg(argv,0));

    const char                   *arg   = lub_argv__get_arg(argv,1);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    float value  =	atof(arg);



    clish_shell_t *this = (clish_shell_t *)context->shell;

    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != BLOAD_OSS)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not oss Or not exist\n",saveSlotid);
        return ;
    }
    if(value < -50 || value > 10)
    {
        tinyrl_printf(this->tinyrl,"parameter error,value isn't in(-50.0  10.0)\n");
        return BOOL_TRUE;
    }
    oss_set_value(channel,5,value*10);

    return BOOL_TRUE;
}

static bool_t
shell_oss_set_txpower(const clish_context_t *context,
                      const lub_argv_t    *argv)
{
    int channel =  atoi(lub_argv__get_arg(argv,0));

    const char *arg   = lub_argv__get_arg(argv,1);

    float value  =	atof(arg);


    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;
    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != BLOAD_OSS)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not oss Or not exist\n",saveSlotid);
        return ;
    }
    if(value < -50 || value > 10)
    {
        tinyrl_printf(this->tinyrl,"parameter error,value isn't in(-50.0  10.0)\n");
        return BOOL_TRUE;
    }
    oss_set_value(channel,6,value*10);

    return BOOL_TRUE;
}

static bool_t
shell_oss_set_backtime(const clish_context_t *context,
                       const lub_argv_t    *argv)
{
    int channel =  atoi(lub_argv__get_arg(argv,0));

    const char                   *arg   = lub_argv__get_arg(argv,1);

    short value  =	atoi(arg);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;

    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != BLOAD_OSS)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not oss Or not exist\n",saveSlotid);
        return ;
    }

    oss_set_value(channel,7,value);


    return BOOL_TRUE;
}

static bool_t
shell_edfa_set_mode(const clish_context_t *context,
                    const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;

    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != BLOAD_EDFA)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not edfa Or not exist\n",saveSlotid);
        return ;
    }

    if(strcmp(arg,"auto") == 0)
    {
        edfa_set_value(6,1);

    }
    else  if(strcmp(arg,"manual") == 0)
    {
        edfa_set_value(6,2);
    }

    return BOOL_TRUE;
}



static bool_t
shell_edfa_set_state(const clish_context_t *context,
                     const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    /* the exception proves the rule... */
    int  temp;
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;

    char*   status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != BLOAD_EDFA)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not olp Or not exist\n",saveSlotid);
        return ;
    }

    if(strcmp(arg,"auto") == 0)
    {
        edfa_set_value(6,1);

    }
    else  if(strcmp(arg,"manual") == 0)
    {
        edfa_set_value(6,2);
    }

    return BOOL_TRUE;
}

static bool_t
shell_system_set_temp(const clish_context_t *context,
                      const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);

    const char                   *value   = lub_argv__get_arg(argv,1);

    /* the exception proves the rule... */
    int  temp = atoi(value);
//	printf("jiangyibo %d \n",temp);
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;

    chassisThreshold_t *pThreshold = getChassisThreshold(0);

    if(pThreshold == NULL)
    {
        tinyrl_printf(this->tinyrl,"can't get threshold  \n");
        return ;

    }

    if(strcmp(arg,"1") == 0)
    {
        if(pThreshold->max_temperature <= temp)
        {
            tinyrl_printf(this->tinyrl,"max temperature less than or equal to min temperature \n");

            return ;
        }
        system_set_value(1,temp);
        tinyrl_printf(this->tinyrl,"set success \n");
    }
    else  if(strcmp(arg,"2") == 0)
    {
        if(pThreshold->min_temperature >= temp)
        {
            tinyrl_printf(this->tinyrl,"max temperature less than or equal to min temperature \n");

            return ;
        }
        system_set_value(2,temp);
        tinyrl_printf(this->tinyrl,"set success \n");
    }

    return BOOL_TRUE;
}

static bool_t
shell_system_set_volt(const clish_context_t *context,
                      const lub_argv_t    *argv)
{
    /* the exception proves the rule... */
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;
    const char                   *arg   = lub_argv__get_arg(argv,0);

    const char                   *index   = lub_argv__get_arg(argv,1);

    const char                   *value   = lub_argv__get_arg(argv,2);

    short temp  = (short)atoi(value);

    chassisThreshold_t *pThreshold = getChassisThreshold(0);

    if(pThreshold == NULL)
    {
        tinyrl_printf(this->tinyrl,"can't get threshold  \n");
        return ;

    }


    if(strcmp(arg,"1") == 0 && strcmp(index,"1") == 0 )
    {
        if(pThreshold->max_volt<= temp)
        {
            tinyrl_printf(this->tinyrl,"max volt less than or equal to min volt \n");

            return ;
        }
        system_set_value(3,temp);

        tinyrl_printf(this->tinyrl,"set success \n");

    }
    else  if(strcmp(arg,"1") == 0 && strcmp(index,"2") == 0 )
    {
        if(pThreshold->min_volt>= temp)
        {
            tinyrl_printf(this->tinyrl,"max volt less than or equal to min volt \n");


            return ;
        }
        system_set_value(4,temp);
        tinyrl_printf(this->tinyrl,"set success \n");
    }

    return BOOL_TRUE;
}

static bool_t
shell_system_set_buzz(const clish_context_t *context,
                      const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);



    /* the exception proves the rule... */
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;


    if(strcmp(arg,"1") == 0  )
    {
        set_buzz_state(1);
    }
    else
    {

        set_buzz_state(2);
    }

    return BOOL_TRUE;
}




static bool_t
clish_awg_system(const clish_context_t *context,
                 const lub_argv_t    *argv)
{

    uchar_t chassis, slot, index;

    uchar_t *status;

    char output[64];

    unit_user_data_t udata;


    clish_shell_t *this = (clish_shell_t *)context->shell;

    status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != BLOAD_PASSIVE)
    {

        tinyrl_printf(this->tinyrl,"slot[%d] is not awg %d\n",saveSlotid,status[saveSlotid]);

        return ;
    }

    /*
    	unitProperty_t *pro = getUnitProperty( 0 , saveSlotid, 1);

        tinyrl_printf(this->tinyrl,"olp %s\n",pro->unit_class);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->unit_type);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->hardware_type);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->firmware_type);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->struct_type);
    */

    unit_base_info_t *info = getBaseInfoMap(0 , saveSlotid);

    tinyrl_printf(this->tinyrl,"Sn :%s\n",info->sn);
    tinyrl_printf(this->tinyrl,"Model :%s\n",info->model);
    tinyrl_printf(this->tinyrl,"Creation :%s\n",info->creation);
    tinyrl_printf(this->tinyrl,"Soft Version :%s\n",info->fwver);
    tinyrl_printf(this->tinyrl,"Hard Version :%s\n",info->hwver);
    tinyrl_printf(this->tinyrl,"Software Last Time :%s",ctime(&info->uptime));
    int tTemp = difftime(time(NULL) , info->uptime);

    tinyrl_printf(this->tinyrl,"Software Running Time :%s\n",parseTimeSeconds(tTemp));

    getUserDataFromDB(0 , saveSlotid ,&udata);

    u2g(udata.name, strlen(udata.name), output,64);
    tinyrl_printf(this->tinyrl,"Name :%s\n",output);
    u2g(udata.location, strlen(udata.location), output,64);
    tinyrl_printf(this->tinyrl,"Location :%s\n",output);
    u2g(udata.contact, strlen(udata.contact), output,64);
    tinyrl_printf(this->tinyrl,"Contact :%s\n",output);
    u2g(udata.updev, strlen(udata.updev), output,64);
    tinyrl_printf(this->tinyrl,"Up device :%s\n",output);
    u2g(udata.userdata, strlen(udata.userdata), output,64);
    tinyrl_printf(this->tinyrl,"User data :%s\n",output);
    u2g(udata.service, strlen(udata.service), output,64);
    tinyrl_printf(this->tinyrl,"Service :%s\n",output);
}


static bool_t
clish_oss_system(const clish_context_t *context,
                 const lub_argv_t    *argv)
{

    uchar_t chassis, slot, index;

    uchar_t *status;

    char output[64];

    unit_user_data_t udata;


    clish_shell_t *this = (clish_shell_t *)context->shell;

    status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != BLOAD_OSS)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not oss \n",saveSlotid);
        return ;
    }

    olp1P1Info_t  *ptr = getOlp1P1Info(0 ,saveSlotid);

    if(ptr == NULL)
    {
        tinyrl_printf(this->tinyrl,"Oss Is Not Online!\n");
        return BOOL_TRUE;
    }


    /*
    	unitProperty_t *pro = getUnitProperty( 0 , saveSlotid, 1);

        tinyrl_printf(this->tinyrl,"olp %s\n",pro->unit_class);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->unit_type);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->hardware_type);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->firmware_type);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->struct_type);
    */

    unit_base_info_t *info = getBaseInfoMap(0 , saveSlotid);

    tinyrl_printf(this->tinyrl,"Sn :%s\n",info->sn);
    tinyrl_printf(this->tinyrl,"Model :%s\n",info->model);
    tinyrl_printf(this->tinyrl,"Creation :%s\n",info->creation);
    tinyrl_printf(this->tinyrl,"Soft Version :%s\n",info->fwver);
    tinyrl_printf(this->tinyrl,"Hard Version :%s\n",info->hwver);
    tinyrl_printf(this->tinyrl,"Software Last Time :%s",ctime(&info->uptime));
    int tTemp = difftime(time(NULL) , info->uptime);

    tinyrl_printf(this->tinyrl,"Software Running Time :%s\n",parseTimeSeconds(tTemp));

    getUserDataFromDB(0 , saveSlotid ,&udata);

    u2g(udata.name, strlen(udata.name), output,64);
    tinyrl_printf(this->tinyrl,"Name :%s\n",output);
    u2g(udata.location, strlen(udata.location), output,64);
    tinyrl_printf(this->tinyrl,"Location :%s\n",output);
    u2g(udata.contact, strlen(udata.contact), output,64);
    tinyrl_printf(this->tinyrl,"Contact :%s\n",output);
    u2g(udata.updev, strlen(udata.updev), output,64);
    tinyrl_printf(this->tinyrl,"Up device :%s\n",output);
    u2g(udata.userdata, strlen(udata.userdata), output,64);
    tinyrl_printf(this->tinyrl,"User data :%s\n",output);
    u2g(udata.service, strlen(udata.service), output,64);
    tinyrl_printf(this->tinyrl,"Service :%s\n",output);

    tinyrl_printf(this->tinyrl,"OSS channel1 information\n");
    if(ptr->mode == 1)
    {
        tinyrl_printf(this->tinyrl,"Work Mode :Automatic\n");

    }
    else if(ptr->mode == 2)
    {

        tinyrl_printf(this->tinyrl,"Work Mode :Manual\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"Work Mode :Unknow\n");
    }

    if(ptr->line == 1)
    {
        tinyrl_printf(this->tinyrl,"Line Rule :Select R1-1\n");

    }
    else if(ptr->line == 2)
    {

        tinyrl_printf(this->tinyrl,"Line Rule :Select R1-2\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"Line Rule :Unknow\n");
    }



    if(ptr->rule == 1)
    {
        tinyrl_printf(this->tinyrl,"Rule Working :Firbe First\n");

    }
    else if(ptr->rule == 2)
    {

        tinyrl_printf(this->tinyrl,"Rule Optical :power High First\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"Rule :Unknow\n");
    }


    if(ptr->ret_mode == 1)
    {
        tinyrl_printf(this->tinyrl,"Return Mode :Automatic\n");

    }
    else if(ptr->ret_mode == 2)
    {

        tinyrl_printf(this->tinyrl,"Return Mode :Manual\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"Return Mode :Unknow\n");
    }
    tinyrl_printf(this->tinyrl,"Return Time :%d (min)\n",ptr->ret_time);



    tinyrl_printf(this->tinyrl,"Port  Optical power(dbm)  Wave length(nm) Switch power(dbm)\n" );


    tinyrl_printf(this->tinyrl,"R1-1        %.1f           ",ptr->rx1_power*0.1);


    if(ptr->rx1_wave_len == 1)
    {
        tinyrl_printf(this->tinyrl," 1310");

    }
    else if(ptr->rx1_wave_len == 2)
    {

        tinyrl_printf(this->tinyrl," 1550");
    }
    else
    {
        tinyrl_printf(this->tinyrl," unknown");
    }

    tinyrl_printf(this->tinyrl,"          %.1f\n",ptr->l1_power*0.1);


    tinyrl_printf(this->tinyrl,"R1-2        %.1f           ",ptr->rx2_power*0.1);

    if(ptr->rx2_wave_len == 1)
    {
        tinyrl_printf(this->tinyrl," 1310");

    }
    else if(ptr->rx2_wave_len == 2)
    {

        tinyrl_printf(this->tinyrl," 1550");
    }
    else
    {
        tinyrl_printf(this->tinyrl," unknown");
    }
    tinyrl_printf(this->tinyrl,"          %.1f\n",ptr->l2_power*0.1);


    ptr = getOssChannelInfo(0 ,saveSlotid,2);


    tinyrl_printf(this->tinyrl,"OSS channel2 information\n");

    if(ptr->mode == 1)
    {
        tinyrl_printf(this->tinyrl,"Work Mode :Automatic\n");

    }
    else if(ptr->mode == 2)
    {

        tinyrl_printf(this->tinyrl,"Work Mode :Manual\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"Work Mode :Unknow\n");
    }

    if(ptr->line == 1)
    {
        tinyrl_printf(this->tinyrl,"Line Rule :Select R2-1\n");

    }
    else if(ptr->line == 2)
    {

        tinyrl_printf(this->tinyrl,"Line Rule :Select R2-2\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"Line Rule :Unknow\n");
    }



    if(ptr->rule == 1)
    {
        tinyrl_printf(this->tinyrl,"Rule Working :Firbe First\n");

    }
    else if(ptr->rule == 2)
    {

        tinyrl_printf(this->tinyrl,"Rule Optical :power High First\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"Rule :Unknow\n");
    }


    if(ptr->ret_mode == 1)
    {
        tinyrl_printf(this->tinyrl,"Return Mode :Automatic\n");

    }
    else if(ptr->ret_mode == 2)
    {

        tinyrl_printf(this->tinyrl,"Return Mode :Manual\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"Return Mode :Unknow\n");
    }
    tinyrl_printf(this->tinyrl,"Return Time :%d (min)\n",ptr->ret_time);



    tinyrl_printf(this->tinyrl,"Port  Optical power(dbm)  Wave length(nm) Switch power(dbm)\n" );


    tinyrl_printf(this->tinyrl,"R2-1        %.1f           ",ptr->rx1_power*0.1);


    if(ptr->rx1_wave_len == 1)
    {
        tinyrl_printf(this->tinyrl," 1310");

    }
    else if(ptr->rx1_wave_len == 2)
    {

        tinyrl_printf(this->tinyrl," 1550");
    }
    else
    {
        tinyrl_printf(this->tinyrl," unknown");
    }

    tinyrl_printf(this->tinyrl,"          %.1f\n",ptr->l1_power*0.1);


    tinyrl_printf(this->tinyrl,"R2-2        %.1f           ",ptr->rx2_power*0.1);

    if(ptr->rx2_wave_len == 1)
    {
        tinyrl_printf(this->tinyrl," 1310");

    }
    else if(ptr->rx2_wave_len == 2)
    {

        tinyrl_printf(this->tinyrl," 1550");
    }
    else
    {
        tinyrl_printf(this->tinyrl," unknown");
    }
    tinyrl_printf(this->tinyrl,"          %.1f\n",ptr->l2_power*0.1);


}

static bool_t
clish_edfa_system(const clish_context_t *context,
                  const lub_argv_t    *argv)
{

    uchar_t chassis, slot, index;

    uchar_t *status;

    char *input = "��C";

    char output[64];

    unit_user_data_t udata;


    clish_shell_t *this = (clish_shell_t *)context->shell;

    status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != BLOAD_EDFA)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not edfa \n",saveSlotid);
        return ;
    }

    edfaInfo_t  *ptr = getEdfaInfo(0 ,saveSlotid);

    if(ptr == NULL)
    {
        tinyrl_printf(this->tinyrl,"Edfa Is Not Online!\n");
        return BOOL_TRUE;
    }

    unit_base_info_t *info = getBaseInfoMap(0 , saveSlotid);

    tinyrl_printf(this->tinyrl,"Sn :%s\n",info->sn);
    tinyrl_printf(this->tinyrl,"Model :%s\n",info->model);
    tinyrl_printf(this->tinyrl,"Creation :%s\n",info->creation);
    tinyrl_printf(this->tinyrl,"Soft Version :%s\n",info->fwver);
    tinyrl_printf(this->tinyrl,"Hard Version :%s\n",info->hwver);
    tinyrl_printf(this->tinyrl,"Software Last Time :%s",ctime(&info->uptime));
    int tTemp = difftime(time(NULL) , info->uptime);

    tinyrl_printf(this->tinyrl,"Software Running Time :%s\n",parseTimeSeconds(tTemp));

    getUserDataFromDB(0 , saveSlotid ,&udata);

    u2g(udata.name, strlen(udata.name), output,64);
    tinyrl_printf(this->tinyrl,"Name :%s\n",output);
    u2g(udata.location, strlen(udata.location), output,64);
    tinyrl_printf(this->tinyrl,"Location :%s\n",output);
    u2g(udata.contact, strlen(udata.contact), output,64);
    tinyrl_printf(this->tinyrl,"Contact :%s\n",output);
    u2g(udata.updev, strlen(udata.updev), output,64);
    tinyrl_printf(this->tinyrl,"Up device :%s\n",output);
    u2g(udata.userdata, strlen(udata.userdata), output,64);
    tinyrl_printf(this->tinyrl,"User data :%s\n",output);

    u2g(udata.service, strlen(udata.service), output,64);
    tinyrl_printf(this->tinyrl,"Service :%s\n",output);


    tinyrl_printf(this->tinyrl,"EDFA information\n");

    tinyrl_printf(this->tinyrl,"Input power(dbm)  Output power(dbm)  Temperature(%s) \n",input );
    tinyrl_printf(this->tinyrl,"  %.1f               %.1f              %.1f\n",ptr->inputPower*0.1,ptr->outputPower*0.1,ptr->temperature*0.1);

    tinyrl_printf(this->tinyrl,"Operating current(mA)  Refrigeration current(mA)   Power(dbm)  Temperature(%s)     state\n",input );
    tinyrl_printf(this->tinyrl,"       %d                    %d                      %.1f         %.1f            ",
                  ptr->pump_status[0].working_current,ptr->pump_status[0].cool_current,
                  ptr->pump_status[0].power*0.1,ptr->pump_status[0].temperature*0.1);


    if(ptr->pump1_status == 1)
    {
        printf("enable\n");
    }
    else
    {
        printf("disable\n");
    }

    /*
        edfaThreshold_t *pThre = getEdfaThreshold(0 ,saveSlotid);

        tinyrl_printf(this->tinyrl,"EDFA threshold\n");
        tinyrl_printf(this->tinyrl,"Input min power(dbm)  Output max power(dbm)  Min Temperature(%s) Man Temperature(%s) \n",input,input );
        tinyrl_printf(this->tinyrl,"  %.1f       %.1f        %.1f   %.1f\n",
                      pThre->minInputPower*0.1,pThre->minOutputPower*0.1,pThre->minTemperature*0.1,pThre->maxTemperature*0.1);

        tinyrl_printf(this->tinyrl,"EDFA threshold\n");
        tinyrl_printf(this->tinyrl,"Min power(dbm)  Max power(dbm)  Min Temperature(%s) Man Temperature(%s) \n",input,input );
        tinyrl_printf(this->tinyrl,"  %.1f       %.1f        %.1f   %.1f\n",
                      pThre->pump[0].minCurrent*0.1,pThre->pump[0].maxCurrent*0.1,
                      pThre->pump[0].minTemperature*0.1,pThre->pump[0].maxTemperature*0.1);
    */

}

static bool_t
clish_etu_system(const clish_context_t *context,
                  const lub_argv_t    *argv)
{

    uchar_t chassis, slot, index;

    uchar_t *status;

    char *input = "��C";

    char output[64];

    unit_user_data_t udata;


    clish_shell_t *this = (clish_shell_t *)context->shell;

    status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != BLOAD_ETU)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not etu \n",saveSlotid);
        return ;
    }
/*

    */

    unit_base_info_t *info = getBaseInfoMap(0 , saveSlotid);

    tinyrl_printf(this->tinyrl,"Sn :%s\n",info->sn);
    tinyrl_printf(this->tinyrl,"Model :%s\n",info->model);
    tinyrl_printf(this->tinyrl,"Creation :%s\n",info->creation);
    tinyrl_printf(this->tinyrl,"Soft Version :%s\n",info->fwver);
    tinyrl_printf(this->tinyrl,"Hard Version :%s\n",info->hwver);
    tinyrl_printf(this->tinyrl,"Software Last Time :%s",ctime(&info->uptime));
    int tTemp = difftime(time(NULL) , info->uptime);

    tinyrl_printf(this->tinyrl,"Software Running Time :%s\n",parseTimeSeconds(tTemp));

    getUserDataFromDB(0 , saveSlotid ,&udata);

    u2g(udata.name, strlen(udata.name), output,64);
    tinyrl_printf(this->tinyrl,"Name :%s\n",output);
    u2g(udata.location, strlen(udata.location), output,64);
    tinyrl_printf(this->tinyrl,"Location :%s\n",output);
    u2g(udata.contact, strlen(udata.contact), output,64);
    tinyrl_printf(this->tinyrl,"Contact :%s\n",output);
    u2g(udata.updev, strlen(udata.updev), output,64);
    tinyrl_printf(this->tinyrl,"Up device :%s\n",output);
    u2g(udata.userdata, strlen(udata.userdata), output,64);
    tinyrl_printf(this->tinyrl,"User data :%s\n",output);

    u2g(udata.service, strlen(udata.service), output,64);
    tinyrl_printf(this->tinyrl,"Service :%s\n",output);


	return BOOL_TRUE;

	    edfaInfo_t  *ptr = getEdfaInfo(0 ,saveSlotid);

    if(ptr == NULL)
    {
        tinyrl_printf(this->tinyrl,"Etu Is Not Online!\n");
        return BOOL_TRUE;
    }


    tinyrl_printf(this->tinyrl,"Etu information\n");

    tinyrl_printf(this->tinyrl,"Input power(dbm)  Output power(dbm)  Temperature(%s) \n",input );
    tinyrl_printf(this->tinyrl,"  %.1f               %.1f              %.1f\n",ptr->inputPower*0.1,ptr->outputPower*0.1,ptr->temperature*0.1);

    tinyrl_printf(this->tinyrl,"Operating current(mA)  Refrigeration current(mA)   Power(dbm)  Temperature(%s)     state\n",input );
    tinyrl_printf(this->tinyrl,"       %d                    %d                      %.1f         %.1f            ",
                  ptr->pump_status[0].working_current,ptr->pump_status[0].cool_current,
                  ptr->pump_status[0].power*0.1,ptr->pump_status[0].temperature*0.1);


    if(ptr->pump1_status == 1)
    {
        printf("enable\n");
    }
    else
    {
        printf("disable\n");
    }

}

static bool_t
clish_osu_system(const clish_context_t *context,
                 const lub_argv_t    *argv)
{

    uchar_t chassis, slot, index;

    uchar_t *status;

    char output[64];

    unit_user_data_t udata;


    clish_shell_t *this = (clish_shell_t *)context->shell;

    status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != BLOAD_PASSIVE)
    {

        tinyrl_printf(this->tinyrl,"slot[%d] is not osu %d\n",saveSlotid,status[saveSlotid]);

        return ;
    }

    /*
    	unitProperty_t *pro = getUnitProperty( 0 , saveSlotid, 1);

        tinyrl_printf(this->tinyrl,"olp %s\n",pro->unit_class);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->unit_type);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->hardware_type);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->firmware_type);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->struct_type);
    */

    unit_base_info_t *info = getBaseInfoMap(0 , saveSlotid);

    tinyrl_printf(this->tinyrl,"Sn :%s\n",info->sn);
    tinyrl_printf(this->tinyrl,"Model :%s\n",info->model);
    tinyrl_printf(this->tinyrl,"Creation :%s\n",info->creation);
    tinyrl_printf(this->tinyrl,"Soft Version :%s\n",info->fwver);
    tinyrl_printf(this->tinyrl,"Hard Version :%s\n",info->hwver);
    tinyrl_printf(this->tinyrl,"Software Last Time :%s",ctime(&info->uptime));
    int tTemp = difftime(time(NULL) , info->uptime);

    tinyrl_printf(this->tinyrl,"Software Running Time :%s\n",parseTimeSeconds(tTemp));

    getUserDataFromDB(0 , saveSlotid ,&udata);

    u2g(udata.name, strlen(udata.name), output,64);
    tinyrl_printf(this->tinyrl,"Name :%s\n",output);
    u2g(udata.location, strlen(udata.location), output,64);
    tinyrl_printf(this->tinyrl,"Location :%s\n",output);
    u2g(udata.contact, strlen(udata.contact), output,64);
    tinyrl_printf(this->tinyrl,"Contact :%s\n",output);
    u2g(udata.updev, strlen(udata.updev), output,64);
    tinyrl_printf(this->tinyrl,"Up device :%s\n",output);
    u2g(udata.userdata, strlen(udata.userdata), output,64);
    tinyrl_printf(this->tinyrl,"User data :%s\n",output);
    u2g(udata.service, strlen(udata.service), output,64);
    tinyrl_printf(this->tinyrl,"Service :%s\n",output);
}


static bool_t
clish_odm_system(const clish_context_t *context,
                 const lub_argv_t    *argv)
{

    uchar_t chassis, slot, index;

    uchar_t *status;

    char output[64];

    unit_user_data_t udata;


    clish_shell_t *this = (clish_shell_t *)context->shell;

    status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != BLOAD_PASSIVE)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not odm \n",saveSlotid);
        return ;
    }


    unit_base_info_t *info = getBaseInfoMap(0 , saveSlotid);

    tinyrl_printf(this->tinyrl,"Sn :%s\n",info->sn);
    tinyrl_printf(this->tinyrl,"Model :%s\n",info->model);
    tinyrl_printf(this->tinyrl,"Creation :%s\n",info->creation);
    tinyrl_printf(this->tinyrl,"Soft Version :%s\n",info->fwver);
    tinyrl_printf(this->tinyrl,"Hard Version :%s\n",info->hwver);
    tinyrl_printf(this->tinyrl,"Software Last Time :%s",ctime(&info->uptime));
    int tTemp = difftime(time(NULL) , info->uptime);

    tinyrl_printf(this->tinyrl,"Software Running Time :%s\n",parseTimeSeconds(tTemp));

    getUserDataFromDB(0 , saveSlotid ,&udata);

    u2g(udata.name, strlen(udata.name), output,64);
    tinyrl_printf(this->tinyrl,"Name :%s\n",output);
    u2g(udata.location, strlen(udata.location), output,64);
    tinyrl_printf(this->tinyrl,"Location :%s\n",output);
    u2g(udata.contact, strlen(udata.contact), output,64);
    tinyrl_printf(this->tinyrl,"Contact :%s\n",output);
    u2g(udata.updev, strlen(udata.updev), output,64);
    tinyrl_printf(this->tinyrl,"Up device :%s\n",output);
    u2g(udata.userdata, strlen(udata.userdata), output,64);
    tinyrl_printf(this->tinyrl,"User data :%s\n",output);
    u2g(udata.service, strlen(udata.service), output,64);
    tinyrl_printf(this->tinyrl,"Service :%s\n",output);
}


static bool_t
clish_olp_system(const clish_context_t *context,
                 const lub_argv_t    *argv)
{

    uchar_t chassis, slot, index;

    uchar_t *status;

    char output[64];

    unit_user_data_t udata;


    clish_shell_t *this = (clish_shell_t *)context->shell;

    status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != olp)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not olp \n",saveSlotid);
        return ;
    }

    olp1P1Info_t  *ptr = getOlp1P1Info(0 ,saveSlotid);

    if(ptr == NULL)
    {
        tinyrl_printf(this->tinyrl,"Olp Is Not Online!\n");
        return BOOL_TRUE;
    }


    /*
    	unitProperty_t *pro = getUnitProperty( 0 , saveSlotid, 1);

        tinyrl_printf(this->tinyrl,"olp %s\n",pro->unit_class);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->unit_type);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->hardware_type);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->firmware_type);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->struct_type);
    */

    unit_base_info_t *info = getBaseInfoMap(0 , saveSlotid);

    tinyrl_printf(this->tinyrl,"Sn :%s\n",info->sn);
    tinyrl_printf(this->tinyrl,"Model :%s\n",info->model);
    tinyrl_printf(this->tinyrl,"Creation :%s\n",info->creation);
    tinyrl_printf(this->tinyrl,"Soft Version :%s\n",info->fwver);
    tinyrl_printf(this->tinyrl,"Hard Version :%s\n",info->hwver);
    tinyrl_printf(this->tinyrl,"Software Last Time :%s",ctime(&info->uptime));
    int tTemp = difftime(time(NULL) , info->uptime);

    tinyrl_printf(this->tinyrl,"Software Running Time :%s\n",parseTimeSeconds(tTemp));

    getUserDataFromDB(0 , saveSlotid ,&udata);

    u2g(udata.name, strlen(udata.name), output,64);
    tinyrl_printf(this->tinyrl,"Name :%s\n",output);
    u2g(udata.location, strlen(udata.location), output,64);
    tinyrl_printf(this->tinyrl,"Location :%s\n",output);
    u2g(udata.contact, strlen(udata.contact), output,64);
    tinyrl_printf(this->tinyrl,"Contact :%s\n",output);
    u2g(udata.updev, strlen(udata.updev), output,64);
    tinyrl_printf(this->tinyrl,"Up device :%s\n",output);
    u2g(udata.userdata, strlen(udata.userdata), output,64);
    tinyrl_printf(this->tinyrl,"User data :%s\n",output);
    u2g(udata.service, strlen(udata.service), output,64);
    tinyrl_printf(this->tinyrl,"Service :%s\n",output);

    tinyrl_printf(this->tinyrl,"OLP information\n");
    if(ptr->mode == 1)
    {
        tinyrl_printf(this->tinyrl,"Work Mode :Automatic\n");

    }
    else if(ptr->mode == 2)
    {

        tinyrl_printf(this->tinyrl,"Work Mode :Manual\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"Work Mode :Unknow\n");
    }

    if(ptr->line == 1)
    {
        tinyrl_printf(this->tinyrl,"Line Rule :Select L1\n");

    }
    else if(ptr->line == 2)
    {

        tinyrl_printf(this->tinyrl,"Line Rule :Select L2\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"Line Rule :Unknow\n");
    }



    if(ptr->rule == 1)
    {
        tinyrl_printf(this->tinyrl,"Rule Working :Firbe First\n");

    }
    else if(ptr->rule == 2)
    {

        tinyrl_printf(this->tinyrl,"Rule Optical :power High First\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"Rule :Unknow\n");
    }


    if(ptr->ret_mode == 1)
    {
        tinyrl_printf(this->tinyrl,"Return Mode :Automatic\n");

    }
    else if(ptr->ret_mode == 2)
    {

        tinyrl_printf(this->tinyrl,"Return Mode :Manual\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"Return Mode :Unknow\n");
    }
    tinyrl_printf(this->tinyrl,"Return Time :%d (min)\n",ptr->ret_time);



    tinyrl_printf(this->tinyrl,"Port  Optical power(dbm)  Wave length(nm) Switch power(dbm)\n" );


    tinyrl_printf(this->tinyrl,"R1        %.1f           ",ptr->rx1_power*0.1);


    if(ptr->rx1_wave_len == 1)
    {
        tinyrl_printf(this->tinyrl," 1310");

    }
    else if(ptr->rx1_wave_len == 2)
    {

        tinyrl_printf(this->tinyrl," 1550");
    }
    else
    {
        tinyrl_printf(this->tinyrl," unknown");
    }

    tinyrl_printf(this->tinyrl,"          %.1f\n",ptr->l1_power*0.1);


    tinyrl_printf(this->tinyrl,"R2        %.1f           ",ptr->rx2_power*0.1);

    if(ptr->rx2_wave_len == 1)
    {
        tinyrl_printf(this->tinyrl," 1310");

    }
    else if(ptr->rx2_wave_len == 2)
    {

        tinyrl_printf(this->tinyrl," 1550");
    }
    else
    {
        tinyrl_printf(this->tinyrl," unknown");
    }
    tinyrl_printf(this->tinyrl,"          %.1f\n",ptr->l2_power*0.1);


    tinyrl_printf(this->tinyrl,"Tx        %.1f           ",ptr->tx_power*0.1);
    if(ptr->tx_wave_len == 1)
    {
        tinyrl_printf(this->tinyrl," 1310");

    }
    else if(ptr->tx_wave_len == 2)
    {

        tinyrl_printf(this->tinyrl," 1550");
    }
    else
    {
        tinyrl_printf(this->tinyrl," unknown");
    }

    tinyrl_printf(this->tinyrl,"          %.1f\n",ptr->tx_alm_power*0.1);

}



static bool_t
clish_dvoa_system(const clish_context_t *context,
                 const lub_argv_t    *argv)
{

    uchar_t chassis, slot, index;

    uchar_t *status;

    char output[64];

    unit_user_data_t udata;


    clish_shell_t *this = (clish_shell_t *)context->shell;

    status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != BLOAD_DVOA)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not DVOA \n",saveSlotid);
        return ;
    }

 /*   olp1P1Info_t  *ptr = getOlp1P1Info(0 ,saveSlotid);

    if(ptr == NULL)
    {
        tinyrl_printf(this->tinyrl,"DVOA Is Not Online!\n");
        return BOOL_TRUE;
    }
*/

    /*
    	unitProperty_t *pro = getUnitProperty( 0 , saveSlotid, 1);

        tinyrl_printf(this->tinyrl,"olp %s\n",pro->unit_class);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->unit_type);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->hardware_type);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->firmware_type);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->struct_type);
    */

    unit_base_info_t *info = getBaseInfoMap(0 , saveSlotid);

    tinyrl_printf(this->tinyrl,"Sn :%s\n",info->sn);
    tinyrl_printf(this->tinyrl,"Model :%s\n",info->model);
    tinyrl_printf(this->tinyrl,"Creation :%s\n",info->creation);
    tinyrl_printf(this->tinyrl,"Soft Version :%s\n",info->fwver);
    tinyrl_printf(this->tinyrl,"Hard Version :%s\n",info->hwver);
    tinyrl_printf(this->tinyrl,"Software Last Time :%s",ctime(&info->uptime));
    int tTemp = difftime(time(NULL) , info->uptime);

    tinyrl_printf(this->tinyrl,"Software Running Time :%s\n",parseTimeSeconds(tTemp));

    getUserDataFromDB(0 , saveSlotid ,&udata);

    u2g(udata.name, strlen(udata.name), output,64);
    tinyrl_printf(this->tinyrl,"Name :%s\n",output);
    u2g(udata.location, strlen(udata.location), output,64);
    tinyrl_printf(this->tinyrl,"Location :%s\n",output);
    u2g(udata.contact, strlen(udata.contact), output,64);
    tinyrl_printf(this->tinyrl,"Contact :%s\n",output);
    u2g(udata.updev, strlen(udata.updev), output,64);
    tinyrl_printf(this->tinyrl,"Up device :%s\n",output);
    u2g(udata.userdata, strlen(udata.userdata), output,64);
    tinyrl_printf(this->tinyrl,"User data :%s\n",output);
    u2g(udata.service, strlen(udata.service), output,64);
    tinyrl_printf(this->tinyrl,"Service :%s\n",output);

    tinyrl_printf(this->tinyrl,"DVOA information\n");
 
}





static bool_t
clish_olpo_system(const clish_context_t *context,
                 const lub_argv_t    *argv)
{

    uchar_t chassis, slot, index;

    uchar_t *status;

    char output[64];

    unit_user_data_t udata;


    clish_shell_t *this = (clish_shell_t *)context->shell;

    status = getUnitStatusMap(0);

    if (NULL == status|| status[saveSlotid] != olp)
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not olp \n",saveSlotid);
        return ;
    }

    olp1P1Info_t  *ptr = getOlp1P1Info(0 ,saveSlotid);

    if(ptr == NULL)
    {
        tinyrl_printf(this->tinyrl,"Olp Is Not Online!\n");
        return BOOL_TRUE;
    }


    /*
    	unitProperty_t *pro = getUnitProperty( 0 , saveSlotid, 1);

        tinyrl_printf(this->tinyrl,"olp %s\n",pro->unit_class);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->unit_type);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->hardware_type);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->firmware_type);
        tinyrl_printf(this->tinyrl,"olp %s\n",pro->struct_type);
    */

    unit_base_info_t *info = getBaseInfoMap(0 , saveSlotid);

    tinyrl_printf(this->tinyrl,"Sn :%s\n",info->sn);
    tinyrl_printf(this->tinyrl,"Model :%s\n",info->model);
    tinyrl_printf(this->tinyrl,"Creation :%s\n",info->creation);
    tinyrl_printf(this->tinyrl,"Soft Version :%s\n",info->fwver);
    tinyrl_printf(this->tinyrl,"Hard Version :%s\n",info->hwver);
    tinyrl_printf(this->tinyrl,"Software Last Time :%s",ctime(&info->uptime));
    int tTemp = difftime(time(NULL) , info->uptime);

    tinyrl_printf(this->tinyrl,"Software Running Time :%s\n",parseTimeSeconds(tTemp));

    getUserDataFromDB(0 , saveSlotid ,&udata);

    u2g(udata.name, strlen(udata.name), output,64);
    tinyrl_printf(this->tinyrl,"Name :%s\n",output);
    u2g(udata.location, strlen(udata.location), output,64);
    tinyrl_printf(this->tinyrl,"Location :%s\n",output);
    u2g(udata.contact, strlen(udata.contact), output,64);
    tinyrl_printf(this->tinyrl,"Contact :%s\n",output);
    u2g(udata.updev, strlen(udata.updev), output,64);
    tinyrl_printf(this->tinyrl,"Up device :%s\n",output);
    u2g(udata.userdata, strlen(udata.userdata), output,64);
    tinyrl_printf(this->tinyrl,"User data :%s\n",output);
    u2g(udata.service, strlen(udata.service), output,64);
    tinyrl_printf(this->tinyrl,"Service :%s\n",output);

    tinyrl_printf(this->tinyrl,"OLP information\n");
    if(ptr->mode == 1)
    {
        tinyrl_printf(this->tinyrl,"Work Mode :Automatic\n");

    }
    else if(ptr->mode == 2)
    {

        tinyrl_printf(this->tinyrl,"Work Mode :Manual\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"Work Mode :Unknow\n");
    }

    if(ptr->line == 1)
    {
        tinyrl_printf(this->tinyrl,"Line Rule :Select L1\n");

    }
    else if(ptr->line == 2)
    {

        tinyrl_printf(this->tinyrl,"Line Rule :Select L2\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"Line Rule :Unknow\n");
    }


    tinyrl_printf(this->tinyrl,"Port  Optical power(dbm)  Wave length(nm) Switch power(dbm)\n" );


    tinyrl_printf(this->tinyrl,"R1        %.1f           ",ptr->rx1_power*0.1);


    if(ptr->rx1_wave_len == 1)
    {
        tinyrl_printf(this->tinyrl," 1310");

    }
    else if(ptr->rx1_wave_len == 2)
    {

        tinyrl_printf(this->tinyrl," 1550");
    }
    else
    {
        tinyrl_printf(this->tinyrl," unknown");
    }

    tinyrl_printf(this->tinyrl,"          %.1f\n",ptr->l1_power*0.1);


    tinyrl_printf(this->tinyrl,"R2        %.1f           ",ptr->rx2_power*0.1);

    if(ptr->rx2_wave_len == 1)
    {
        tinyrl_printf(this->tinyrl," 1310");

    }
    else if(ptr->rx2_wave_len == 2)
    {

        tinyrl_printf(this->tinyrl," 1550");
    }
    else
    {
        tinyrl_printf(this->tinyrl," unknown");
    }
    tinyrl_printf(this->tinyrl,"          %.1f\n",ptr->l2_power*0.1);


    tinyrl_printf(this->tinyrl,"Tx        %.1f           ",ptr->tx_power*0.1);
    if(ptr->tx_wave_len == 1)
    {
        tinyrl_printf(this->tinyrl," 1310");

    }
    else if(ptr->tx_wave_len == 2)
    {

        tinyrl_printf(this->tinyrl," 1550");
    }
    else
    {
        tinyrl_printf(this->tinyrl," unknown");
    }

    tinyrl_printf(this->tinyrl,"          %.1f\n",ptr->tx_alm_power*0.1);

}
char strValue[30];
char* inputGetFromValue(int inValue)
{
            memset(strValue,0,30);

            if (inValue == 1000)
            {
                  strcpy(strValue,"10G-AUTO");
            }
            else if (inValue == 1104)
            {
                  strcpy(strValue,"OTU-1e");
            }
            else if (inValue == 1070)
            {
                   strcpy(strValue,"OTU-2");
            }
            else if (inValue == 1109)
            {
                  strcpy(strValue,"OTU-2e");
            }
            else if (inValue == 995)
            {
                   strcpy(strValue,"STM-64/OC-192");
            }
            else if (inValue == 1079)
            {
                  strcpy(strValue, "STM-64/OC-192+FEC");
            }
            else if (inValue == 249)
            {
                   strcpy(strValue,"STM-16/OC-48");
            }
            else if (inValue == 1031)
            {
                   strcpy(strValue,"10GbE");
            }
            else if (inValue == 250)
            {
                   strcpy(strValue,"2.5GbE");
            }
            else if (inValue == 1051)
            {
                   strcpy(strValue,"10GFC");
            }
            else if (inValue == 1131)
            {
                   strcpy(strValue,"10GFC+FEC");
            }
            else if (inValue == 850)
            {
                   strcpy(strValue,"8GFC");
            }
            else if (inValue == 425)
            {
                   strcpy(strValue,"4GFC");
            }
            else if (inValue == 212)
            {
                   strcpy(strValue,"2GFC");
            }
            else
            {
                   strcpy(strValue,"Unknow");
            }
            return strValue;
}

 char *  inputWavelengValue(short inValue)
 {
     memset(strValue,0,30);
	 
     if(inValue == 1)
         sprintf(strValue,"%d" ,850);
     else if(inValue >= 10 && inValue <=28)
     {
          sprintf(strValue,"%d" ,(1270+(inValue-10)*20));
     }
     else if(inValue >= 30 && inValue <= 94)
     {
          sprintf(strValue,"C%d" ,(inValue-20));
     }
     else if (inValue > 100)
     {
          sprintf(strValue,"%.2f" ,(float)inValue/20);
     }
	 else
	 {
        return "    ";
	 }
	 return strValue;

	 
 }

 char strValuea[30];

 char* inputGetFromValueTi(int inValue)
 {
            memset(strValuea,0,30);

            if (inValue == 850)
            {
                  strcpy(strValuea,"1Gbps~9.95Gbps  ");
            }
            else if (inValue == 1000)
            {
                  strcpy(strValuea,"9.95Gbps~11.3Gbps");
            }
            else 
            {
                   strcpy(strValuea,"                 ");
            }
			return strValuea;
  }	

	

static bool_t
clish_otu_system(const clish_context_t *context,
                 const lub_argv_t    *argv)
{

    uchar_t chassis, slot, index;

    unit_user_data_t udata;
    char output[64];
    int i;
    char *input = "��C";

    clish_shell_t *this = (clish_shell_t *)context->shell;
    uchar_t *status;
    int n;
    status = getUnitStatusMap(0);

    if (NULL == status|| (status[saveSlotid] != otu &&status[saveSlotid] != oeo))
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not otu Or not oeo\n",saveSlotid);
        return ;
    }

    unit_base_info_t *info = getBaseInfoMap(0 , saveSlotid);

    int tTemp = difftime(time(NULL) , info->uptime);

    tinyrl_printf(this->tinyrl,"Sn :%s\n",info->sn);
    tinyrl_printf(this->tinyrl,"Model :%s\n",info->model);
    tinyrl_printf(this->tinyrl,"Creation :%s\n",info->creation);
    tinyrl_printf(this->tinyrl,"Soft Version :%s\n",info->fwver);
    tinyrl_printf(this->tinyrl,"Hard Version :%s\n",info->hwver);
    tinyrl_printf(this->tinyrl,"Software Last Time :%s",ctime(&info->uptime));
    tinyrl_printf(this->tinyrl,"Software Running Time :%s\n",parseTimeSeconds(tTemp));


    getUserDataFromDB(0 , saveSlotid ,&udata);

    u2g(udata.name, strlen(udata.name), output,64);
    tinyrl_printf(this->tinyrl,"Name :%s\n",output);
    u2g(udata.location, strlen(udata.location), output,64);
    tinyrl_printf(this->tinyrl,"Location :%s\n",output);
    u2g(udata.contact, strlen(udata.contact), output,64);
    tinyrl_printf(this->tinyrl,"Contact :%s\n",output);
    u2g(udata.updev, strlen(udata.updev), output,64);
    tinyrl_printf(this->tinyrl,"Up device :%s\n",output);
    u2g(udata.userdata, strlen(udata.userdata), output,64);
    tinyrl_printf(this->tinyrl,"User data :%s\n",output);
    u2g(udata.service, strlen(udata.service), output,64);
    tinyrl_printf(this->tinyrl,"Service :%s\n",output);


    otuUnitStatus_t  *pOtuStatus = getOtuStatus(0 ,saveSlotid);

    otuPortStatus_t *pPortStatus;

    tinyrl_printf(this->tinyrl,"OTU information\n");
    if(pOtuStatus->work_mode == 1)
    {
        tinyrl_printf(this->tinyrl,"work mode normal\n");
    }
    else if(pOtuStatus->work_mode == 2)
    {
        tinyrl_printf(this->tinyrl,"work mode protect\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"work mode error\n");
    }


    tinyrl_printf(this->tinyrl,"Port    Status    TX enable   Current speed      Distance(Km) Max speed  Wave lenght(nm)    Loopback\n");

    for( i= 1 ; i <= 4 ; i++)
    {
        pPortStatus   =  &pOtuStatus->portStatus[i-1];
        if(pPortStatus->on_module == 1)
        {

            if(pPortStatus->port_status == 1)
            {
                tinyrl_printf(this->tinyrl,"%d       normal     ",i);
            }
            else 		if(pPortStatus->port_status == 2)
            {
                tinyrl_printf(this->tinyrl,"%d       abnormal    ",i);
            }
            else 		if(pPortStatus->port_status == 3)
            {
                tinyrl_printf(this->tinyrl,"%d       other      ",i);
            }
            else
            {

            }


            if(pPortStatus->tx_enable == 1)
            {
                tinyrl_printf(this->tinyrl,"enable    ");
            }
            else 		if(pPortStatus->tx_enable == 2)
            {
                tinyrl_printf(this->tinyrl,"disable    ");
            }
            else 		if(pPortStatus->tx_enable == 3)
            {
                tinyrl_printf(this->tinyrl,"auto       ");
            }
            else
            {

            }

            otuPortProperty_t *pOtuPortProperty = getOtuPortProperty( 0 , saveSlotid, i);
            if(pOtuPortProperty == NULL)
            {
                return ;
            }

            tinyrl_printf(this->tinyrl," %.30s   %.2f          %.1fG       %.5s",inputGetFromValueTi(pOtuPortProperty->current_speed),(float)pOtuPortProperty->distance/100,
                          (float)pOtuPortProperty->max_speed/100,inputWavelengValue(pOtuPortProperty->wave_lenght));

            otuPortFEC_t  *pOtuFec  = getOtuFec(0 ,saveSlotid,i);
            if(pOtuFec->loopback == 1)
            {
                tinyrl_printf(this->tinyrl,"       enable\n",i);
            }
            else 	if(pOtuFec->loopback == 2)
            {
                tinyrl_printf(this->tinyrl,"       disable\n",i);
            }
            else
            {

            }
        }
    }

    tinyrl_printf(this->tinyrl,"Port ddm info\n");
    tinyrl_printf(this->tinyrl,"Port     RX Power(dbm)  TX Power(dbm)  Temperature(%s)  Volt(v)  Bias current(mA)\n",input);
    for( i= 1 ; i <= 4 ; i++)
    {
        ddm_t *pDdmInfo =  getPortDdmInfo(0 ,saveSlotid, i);
        if(pDdmInfo == NULL)
        {
            return ;
        }
        pPortStatus   =  &pOtuStatus->portStatus[i-1];
        if(pPortStatus->on_module == 1)
        {
            tinyrl_printf(this->tinyrl,"%d            %.1f            %.1f        %.1f           %.1f         %d\n",i,(float)pDdmInfo->rxPower/10,
                          (float)pDdmInfo->txPower/10,(float)pDdmInfo->temperature/10,(float)pDdmInfo->rxVolt/10,pDdmInfo->rxBiasCurrent);
        }
    }


    tinyrl_printf(this->tinyrl,"Port ddm threshold\n");
    tinyrl_printf(this->tinyrl,"Port      RX Power(dbm)          TX Power(dbm)         temp(%s)           Volt(v)          TX Bias(mA)\n",input);
    tinyrl_printf(this->tinyrl,"         Min        Max         Min         Max      Min     Max      Min      Max        Min     Max\n");
    for( i= 1 ; i <= 4 ; i++)
    {
        ddmThreshold_t *pDdmThr = getPortDdmThreshold(0 ,saveSlotid,i);
        if(pDdmThr == NULL)
        {
            return ;
        }
        tinyrl_printf(this->tinyrl,"%d       %d          %d           %d         %d         %d        %d      %d         %d       %d      %d\n",i,pDdmThr->rxPowerMin,pDdmThr->rxPowerMax
                      ,pDdmThr->txPowerMin,pDdmThr->txPowerMax,pDdmThr->tempMin,pDdmThr->tempMax,pDdmThr->voltMin,pDdmThr->voltMax,pDdmThr->rxBiasMin
                      ,pDdmThr->rxBiasMax);
    }

}



static bool_t
clish_otueight_system(const clish_context_t *context,
                      const lub_argv_t    *argv)
{

    uchar_t chassis, slot, index;

    unit_user_data_t udata;
    char output[64];
    int i;
    char *input = "��C";

    clish_shell_t *this = (clish_shell_t *)context->shell;
    uchar_t *status;
    int n;
    status = getUnitStatusMap(0);

    if (NULL == status|| (status[saveSlotid] != otu &&status[saveSlotid] != oeo))
    {
        tinyrl_printf(this->tinyrl,"slot[%d] is not otu Or not oeo\n",saveSlotid);
        return ;
    }



    unit_base_info_t *info = getBaseInfoMap(0 , saveSlotid);

    int tTemp = difftime(time(NULL) , info->uptime);

    tinyrl_printf(this->tinyrl,"Sn :%s\n",info->sn);
    tinyrl_printf(this->tinyrl,"Model :%s\n",info->model);
    tinyrl_printf(this->tinyrl,"Creation :%s\n",info->creation);
    tinyrl_printf(this->tinyrl,"Soft Version :%s\n",info->fwver);
    tinyrl_printf(this->tinyrl,"Hard Version :%s\n",info->hwver);
    tinyrl_printf(this->tinyrl,"Software Last Time :%s",ctime(&info->uptime));
    tinyrl_printf(this->tinyrl,"Software Running Time :%s\n",parseTimeSeconds(tTemp));


    getUserDataFromDB(0 , saveSlotid ,&udata);

    u2g(udata.name, strlen(udata.name), output,64);
    tinyrl_printf(this->tinyrl,"Name :%s\n",output);
    u2g(udata.location, strlen(udata.location), output,64);
    tinyrl_printf(this->tinyrl,"Location :%s\n",output);
    u2g(udata.contact, strlen(udata.contact), output,64);
    tinyrl_printf(this->tinyrl,"Contact :%s\n",output);
    u2g(udata.updev, strlen(udata.updev), output,64);
    tinyrl_printf(this->tinyrl,"Up device :%s\n",output);
    u2g(udata.userdata, strlen(udata.userdata), output,64);
    tinyrl_printf(this->tinyrl,"User data :%s\n",output);
    u2g(udata.service, strlen(udata.service), output,64);
    tinyrl_printf(this->tinyrl,"Service :%s\n",output);


    otuUnitStatus_t  *pOtuStatus = getOtuStatus(0 ,saveSlotid);

    otuPortStatus_t *pPortStatus;

    tinyrl_printf(this->tinyrl,"OTU information\n");
    if(pOtuStatus->work_mode == 1)
    {
        tinyrl_printf(this->tinyrl,"work mode normal\n");
    }
    else if(pOtuStatus->work_mode == 2)
    {
        tinyrl_printf(this->tinyrl,"work mode protect\n");
    }
    else
    {
        tinyrl_printf(this->tinyrl,"work mode error\n");
    }


    tinyrl_printf(this->tinyrl,"Port    Distance(Km)  Wave lenght  Current speed    TX enable   Status  \n");

    for( i= 1 ; i <= 8 ; i++)
    {
        pPortStatus   =  &pOtuStatus->portStatus[i-1];
        if(pPortStatus->on_module == 1)
        {
            otuPortProperty_t *pOtuPortProperty = getOtuPortProperty( 0 , saveSlotid, i);
            tinyrl_printf(this->tinyrl,"%d           %.4dKm       %.2fnm       2.50G         ",i,pOtuPortProperty->distance/100,(float)pOtuPortProperty->wave_lenght/20);

            if(pPortStatus->port_status == 1)
            {
                tinyrl_printf(this->tinyrl,"normal      ");
            }
            else 		if(pPortStatus->port_status == 2)
            {
                tinyrl_printf(this->tinyrl,"abnormal    ");
            }
            else 		if(pPortStatus->port_status == 3)
            {
                tinyrl_printf(this->tinyrl,"other       ");
            }
            else
            {

            }


            if(pPortStatus->tx_enable == 1)
            {
                tinyrl_printf(this->tinyrl,"enable    ");
            }
            else 		if(pPortStatus->tx_enable == 2)
            {
                tinyrl_printf(this->tinyrl,"disable    ");
            }
            else 		if(pPortStatus->tx_enable == 3)
            {
                tinyrl_printf(this->tinyrl,"auto       ");
            }
            else
            {

            }
            tinyrl_printf(this->tinyrl,"\n");
            /*
                    otuPortProperty_t *pOtuPortProperty = getOtuPortProperty( 0 , saveSlotid, i);
                    if(pOtuPortProperty == NULL)
                    {
                        return ;
                    }

                    tinyrl_printf(this->tinyrl," %.2fG            %d          %.1fG     %.2f",(float)pOtuPortProperty->current_speed/100,pOtuPortProperty->distance/100,
                                  (float)pOtuPortProperty->max_speed/100,(float)pOtuPortProperty->wave_lenght/20);

                    otuPortFEC_t  *pOtuFec  = getOtuFec(0 ,saveSlotid,i);
                    if(pOtuFec->loopback == 1)
                    {
                        tinyrl_printf(this->tinyrl,"       enable\n",i);
                    }
                    else 	if(pOtuFec->loopback == 2)
                    {
                        tinyrl_printf(this->tinyrl,"       disable\n",i);
                    }
                    else
                    {

                    }
            */
        }
    }

    tinyrl_printf(this->tinyrl,"Port ddm info\n");
    tinyrl_printf(this->tinyrl,"Port     RX Power(dbm)  TX Power(dbm)  Temperature(%s)  Volt(v)  Bias current(mA)\n",input);
    for( i= 1 ; i <= 8 ; i++)
    {

        ddm_t *pDdmInfo =  getPortDdmInfo(0 ,saveSlotid, i);
        pPortStatus   =  &pOtuStatus->portStatus[i-1];
        if(pPortStatus->on_module == 1)
        {
            if(pDdmInfo == NULL)
            {
                return ;
            }
            tinyrl_printf(this->tinyrl,"%d            %.1f            %.1f        %.1f           %.1f         %d\n",i,(float)pDdmInfo->rxPower/10,
                          (float)pDdmInfo->txPower/10,(float)pDdmInfo->temperature/10,(float)pDdmInfo->rxVolt/10,pDdmInfo->rxBiasCurrent);
        }
    }

/*
    tinyrl_printf(this->tinyrl,"Port ddm threshold\n");
    tinyrl_printf(this->tinyrl,"Port      RX Power(dbm)          TX Power(dbm)         temp(%s)           Volt(v)          TX Bias(mA)\n",input);
    tinyrl_printf(this->tinyrl,"         Min        Max         Min         Max      Min     Max      Min      Max        Min     Max\n");
    for( i= 1 ; i <= 8 ; i++)
    {
        ddmThreshold_t *pDdmThr = getPortDdmThreshold(0 ,saveSlotid,i);
        if(pDdmThr == NULL)
        {
            return ;
        }
        tinyrl_printf(this->tinyrl,"%d       %d          %d           %d         %d         %d        %d      %d         %d       %d      %d\n",i,pDdmThr->rxPowerMin,pDdmThr->rxPowerMax
                      ,pDdmThr->txPowerMin,pDdmThr->txPowerMax,pDdmThr->tempMin,pDdmThr->tempMax,pDdmThr->voltMin,pDdmThr->voltMax,pDdmThr->rxBiasMin
                      ,pDdmThr->rxBiasMax);
    }
    */

}



static bool_t
clish_show_nmu(const clish_context_t *context,
               const lub_argv_t    *argv)
{
    clish_shell_t *this = (clish_shell_t *)context->shell;
    const char        *arg   = lub_argv__get_arg(argv,0);
    char output[64];

    int slotid = atoi(arg);

    unit_user_data_t udata;
    uchar_t *status;

    unit_base_info_t *info;
    int n;
    status = getUnitStatusMap(0);

    if(slotid == CurrentBloadNum)
    {

        chassisThreshold_t *pThreshold = getChassisThreshold(0);

        if(pThreshold == NULL)
        {

            tinyrl_printf(this->tinyrl,"System wrong,please check!!!!\n");

            return BOOL_TRUE;

        }

        chassis_t *pChassis = getChassisInfo(0);

        if(pChassis == NULL)
        {

            tinyrl_printf(this->tinyrl,"pChassis wrong,please check!!!!\n");

            return BOOL_TRUE;

        }

        unit_base_info_t *info ;
//getBaseInfoMap
        info  = getBaseInfoMap(0 , CurrentBloadNum);
        /*
        	if(info == NULL)
            {

                tinyrl_printf(this->tinyrl,"info wrong,please check!!!!\n");

                return BOOL_TRUE;

            }
        */

        getUserDataFromDB(0 , CurrentBloadNum ,&udata);

        tinyrl_printf(this->tinyrl,"System Type: %s\n","4U Platform");

        u2g(udata.name, strlen(udata.name), output,64);
        tinyrl_printf(this->tinyrl,"System Name: %s\n",output);
        u2g(udata.location, strlen(udata.location), output,64);
        tinyrl_printf(this->tinyrl,"System Location: %s\n",output);
        u2g(udata.contact, strlen(udata.contact), output,64);
        tinyrl_printf(this->tinyrl,"System Contact: %s\n",output);
        u2g(udata.updev, strlen(udata.updev), output,64);
        tinyrl_printf(this->tinyrl,"System Up device: %s\n",output);
        u2g(udata.userdata, strlen(udata.userdata), output,64);
        tinyrl_printf(this->tinyrl,"System User data: %s\n",output);
        u2g(udata.service, strlen(udata.service), output,64);
        tinyrl_printf(this->tinyrl,"System Service: %s\n",output);


        tinyrl_printf(this->tinyrl,"System Model: %s\n",info->model);

        tinyrl_printf(this->tinyrl,"System Creation: %s\n",info->creation);

        tinyrl_printf(this->tinyrl,"System Sn: %s\n",info->sn);

        tinyrl_printf(this->tinyrl,"Hardware  Version: %s\n",info->hwver);

        tinyrl_printf(this->tinyrl,"Software  Version: %s\n",info->fwver);
    }
    else
    {
        if (status[slotid] == olp)
        {
            saveSlotid = slotid;
            
			if(getUnitType(0,slotid)== OTU_FOUR||getUnitType(0,slotid)== OTU_CTYPE)
            {
                clish_olp_system(context,argv);
            }
            else if(getUnitType(0,slotid)== OTU_EIGHT)
            {

               clish_olpo_system(context,argv);
            }
        }
        else if (status[slotid] == otu)
        {

            saveSlotid = slotid;
            if(getUnitType(0,slotid)== OTU_FOUR)
            {
                clish_otu_system(context,argv);
            }
            else if(getUnitType(0,slotid)== OTU_EIGHT)
            {

                clish_otueight_system(context,argv);
            }
            else if(getUnitType(0,slotid)== OTU_LIU_FOUR)
            {

                clish_otu_system(context,argv);
            }
        }
        else if (status[slotid] == oeo)
        {

            saveSlotid = slotid;
            if(getUnitType(0,slotid)== OEO_FOUR)
            {
                clish_otu_system(context,argv);
            }
            else if(getUnitType(0,slotid)== OEO_EIGHT)
            {

                clish_otueight_system(context,argv);
            }
			else if(getUnitType(0,slotid)== OEO_BEIGHT)
            {

                clish_otueight_system(context,argv);
            }

        }		
        else if (status[slotid] == BLOAD_EDFA)
        {
            saveSlotid = slotid;
            clish_edfa_system(context,argv);

        }
		else if (status[slotid] == BLOAD_ETU)
        {
            saveSlotid = slotid;
            clish_etu_system(context,argv);

        }
        else if (status[slotid] == BLOAD_OSS)
        {
            saveSlotid = slotid;
            clish_oss_system(context,argv);

        }

        else if (status[slotid] == BLOAD_PASSIVE)
        {
            if(getUnitType(0,slotid)== BLOAD_OSU)
            {
                saveSlotid = slotid;
                clish_osu_system(context,argv);
            }
            else if(getUnitType(0,slotid)== BLOAD_ODM)
            {
                saveSlotid = slotid;
                clish_odm_system(context,argv);

            }
            else if(getUnitType(0,slotid)== BLOAD_AWG)
            {
                saveSlotid = slotid;
                clish_awg_system(context,argv);

            }
        }

        else
        {

            tinyrl_printf(this->tinyrl,"Slot doesn't exist device\n");
        }

    }
}



static bool_t
shell_dev_set_name(const clish_context_t *context,
                   const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    if(FALSE != setUserDataItemToDB(0,saveSlotid,UNIT_NAME_ID,arg))
    {
        printf("Set unit name as %s \n",arg);
    }

}


static bool_t
shell_dev_set_location(const clish_context_t *context,
                       const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    if(FALSE != setUserDataItemToDB(0,saveSlotid,UNIT_LOC_ID,arg))
    {
        printf("Set unit location as %s \n",arg);
    }
}


static bool_t
shell_dev_set_contact(const clish_context_t *context,
                      const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    if(FALSE != setUserDataItemToDB(0,saveSlotid,UNIT_CONTACT_ID,arg))
    {
        printf("Set unit contact as %s \n",arg);
    }
}

static bool_t
shell_dev_set_updev(const clish_context_t *context,
                    const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    if(FALSE != setUserDataItemToDB(0,saveSlotid,UNIT_UP_DEV_ID,arg))
    {
        printf("Set unit updev as %s \n",arg);
    }
}

static bool_t
shell_dev_set_userdata(const clish_context_t *context,
                       const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    if(FALSE != setUserDataItemToDB(0,saveSlotid,UNIT_USER_DATA_ID,arg))
    {
        printf("Set unit userdata as %s \n",arg);
    }
}


static bool_t
shell_dev_set_service(const clish_context_t *context,
                      const lub_argv_t    *argv)
{
    const char                   *arg   = lub_argv__get_arg(argv,0);
    if(FALSE != setUserDataItemToDB(0,saveSlotid,UNIT_SERVICE_ID,arg))
    {
        printf("Set unit service as %s \n",arg);
    }
}



static clish_shell_builtin_t clish_cmd_list[] =
{
    {"clish_close", clish_close},
    {"clish_overview", clish_overview},
    {"clish_source", clish_source},
    {"clish_source_nostop", clish_source_nostop},
    {"clish_history", clish_history},
    {"clish_nested_up", clish_nested_up},
    {"clish_nop", clish_nop},
    {"clish_wdog", clish_wdog},
    {"clish_getsystem",       clish_getsystem},
    {"clish_show_bload",       clish_show_bload},
    {"clish_exit",       clish_exit},
    {"clish_test_show_share_mem",clish_test_show_share_mem},
    {"clish_set_vispace_enetport_type",clish_set_vispace_enetport_type},
    {"clish_reset_bload",clish_reset_bload},
    {"clish_system_default",clish_system_default},
    {"shell_set_snmp_com",shell_set_snmp_com},
    {"shell_set_snmp_trap",shell_set_snmp_trap},
    {"shell_show_snmp",shell_show_snmp},
    {"shell_show_module",shell_show_module},
    {"shell_set_ipaddr",shell_set_ipaddr},
    {"shell_set_gateway",shell_set_gateway},
    {"shell_upgrade_config",shell_upgrade_config},
    {"shell_upgrade_image",shell_upgrade_image},
    {"shell_download_config",shell_download_config},
    {"shell_session_bload",shell_session_bload},
    {"shell_show_users",shell_show_users},
    {"shell_add_changepass",shell_add_changepass},
    {"shell_delete_user",shell_delete_user},
    {"shell_add_user",shell_add_user},
    {"shell_show_alarm",shell_show_alarm},
    {"shell_show_alarmtestadd",shell_show_alarmtestadd},
    {"shell_view_name",shell_view_name},
    {"shell_delete_snmp_trap",shell_delete_snmp_trap},
    {"shell_set_time",shell_set_time},
    {"shell_olp_set_mode",shell_olp_set_mode},
    {"shell_olp_set_selectline",shell_olp_set_selectline},
    {"shell_olp_set_backmode",shell_olp_set_backmode},
    {"shell_olp_set_switchrule",shell_olp_set_switchrule},
    {"shell_olp_set_r1power",shell_olp_set_r1power},
    {"shell_olp_set_r2power",shell_olp_set_r2power},
    {"shell_olp_set_txpower",shell_olp_set_txpower},
    {"shell_olp_set_backtime",shell_olp_set_backtime},
    {"shell_olp_test_in",shell_olp_test_in},
    {"shell_olp_test_out",shell_olp_test_out},
    {"shell_olp_show_all",shell_olp_show_all},
    {"shell_oss_set_mode",shell_oss_set_mode},
    {"shell_oss_set_selectline",shell_oss_set_selectline},
    {"shell_oss_set_backmode",shell_oss_set_backmode},
    {"shell_oss_set_switchrule",shell_oss_set_switchrule},
    {"shell_oss_set_r1power",shell_oss_set_r1power},
    {"shell_oss_set_r2power",shell_oss_set_r2power},
    {"shell_oss_set_txpower",shell_oss_set_txpower},
    {"shell_oss_set_backtime",shell_oss_set_backtime},
    {"shell_edfa_set_mode",shell_edfa_set_mode},
    {"shell_edfa_set_state",shell_edfa_set_state},
    {"shell_setosuport_info",shell_setosuport_info},
    {"shell_setodmport_info",shell_setodmport_info},
    {"shell_setotu_extern",shell_setotu_extern},


    {"shell_system_set_temp",shell_system_set_temp},
    {"shell_system_set_volt",shell_system_set_volt},
    {"shell_system_set_volt",shell_system_set_buzz},
    {"clish_olp_system",clish_olp_system},
    {"clish_olpo_system",clish_olpo_system},
    {"clish_dvoa_system",clish_dvoa_system},

	{"shell_obp_set_heartbeat",shell_obp_set_heartbeat},
	{"shell_obp_set_iplist",shell_obp_set_iplist},
    {"shell_obp_set_parameters",shell_obp_set_parameters},
    {"clish_obp_show_heartbeat",clish_obp_show_heartbeat},

	
    {"clish_otu_system",clish_otu_system},
    {"clish_osu_system",clish_osu_system},
    {"clish_oss_system",clish_oss_system},
    {"clish_edfa_system",clish_edfa_system},
    {"clish_etu_system",clish_etu_system},
    {"clish_odm_system",clish_odm_system},
    {"clish_awg_system",clish_awg_system},
    {"clish_otueight_system",clish_otueight_system},



    {"shell_dev_set_name",shell_dev_set_name},
    {"shell_dev_set_location",shell_dev_set_location},
    {"shell_dev_set_contact",shell_dev_set_contact},
    {"shell_dev_set_updev",shell_dev_set_updev},
    {"shell_dev_set_userdata",shell_dev_set_userdata},
    {"shell_dev_set_service",shell_dev_set_service},
    {"shell_setslot_info",shell_setslot_info},
    {"shell_setotu_info",shell_setotu_info},
    {"shell_setolp_info",shell_setolp_info},
    {"shell_setoss_info",shell_setoss_info},
    {"shell_setotuport_info",shell_setotuport_info},
    {"shell_setotuview_info",shell_setotuview_info},
    {"shell_otu_set_mode",shell_otu_set_mode},
    {"shell_otu_setspeed",shell_otu_setspeed    },
    {"shell_otu_setloopback", shell_otu_setloopback},
    {"shell_otu_setprbs", shell_otu_setprbs    },
    {"shell_otu_setfec",  shell_otu_setfec    },
    {"shell_otu_setfibersend",shell_otu_setfibersend},
    {"shell_olp_set_wavelength",shell_olp_set_wavelength},
    {"shell_otu_setddm",shell_otu_setddm},
    {"shell_delete_snmp_com",shell_delete_snmp_com},
    {"shell_delete_alarm",shell_delete_alarm},
    {"clish_show_nmu",clish_show_nmu},
    {"shell_dev_set_Company",shell_dev_set_Company},    
    
    {NULL, NULL}
};







/*----------------------------------------------------------- */
/* Terminate the current shell session */
static int clish_close(clish_context_t *context, const lub_argv_t * argv)
{
    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;

    argv = argv; /* not used */
    this->state = SHELL_STATE_CLOSING;




    return 0;
}

/*----------------------------------------------------------- */
/*
 Open a file and interpret it as a script in the context of a new
 thread. Whether the script continues after command, but not script,
 errors depends on the value of the stop_on_error flag.
*/
static int clish_source_internal(clish_context_t *context,
                                 const lub_argv_t * argv, bool_t stop_on_error)
{
    int result = -1;
    const char *filename = lub_argv__get_arg(argv, 0);
    struct stat fileStat;

    /* the exception proves the rule... */
    clish_shell_t *this = (clish_shell_t *)context->shell;

    /*
     * Check file specified is not a directory
     */
    if ((0 == stat((char *)filename, &fileStat)) &&
            (!S_ISDIR(fileStat.st_mode)))
    {
        /*
         * push this file onto the file stack associated with this
         * session. This will be closed by clish_shell_pop_file()
         * when it is finished with.
         */
        result = clish_shell_push_file(this, filename,
                                       stop_on_error);
    }

    return result ? -1 : 0;
}

/*----------------------------------------------------------- */
/*
 Open a file and interpret it as a script in the context of a new
 thread. Invoking a script in this way will cause the script to
 stop on the first error
*/
static int clish_source(clish_context_t *context, const lub_argv_t * argv)
{
    return (clish_source_internal(context, argv, BOOL_TRUE));
}

/*----------------------------------------------------------- */
/*
 Open a file and interpret it as a script in the context of a new
 thread. Invoking a script in this way will cause the script to
 continue after command, but not script, errors.
*/
static int clish_source_nostop(clish_context_t *context, const lub_argv_t * argv)
{
    return (clish_source_internal(context, argv, BOOL_FALSE));
}

/*----------------------------------------------------------- */
/*
 Show the shell overview
*/
static int clish_overview(clish_context_t *context, const lub_argv_t * argv)
{
    clish_shell_t *this = context->shell;
    argv = argv; /* not used */

    tinyrl_printf(this->tinyrl, "%s\n", context->shell->overview);

    return 0;
}

/*----------------------------------------------------------- */
static int clish_history(clish_context_t *context, const lub_argv_t * argv)
{
    clish_shell_t *this = context->shell;
    tinyrl_history_t *history = tinyrl__get_history(this->tinyrl);
    tinyrl_history_iterator_t iter;
    const tinyrl_history_entry_t *entry;
    unsigned limit = 0;
    const char *arg = lub_argv__get_arg(argv, 0);

    if (arg && ('\0' != *arg))
    {
        limit = (unsigned)atoi(arg);
        if (0 == limit)
        {
            /* unlimit the history list */
            (void)tinyrl_history_unstifle(history);
        }
        else
        {
            /* limit the scope of the history list */
            tinyrl_history_stifle(history, limit);
        }
    }
    for (entry = tinyrl_history_getfirst(history, &iter);
            entry; entry = tinyrl_history_getnext(&iter))
    {
        /* dump the details of this entry */
        tinyrl_printf(this->tinyrl,
                      "%5d  %s\n",
                      tinyrl_history_entry__get_index(entry),
                      tinyrl_history_entry__get_line(entry));
    }
    return 0;
}

/*----------------------------------------------------------- */
/*
 * Searches for a builtin command to execute
 */
static clish_shell_builtin_fn_t *find_builtin_callback(const
        clish_shell_builtin_t * cmd_list, const char *name)
{
    const clish_shell_builtin_t *result;

    /* search a list of commands */
    for (result = cmd_list; result && result->name; result++)
    {
        if (0 == strcmp(name, result->name))
            break;
    }
    return (result && result->name) ? result->callback : NULL;
}

/*----------------------------------------------------------- */
void clish_shell_cleanup_script(void *script)
{
    /* simply release the memory */
    lub_string_free(script);
}

/*-------------------------------------------------------- */
static int clish_shell_lock(const char *lock_path)
{
    int i;
    int res;
    int lock_fd = -1;
    struct flock lock;

    if (!lock_path)
        return -1;
    lock_fd = open(lock_path, O_WRONLY | O_CREAT, 00644);
    if (-1 == lock_fd)
    {
        fprintf(stderr, "Can't open lockfile %s.\n", lock_path);
        return -1;
    }
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    for (i = 0; i < CLISH_LOCK_WAIT; i++)
    {
        res = fcntl(lock_fd, F_SETLK, &lock);
        if (res != -1)
            break;
        if (EINTR == errno)
            continue;
        if ((EAGAIN == errno) || (EACCES == errno))
        {
            if (0 == i)
                fprintf(stderr,
                        "Try to get lock. Please wait...\n");
            sleep(1);
            continue;
        }
        break;
    }
    if (res == -1)
    {
        fprintf(stderr, "Can't get lock.\n");
        close(lock_fd);
        return -1;
    }
    return lock_fd;
}

/*-------------------------------------------------------- */
static void clish_shell_unlock(int lock_fd)
{
    struct flock lock;

    if (lock_fd == -1)
        return;
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    fcntl(lock_fd, F_SETLK, &lock);
    close(lock_fd);
}

/*----------------------------------------------------------- */
int clish_shell_execute(clish_context_t *context, char **out)
{
    clish_shell_t *this = context->shell;
    const clish_command_t *cmd = context->cmd;
    clish_action_t *action;
    int result = 0;
    char *lock_path = clish_shell__get_lockfile(this);
    int lock_fd = -1;
    sigset_t old_sigs;
    struct sigaction old_sigint, old_sigquit, old_sighup;
    clish_view_t *cur_view = clish_shell__get_view(this);
    unsigned int saved_wdog_timeout = this->wdog_timeout;

    assert(cmd);
    action = clish_command__get_action(cmd);

    /* Pre-change view if the command is from another depth/view */
    {
        clish_view_restore_t restore = clish_command__get_restore(cmd);
        if ((CLISH_RESTORE_VIEW == restore) &&
                (clish_command__get_pview(cmd) != cur_view))
        {
            clish_view_t *view = clish_command__get_pview(cmd);
            clish_shell__set_pwd(this, NULL, view, NULL, context);
        }
        else if ((CLISH_RESTORE_DEPTH == restore) &&
                 (clish_command__get_depth(cmd) < this->depth))
        {
            this->depth = clish_command__get_depth(cmd);
        }
    }

    /* Lock the lockfile */
    if (lock_path && clish_command__get_lock(cmd))
    {
        lock_fd = clish_shell_lock(lock_path);
        if (-1 == lock_fd)
        {
            result = -1;
            goto error; /* Can't set lock */
        }
    }

    /* Ignore and block SIGINT, SIGQUIT, SIGHUP */
    if (!clish_command__get_interrupt(cmd))
    {
        struct sigaction sa;
        sigset_t sigs;
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);
        sa.sa_handler = SIG_IGN;
        sigaction(SIGINT, &sa, &old_sigint);
        sigaction(SIGQUIT, &sa, &old_sigquit);
        sigaction(SIGHUP, &sa, &old_sighup);
        sigemptyset(&sigs);
        sigaddset(&sigs, SIGINT);
        sigaddset(&sigs, SIGQUIT);
        sigaddset(&sigs, SIGHUP);
        sigprocmask(SIG_BLOCK, &sigs, &old_sigs);
    }

    /* Execute ACTION */
    result = clish_shell_exec_action(action, context, out);

    /* Restore SIGINT, SIGQUIT, SIGHUP */
    if (!clish_command__get_interrupt(cmd))
    {
        sigprocmask(SIG_SETMASK, &old_sigs, NULL);
        /* Is the signals delivery guaranteed here (before
           sigaction restore) for previously blocked and
           pending signals? The simple test is working well.
           I don't want to use sigtimedwait() function bacause
           it needs a realtime extensions. The sigpending() with
           the sleep() is not nice too. Report bug if clish will
           get the SIGINT after non-interruptable action.
        */
        sigaction(SIGINT, &old_sigint, NULL);
        sigaction(SIGQUIT, &old_sigquit, NULL);
        sigaction(SIGHUP, &old_sighup, NULL);
    }

    /* Call config callback */
    if (!result && this->client_hooks->config_fn)
        this->client_hooks->config_fn(context);

    /* Call logging callback */
    if (clish_shell__get_log(this) && this->client_hooks->log_fn)
    {
        char *full_line = clish_shell__get_full_line(context);
        this->client_hooks->log_fn(context, full_line, result);
        lub_string_free(full_line);
    }

    /* Unlock the lockfile */
    if (lock_fd != -1)
        clish_shell_unlock(lock_fd);

    /* Move into the new view */
    if (!result)
    {

        clish_view_t *view = clish_command__get_view(cmd);



        /* Save the PWD */
        if (view)
        {
            char *line = clish_shell__get_line(context);


            view =    clish_command__set_name(this , cmd ,line);

            clish_shell__set_pwd(this, line, view,
                                 clish_command__get_viewid(cmd), context);
            lub_string_free(line);
        }
    }

    /* Set appropriate timeout. Workaround: Don't turn on  watchdog
    on the "set watchdog <timeout>" command itself. */
    if (this->wdog_timeout && saved_wdog_timeout)
    {
        tinyrl__set_timeout(this->tinyrl, this->wdog_timeout);
        this->wdog_active = BOOL_TRUE;
        fprintf(stderr, "Warning: The watchdog is active. Timeout is %u "
                "seconds.\nWarning: Press any key to stop watchdog.\n",
                this->wdog_timeout);
    }
    else
        tinyrl__set_timeout(this->tinyrl, this->idle_timeout);

error:
    return result;
}

/*----------------------------------------------------------- */
int clish_shell_exec_action(clish_action_t *action,
                            clish_context_t *context, char **out)
{
    clish_shell_t *this = context->shell;
    int result = 0;
    const char *builtin;
    char *script;

    builtin = clish_action__get_builtin(action);
    script = clish_shell_expand(clish_action__get_script(action), SHELL_VAR_ACTION, context);
    if (builtin)
    {
        clish_shell_builtin_fn_t *callback;
        lub_argv_t *argv = script ? lub_argv_new(script, 0) : NULL;
        result = -1;
        /* search for an internal command */
        callback = find_builtin_callback(clish_cmd_list, builtin);
        if (!callback)
        {
            /* search for a client command */
            callback = find_builtin_callback(
                           this->client_hooks->cmd_list, builtin);
        }
        /* invoke the builtin callback */
        if (callback)
            result = callback(context, argv);
        if (argv)
            lub_argv_delete(argv);
    }
    else if (script)
    {
        /* now get the client to interpret the resulting script */
        result = this->client_hooks->script_fn(context, action, script, out);
    }
    lub_string_free(script);

    return result;
}

/*----------------------------------------------------------- */
/*
 * Find out the previous view in the stack and go to it
 */
static int clish_nested_up(clish_context_t *context, const lub_argv_t *argv)
{
    clish_shell_t *this = context->shell;

    if (!this)
        return -1;

    argv = argv; /* not used */

    /* If depth=0 than exit */
    if (0 == this->depth)
    {
        this->state = SHELL_STATE_CLOSING;
        return 0;
    }
    this->depth--;

    return 0;
}

/*----------------------------------------------------------- */
/*
 * Builtin: NOP function
 */
static int clish_nop(clish_context_t *context, const lub_argv_t *argv)
{
    return 0;
}

/*----------------------------------------------------------- */
/*
 * Builtin: Set watchdog timeout. The "0" to turn watchdog off.
 */
static int clish_wdog(clish_context_t *context, const lub_argv_t *argv)
{
    const char *arg = lub_argv__get_arg(argv, 0);
    clish_shell_t *this = context->shell;

    /* Turn off watchdog if no args */
    if (!arg || ('\0' == *arg))
    {
        this->wdog_timeout = 0;
        return 0;
    }

    this->wdog_timeout = (unsigned int)atoi(arg);

    return 0;
}

/*----------------------------------------------------------- */
const char *clish_shell__get_fifo(clish_shell_t * this)
{
    char *name;
    int res;

    if (this->fifo_name)
    {
        if (0 == access(this->fifo_name, R_OK | W_OK))
            return this->fifo_name;
        unlink(this->fifo_name);
        lub_string_free(this->fifo_name);
        this->fifo_name = NULL;
    }

    do
    {
        char template[] = "/tmp/klish.fifo.XXXXXX";
        name = mkstemp(template);
        if (name[0] == '\0')
            return NULL;
        res = mkfifo(name, 0600);
        if (res == 0)
            this->fifo_name = lub_string_dup(name);
    }
    while ((res < 0) && (EEXIST == errno));

    return this->fifo_name;
}

/*--------------------------------------------------------- */
void *clish_shell__get_client_cookie(const clish_shell_t * this)
{
    return this->client_cookie;
}

/*-------------------------------------------------------- */
void clish_shell__set_log(clish_shell_t *this, bool_t log)
{
    assert(this);
    this->log = log;
}

/*-------------------------------------------------------- */
bool_t clish_shell__get_log(const clish_shell_t *this)
{
    assert(this);
    return this->log;
}


/*----------------------------------------------------------- */
