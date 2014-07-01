#include "GPS_Calman.h"
#include "GPS_Info.h"
#include "../Common/CRS/GlobalCrt.h"
#include <math.h>

using namespace std;
using namespace GIS_Embedded;

GPS_Calman::GPS_Calman(void)
{
	m_last_state.speed = 0;
	m_last_state.heading = 0;
	m_last_state.latitude = 0;
	m_last_state.longitude = 0;
	p = 10;
	q = 0.000001;	//可能需要根据实际测试情况修改
	r = 0.1;		//同上
}

GPS_Calman::~GPS_Calman(void)
{
}

bool GPS_Calman::IsOriginMode()
{
	return m_last_state.speed == 0 && m_last_state.heading == 0 && m_last_state.latitude == 0 && m_last_state.longitude == 0;
}

GPS_Info GPS_Calman::estimate(const GPS_Info &info_read)
{
	GPS_Info result;
	result = info_read;
	double prj_x, prj_y;
	GlobalCrt crt;
	crt.geoToPrj(m_last_state.longitude, m_last_state.latitude, &prj_x, &prj_y);
	float speed_x, speed_y;
	speed_x = m_last_state.speed * sin(m_last_state.heading);
	speed_y = m_last_state.speed * cos(m_last_state.heading);
	prj_x = prj_x + speed_x * 1852 * 0.3 / 3600;
	prj_y = prj_y + speed_y * 1852 * 0.3 / 3600;
	crt.prjToGeo(prj_x, prj_y, &(result.longitude), &(result.latitude));
	result.speed = m_last_state.speed;
	result.heading = m_last_state.heading;
	return result;
}

GPS_Info GPS_Calman::filter(const GPS_Info &info_read)
{
	if(IsOriginMode())
	{
		m_last_state = info_read;
		return info_read;
	}
	GPS_Info result;
	result = estimate(info_read);
	p = p + q;
	kg = p / (p + r);
	result.speed = m_last_state.speed + kg * (info_read.speed - m_last_state.speed);
	result.heading = m_last_state.heading + kg * (info_read.heading - m_last_state.heading);
	result.latitude = result.latitude + kg * (info_read.latitude - result.latitude);
	result.longitude = result.longitude + kg * (info_read.longitude - result.longitude);
	p = (1 - kg) * p;
	m_last_state = result;
	return result;
}
