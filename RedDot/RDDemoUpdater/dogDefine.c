#include "dogDefine.h"

///< 纬度对应的每纬度公里值
static const UINT8 KmPerLongitude[] =
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
void SetPkgHead(STU_PACKAGE_HEADER* header,UINT8*p8Data,UINT16 paramLen)
{		
	UINT16* p16Data;
	UINT16 enCodeIndex; ///< 加密因子
	UINT16 i;

	p16Data=(UINT16*)p8Data;

	header->nFlag = PKG_HEAD_FLAG;						
	header->nReq ^= header->nFlag;									
	header->nPktSize = paramLen^(((header->nFlag+0x11)<<8) | header->nFlag);	

	enCodeIndex = (((header->nFlag+0x55)<<8) | header->nReq);
	///< 参数加密
	i= (paramLen>>1);
	for(; i>0; i--)
	{
		*p16Data^=enCodeIndex;;
		p16Data++;
	}

	header->nCrc16=crc16Get(p8Data,paramLen);		  
}

int CheckPkgHead(UINT8* p8Data, UINT16 len)
{
	STU_PACKAGE_HEADER* header = (STU_PACKAGE_HEADER*)p8Data;
	DecodePkgHead(header);
	if (header->nFlag != PKG_HEAD_FLAG)
		return -1;

	UINT16 enCodeIndex = (((header->nFlag+0x55)<<8)|(header->nReq ^ header->nFlag));
	UINT16*p16Data = (UINT16*)(p8Data + sizeof(STU_PACKAGE_HEADER));
	int paramLen = len - sizeof(STU_PACKAGE_HEADER);
	///< 参数解密
	int i= (paramLen>>1);
	for(; i>0; i--)
	{
		*p16Data^=enCodeIndex;;
		p16Data++;
	}
	
	return header->nReq;
}


BOOL CheckUpdateInfo()
{
	STU_PACKAGE_HEADER* header = (STU_PACKAGE_HEADER*)m_pSendBuffer;
	STU_UPDATE_CHECK_REQ* req = (STU_UPDATE_CHECK_REQ*)(m_pSendBuffer + sizeof(STU_PACKAGE_HEADER));
	memset(req, 0, sizeof(STU_UPDATE_CHECK_REQ));
	memcpy(req->SerNo,m_uSerNo,12);
	memcpy(req->HWType,"SF1301",6);
	req->FWVer=m_dwAppVersion;					//固件版本
	req->HWVer=m_dwHWVersion;					//硬件版本
	req->DataTime=m_dwDataTime;	//最新数据日期 	
	req->nStartLatitude=0;						//当前最新数据的起始点纬度，单位为度；
	req->nStartLongitude=0;					    //当前最新数据的起始点经度，单位为度；
	req->nEndLatitude=0;						    //当前最新数据的结束点纬度，单位为度；
	req->nEndLongitude=0;						    //当前最新数据的结束点经度，单位为度；
	req->nDataOption=0;						    //数据选项，待定义；
	req->nPtLatitude=(UINT16)(m_dRMCLatitude*100);				//当前所在位置的纬度，单位为度；
	req->nPtLongitude=(UINT16)(m_dRMCLongitude*100);			    //当前所在位置的经度，单位为度；
	req->nBufferSize=0;						    //内部数据包缓冲区大小	
	
	header->nReq = REQ_UPDATE_CHECK_REQ;
	SetPkgHead(header, (UINT8*)req, sizeof(STU_UPDATE_CHECK_REQ));

	int len = sizeof(STU_PACKAGE_HEADER) + sizeof(STU_UPDATE_CHECK_REQ);

	if (CheckTcpConnected() == FALSE)
		return FALSE;
	if (m_pSocket->writen(m_pSendBuffer, len) != len)
		return FALSE;
	len = m_pSocket->read(m_pSendBuffer, 1400);
	if (len < 6 || CheckPkgHead(m_pSendBuffer, len) != REQ_UPDATE_CHECK_ACK)
	{
		return FALSE;
	}
	STU_UPDATE_CHECK_ACK ack;
	memcpy((UINT8*)&ack,m_pSendBuffer+sizeof(STU_PACKAGE_HEADER),sizeof(ack));
	if (ack.nResult == 0)
		return FALSE;

	m_dwUpdateBytes = ack.dwBytes;

	//m_pSocket->close();
	// Start thread to download data
	cout << "Start thread to download" << endl;
	Start();
	//m_pThread

	
	return TRUE;
}


