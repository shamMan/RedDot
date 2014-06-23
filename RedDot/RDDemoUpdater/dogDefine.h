#ifndef DOG_DEFINE_H__
#define DOG_DEFINE_H__

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

//#define DSA_UPDATE_SERVER   "update.gps-car.cn"
#define DSA_UPDATE_SERVER	"data.gps-car.cn"
#define DSA_UPDATE_PORT		21211

//typedef signed char BOOL;
#define TRUE    1
#define FALSE   0

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
//	kal_uint16 url_len;
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

typedef struct{
	UINT8 result;
	UINT8 reason;
}STU_POI_UPLOAD_CHK_ACK;

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

// API
extern UINT16 crc16Get(const UINT8  *buf, UINT16 len);
extern void DecodePkgHead(STU_PACKAGE_HEADER*pkgHead);
extern void SetPkgHead(STU_PACKAGE_HEADER* header,UINT8*p8Data,UINT16 paramLen);
extern int CheckPkgHead(UINT8* p8Data, UINT16 len);

#endif //INC_UPDATE_H__