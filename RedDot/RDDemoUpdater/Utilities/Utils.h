///////////////////////////////////////////////////////////
//
//  Utils.h
//
//  Implementation of some utils functions
//  Created on:      13-Sep-2007 16:34:55
//  Original author: chetang
///////////////////////////////////////////////////////////

#ifndef	UTILS_H
#define	UTILS_H

#include <string>
#include <algorithm>

#ifdef	WIN32
#include <time.h>
#include <Windows.h>

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

struct timezone 
{
	long  tz_minuteswest; /* minutes W of Greenwich */
	long  tz_dsttime;     /* type of dst correction */
};

int gettimeofday(struct timeval *tv, struct timezone *tz);
#else
#include <sys/time.h>
#endif

std::string Dump2Hex(const void* pbuf, size_t len);
double difftime(struct timeval& tv0, struct timeval& tv1);
std::ostream& operator << (std::ostream& os, const struct timeval tv);
void msleep(int msec);

#endif	/*UTILS_H*/

