#include "Event.h"

Event::Event()
{
	novak_cond_init(&cond,NULL);
	novak_mutex_init(&mutex,NULL) ;
}
Event::~Event()
{
	novak_cond_destroy(&cond);
	novak_mutex_destroy(&mutex);
}

bool Event::Wait(unsigned long timeout)
{
	return (novak_cond_timewait(&cond, &mutex, timeout) != 1);
}

void Event::Set()
{
	novak_mutex_lock(&mutex) ;
	novak_cond_signal(&cond);
	novak_mutex_unlock(&mutex);
}


