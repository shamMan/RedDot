#ifndef TEST_GPS_H_
#define TEST_GPS_H_

#include "GPS_If.h"
//#include "../../SRC_Communication/System/Syncable.h"
//#include "../Common/Types/Syncable.h"
//#include <windows.h>

//using namespace GIS_Embedded;
namespace IS_GIS
{
class ComReadThread;
class GPS : public GPS_If, public Syncable
{
	HANDLE m_hdl;
	ComReadThread *m_read_thread;
	GPS_Info m_info;
	Satellites_Info sa_info;
public:
	GPS();
	~GPS();
	virtual int open(const wchar_t *com_port, unsigned int baud);
	virtual int read(GPS_Info *result);
	virtual int read( Satellites_Info *result);
	virtual int close();

public:
	void write(const GPS_Info &val);
	void write(const Satellites_Info &val);
};
}
#endif // TEST_GPS_H_