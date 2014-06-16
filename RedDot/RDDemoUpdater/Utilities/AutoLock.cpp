#include "AutoLock.h"

AutoLock::AutoLock(novak_mutex_t& mutex):m_mutex(mutex)
{
	novak_mutex_lock(&m_mutex);	
}

AutoLock::~AutoLock()
{
	novak_mutex_unlock(&m_mutex);
}


NovakAutoLock::NovakAutoLock(NovakMutex& mutex):m_mutex(mutex)
{
	m_mutex.Lock();
}

NovakAutoLock::~NovakAutoLock()
{
	m_mutex.Unlock();
}

