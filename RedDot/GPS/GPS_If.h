#ifndef TEST_GIS_H_
#define TEST_GIS_H_

//#include <time.h>
#include "GPS_Info.h"
#include <string>
#ifdef _DEBUG
#include <assert.h>
#endif
//struct GPS_Info
//{
//	tm time;
//	// Latitude, in degrees. Positive numbers indicate north latitude
//	double latitude;
//	// Longitude, in degrees. Positive numbers indicate east longitude
//	double longitude;
//	// Speed, in knots (nautical miles).
//	float speed;
//	// Heading, in degrees. A heading of zero is true north
//	float heading;		
//	// Altitude, in meters, with respect to sea level.
//	float altitude_sea_level;
//	// Altitude, in meters, with respect to the WGS84 ellipsoid. 
//	float altitude_84;
//};
namespace IS_GIS
{
enum GPS_Error
{
	GPSE_UNABLE_TO_OPEN_PORT = 1,
	GPSE_SET_COM_STATE_ERR
};

class GPS_If
{
	GPS_If *m_gps;
public:
	GPS_If();
	void init();
	virtual ~GPS_If()
	{
		delete m_gps;
		m_gps = 0;//NULL;
	}
	virtual int open(const wchar_t *com_port, unsigned int baud)
	{
#ifdef _DEBUG
		assert(m_gps);
#endif
		return m_gps->open(com_port, baud);
	}
	virtual int read(GPS_Info *result)
	{
#ifdef _DEBUG
		assert(m_gps);
#endif
		return m_gps->read(result);
	}


virtual int read(Satellites_Info *result)
	{
/*#ifdef _DEBUG
		assert(m_gps);
#endif*/
		return m_gps->read(result);
	}

	virtual int close()
	{
#ifdef _DEBUG
		assert(m_gps);
#endif
		return m_gps->close();
	}
};
}
#endif //TEST_GIS_H_