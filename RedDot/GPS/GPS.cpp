//#include "stdafx.h"
#include "gps.h"
#include "CommReadThread.h"
namespace IS_GIS
{
GPS::GPS()
{
	m_hdl = NULL;
	m_read_thread = NULL;
	sa_info=Satellites_Info();
}

GPS::~GPS()
{
	close();
	//if(m_read_thread)
	//{
	//	m_read_thread->markStop();
	//}

	//if(m_hdl)
	//	CloseHandle(m_hdl);
	//m_hdl = NULL;

	//m_read_thread->waitUntilExit();
	//m_read_thread = NULL;
}

int GPS::close()
{
	if(m_read_thread)
	{
		m_read_thread->markStop();
	}

	if(m_hdl)
		CloseHandle(m_hdl);
	m_hdl = NULL;

	if(m_read_thread)
	{
		m_read_thread->waitUntilExit();
		delete m_read_thread;
		m_read_thread = NULL;
	}

	return 0;
}

int GPS::open(const wchar_t *com_port, unsigned int baud)
{
	if( m_hdl != NULL )
		::CloseHandle( m_hdl );

//	wchar_t devicename[10];
//	if( device == 2 )
//		sprintf( devicename, "COM2" );
//	else
//		sprintf( devicename, "COM1" );

	// 打开COM端口
	m_hdl = ::CreateFile(com_port, GENERIC_READ|GENERIC_WRITE, 
			0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
			NULL );
	if( m_hdl == INVALID_HANDLE_VALUE )
	{
		m_hdl = NULL;
		DWORD err_code = GetLastError();
		return err_code;
	}

	// 设置COM数值
	::SetCommMask(m_hdl, EV_RXCHAR );//| EV_ERR | EV_RXFLAG); 
	::SetupComm(m_hdl, 4096, 4096); 
	::PurgeComm(m_hdl, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR); 

	COMMTIMEOUTS CommTimeOuts; 
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF; 
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0; 
	CommTimeOuts.ReadTotalTimeoutConstant = 1000; 
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0; 
	CommTimeOuts.WriteTotalTimeoutConstant = 1000; 
	::SetCommTimeouts(m_hdl, &CommTimeOuts); 

	DCB dcb; 
	dcb.DCBlength = sizeof(DCB); 
	
	::GetCommState(m_hdl, &dcb); 

	switch( baud )
	{
	case 1200 :
		dcb.BaudRate = CBR_1200;
		break;
	case 4800 :
		dcb.BaudRate = CBR_4800; 
		break;
	case 9600 :
		dcb.BaudRate = CBR_9600;
		break;
	case 19200 :
		dcb.BaudRate = CBR_19200;
		break;
	case 115200:
		dcb.BaudRate = CBR_115200;
		break;
	default :
		dcb.BaudRate = baud;//CBR_4800;
		break;
	}

	dcb.fBinary = TRUE; 
	dcb.fParity = FALSE; 
	
	dcb.fOutxDsrFlow = FALSE; 
	dcb.fDtrControl = DTR_CONTROL_ENABLE; 
	dcb.fOutxCtsFlow = FALSE; 
	dcb.fRtsControl = RTS_CONTROL_ENABLE; 
	dcb.fInX = dcb.fOutX = FALSE; 

	dcb.StopBits = ONESTOPBIT; 
	dcb.ByteSize = 8;//bitsum; // 8
	dcb.Parity = NOPARITY; 
	
	BOOL fRetVal = SetCommState(m_hdl, &dcb); 
	if( fRetVal == FALSE )
	{
		DWORD err_code = GetLastError();
		::CloseHandle( m_hdl );
		m_hdl = NULL;
		return (int)err_code;
	}

	// 生成线程，
//	m_MonitorThread = ::CreateThread((LPSECURITY_ATTRIBUTES )NULL, 0, 
//			COMMonitorProc, this, 0, &m_threadID ); 
//	if( m_MonitorThread == NULL )
//	{
//		::CloseHandle( m_hdl );
//		m_hdl = NULL;
//		return S_FALSE;
//	}
	if(m_read_thread)
	{
		assert(0);
	}
	m_read_thread = new ComReadThread();
	m_read_thread->setCOMPort(m_hdl);
	m_read_thread->setWriteTarget(this);
	m_read_thread->start();

//	m_hdl = CreateFile(L"COM3:", GENERIC_READ, FILE_SHARE_READ, 
//		0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

//	wchar_t buf[100];
//	wsprintf(buf, L"%d", (int)m_hdl);
//	::MessageBox(NULL, buf, L"", MB_OK);
	return 0;
}

int GPS::read(GPS_Info *result)
{
	lock();
	*result = m_info;
	unlock();
	return 0;
}
int GPS::read( Satellites_Info *result)
{
	lock();
	*result=sa_info;
	unlock();
	return 0;
}

void GPS::write(const GPS_Info &val)
{
	lock();
	m_info = val;
	unlock();
}

void GPS::write(const Satellites_Info& val)
{
	lock();
	sa_info=val;
	unlock();
}

}