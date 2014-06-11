/**
@file gprsProc.h
@author liusk
@version 1.0
@date 2013/10/15
@brief GPRS数据处理相关函数声明以及宏定义
@copyright (C)版权所有，深圳市兆图电子有限公司，保留所有权利
*/

#include "undef.h"
#ifdef __cplusplus
extern "C"{
#endif

#ifndef _GPRSPROC_H_
#define _GPRSPROC_H_
///< 增量升级服务器
#define DSA_UPDATA_LINK				"update.gps-car.cn:21211"//"data.gps-car.cn:21211"//

///< GPRS模块状态
#define AT_READY					0
#define	AT_LINKSETUP				AT_READY+1					
#define AT_GPRSSEND                 AT_LINKSETUP+1
#define AT_LINKCLOSE                AT_GPRSSEND+1				
#define GPRS_INIT_OK	            0x50

#define NORMAL_CMD_ACK              0x01
#define SIM_CMD_ACK                 0x02


#define GPRS_BUF_MAX                1100        ///<缓冲区大小
#define GPRS_BUF_SUM                1           ///< 缓冲区格个数

///< 定义区域的范围(KM)
#define DSA_AREA_DIST           160
///< 包头标志字
#define PKG_HEAD_FLAG           0xA5
///< 启动服务器超时检测
#define SERVER_TIMEOUT_DETEC    0X01
///<数据命令
typedef enum
{
    REQ_IDLE_REQ            = 0X00,
    REQ_UPDATE_CHECK_REQ	= 0x10, ///<查询是否需要升级
	REQ_UPDATE_REQ	        = 0x14, ///< 请求发送某数据包
	REQ_UPDATE_CHECK_ACK    = 0x20, ///< 查询是否需要升级的应答
	REQ_UPDATE_ACK          = 0x21, ///< 发送请求的数据包
	REQ_PHONENUM_REQ		= 0x32,	///< 请求手机号码
	REQ_PHONENUM_ACK		= 0x33,	///< 应答手机号码	
	REQ_POI_UPLOAD_REQ		= 0x40, ///< POI上传
	REQ_POI_UPLOAD_ACK		= 0x41, ///< POI上传应答
	REQ_POI_UPDATE_CHK_REQ  = 0x42,	///< 查询是否需要升级POI数据
	REQ_POI_UPDATE_CHK_ACK  = 0x43,	///< 查询是否需要升级POI数据应答	
	REQ_WEATHER_REQ         = 0X50, ///< 查询天气
	REQ_ROAD_REQ			= 0X51,	///< 路况查询
	REQ_ROAD_REQ_ACK		= 0X57,	///< 路况查询
	REQ_WEATHER_ACK         = 0X58, ///< 天气查询应答
	REQ_TRACE_UPLOAD		= 0X59,	///< 轨迹上传
}EN_DSA_REQ;
///< 包头结构体
typedef struct{
	UINT8	nFlag;		///<包头标志0xA5
	UINT8	nReq;		///<请求类型字,等同于命令码
	UINT16	nPktSize;	///<数据包大小^(((nFlag+0x11)<<8)|nFlag)
	UINT16	nCrc16;		///<整个数据包的CRC16校验字（不包括nCrc16本身）；
}STU_PACKAGE_HEADER;
///< 升级查询请求结构体
typedef struct{
	UINT8	SerNo[12];		///<序列号
	UINT8	HWType[12];		///<硬件类型
	INT32	FWVer;			///<固件版本
	INT32	HWVer;			///<硬件版本
	INT32	DataTime;		///<最新数据日期
	INT16	nStartLatitude;	///<当前最新数据的起始点纬度，单位为度；
	INT16	nStartLongitude;///<当前最新数据的起始点经度，单位为度；
	INT16	nEndLatitude;	///<当前最新数据的结束点纬度，单位为度；
	INT16	nEndLongitude;	///<当前最新数据的结束点经度，单位为度；
	UINT32	nDataOption;	///<数据选项，待定义；
	INT16	nPtLatitude;	///<当前所在位置的纬度，单位为度；
	INT16	nPtLongitude;	///<当前所在位置的经度，单位为度；
	INT16	nBufferSize;	///<内部数据包缓冲区大小；
	INT16 	nReserved;		///<保留
}STU_UPDATE_CHECK_REQ;
///<  升级查询请求服务器应答结构体
typedef struct{
	UINT8	nResult;		///<是否需要升级，0无需升级；1升级数据；2升级固件
	UINT8	nDataMode;		///<数据升级模式，0增量升级；1全部升级；
	UINT16	nPackSize;		///<升级数据包大小，实际字节数；
	UINT32	dwBytes;		///<待传输的数据大小；
}STU_UPDATE_CHECK_ACK;
///< 升级数据请求结构体
typedef struct{
	UINT32	nPktIndex;		///<数据包索引号；
	UINT16	nPackSize;		///<本次传输的数据包大小；
	UINT8	nLastPacket;	///<是否为最后一个数据包
	UINT8	nData[5];		///<数据,保留
}STU_UPDATE_REQ;
///< 天气预报请求参数结构体
typedef struct{
	INT32	longitude;		    ///<经度，精确到1%度
	INT32	latitude;			///<纬度，精确到1%度
}STU_WEATHER_PARAM;
///< 处理手机号码请求
typedef struct{
	UINT8 simNum[20];
	UINT8 SerNo[10];
}STU_PHONENUM_PARAM;
extern STU_PHONENUM_PARAM stu_PhoneNumParam;
///< 链接序号
typedef enum{
	LINK_WAIT,
	LINK_OK,
}EM_LINK_INDEX;
///< 一个结构体，专门用于与服务器的通讯处理
#define ERR_CNT_MAX		3
typedef struct{
    UINT8  state;       ///< 当前状态，见EN_DSA_REQ
    UINT8  lastState;   ///< 上次的状态
    UINT8  flag;        ///< 处理是否最后一包数据等
    UINT16 pkgSize;     ///< 包大小
    UINT16 pkgNoSum;    ///< 总的包数
    UINT32 pkgNo;       ///< 包序号
    UINT32 dataSizeSum;  ///< 数据总大小
    UINT32 dataAddr;    ///< 数据存放地址
    UINT8  linkIndex;	///< 链接序号,
    UINT8 errCnt;		///< 连续发送数据出错次数
    
}STU_SERVERCTRL;
///< 云服务状态
typedef enum{
	DSA_UPDATE= 0x01,
	WEATHER_FORECAST=0x02,
	ROAD_STATE_REPORT=0x04,
	PHONE_NUM_GET=0X08,
	TRACE_UPLOAD=0x10,
	POI_UPLOAD=0x20,
	POI_UPDATE=0x40,
	SERVER_STATE_SUM,
}EM_SERVER_STATE;
///< 一个结构体 处理云服务
typedef struct{
	UINT8 state;	///< 见EM_SERVER_STATE，处于对应状态处理相应云服务
	UINT8 lastState; ///< 上个状态
	UINT8 flag;		///< 标志，处理对应服务的时候设置，与state相或
	UINT8 encrypFlag;	///< 1 数据加解密 0 否
	const UINT8*pLink[5];	///< 指向对应状态的服务链接
	UINT8 addr[80]; ///< 网址或者IP地址
	UINT32 port;	///< 端口
	UINT8 linkType;	///< 链接类型
	UINT8 procErrCnt; ///< 处理错误计数
}STU_CLOUD_SERVER;
///< 一个结构体，处理GPRS数据
typedef struct{
    UINT8 (*pBuf)[GPRS_BUF_MAX];    ///<指向缓存数组
    UINT8 frameCnt;                ///< 帧数
    UINT8 status;                  ///< 当前所处状态
    UINT8 faultCnt;                ///< 应答出错计数
    UINT8 flag;                    ///< 数据标识
    UINT16 list[GPRS_BUF_SUM];     ///< 缓冲区位移指针
}STU_GPRS_DEV;
///< 一个结构体,用于GPRS数据发送
typedef enum{
    GPRS_IDLE,
    GPRS_SEND,
    GPRS_REC,
}GPRS_COMM_STATUS;
typedef struct{
    UINT8 status;         ///< 状态信息，比如发送/接收,见GPRS_COMM_STATUS
    UINT16 len;
    UINT8*pBuf;    
}STU_GPRS_DATA;
enum
{

    CONNECTION = 0x70,
    GPRS_ATTACHED,
    SEND_INFO,
    RECV_INFO,
    SOC_DISC, 
    GET_ICCID,
    GPRS_DETACHED
};

enum
{
    EN_SOC_INVAL_PARA = -10,
    EN_SOC_CREATING_ERROR,
    EN_SOC_DNS_QUERY_ERROR,
    EN_SOC_DNS_IND_ERROR,
    EN_SOC_SEND_ERROR,
    EN_SOC_RECV_ERROR,
    EN_SOC_NOTIFY_ERROR,
    EN_SOC_CONNECT_ERROR,
    EN_SOC_CLOSED,
    EN_SOC_ERROR,
    EN_SOC_SUCCESS = 0
} ;

enum
{
	SOC_AUTO = 0,
    SOC_MANUAL
} ;

enum
{
    IDLE,
    SOC_CREATING,
    SOC_DNS_QUERY,
    SOC_DNS_RESOLVED,
    SOC_CREATED,
    SOC_CONNECTING,
    SOC_CONNECTED,
    REQ_SENDING,
    REQ_SENT,
    RSP_RCVING,
    RSP_RCVD,
    SOC_CLOSING,
   SOC_CLOSED
} ;

typedef struct url_info {

	char* url;
	kal_uint16 url_len;
}URL_INFO;

///< 一个结构体，保存路况信息参数
typedef struct{
	UINT32 rLongitude;
	UINT32 rLatitude;
	UINT16 rSpeed;
}STU_ROAD_PARAM;
///< 投诉点上传结构体
typedef struct{
	UINT8	SerNo[12];		///< 序列号
	UINT32	nPtLatitude;	///< 当前所在位置的纬度，单位为度；
	UINT32	nPtLongitude;	///< 当前所在位置的经度，单位为度；
	UINT16  angle;			///< 方向角度
	UINT8   voiceID;		///< 语音ID
	UINT8   flag;			///< 1为新增 0为删除
	UINT8 	nReserved[8];	///< 保留
}STU_POI_UPLOAD_REQ;

///< 一个结构体,处理POI私有数据的升级查询
typedef struct{
	UINT8 time[20];
	UINT32	nPtLatitude;	///< 当前所在位置的纬度，单位为度；
	UINT32	nPtLongitude;	///< 当前所在位置的经度，单位为度；
	UINT8	SerNo[10];		///< 序列号
	UINT8   nReserved[6];	///< 保留字
}STU_POI_UPDATE_REQ;
///<  poi升级查询请求服务器应答结构体
typedef struct{
	UINT8   time[20];		///< 时间点
	UINT8	nResult;		///<是否需要升级，0无需升级；1升级数据；2升级固件
	UINT8	nDataMode;		///<数据升级模式，0增量升级；1全部升级；
	UINT16	nPackSize;		///<升级数据包大小，实际字节数；
	UINT32	dwBytes;		///<待传输的数据大小；
}STU_POIUPDATE_CHK_ACK;
    
///< 一个结构体,处理POI私有数据的升级查询
extern __align(4) STU_POI_UPDATE_REQ stu_PoiUpdateReq;
extern STU_ROAD_PARAM stu_RoadParam;
extern STU_CLOUD_SERVER stu_CldServer;
extern __align(4) STU_POI_UPLOAD_REQ stu_PoiUploadReq;
///< 请求检验是否需要增量升级
extern __align(4) STU_UPDATE_CHECK_REQ stu_UpdateCheckReq;
///< 获取增量升级区域的信息
extern __align(4) CAMERA_DATAINFO stu_CameraAreaInf;
///< 升级查询应答结构体变量
extern __align(4) STU_UPDATE_CHECK_ACK stu_UpdateChkAck;
///< 包头结构体
extern __align(4) STU_PACKAGE_HEADER stu_PkgHead;
///< 纬度对应的每纬度公里值
extern const UINT8 KmPerLongitude[];
///<处理接收的GPRS数据
extern STU_GPRS_DEV stu_GprsDev;
///< GPRS初始化后的发送
extern STU_GPRS_DATA stu_GprsData;
///< GPRS链接服务器后的数据接收
extern STU_GPRS_DATA stu_ServerData;
///< GPRS数据缓冲区结构体
extern STU_GPRS_DATA stu_GprsDataTmp;
///< 信号强度
extern U8 gGprsSignalInf[100];
///< 服务器控制用
extern STU_SERVERCTRL stu_ServerCtrl;
/**
*@fn void FunDefault(UINT8 cmd)
*@brief 未定义命令应答
*details  
*@date 2013/8/21
*@author liusk
*/
void FunDefault(UINT8 cmd);
/**
*@fn void UpdateChk(UINT8 cmd)
*@brief 升级查询
*details  
*@date 2013/8/21
*@author liusk
*/
void UpdateChk(UINT8 cmd);
/**
*@fn void DownLoadDataReq(UINT8 cmd)
*@brief 升级数据请求
*details  
*@date 2013/8/21
*@author liusk
*/
void DownLoadDataReq(UINT8 cmd);
/**
*@fn void UploadChk(UINT8 cmd)
*@brief 上传查询
*details  
*@date 2013/8/21
*@author liusk
*/
void UploadChk(UINT8 cmd);
/**
*@fn void UploadReq(UINT8 cmd)
*@brief 上传数据
*details  
*@date 2013/8/21
*@author liusk
*/
void UploadReq(UINT8 cmd);
/**
*@fn void UpdateChkAck(UINT8 cmd)
*@brief 升级查询应答
*details  
*@date 2013/8/21
*@author liusk
*/
void UpdateChkAck(UINT8 cmd);
/**
*@fn void DownLoadDataReqAck(UINT8 cmd)
*@brief 升级数据应答
*details  
*@date 2013/8/21
*@author liusk
*/
void DownLoadDataReqAck(UINT8 cmd);
/**
*@fn void UploadChkAck(UINT8 cmd)
*@brief 上传数据查询应答
*details  
*@date 2013/8/21
*@author liusk
*/
void UploadChkAck(UINT8 cmd);
/**
*@fn void UploadReqAck(UINT8 cmd)
*@brief 上传数据应答
*details  
*@date 2013/8/21
*@author liusk
*/
void UploadReqAck(UINT8 cmd);
/**
*@fn void SetPkgHead(UINT8 cmd,UINT8*p8Data,UINT16 paramLen)
*@brief 设置包头相关信息
*@param cmd 命令字 p8Data 参数信息指针，paramLen 参数大小
*details  
*@date 2013/8/21
*@author liusk
*/
void SetPkgHead(UINT8 cmd,UINT8*p8Data,UINT16 paramLen);
/**
*@fn void GprsServerDataProc(UINT8* p8Data,UINT16 len)
*@brief GPRS服务器数据处理
*@param p8Data 数据指针 len 数据长度
*@date 2013/8/21
*@author liusk
*/
void GprsServerDataProc(UINT8* p8Data,UINT16 len);
/**
*@fn void WeatherInfReq((UINT8 cmd))
*@brief 升级查询
*@param cmd 命令字
*details  
*@date 2013/8/21
*@author liusk
*/
void WeatherInfReq(UINT8 cmd);
/**
*@fn void DecodePkgHead(STU_PACKAGE_HEADER*pkgHead)
*@brief 解析包头
*@param pkgHead 包头结构体指针
*details  
*@date 2013/8/21
*@author liusk
*/
void DecodePkgHead(STU_PACKAGE_HEADER*pkgHead);
/**
*@fn void  UpdateJudgeBaseGps(void)
*@brief 基于GPS判断是否升级
*@date 2013/8/22
*@author liusk
*/
void UpdateJudgeBaseGps(void);
/**
*@fn void GprsDataSend2Server(void)
*@brief 发数据到GPRS服务器
*@date 2013/8/22
*@author liusk
*/
void GprsDataSend2Server(void);
/**
*@fn void UpdateDSAPrepare(UINT32 addr,UINT32 size,UINT32 blkSize)
*@brief 升级DSA增量区域前准备
*@ addr 起始地址 size 数据大小 blkSize 块大小
*details  
*@date 2013/8/23
*@author liusk
*/
void UpdateDSAPrepare(UINT32 addr,UINT32 size,UINT32 blkSize);
/**
*@fn void ReadyRequestAndDetect(UINT8 detectFlag)
*@brief 准备发送服务器端请求以及检测
*@param detectFlag 0 为不启动服务器应答超时检测 其他为启动 
*@date 2013/8/23
*@author liusk
*/
void ReadyRequestAndDetect(UINT8 detectFlag);
/**
*@fn void StopSerTimeoutDetec(void)
*@brief 停止服务器超时检测
*@date 2013/8/23
*@author liusk
*/
void StopSerTimeoutDetec(void);
/**
*@fn void ServerOverTimeProc(void)
*@brief 服务器超时处理
*@date 2013/8/23
*@author liusk
*/
void ServerOverTimeProc(void);
/**
*@fn void  WeatherForeCastProc(void)
*@brief 基于GPS判断进行天气预报处理
*@date 2013/8/31
*@author liusk
*/
void WeatherForeCastProc(void);
/**
*@fn void WeatherInfReqAck((UINT8 cmd))
*@brief 天气预报数据
*@param cmd 命令字
*details  
*@date 2013/9/1
*@author liusk
*/
void WeatherInfReqAck(UINT8 cmd);
/**
*@fn void PhoneNumReq(UINT8 cmd)
*@brief 获取手机号请求
*@param cmd 命令字
*details  
*@date 2013/9/4
*@author liusk
*/
void PhoneNumReq(UINT8 cmd);
/**
*@fn void PhoneNumReqAck(UINT8 cmd)
*@brief 获取手机号请求应答
*@param cmd 命令字
*details  
*@date 2013/9/4
*@author liusk
*/
void PhoneNumReqAck(UINT8 cmd);
//void gprs_request_info(void* p, kal_uint16 info);
/**
*@fn void LinkIpProc(void)
*@brief 链接IP处理
*@date 2013/10/17
*@author liusk
*/
void LinkIpProc(void);
/**
*@fn void GprsDataSend(const UINT8*pData,UINT16 len,UINT8 flag)
*@brief GPRS数据发送
*@param pData 要发送的数据 len 数据长度 flag 是否数字转ASCII码 1为是 反之为否
*@date 2013/10/16
*@author liusk
*/
void GprsDataSend(const UINT8*pData,UINT16 len,UINT8 flag);
/**
*@fn void GprsRecProc(void)
*@brief GPRS接收处理
*@date 2013/10/18
*@author liusk
*/
void GprsRecProc(void);
/**
*@fn void RoadInfReq((UINT8 cmd))
*@brief 路况查询请求
*@param cmd 命令字
*details  
*@date 2013/10/29
*@author liusk
*/
void RoadInfReq(UINT8 cmd);
/**
*@fn void RoadInfReqAck(UINT8 cmd)
*@brief 路况查询应答
*@param cmd 命令字
*details  
*@date 2013/10/29
*@author liusk
*/
void RoadInfReqAck(UINT8 cmd);
/**
*@fn void RoadParamInfUpdate(UINT8* pRoadParam)
*@brief 路况参数更新
*@paramp pRoadParam 路况参数
*details  
*@date 2013/10/30
*@author liusk
*/
void RoadParamInfUpdate(UINT8* pRoadParam);
/**
*@fn void TraceUploadProc(void)
*@brief 轨迹数据上传处理
*@date 2013/10/31
*@author liusk
*/
void TraceUploadProc(void);
/**
*@fn void TraceUploadReady(const UINT8 pTrace,UINT16 len )
*@brief 轨迹上传准备
*@ pTrace  指向轨迹数据 len 数据长度
*@date 2013/10/31
*@author liusk
*/
void TraceUploadReady(const UINT8* pTrace,UINT16 len );
/**
*@fn void GpsTraceUpload((UINT8 cmd))
*@brief GPS 轨迹上传
*@param cmd 命令字
*details  
*@date 2013/10/31
*@author liusk
*/
void GpsTraceUpload(UINT8 cmd);
/**
*@fn void GprsTimeOutStart(UINT16 timeOut)
*@brief gprs超时处理启动
*@param timeOut  超时时间
*@date 2013/10/31
*@author liusk
*/
void GprsTimeOutStart(UINT16 timeOut);
/**
*@fn void GprsTimeOutProc(void)
*@brief  gprs超时处理
*@date 2013/10/31
*@author liusk
*/
void GprsTimeOutProc(void);
/**
*@fn void GprsLinkErrorProc(void)
*@brief  gprs链接错误处理
*@date 2013/11/12
*@author liusk
*/
void GprsLinkErrorProc(void);
/**
*@fn void ServerOverTimeProc(void)
*@brief 服务器超时处理
*@date 2013/8/23
*@author liusk
*/
void ServerOverTimeProc(void);
/**
*@fn void GprsSocCloseAutoProc(void)
*@brief  gprs自动关闭 socket处理
*@date 2013/11/14
*@author liusk
*/
void GprsSocCloseAutoProc(void);
/**
*@fn void PoiUploadProc(void)
*@brief POI数据上传处理
*@date 2013/10/31
*@author liusk
*/
void PoiUploadProc(void);
/**
*@fn void PoiUploadAck(UINT8 cmd)
*@brief POI数据上传应答
*@param cmd 命令字
*details  
*@date 2013/11/19
*@author liusk
*/
void PoiUploadAck(UINT8 cmd);
/**
*@fn void PoiUploadReq(UINT8 cmd)
*@brief POI数据上传
*@param cmd 命令字
*details  
*@date 2013/11/19
*@author liusk
*/
void PoiUploadReq(UINT8 cmd);
void sl_soc_send(char* ip, kal_uint16 ip_len, kal_uint16 port, socket_type_enum protocol, kal_uint8* buf, kal_uint16 len);
/**
*@fn void SocketClose(void)
*@brief 关闭链接
*@date 2013/11/26
*@author liusk
*/
void SocketClose(void);
/**
*@fn void PoiUpdateChk((UINT8 cmd))
*@brief POI私有数据升级查询
*@param cmd 命令字
*details  
*@date 2013/12/9
*@author liusk
*/
void PoiUpdateChk(UINT8 cmd);
/**
*@fn void PoiUpdateAck(UINT8 cmd)
*@brief POI升级查询应答
*@date 2013/12/09
*@author liusk
*/
void PoiUpdateAck(UINT8 cmd);
    
#ifdef __cplusplus
}
#endif

#endif




