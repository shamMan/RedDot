
#ifndef __AUTOLOCK__H__
#define __AUTOLOCK__H__

#include "thread.h"
class AutoLock
{
public:
	AutoLock(novak_mutex_t& mutex);
	~AutoLock() ;
private:
	novak_mutex_t& m_mutex ;
};

class NovakAutoLock
{
public:
	NovakAutoLock(NovakMutex& mutex);
	~NovakAutoLock() ;
private:
	NovakMutex& m_mutex ;
};
#endif
