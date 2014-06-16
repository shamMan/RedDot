///////////////////////////////////////////////////////////
//
//  Utils.cpp
//
//  Implementation of some utils functions
//  Created on:      13-Sep-2007 16:34:55
//  Original author: chetang
///////////////////////////////////////////////////////////

#include <sstream>
#include <iomanip>
#include <ctype.h>
#ifdef WIN32
#include <WTypes.h>
#else
#include <sys/select.h>
#endif
#include "Utils.h"

#ifdef WIN32
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;
	static int tzflag;

	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);

		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		/*converting file time to unix epoch*/
		tmpres /= 10;  /*convert into microseconds*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS; 
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}

	if (NULL != tz)
	{
		if (!tzflag)
		{
			_tzset();
			tzflag++;
		}
		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}

	return 0;
}
#endif

std::string Dump2Hex(const void* pbuf, size_t len)
{
	std::stringstream	ss_;
	const size_t	nperlen = 16;
	const unsigned char*	ptr = (const unsigned char*)pbuf;
	size_t	ii_, jj_, curpos;
	size_t	lines_ = (len + nperlen - 1) / nperlen;

	ss_.fill('0');
	ss_ << std::endl << "Dump2Hex(" << len << "):" << std::endl;
	for (ii_ = 0; ii_ < lines_; ++ii_) {
		ss_ << std::setw(4) << ii_ << ": ";
		for (jj_ = 0; jj_ < nperlen; ++jj_) {
			curpos = ii_ * nperlen + jj_;
			if (curpos >= len) {
				ss_ << "   ";
			}
			else {
				ss_ << std::setw(2) << std::hex << (unsigned)ptr[curpos] << std::dec << ' ';
			}
			if (7 == jj_) {
				ss_ << "- ";
			}
		}
		ss_ << " ";
		for (jj_ = 0; jj_ < nperlen; ++jj_) {
			curpos = ii_ * nperlen + jj_;
			if (curpos >= len) {
				break;
			}

			if (isprint(ptr[curpos])) {
				ss_ << (char)ptr[curpos] << ' ';
			}
			else {
				ss_ << ". ";
			}
			if (7 == jj_) {
				ss_ << "- ";
			}
		}
		ss_ << std::endl;
	}
	
	return ss_.str();
}

double difftime(struct timeval& tv0, struct timeval& tv1)
{
	double diff = 0;

	diff = tv1.tv_sec + double(tv1.tv_usec) / 1000000;
	diff -= tv0.tv_sec + double(tv0.tv_usec) / 1000000;

	return diff;
}

std::ostream& operator << (std::ostream& os, const struct timeval tv)
{
	os << "{tv_sec = " << tv.tv_sec << ", tv_usec = " << tv.tv_usec << "}";

	return os;
}

void msleep(int msec)
{
#ifdef	WIN32
	Sleep(msec);
#else
	struct timeval	tv;

	tv.tv_sec = msec / 1000;
	tv.tv_usec = (msec % 1000) * 1000;

	select(0, NULL, NULL, NULL, &tv);
#endif
}

