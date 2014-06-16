///////////////////////////////////////////////////////////
//  Trace.h
//  Implementation of the Class Trace
//  Created on:      09-04-2007 10:55:13
//  Original author: RogerSong
//  Modified by: Yitang Cheng
///////////////////////////////////////////////////////////

#ifndef	TRACE_H
#define TRACE_H

#include <iostream>
#include <errno.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef void _trace_func_t(int level, const char *file, int line, const char* pstr);

class Trace
{
public:
	typedef enum {LEVEL_NONE, LEVEL_RAW, LEVEL_EMERG, LEVEL_ERROR, LEVEL_DEBUG, LEVEL_TRACE} LEVEL;

	static Trace* GetInstance();
	~Trace();

	void Initialize(LEVEL level, const char* file = 0);
	void SetTraceLevel(LEVEL level);
	bool CanTrace(LEVEL level) const;
	void TraceOut(LEVEL level, const char* file, int line, const char* pstr);
	void SetTraceFunc(_trace_func_t* f) { tracefunc = f;}
private:
	Trace();
	Trace(const Trace&);
	Trace& operator=(const Trace&);
	std::string Header(LEVEL level, const char* fileName, int lineNum);

	LEVEL		traceThreshold_;
	bool		isInitialized_;
	FILE*		fp_;
	_trace_func_t *tracefunc;
};


#ifdef _DEBUG
#define DOTRACE(level, args)	\
	do { \
		if (Trace::GetInstance()->CanTrace(level)) {	\
		std::stringstream	__ss;	\
		__ss << args << std::endl;	\
		Trace::GetInstance()->TraceOut(level, __FILE__, __LINE__, __ss.str().c_str());	\
		}	\
	} while(0)

#define	NLINIT(level, filename)	\
	do {	\
		Trace::GetInstance()->Initialize(level, filename);	\
	} while(0)

#define	NLLEVEL(level)	\
	do {	\
		Trace::GetInstance()->SetTraceLevel(level);	\
	} while(0)
#else
#define DOTRACE(level, args)
#define	NLINIT(level, filename)
#define	NLLEVEL(level)
#endif

#define	NLRAW(args)	DOTRACE(Trace::LEVEL_RAW, args)
#define	NLEMERG(args)	DOTRACE(Trace::LEVEL_EMERG, args)
#ifdef	WIN32
#define	NLERROR(args)	DOTRACE(Trace::LEVEL_ERROR, args << ", WSAGetLastError() = " << WSAGetLastError() << ")!!!")
#else
#define	NLERROR(args)	DOTRACE(Trace::LEVEL_ERROR, args << ", errno = " << errno << "(" << strerror(errno) << ")!!!")
#endif
#define	NLDEBUG(args)	DOTRACE(Trace::LEVEL_DEBUG, args)
#define	NLTRACE(args)	DOTRACE(Trace::LEVEL_TRACE, args)

#endif /*TRACE_H*/

