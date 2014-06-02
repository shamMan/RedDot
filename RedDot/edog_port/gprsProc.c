/**
@file gprsProc.c
@author liusk
@version 1.0
@date 2013/10/15
@brief GPRS数据处理相关函数
@copyright (C)版权所有，深圳市兆图电子有限公司，保留所有权利
*/
#include    "gprsProc.h"
///< GPRS数据接收缓存区
UINT8 gGprsDataBuf[GPRS_BUF_SUM][GPRS_BUF_MAX];
///< 服务器发来的数据缓存
UINT8 gGprsNeedBuf[GPRS_BUF_MAX];
///< GPRS数据发送缓存区
UINT8 gGprsDataSendBuf[GPRS_BUF_MAX];
///< GPRS数据缓冲区
 __align(4)  UINT8 gGprsDataTmp[GPRS_BUF_MAX];
///<处理接收的GPRS数据
STU_GPRS_DEV stu_GprsDev={gGprsDataBuf,0,AT_READY,0,0,{0}};
///< GPRS初始化后的发送
STU_GPRS_DATA stu_GprsData={GPRS_IDLE,0,gGprsDataSendBuf};
///< GPRS链接服务器后的数据接收
STU_GPRS_DATA stu_ServerData={GPRS_IDLE,0,gGprsNeedBuf};
///< GPRS数据缓冲区结构体
STU_GPRS_DATA stu_GprsDataTmp={GPRS_IDLE,0,gGprsDataTmp};
///< GPS轨迹数据暂存
UINT8 GpsTraceBuf[256];

///<投诉点上传结构体
__align(4) STU_POI_UPLOAD_REQ stu_PoiUploadReq;
///< 请求检验是否需要增量升级
__align(4) STU_UPDATE_CHECK_REQ stu_UpdateCheckReq;
///< 获取增量升级区域的信息
__align(4) CAMERA_DATAINFO stu_CameraAreaInf;
///< 包头结构体
__align(4) STU_PACKAGE_HEADER stu_PkgHead;
///< 升级查询应答结构体变量
__align(4) STU_UPDATE_CHECK_ACK stu_UpdateChkAck;
///< POI升级查询应答
__align(4) STU_POIUPDATE_CHK_ACK stu_PoiUpdateChkAck;
///< 升级请求处理
__align(4) STU_UPDATE_REQ stu_UpdateReq;
///< 获取手机号用
__align(4) STU_PHONENUM_PARAM stu_PhoneNumParam;

///< 一个结构体,处理POI私有数据的升级查询
__align(4) STU_POI_UPDATE_REQ stu_PoiUpdateReq;
///< 服务器控制用
STU_SERVERCTRL stu_ServerCtrl={REQ_IDLE_REQ,REQ_IDLE_REQ,0,0,0,0,0,0,LINK_WAIT};
///< 云服务处理
STU_CLOUD_SERVER stu_CldServer={PHONE_NUM_GET,PHONE_NUM_GET,0,1,{DSA_UPDATA_LINK,WEACAST_LINK,ROAD_STATE_LINK,DSA_UPDATA_LINK,TRACE_UPLOAD_LINK},0x00};
///< 处理各个相关命令
void(* const GprsCmdProc[][16])(UINT8 cmd) =
{
    {
     FunDefault,  FunDefault,       FunDefault, FunDefault, FunDefault,     FunDefault, FunDefault,  FunDefault,
     FunDefault,  FunDefault,       FunDefault, FunDefault, FunDefault,     FunDefault, FunDefault,  FunDefault, 
     },

    {
     UpdateChk,   FunDefault,       FunDefault, FunDefault, DownLoadDataReq, FunDefault, UploadChk,   UploadReq,
     FunDefault,  FunDefault,       FunDefault, FunDefault, FunDefault,      FunDefault, FunDefault,  FunDefault, 
     },

    {
     UpdateChkAck,DownLoadDataReqAck,FunDefault, FunDefault, FunDefault,      FunDefault, UploadChkAck,UploadReqAck,
     FunDefault,  FunDefault,       FunDefault, FunDefault, FunDefault,      FunDefault, FunDefault,  FunDefault, 
     },
     {
     FunDefault,  FunDefault,       PhoneNumReq, PhoneNumReqAck, FunDefault,     FunDefault, FunDefault,  FunDefault,
     FunDefault,  FunDefault,       FunDefault, FunDefault, 	FunDefault,     FunDefault, FunDefault,  FunDefault, 
     },
     {
     PoiUploadReq,  PoiUploadAck,   PoiUpdateChk, PoiUpdateAck, FunDefault,     FunDefault, FunDefault,  FunDefault,
     FunDefault,  FunDefault,       FunDefault, FunDefault, FunDefault,     FunDefault, FunDefault,  FunDefault, 
     },
     {
     WeatherInfReq,  RoadInfReq,       FunDefault, FunDefault, FunDefault,     FunDefault, FunDefault,  RoadInfReqAck,
     WeatherInfReqAck,  GpsTraceUpload,    FunDefault, FunDefault, FunDefault,     FunDefault, FunDefault,  FunDefault, 
     },
};
///< CRC16计算表
static const UINT16 CRC16Tab[256]= {
    0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
    0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
    0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
    0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
    0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
    0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
    0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
    0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
    0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
    0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
    0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
    0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
    0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
    0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
    0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
    0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
    0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
    0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
    0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
    0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
    0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
    0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
    0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
    0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
    0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
    0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
    0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
    0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
    0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
    0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
    0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
    0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};