int CIncUpdater::UploadPOIInfo(UINT32 nPtLatitude, UINT32 nPtLongitude, UINT16 angle, UINT8 voiceID)
{
	STU_PACKAGE_HEADER* header = (STU_PACKAGE_HEADER*)m_pSendBuffer;
	STU_POI_UPLOAD_REQ* req = (STU_POI_UPLOAD_REQ*)(m_pSendBuffer + sizeof(STU_PACKAGE_HEADER));
	memset(req, 0, sizeof(STU_POI_UPLOAD_REQ));
	memcpy(req->SerNo,m_uSerNo,12);
	req->nPtLatitude = nPtLatitude;
	req->nPtLongitude = nPtLongitude;
	req->angle = angle;
	req->voiceID = voiceID;

	header->nReq = REQ_POI_UPLOAD_REQ;
	SetPkgHead(header, (UINT8*)req, sizeof(STU_POI_UPLOAD_REQ));
	int len = sizeof(STU_PACKAGE_HEADER) + sizeof(STU_POI_UPLOAD_REQ);

	// send the package, and check the response
	if (CheckTcpConnected() == FALSE)
		return -1;
	if (m_pSocket->writen(m_pSendBuffer, len) != len)
		return -1;
	len = m_pSocket->read(m_pSendBuffer, 1024);
	if (len < 6 || CheckPkgHead(m_pSendBuffer, len) != REQ_POI_UPLOAD_ACK)
	{
		return -1;
	}
	STU_POI_UPLOAD_CHK_ACK* ack = (STU_POI_UPLOAD_CHK_ACK*)(m_pSendBuffer+sizeof(STU_PACKAGE_HEADER));

	return *(UINT16*)ack ;

}

int CIncUpdater::CheckPrivateUpdateInfo()
{
	UINT8* pData = m_pSendBuffer;
	STU_PACKAGE_HEADER* header = (STU_PACKAGE_HEADER*)pData;
	STU_POI_UPDATE_REQ* req = (STU_POI_UPDATE_REQ*)(pData + sizeof(STU_PACKAGE_HEADER));
	memset(req, 0, sizeof(STU_POI_UPDATE_REQ));
// 	struct tm *tm_ptr;
// 	time_t the_time;
// 	(void) time(&the_time);
// 	tm_ptr = gmtime(&the_time);
// 	sprintf((char*)req->time, "%4d-%2d-%2d %2d:%2d:%2d", tm_ptr->tm_year, tm_ptr->tm_mon+1, tm_ptr->tm_mday,
// 		tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec);
	req->nPtLatitude = m_dRMCLatitude;
	req->nPtLongitude = m_dRMCLongitude;
	memcpy(req->SerNo,m_uSerNo,12);

	header->nReq = REQ_UPDATE_CHECK_REQ;
	SetPkgHead(header, (UINT8*)req, sizeof(STU_POI_UPDATE_REQ));
	int len = sizeof(STU_PACKAGE_HEADER) + sizeof(STU_POI_UPDATE_REQ);

	// send the package, and check the response
	if (CheckTcpConnected() == FALSE)
		return -1;
	if (m_pSocket->writen(m_pSendBuffer, len) != len)
		return -1;
	len = m_pSocket->read(m_pSendBuffer, 1024);
	if (len < 6 || CheckPkgHead(m_pSendBuffer, len) != REQ_UPDATE_CHECK_ACK)
	{
		return -1;
	}
	STU_POIUPDATE_CHK_ACK* ack = (STU_POIUPDATE_CHK_ACK*)(m_pSendBuffer+sizeof(STU_PACKAGE_HEADER));
	
	// to download private info

	return 0;
}

int CIncUpdater::MakeUpdateRequest(UINT8* pData, UINT32 index, UINT8 lastpkg)
{
	STU_PACKAGE_HEADER* header = (STU_PACKAGE_HEADER*)pData;
	STU_UPDATE_REQ* req = (STU_UPDATE_REQ*)(pData + sizeof(STU_PACKAGE_HEADER));
	cout << "Make update reqeust" << endl;
	memset(req, 0, sizeof(STU_UPDATE_REQ));
	req->nPktIndex = index;
	req->nPackSize = 1024;
	req->nLastPacket = lastpkg;

	header->nReq = REQ_UPDATE_REQ;
	SetPkgHead(header, (UINT8*)req, sizeof(STU_UPDATE_CHECK_REQ));

	return sizeof(STU_PACKAGE_HEADER) + sizeof(STU_UPDATE_REQ);
}

int CIncUpdater::UpdateProcImpl()
{
	int total = (m_dwUpdateBytes >> 10);
	cout << "UpdateProcImpl()" << endl;
	for (int index = 0; index < total; index++)
	{
		int len = MakeUpdateRequest(m_pSendBuffer, index, index == total -1);
		if (CheckTcpConnected() == FALSE)
			return -1;
		if (m_pSocket->writen(m_pSendBuffer, len) != len)
			return -1;
		len = m_pSocket->read(m_pSendBuffer, 1024);
		if (len < 6 || CheckPkgHead(m_pSendBuffer, len) != REQ_UPDATE_ACK)
		{
			return -1;
		}
		STU_UPDATE_REQ* req = (STU_UPDATE_REQ*)(m_pSendBuffer+sizeof(STU_PACKAGE_HEADER));
		UINT8* pData = (UINT8*)req + sizeof(STU_UPDATE_REQ);

	}
	return 1;
}

int CIncUpdater::MainFun()
{
	return UpdateProcImpl();
}