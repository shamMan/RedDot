#pragma once
#include "GPS_Info.h"
//using namespace std;


class GPS_Calman
{
private:
	GPS_Info m_last_state;
	float p;
	float q;
	float r;
	float kg;
public:
	GPS_Calman(void);
public:
	~GPS_Calman(void);
private:
	bool IsOriginMode();
	GPS_Info estimate(const GPS_Info &info_read);
public:
	GPS_Info filter(const GPS_Info &info_read);
};