///< 纬度对应的每纬度公里值
const UINT8 KmPerLongitude[] = 
{
	111,    111,    111,    111,    111,    110,    110,    110,    110,    109,
	109,    109,    108,    108,    107,    107,    106,    106,    105,    104,
	104,    103,    102,    101,    101,    100,    99,     98,     97,     96,
	95,     94,     93,     92,     91,     90,     89,     88,     87,     86,
	85,     84,     82,     81,     80,     78,     77,     76,     74,     73,
	71,     70,     68,     67,     65,     64,     62,     60,     59,     57,
	55,     54,     52,     50,     49,     47,     45,     43,     42,     40,
	38,     36,     34,     32,     31,     29,     27,     25,     23,     21,
	19,     17,     15,     14,     12,     10,     8,      6,      4,      2
};
///< 路况参数信息
STU_ROAD_PARAM stu_RoadParam;
/**
*@fn UINT16 crc16Get(const UINT8  *buf, UINT16 len)
*@brief 获取CRC16检验字
*@date 2013/07/01
*/
UINT16 crc16Get(const UINT8  *buf, UINT16 len)
{
    UINT16 counter;
    UINT16 crc = 0;

    for( counter = 0; counter < len; counter++)
        crc = (crc<<8) ^ CRC16Tab[((crc>>8) ^ *(UINT8*)buf++)&0x00FF];
    return crc;
}
/**
*@fn void ReadyRequestAndDetect(UINT8 detectFlag)
*@brief 准备发送服务器端请求以及检测
*@param detectFlag 0 为不启动服务器应答超时检测 其他为启动 
*@date 2013/8/23
*@author liusk
*/
void ReadyRequestAndDetect(UINT8 detectFlag)
{
    RESET_TIMER(GPRS_SERVER_TIMER);
    START_TIMER(GPRS_SERVER_TIMER);
    if(detectFlag)
    {
        RESET_TIMER(SERVER_DETE_TIMER);
        START_TIMER(SERVER_DETE_TIMER);  
    }
}
/**
*@fn void StopSerTimeoutDetec(void)
*@brief 停止服务器超时检测
*@date 2013/8/23
*@author liusk
*/
void StopSerTimeoutDetec(void)
{
    RESET_TIMER(SERVER_DETE_TIMER);
}
/**
*@fn void  UpdateJudgeBaseGps(void)
*@brief 基于GPS判断是否升级
*@date 2013/8/22
*@author liusk
*/
void UpdateJudgeBaseGps(void)
{   
	UINT32 curTime;				    ///<当前时间，用于比较
	UINT16 latitudeDist;			///< 纬度映射距离
	UINT16 longitudeDist;			///< 经度映射距离
	
	if(IS_SIM_USE_NO())
		return;
	if(stu_CldServer.state!= DSA_UPDATE)
		return;
	///<已在处理中
	if(stu_CldServer.flag&stu_CldServer.state)
		return;
	if(IS_TIMER_RUN(SOC_CLS_AUTO_TIMER))
	{
		//AUDIO_REPORT(NET_BUSY_V);
		return;
	}
	if(IS_TIMER_RUN(LINK_ERROR_TIMER))
	{
		//AUDIO_REPORT(NET_BUSY_V);
		return;
	}
    ///< 已经定位并且GPRS已经链接上TCP后
    if((theNMEA.m_btRMCDataValid=='A')
        &&(stu_GprsDev.status>=AT_LINKSETUP))
     {
        if((stu_SysMode.referLatitude== DEFAULT_LAST_LATITUDE)
            ||(stu_SysMode.referLongitude== DEFAULT_LAST_LONGITUDE))
        {
            stu_SysMode.referLatitude=  theNMEA.m_dRMCLatitude*100;
            stu_SysMode.referLongitude= theNMEA.m_dRMCLongitude*100;
        }
        longitudeDist=(UINT16)(abs(stu_SysMode.referLatitude-(UINT16)(theNMEA.m_dRMCLatitude*100))*1.11);
		latitudeDist=(UINT16)((KmPerLongitude[stu_SysMode.referLatitude/100])*(abs(stu_SysMode.referLongitude-(UINT16)(theNMEA.m_dRMCLongitude*100))/100));
		curTime=theNMEA.m_wRMCYear*10000+theNMEA.m_btRMCMonth*100+theNMEA.m_btRMCDay;
        ///< 判断是否需要升级
		if(longitudeDist>DSA_AREA_DIST || latitudeDist>DSA_AREA_DIST || curTime!=stu_SysMode.lastUpdateTime)
		{
			if(stu_CldServer.lastState==TRACE_UPLOAD)
			{
				stu_GprsDev.status= AT_LINKCLOSE;
				gprs_request_sock(SOC_DISC);
				
			}
	        stu_ServerCtrl.state= REQ_UPDATE_CHECK_REQ;
			stu_CldServer.flag|=stu_CldServer.state;
			stu_CldServer.lastState=stu_CldServer.state;
            stu_SysMode.lastUpdateTime= curTime;
            stu_SysMode.referLatitude=  theNMEA.m_dRMCLatitude*100;
            stu_SysMode.referLongitude= theNMEA.m_dRMCLongitude*100;
            Debug_printf("update start\n");
			ReadyRequestAndDetect(SERVER_TIMEOUT_DETEC);
			if(stu_CldServer.procErrCnt==0)
				AUDIO_REPORT(DSA_GET_V);
			stu_ServerCtrl.errCnt=0;
		}
	           
      } 
}
/**
*@fn void  WeatherForeCastProc(void)
*@brief 基于GPS判断进行天气预报处理
*@date 2013/8/31
*@author liusk
*/
void WeatherForeCastProc(void)
{
	if(IS_SIM_USE_NO())
		return;
	if(stu_CldServer.state!= WEATHER_FORECAST)
		return;
	///<已在处理中
	if(stu_CldServer.flag&stu_CldServer.state)
		return;
	if(IS_TIMER_RUN(SOC_CLS_AUTO_TIMER))
	{
		AUDIO_REPORT(NET_BUSY_V);
		return;
	}
	if(IS_TIMER_RUN(LINK_ERROR_TIMER))
	{
		AUDIO_REPORT(NET_BUSY_V);
		return;
	}
	if(gSysRestartFlag==1)
	{
		stu_CldServer.state= DSA_UPDATE;
		stu_CldServer.lastState= DSA_UPDATE;
		return;
	}
    ///< 已经定位并且GPRS已经链接上TCP后
    if((theNMEA.m_btRMCDataValid=='A')
        &&(stu_GprsDev.status>=AT_LINKSETUP))
    {
		if(stu_CldServer.lastState==TRACE_UPLOAD)
		{
			stu_GprsDev.status= AT_LINKCLOSE;
			gprs_request_sock(SOC_DISC);
		}
		stu_CldServer.flag|=stu_CldServer.state;
		stu_CldServer.lastState=stu_CldServer.state;
		stu_ServerCtrl.state= REQ_WEATHER_REQ;
        ReadyRequestAndDetect(SERVER_TIMEOUT_DETEC);
        Debug_printf("weather forecast start\n");	
		AUDIO_REPORT(WEATHER_GET_V);
		stu_ServerCtrl.errCnt=0;
    }

}
/**
*@fn void PhoneNumProc(void)
*@brief 从服务器获得手机号码
*@date 2013/8/31
*@author liusk
*/
void PhoneNumProc(void)
{
	if(IS_SIM_USE_NO())
		return;
	if(stu_CldServer.state!= PHONE_NUM_GET)
		return;
	///<已在处理中
	if(stu_CldServer.flag&stu_CldServer.state)
		return;
	if(!SYS_START_OK())
		return;
	if(IS_TIMER_RUN(SIM_DETECT_TIMER))
		return;
	if(gSysRestartFlag==1)
	{
		stu_CldServer.state= DSA_UPDATE;
		stu_CldServer.lastState= DSA_UPDATE;
		return;
	}
	if(IS_TIMER_RUN(SOC_CLS_AUTO_TIMER))
	{
		AUDIO_REPORT(NET_BUSY_V);
		return;
	}
	if(IS_TIMER_RUN(LINK_ERROR_TIMER))
	{
		//AUDIO_REPORT(NET_BUSY_V);
		return;
	}
	///< 手机号码已经存在
	if(PhoneNumChk(stu_SysMode.phoneNum))
	{
		stu_GprsDataTmp.pBuf[0]='T';
		stu_GprsDataTmp.len=myMemcpy(&stu_GprsDataTmp.pBuf[1],&stu_SysMode.phoneNum[2],'\0');
		stu_GprsDataTmp.len++;
		PhoneNumReqAck(NULL);
		Debug_printf("phone num exit\r\n");
	}
    ///< 已经定位并且GPRS已经链接上TCP后
    else if((stu_GprsDev.status>=AT_LINKSETUP))
    {
		if(stu_CldServer.lastState==TRACE_UPLOAD)
		{
			stu_GprsDev.status= AT_LINKCLOSE;
			gprs_request_sock(SOC_DISC);
		}
		stu_CldServer.flag|=stu_CldServer.state;
		stu_CldServer.lastState=stu_CldServer.state;
		stu_ServerCtrl.state= REQ_PHONENUM_REQ;
        ReadyRequestAndDetect(SERVER_TIMEOUT_DETEC);
        Debug_printf("Get phone num\n");
		AUDIO_REPORT(PHONE_GET_V);
		stu_ServerCtrl.errCnt=0;
    }

}
/**
*@fn void TraceUploadProc(void)
*@brief 轨迹数据上传处理
*@date 2013/10/31
*@author liusk
*/
void TraceUploadProc(void)
{
	if(IS_TIMER_OVER(GPRS_TRACE_TIMER))
    {
        RESET_TIMER(GPRS_TRACE_TIMER);
		///<GPRS忙碌,继续等待
		if(stu_CldServer.flag&stu_CldServer.state)
		{
			 START_TIMER(GPRS_TRACE_TIMER);
			 
		}
		else if(IS_TIMER_RUN(SOC_CLS_AUTO_TIMER))
		{
			//AUDIO_REPORT(NET_BUSY_V);
			START_TIMER(GPRS_TRACE_TIMER);
			return;
		}
		else if(IS_TIMER_RUN(LINK_ERROR_TIMER))
		{
			//AUDIO_REPORT(NET_BUSY_V);
			return;
		}
		else
		{
			stu_CldServer.state= TRACE_UPLOAD;
			stu_CldServer.flag|=stu_CldServer.state;
			stu_CldServer.lastState=stu_CldServer.state;
			stu_ServerCtrl.state= REQ_TRACE_UPLOAD;
	        ReadyRequestAndDetect(SERVER_TIMEOUT_DETEC);
	        Debug_printf("Trace Upload start\n");	
			stu_ServerCtrl.errCnt=0;
		}
	}
}
/**
*@fn void DecodePkgHead(STU_PACKAGE_HEADER*pkgHead)
*@brief 解析包头
*@param pkgHead 包头结构体指针
*details  
*@date 2013/8/21
*@author liusk
*/
void DecodePkgHead(STU_PACKAGE_HEADER*pkgHead)
{
    pkgHead->nReq^=PKG_HEAD_FLAG;
    pkgHead->nPktSize^=(((pkgHead->nFlag+0x11)<<8)|pkgHead->nFlag);
    
}
/**
*@fn void SetPkgHead(UINT8 cmd,UINT8*p8Data,UINT16 paramLen)
*@brief 设置包头相关信息
*@param cmd 命令字 p8Data 参数信息指针，paramLen 参数大小
*details  
*@date 2013/8/21
*@author liusk
*/
void SetPkgHead(UINT8 cmd,UINT8*p8Data,UINT16 paramLen)
{		
	UINT16* p16Data;
    UINT16 enCodeIndex; ///< 加密因子
    UINT16 i;


	p16Data=(UINT16*)p8Data;
		
	stu_PkgHead.nFlag=PKG_HEAD_FLAG;						
	stu_PkgHead.nReq=cmd^stu_PkgHead.nFlag;									
	stu_PkgHead.nPktSize=paramLen^(((stu_PkgHead.nFlag+0x11)<<8)|stu_PkgHead.nFlag);	
		
	enCodeIndex = (((stu_PkgHead.nFlag+0x55)<<8)|stu_PkgHead.nReq);
    ///< 参数加密
    i= (paramLen>>1);
	for(; i>0; i--)
	{
	   *p16Data^=enCodeIndex;;
	   p16Data++;
	}
		
	stu_PkgHead.nCrc16=crc16Get(p8Data,paramLen);		  
}
/**
*@fn void FillUpdateChkPkg(void)
*@brief 填充升级查询参数
*details  
*@date 2013/8/16
*@author liusk
*/
void FillUpdateChkPkg(void)
{
    memset((UINT8*)&stu_UpdateCheckReq,0X00,sizeof(stu_UpdateCheckReq));
	Camera_GetLastInfo(&stu_CameraAreaInf);
	memcpy(stu_UpdateCheckReq.SerNo,stu_SnInf.snData,10); 	        //序列号，保存在stu_UpdateCheckReq.SerNo前8字节
	//stu_UpdateCheckReq.HWType[0]=0x00;						    //硬件类型	
	memcpy(stu_UpdateCheckReq.HWType,"SF1301",6);
	stu_UpdateCheckReq.FWVer=APPVER;							//固件版本
	stu_UpdateCheckReq.HWVer=0;								    //硬件版本
	stu_UpdateCheckReq.DataTime=stu_CameraAreaInf.dwDataTime;	//最新数据日期 	
	stu_UpdateCheckReq.nStartLatitude=0;						//当前最新数据的起始点纬度，单位为度；
	stu_UpdateCheckReq.nStartLongitude=0;					    //当前最新数据的起始点经度，单位为度；
	stu_UpdateCheckReq.nEndLatitude=0;						    //当前最新数据的结束点纬度，单位为度；
	stu_UpdateCheckReq.nEndLongitude=0;						    //当前最新数据的结束点经度，单位为度；
	stu_UpdateCheckReq.nDataOption=0;						    //数据选项，待定义；
	stu_UpdateCheckReq.nPtLatitude=(UINT16)(theNMEA.m_dRMCLatitude*100);				//当前所在位置的纬度，单位为度；
	stu_UpdateCheckReq.nPtLongitude=(UINT16)(theNMEA.m_dRMCLongitude*100);			    //当前所在位置的经度，单位为度；
	stu_UpdateCheckReq.nBufferSize=0;						    //内部数据包缓冲区大小	
};
/**
*@fn void FunDefault(UINT8 cmd)
*@brief 未定义命令应答
*details  
*@date 2013/8/21
*@author liusk
*/
void FunDefault(UINT8 cmd)
{
	return;
}
/**
*@fn void UpdateChk((UINT8 cmd))
*@brief 升级查询
*@param cmd 命令字
*details  
*@date 2013/8/21
*@author liusk
*/
void UpdateChk(UINT8 cmd)
{
    FillUpdateChkPkg(); 
    //VirtualPrintBuf((UINT8*)&stu_UpdateCheckReq,sizeof(stu_UpdateCheckReq));
    SetPkgHead(cmd,(UINT8*)&stu_UpdateCheckReq,sizeof(stu_UpdateCheckReq));
    stu_GprsDataTmp.len=0;
    memcpy(stu_GprsDataTmp.pBuf,(UINT8*)&stu_PkgHead,sizeof(stu_PkgHead));
   // VirtualPrintBuf((UINT8*)&stu_UpdateCheckReq,sizeof(stu_UpdateCheckReq));
    stu_GprsDataTmp.len+=sizeof(stu_PkgHead);
    memcpy(stu_GprsDataTmp.pBuf+stu_GprsDataTmp.len,(UINT8*)&stu_UpdateCheckReq,sizeof(stu_UpdateCheckReq)); 
    stu_GprsDataTmp.len+= sizeof(stu_UpdateCheckReq);
    //Debug_printf("stu_GprsDataTmp.pBuf addr%x,",(UINT16)stu_GprsDataTmp.pBuf);
    //Debug_printf("stu_UpdateCheckReq addr%x,",(UINT16)&stu_UpdateCheckReq);

}
/**
*@fn void DownLoadDataReq(UINT8 cmd)
*@brief 升级数据请求
*details  
*@date 2013/8/22
*@author liusk
*/
void DownLoadDataReq(UINT8 cmd)
{
    static __align(4) STU_UPDATE_REQ stu_UpdateReqTmp;
    
    if(stu_ServerCtrl.flag==1)
     {
        stu_UpdateReq.nPktIndex= stu_ServerCtrl.pkgNo;
        stu_UpdateReq.nPackSize= stu_ServerCtrl.pkgSize; 
        stu_UpdateReq.nLastPacket= 1;
        stu_ServerCtrl.flag= 0;
     }
    ///< 最后一包
    if(stu_UpdateReq.nPktIndex+1>=stu_ServerCtrl.pkgNoSum)
        stu_UpdateReq.nLastPacket= 0;
    stu_UpdateReq.nData[0]=0x00;
    stu_UpdateReq.nData[1]=0x00;
    stu_UpdateReq.nData[2]=0x00;
    stu_UpdateReq.nData[3]=0x00;
    stu_UpdateReq.nData[4]=0x00;
    stu_UpdateReqTmp= stu_UpdateReq;
   // Neoway_Usb_Send((UINT8*)&stu_UpdateReq,sizeof(stu_UpdateReq));
    SetPkgHead(cmd,(UINT8*)&stu_UpdateReqTmp,sizeof(stu_UpdateReqTmp));
    stu_GprsDataTmp.len=0;
    memcpy(stu_GprsDataTmp.pBuf,(UINT8*)&stu_PkgHead,sizeof(stu_PkgHead));
    stu_GprsDataTmp.len+=sizeof(stu_PkgHead);
    memcpy(stu_GprsDataTmp.pBuf+stu_GprsDataTmp.len,(UINT8*)&stu_UpdateReqTmp,sizeof(stu_UpdateReqTmp)); 
    stu_GprsDataTmp.len+= sizeof(stu_UpdateReqTmp);  
}
/**
*@fn void UploadChk(UINT8 cmd)
*@brief 上传查询
*details  
*@date 2013/8/21
*@author liusk
*/
void UploadChk(UINT8 cmd)
{
       
}
/**
*@fn void UploadReq(UINT8 cmd)
*@brief 上传数据
*details  
*@date 2013/8/21
*@author liusk
*/
void UploadReq(UINT8 cmd)
{
    
}
/**
*@fn void UpdateChkAck(UINT8 cmd)
*@brief 升级查询应答
*details  
*@date 2013/8/21
*@author liusk
*/
void UpdateChkAck(UINT8 cmd)
{ 
    //Debug_printf("UpdateChkAck take\n");
    memcpy((UINT8*)&stu_UpdateChkAck,stu_GprsDataTmp.pBuf,sizeof(stu_UpdateChkAck));
    ///< 需要增量升级
    if((stu_UpdateChkAck.nResult== 0x01)
        &&(stu_UpdateChkAck.nDataMode== 0x00)
        &&(stu_UpdateChkAck.dwBytes>0))
    {
		Debug_printf("updata need\n");
        stu_ServerCtrl.state= REQ_UPDATE_REQ;
        stu_ServerCtrl.flag= 1;
        stu_ServerCtrl.pkgSize= stu_UpdateChkAck.nPackSize;
        stu_ServerCtrl.pkgNo= 0;
        stu_ServerCtrl.dataSizeSum= stu_UpdateChkAck.dwBytes;
        stu_ServerCtrl.pkgNoSum=(stu_ServerCtrl.dataSizeSum+stu_ServerCtrl.pkgSize-1)/stu_ServerCtrl.pkgSize; 
        Camera_GetLastInfo(&stu_CameraAreaInf);
        stu_ServerCtrl.dataAddr= stu_CameraAreaInf.dwNextOffset;
		Debug_printf("dsa addr=%x,dsa size=%x",stu_ServerCtrl.dataAddr,stu_ServerCtrl.dataSizeSum);
        UpdateDSAPrepare(stu_ServerCtrl.dataAddr,stu_ServerCtrl.dataSizeSum,FLASH_BLK_SIZE);  
        ReadyRequestAndDetect(SERVER_TIMEOUT_DETEC);
		Debug_printf("stu_UpdateChkAck.dwBytes=%d\n",stu_UpdateChkAck.dwBytes);
    }
	else
	{
		Debug_printf("updata no need\n");
		AUDIO_REPORT(DSA_UPDATA_N_V);
		stu_GprsDev.status= AT_LINKCLOSE;
		stu_CldServer.flag&=(~stu_CldServer.state);
		stu_CldServer.state=DSA_UPDATE;	
		gprs_request_sock(SOC_DISC);
		SocketClose();
		stu_CldServer.procErrCnt=0;
	}
}
/**
*@fn void DownLoadDataReqAck(UINT8 cmd)
*@brief 升级数据应答
*details  
*@date 2013/8/21
*@author liusk
*/
void DownLoadDataReqAck(UINT8 cmd)
{
    STU_UPDATE_REQ stu_UpdateReqAck;
    UINT8*pBuf;

    pBuf=stu_GprsDataTmp.pBuf;
    
    memcpy((UINT8*)&stu_UpdateReqAck,pBuf,sizeof(stu_UpdateReqAck));
    pBuf+=sizeof(stu_UpdateReqAck);
    Debug_printf("stu_UpdateReq.nPktIndex=%x",stu_UpdateReq.nPktIndex);
    ///< 包序号正确
    if(stu_UpdateReqAck.nPktIndex== stu_UpdateReq.nPktIndex)
    {
		VirtualPrintBuf(pBuf,stu_UpdateReqAck.nPackSize);
        FlashWriteWithLock(stu_ServerCtrl.dataAddr,pBuf,stu_UpdateReqAck.nPackSize);
        stu_ServerCtrl.dataAddr+=stu_UpdateReqAck.nPackSize;
        if(stu_UpdateReqAck.nLastPacket== 0)///< 最后一包情况
        {
            sl_Cameera_Init();
            AUDIO_REPORT(AU_GPRS_UPDATEOK);
			stu_GprsDev.status= AT_LINKCLOSE;
			stu_CldServer.flag&=(~stu_CldServer.state);
			stu_CldServer.state=DSA_UPDATE;	
			RESET_TIMER(SERVER_DETE_TIMER);
			gprs_request_sock(SOC_DISC);
			SocketClose();
            Debug_printf("update size=%d,No=%dfinish\n",stu_ServerCtrl.dataSizeSum,stu_ServerCtrl.pkgNoSum);
			stu_CldServer.procErrCnt=0;
			///< 保存当前升级成功相关参数
			stu_SysMode.saveNeed = YES;
			RESET_TIMER(PARA_SAVE_TIMER);
			START_TIMER(PARA_SAVE_TIMER);
         }
        else
         {  
            stu_ServerCtrl.state= REQ_UPDATE_REQ;
            stu_UpdateReq.nPktIndex++; 
            ReadyRequestAndDetect(SERVER_TIMEOUT_DETEC);
          }
     }
    else
    {
        stu_ServerCtrl.state= REQ_UPDATE_REQ;
        ReadyRequestAndDetect(SERVER_TIMEOUT_DETEC);        
    }
	stu_ServerCtrl.errCnt=0;
    //Debug_printf("get dsa data len=%x\n",stu_GprsDataTmp.len);
    //Neoway_Usb_Send(stu_GprsDataTmp.pBuf, 1000);
    //Neoway_UartSend(NEOWAY_UART_PORT_2,stu_GprsDataTmp.pBuf,1000);
}
/**
*@fn void UploadChkAck(UINT8 cmd)
*@brief 上传数据查询应答
*details  
*@date 2013/8/21
*@author liusk
*/
void UploadChkAck(UINT8 cmd)
{
    
}
/**
*@fn void UploadReqAck(UINT8 cmd)
*@brief 上传数据应答
*details  
*@date 2013/8/21
*@author liusk
*/
void UploadReqAck(UINT8 cmd)
{
    
}

