#pragma once
#ifdef WIN32
#include <WTypes.h>
#endif
#include "dogDefine.h"
#include "./Utilities/Thread.h"
class Socket;
class CIncUpdater : public Thread
{
public:
	CIncUpdater(void);
	~CIncUpdater(void);

	BOOL CheckUpdateInfo();
	int CheckPrivateUpdateInfo();
	int UploadPOIInfo(UINT32 nPtLatitude, UINT32 nPtLongitude, UINT16 angle, UINT8 voiceID);
private:
	// Thread
	virtual int MainFun();
	BOOL CheckTcpConnected();
	void DecodePkgHead(STU_PACKAGE_HEADER*pkgHead);
	void SetPkgHead(STU_PACKAGE_HEADER* header,UINT8*p8Data,UINT16 paramLen);
	int CheckPkgHead(UINT8* p8Data, UINT16 len);
	int MakeUpdateRequest(UINT8* pData, UINT32 index, UINT8 lastpkg);
	int UpdateProcImpl();
	STU_PACKAGE_HEADER stuPackageHeader;
	
	Socket* m_pSocket;

	UINT32 m_dwAddr;

	UINT8* m_pSendBuffer;
	// Device info
	UINT8 m_uSerNo[12];
	UINT32 m_dwAppVersion;
	UINT32 m_dwHWVersion;
	UINT32 m_dwDataTime;
	UINT8 m_dRMCLatitude;
	UINT8 m_dRMCLongitude;

	BOOL m_bWifiOnly;

	UINT32 m_dwUpdateBytes;
};
