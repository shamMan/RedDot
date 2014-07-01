//#include "stdafx.h"
#include "gps_if.h"
#include "gps.h"
namespace IS_GIS
{
GPS_If::GPS_If()
{
	m_gps = NULL;
}

void GPS_If::init()
{
	if(m_gps)
	{
		close();
		delete m_gps;
	}
	m_gps = new GPS();
}
}