/**
*@fn void GprsServerDataProc(UINT8* p8Data,UINT16 len)
*@brief GPRS服务器数据处理
*@param p8Data 数据指针 len 数据长度
*@date 2013/8/21
*@author liusk
*/
void GprsServerDataProc(UINT8* p8Data,UINT16 len)
{
    UINT16* p16Data;
    UINT16 enCodeIndex; ///< 加密因子
    UINT16 i; 
    UINT8 j;

	if(stu_CldServer.encrypFlag)
	{
	    //STU_PACKAGE_HEADER stu_PkgHead;
		memcpy((UINT8*)&stu_PkgHead,p8Data,sizeof(STU_PACKAGE_HEADER));
	    DecodePkgHead(&stu_PkgHead);
	    if(stu_PkgHead.nPktSize< GPRS_BUF_MAX)
	        memcpy(&stu_GprsDataTmp.pBuf[0],&p8Data[sizeof(STU_PACKAGE_HEADER)],stu_PkgHead.nPktSize);
	    stu_GprsDataTmp.len= stu_PkgHead.nPktSize;
	    //Debug_printf("stu_PkgHead.nPktSize=%x  ",stu_PkgHead.nPktSize);
	    // Debug_printf("stu_PkgHead.nReq=%x  ",stu_PkgHead.nReq);
	    //Neoway_UartSend(NEOWAY_UART_PORT_2, (UINT8*)&stu_PkgHead, sizeof(stu_PkgHead));
	    //Debug_printf("rec%d",len);
	    ///<判断CRC16校验码
		if(stu_PkgHead.nCrc16==crc16Get(stu_GprsDataTmp.pBuf,stu_PkgHead.nPktSize))
		{
		   if((stu_PkgHead.nFlag==PKG_HEAD_FLAG))
		   {
	            StopSerTimeoutDetec();
	            enCodeIndex = (((stu_PkgHead.nFlag+0x55)<<8)|(stu_PkgHead.nReq^stu_PkgHead.nFlag));
	            p16Data=(UINT16*)stu_GprsDataTmp.pBuf;
				for(i=0;i<(stu_PkgHead.nPktSize/2);i++)
				{
	                   *p16Data^=enCodeIndex;
					   p16Data++;
				}
	            stu_ServerCtrl.state=stu_PkgHead.nReq;
	            i= stu_ServerCtrl.state/0x10;
	            j= stu_ServerCtrl.state-i*0x10;
	            GprsCmdProc[i][j](stu_ServerCtrl.state); 
		    }
	        // Neoway_Usb_Send(stu_GprsDataTmp.pBuf,stu_PkgHead.nPktSize);
		}
	    else
        {
			//Debug_printf(p8Data);
            Debug_printf("crc16 error");
            Debug_printf("stu_PkgHead.nReq=%x",stu_PkgHead.nReq);
            Debug_printf("stu_UpdateReq.nPktIndex=%x",stu_UpdateReq.nPktIndex);
        }
	}
	else
	{
		stu_GprsDataTmp.pBuf[stu_GprsDataTmp.len]='\0';
		if(stu_CldServer.state==WEATHER_FORECAST)
		{
			//stu_GprsDataTmp.pBuf[stu_GprsDataTmp.len]='\0';
			WeatherInfReqAck(NULL);
		}
		else if(stu_CldServer.state==ROAD_STATE_REPORT)
		{
			RoadInfReqAck(NULL);
		}
	}
}
/**
*@fn void GprsDataSend2Server(void)
*@brief 发数据到GPRS服务器
*@date 2013/8/22
*@author liusk
*/
void GprsDataSend2Server(void)
{
    UINT8 i,j;
	//static UINT8 linkFlag=0;

	if(IS_TIMER_OVER(GPRS_SERVER_TIMER))
	{
		if(gpModeMenu->status !=MENU_IDLE)
		{
			ReadyRequestAndDetect(SERVER_TIMEOUT_DETEC);
			return;
		}
		if(IS_TIMER_RUN(SOC_CLS_AUTO_TIMER))
		{
			ReadyRequestAndDetect(SERVER_TIMEOUT_DETEC);
			return;
		}
		if(IS_TIMER_RUN(LINK_ERROR_TIMER))
		{
			ReadyRequestAndDetect(SERVER_TIMEOUT_DETEC);
			return;
		}
		
		if((stu_GprsDev.status!= GPRS_INIT_OK))
		{
			ReadyRequestAndDetect(SERVER_TIMEOUT_DETEC);
			if(stu_ServerCtrl.linkIndex==LINK_WAIT)
			{
				RESET_TIMER(GPRS_LINK_TIMER);
				START_TIMER(GPRS_LINK_TIMER);
				//SET_TIMER_OVER(GPRS_LINK_TIMER);
				stu_ServerCtrl.linkIndex=LINK_OK;
			}
			return;
		}
		#if 0
		///< TCP尚未链接,不做任何处理，这表示着会在SERVER_DETE_TIMER超时后再次执行相关命令
		if((stu_GprsDev.status!= GPRS_INIT_OK))
		{
			ReadyRequestAndDetect(SERVER_TIMEOUT_DETEC);
			//SET_TIMER_OVER(GPRS_LINK_TIMER);
			RESET_TIMER(GPRS_LINK_TIMER);
    		START_TIMER(GPRS_LINK_TIMER);
			//LinkIpProc();
			return;
		}
		#endif
	    i= stu_ServerCtrl.state/0x10;
	    j= stu_ServerCtrl.state-i*0x10;
	    if(stu_ServerCtrl.state!= REQ_IDLE_REQ)
	    {
	        GprsCmdProc[i][j](stu_ServerCtrl.state);
			Debug_printf("to send:");
			//Debug_printf(stu_GprsDataTmp.pBuf);
	        GprsDataSend(stu_GprsDataTmp.pBuf,stu_GprsDataTmp.len,stu_CldServer.encrypFlag);
	        stu_ServerCtrl.lastState= stu_ServerCtrl.state;
	        stu_ServerCtrl.state= REQ_IDLE_REQ;
	        Debug_printf("send%x:\n",stu_GprsData.len);
			Debug_printf(stu_GprsData.pBuf);
	     }
	    RESET_TIMER(GPRS_SERVER_TIMER);
		stu_ServerCtrl.linkIndex=LINK_WAIT;
	}
}
/**
*@fn void UpdateDSAPrepare(UINT32 addr,UINT32 size,UINT32 blkSize)
*@brief 升级DSA增量区域前准备
*@ addr 起始地址 size 数据大小 blkSize 块大小
*details  
*@date 2013/8/23
*@author liusk
*/
void UpdateDSAPrepare(UINT32 addr,UINT32 size,UINT32 blkSize)
{
    UINT16 i;
    UINT16 cnt;
    if(addr&(blkSize-1))
        return;
    if(addr< FLASH_BASE_ADDR)
        return;
    cnt= (size+blkSize-1)/blkSize;
	
    for(i= 0;i<cnt;i++)
    {
		Debug_printf("\nerase rom for dsa CNT=%d,addr=%x\n",cnt,addr);
        FlashEraseWithLock(addr);
        addr+=blkSize;
    }
	Debug_printf("\nstart to initial camera\n");
    sl_Cameera_Init();
}
/**
*@fn void GetIPAndPortFrmLink(const UINT* pLink)
*@brief 从链接中获取IP和端口
*@param pLink->链接地址
*@date 2013/10/18
*@author liusk
*/
void GetIPAndPortFrmLink(const UINT8* pLink)
{
	UINT8*pPort;
	UINT8 index;
	
	index=myMemcpy(stu_CldServer.addr,pLink,':');
	stu_CldServer.addr[index]='\0';
	pPort=(UINT8*)&pLink[index+1];
	
	stack_print(stu_CldServer.addr);	  
	stu_CldServer.port= atoi(pPort);
	Debug_printf("IP=%s,port=%d",stu_CldServer.addr,stu_CldServer.port);	
}
/**
*@fn void LinkIpProc(void)
*@brief 链接IP处理
*@date 2013/10/17
*@author liusk
*/
void LinkIpProc(void)
{
    if(IS_TIMER_OVER(GPRS_LINK_TIMER))
    {
        RESET_TIMER(GPRS_LINK_TIMER);
        switch(stu_GprsDev.status)
        {
			case AT_LINKSETUP:
			case AT_LINKCLOSE:
				switch(stu_CldServer.state)
				{
					case DSA_UPDATE:
					case WEATHER_FORECAST:
					case ROAD_STATE_REPORT:
						if(stu_CldServer.state==DSA_UPDATE)
							stu_CldServer.encrypFlag=1;
						else
							stu_CldServer.encrypFlag=0;
						Debug_printf((UINT8*)stu_CldServer.pLink[(stu_CldServer.state>>1)]);
						Debug_printf("File=%s Line=%d\n,",__FILE__,__LINE__);
						GetIPAndPortFrmLink(stu_CldServer.pLink[(stu_CldServer.state>>1)]);
						stu_CldServer.linkType=SOC_SOCK_STREAM;	
						break;
					case PHONE_NUM_GET:
					case POI_UPLOAD:
					case POI_UPDATE:
						stu_CldServer.encrypFlag=1;
						GetIPAndPortFrmLink(stu_CldServer.pLink[(DSA_UPDATE>>1)]);
						stu_CldServer.linkType=SOC_SOCK_STREAM;
						break;
					case TRACE_UPLOAD:
						stu_CldServer.encrypFlag=0;
						Debug_printf((UINT8*)stu_CldServer.pLink[4]);
						Debug_printf("File=%s Line=%d\n,",__FILE__,__LINE__);
						GetIPAndPortFrmLink(stu_CldServer.pLink[4]);
						stu_CldServer.linkType=SOC_SOCK_DGRAM;	
						break;
					default:
						GetIPAndPortFrmLink(stu_CldServer.pLink[(DSA_UPDATE>>1)]);
						stu_CldServer.linkType=SOC_SOCK_STREAM;
						break;
				}	
				stu_GprsDev.status= GPRS_INIT_OK;
				break;
			default:
				break;
		}
	}
}

