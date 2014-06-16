///////////////////////////////////////////////////////////
//  Trace.cpp
//  Implementation of the Class Trace
//  Created on:      09-04-2007 10:55:13
//  Original author: RogerSong
//  Modified by: Yitang Cheng
///////////////////////////////////////////////////////////
#include <iomanip>
#include <iostream>
#include <string.h>
#include "Trace.h"
#ifndef WIN32
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#else
#include <windows.h>
#endif
#ifndef	NOTHREAD

#include "Event.h"
#include "AutoLock.h"
#include "thread.h"
#include "Utils.h"

static NovakMutex	g_mutex;
#endif


Trace* Trace::GetInstance()
{
	static Trace	s_trace;
	return &s_trace;
}

Trace::Trace()
	: traceThreshold_(LEVEL_TRACE), isInitialized_(false), fp_(0), tracefunc(NULL)
{

}

Trace::~Trace()
{
	if(fp_) {
		fclose(fp_);
		fp_ = 0;
	}
}

void Trace::SetTraceLevel(LEVEL level)
{
	traceThreshold_ = level;
}

bool Trace::CanTrace(LEVEL level) const
{
	return traceThreshold_ >= level;
}

void Trace::Initialize(LEVEL level, const char* file)
{
	if (isInitialized_) {
		if (fp_) {
			fclose(fp_);
			fp_ = 0;
		}
		isInitialized_ = false;
	}

	std::string	filename;

	if (file) {
#if defined(WIN32) && defined(UNICODE)
		TCHAR	buf[MAX_PATH];

		if (GetEnvironmentVariable(TEXT("tmp"), (LPWSTR)buf, MAX_PATH) > 0) {
			int	len;
			char	tmp[MAX_PATH];

			len = WideCharToMultiByte(CP_ACP, 0, buf, -1, NULL, 0, NULL,FALSE);
			WideCharToMultiByte(CP_ACP, 0, buf, -1, tmp, len, NULL,FALSE);

			filename = tmp;
			filename += '\\';
			filename += file;
		}
		else 
#endif
		filename = file;
	}

#ifndef	NOTHREAD
	NovakAutoLock	mh(g_mutex);
#endif
	traceThreshold_ = level;
	if (! filename.empty()) {
		fp_ = fopen(filename.c_str(), "w");
		if (fp_) {
			fclose(fp_);
			fp_ = fopen(filename.c_str(), "a");
			setbuf(fp_, 0);
		if (fp_) {
			isInitialized_ = true;
		}
	}
	}
	else {
		isInitialized_ = true;
	}
}

void Trace::TraceOut(LEVEL level, const char* file, int line, const char* pstr)
{
	if (CanTrace(level) && isInitialized_) {
#ifndef	NOTHREAD
		NovakAutoLock	mh(g_mutex);
#endif
		if (level == LEVEL_RAW) {
			std::cerr << pstr;
		}
		else
		if (fp_) {
			fprintf(fp_, "%s%s", Header(level, file, line).c_str(), pstr);
			fflush(fp_);
		}
		else {
			if (tracefunc)
				tracefunc(level, file, line, pstr);
			std::cerr << Header(level, file, line) << pstr;
		}
	}
}


std::string Trace::Header(LEVEL level, const char* fileName, int lineNum)
{
	std::stringstream	str;

	char	buf_[256];
	time_t	now = time(0);
	struct tm *	ptm = localtime(&now);
	strftime(buf_, 256, "%Y-%m-%d %H:%M:%S", ptm);
	struct timeval	tv_;
	gettimeofday(& tv_, 0);
	str << buf_ << '.' << std::setw(6)
		<< std::setiosflags(std::ios::left) << tv_.tv_usec
		<< ' ';

#ifndef WIN32
	str << std::setw(6) 
		<< std::setiosflags(std::ios::right)
		<< getpid() << ":" << (signed long)pthread_self()
		<< '\t';
#else
	str << std::setw(6) 
		<< std::setiosflags(std::ios::right)
		<< GetCurrentProcessId() << ":"
		<< GetCurrentThreadId() << '\t';
#endif

	str << '(' << level << ") ";

	const char*	file = strrchr(fileName, '/');
	if (file != 0)
		file++;
	else {
		file = strrchr(fileName, '\\');
		if (file != 0)
			file++;
		else
			file = fileName;
	}
	str << std::setiosflags(std::ios::right) << std::setw(16) << file
		<< '-' << lineNum << ":\t";

	return str.str();
}

