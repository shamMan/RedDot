#ifndef __EVENT_H__
#define __EVENT_H__

#include "Thread.h"
class Event
{
public :
	Event() ;
	~Event() ;
	bool Wait(unsigned long timeout) ;
	void Set() ;	
private:
	novak_cond_t cond;
	novak_mutex_t mutex ;
};
#endif