/**
*@fn void GprsDataSend(const UINT8*pData,UINT16 len,UINT8 flag)
*@brief GPRS数据发送
*@param pData 要发送的数据 len 数据长度 flag 是否数字转ASCII码 1为是 反之为否
*@date 2013/10/16
*@author liusk
*/
void GprsDataSend(const UINT8*pData,UINT16 len,UINT8 flag)
{
	if(0)//(flag)
	{
	    stu_GprsData.status= GPRS_SEND;
	    HexToAsc(stu_GprsData.pBuf,pData,len);
	    stu_GprsData.pBuf[len*2]='\0';
	    stu_GprsData.len=len*2+1;
	}
	else
	{
		stu_GprsData.status= GPRS_SEND;
		memcpy(stu_GprsData.pBuf,pData,len);
		stu_GprsData.len= len;
		stu_GprsData.pBuf[stu_GprsData.len+1]='\0';
	}
	Debug_printf("mauual send data");
	sl_soc_send(stu_CldServer.addr, strlen(stu_CldServer.addr), stu_CldServer.port, stu_CldServer.linkType, stu_GprsData.pBuf, stu_GprsData.len);		
}
/**
*@fn void SocketClose(void)
*@brief 关闭链接
*@date 2013/11/26
*@author liusk
*/
void SocketClose(void)
{
	sl_soc_send(stu_CldServer.addr, strlen(stu_CldServer.addr), stu_CldServer.port, stu_CldServer.linkType|0xf0, stu_GprsData.pBuf, 0);		
}
/**
*@fn void GprsRecProc(void)
*@brief GPRS接收处理
*@date 2013/10/18
*@author liusk
*/
void GprsRecProc(void)
{
    if(IS_TIMER_OVER(GPRS_DATA_TIMER))
    {
        RESET_TIMER(GPRS_DATA_TIMER);
		GprsServerDataProc(stu_GprsDataTmp.pBuf,stu_GprsDataTmp.len);		
    }
}
/**
*@fn void TraceUploadReady(const UINT8 pTrace,UINT16 len )
*@brief 轨迹上传准备
*@ pTrace  指向轨迹数据 len 数据长度
*@date 2013/10/31
*@author liusk
*/
void TraceUploadReady(const UINT8* pTrace,UINT16 len )
{
	if(IS_SIM_USE_NO())
	{
		return;
	}
	if((stu_CldServer.state==DSA_UPDATE)
		&&(theNMEA.m_btRMCDataValid=='A')
		&&(stu_GprsDev.status>=AT_LINKSETUP))
	{
		Debug_printf("GPS trace upload\r\n");
		memcpy(GpsTraceBuf,pTrace,len);
		RESET_TIMER(GPRS_TRACE_TIMER);
		START_TIMER(GPRS_TRACE_TIMER);
	}		
}
/**
*@fn void GprsTimeOutStart(UINT16 timeOut)
*@brief gprs超时处理启动
*@param timeOut  超时时间
*@date 2013/10/31
*@author liusk
*/
void GprsTimeOutStart(UINT16 timeOut)
{
	RESET_TIMER(GPRS_TIMEOUT_TIMER);
	SET_TIMER(GPRS_TIMEOUT_TIMER, timeOut);
	START_TIMER(GPRS_TIMEOUT_TIMER);
}
/**
*@fn void GprsTimeOutProc(void)
*@brief  gprs超时处理
*@date 2013/10/31
*@author liusk
*/
void GprsTimeOutProc(void)
{
    if(IS_TIMER_OVER(GPRS_TIMEOUT_TIMER))
    {
        RESET_TIMER(GPRS_TIMEOUT_TIMER);
		//stu_GprsDev.status= AT_LINKCLOSE;
		if(stu_CldServer.flag&stu_CldServer.state)
		{
			stu_CldServer.flag&=(~stu_CldServer.state);
		}	
		stu_CldServer.state=DSA_UPDATE;	
		RESET_TIMER(SERVER_DETE_TIMER);
		//gprs_request_sock(SOC_DISC);
    }
}
/**
*@fn void GprsLinkErrorProc(void)
*@brief  gprs链接错误处理
*@date 2013/11/12
*@author liusk
*/
void GprsLinkErrorProc(void)
{
    if(IS_TIMER_OVER(LINK_ERROR_TIMER))
    {
        RESET_TIMER(LINK_ERROR_TIMER);
		if(IS_TIMER_RUN(GPRS_DATA_TIMER))///< 数据接收未处理,先退出
		{
			RESET_TIMER(GPRS_SERVER_TIMER);
	   		RESET_TIMER(SERVER_DETE_TIMER);
			START_TIMER(LINK_ERROR_TIMER);
			return;
		}
		//RESET_TIMER(GPRS_SERVER_TIMER);
		if((stu_CldServer.state==ROAD_STATE_REPORT)||(stu_CldServer.state==POI_UPLOAD))
		{
			if(stu_CldServer.state==ROAD_STATE_REPORT)
				SET_TIMER_OVER(ROAD_GET_TIMER);
			AUDIO_REPORT(NET_BUSY_V);
		}

		if((stu_CldServer.state==DSA_UPDATE)&&(stu_CldServer.procErrCnt<3))
		{
			stu_SysMode.lastUpdateTime=0xffffffff;
			stu_CldServer.procErrCnt++;
		}
		else
		{
			stu_CldServer.procErrCnt=0;
		}
		if(stu_CldServer.flag&stu_CldServer.state)
		{
			stu_CldServer.flag&=(~stu_CldServer.state);
		}
		stu_ServerCtrl.state=REQ_IDLE_REQ;
		stu_CldServer.state=DSA_UPDATE;
		Debug_printf("gprs error proc\r\n");
		gprs_request_sock(SOC_DISC);
		 stu_GprsDev.status= AT_LINKCLOSE;
		 RESET_TIMER(GPRS_SERVER_TIMER);
		 RESET_TIMER(SERVER_DETE_TIMER);
		 stu_ServerCtrl.linkIndex=LINK_WAIT;
		//RESET_TIMER(GPRS_SERVER_TIMER);
		if(stu_DbgFlag.gprsDbgFlag==DEBUG_EN)
			AUDIO_REPORT(NET_LINK_ER);
    }
}
/**
*@fn void ServerOverTimeProc(void)
*@brief 服务器超时处理
*@date 2013/8/23
*@author liusk
*/
void ServerOverTimeProc(void)
{
	if(IS_TIMER_OVER(SERVER_DETE_TIMER))
	{
		RESET_TIMER(SERVER_DETE_TIMER);
		if(stu_ServerCtrl.errCnt<ERR_CNT_MAX)
		{
			stu_ServerCtrl.errCnt++;
		    stu_ServerCtrl.state= stu_ServerCtrl.lastState;
		    ReadyRequestAndDetect(SERVER_TIMEOUT_DETEC);  
			Debug_printf("time out cmd=%x cnt=%d",stu_ServerCtrl.lastState,stu_ServerCtrl.errCnt);
		}
		else
		{
			START_TIMER(LINK_ERROR_TIMER);
			SET_TIMER_OVER(LINK_ERROR_TIMER);
			SocketClose();	
		}
	}
}
/**
*@fn void GprsSocCloseAutoProc(void)
*@brief  gprs自动关闭 socket处理
*@date 2013/11/14
*@author liusk
*/
void GprsSocCloseAutoProc(void)
{
    if(IS_TIMER_OVER(SOC_CLS_AUTO_TIMER))
    {
        RESET_TIMER(SOC_CLS_AUTO_TIMER);
		if(IS_TIMER_RUN(GPRS_DATA_TIMER))///< 数据接收未处理,先退出
		{
			RESET_TIMER(GPRS_SERVER_TIMER);
	    	RESET_TIMER(SERVER_DETE_TIMER);
			START_TIMER(SOC_CLS_AUTO_TIMER);
			return;
		}
		if(stu_CldServer.flag&stu_CldServer.state)
		{
			stu_CldServer.flag&=(~stu_CldServer.state);
		}
		stu_ServerCtrl.state=REQ_IDLE_REQ;
		stu_GprsDev.status= AT_LINKCLOSE;
		RESET_TIMER(GPRS_SERVER_TIMER);
	    RESET_TIMER(SERVER_DETE_TIMER);
		stu_ServerCtrl.linkIndex=LINK_WAIT;
		Debug_printf("SOC_CLOSE AUTO\n");
    }
}
/**
*@fn void PoiUploadProc(void)
*@brief POI数据上传处理
*@date 2013/10/31
*@author liusk
*/
void PoiUploadProc(void)
{
	if(IS_TIMER_OVER(POI_UPLOAD_TIMER))
    {
        RESET_TIMER(POI_UPLOAD_TIMER);
		///<GPRS忙碌,继续等待
		if(stu_CldServer.flag&stu_CldServer.state)
		{
			 START_TIMER(POI_UPLOAD_TIMER);
			 
		}
		else if(IS_TIMER_RUN(SOC_CLS_AUTO_TIMER))
		{
			START_TIMER(POI_UPLOAD_TIMER);
			return;
		}
		else if(IS_TIMER_RUN(LINK_ERROR_TIMER))
		{
			START_TIMER(POI_UPLOAD_TIMER);
			return;
		}
		else
		{
			if(stu_CldServer.lastState==TRACE_UPLOAD)
			{
				stu_GprsDev.status= AT_LINKCLOSE;
				gprs_request_sock(SOC_DISC);
			}
			stu_CldServer.state= POI_UPLOAD;
			stu_CldServer.flag|=stu_CldServer.state;
			stu_CldServer.lastState=stu_CldServer.state;
			stu_ServerCtrl.state= REQ_POI_UPLOAD_REQ;
	        ReadyRequestAndDetect(SERVER_TIMEOUT_DETEC);
	        Debug_printf("POI Upload start\n");	
			stu_ServerCtrl.errCnt=0;
		}
	}
}

