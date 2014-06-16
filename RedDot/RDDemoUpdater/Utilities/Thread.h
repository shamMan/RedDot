
#ifndef _THREAD_H_
#define _THREAD_H_

//#include "common.h"

#ifdef WIN32
#include <wtypes.h>
typedef HANDLE novak_cond_t;
typedef HANDLE novak_mutex_t;
typedef HANDLE novak_thread_t;

#define novak_mutex_init	win_mutex_init
#define novak_mutex_lock	win_mutex_lock
#define novak_mutex_unlock	win_mutex_unlock
#define novak_mutex_destroy	win_mutex_destroy
#define novak_thread_create	win_thread_create
#define novak_thread_join	win_thread_join
#define novak_thread_exit   win_thread_exit
#define novak_cond_init		win_cond_init
#define novak_cond_signal	win_cond_signal
#define novak_cond_broadcast	win_cond_broadcast
#define novak_cond_wait		win_cond_wait
#define novak_cond_timewait	win_cond_timewait
#define novak_cond_destroy	win_cond_destroy


#ifdef __cplusplus
extern "C"
{
#endif

	int win_mutex_init(novak_mutex_t *m, void *attr_unused);
	int win_mutex_lock(novak_mutex_t *mutex);
	int win_mutex_unlock(novak_mutex_t *mutex);
	int win_mutex_destroy(novak_mutex_t *mutex);
	int win_thread_create(novak_thread_t *t, void *attr_unused, void *(*func)(void*), void *arg);
	int win_thread_join(novak_thread_t thread, void **unused);
	int win_thread_exit(novak_thread_t thread);
	int win_cond_init(novak_cond_t *cond, void *attr_unused);
	int win_cond_wait(novak_cond_t * cond, novak_mutex_t * mutex);
	int win_cond_timewait(novak_cond_t * cond, novak_mutex_t * mutex, unsigned long microsecond);
	int win_cond_signal(novak_cond_t * cond);
	int win_cond_broadcast(novak_cond_t * cond);
	int win_cond_destroy(novak_cond_t * cond);

#ifdef __cplusplus
}
#endif

#else

#include <sys/socket.h>
#include <pthread.h>
#include <sys/time.h>
#if defined (MAC) || defined (IOS)
#include <sys/errno.h>
#endif

#ifndef ETIMEDOUT
#define ETIMEDOUT 110
#endif

typedef pthread_t novak_thread_t;
typedef pthread_mutex_t novak_mutex_t;
typedef pthread_cond_t novak_cond_t;

#ifdef __cplusplus
extern "C"
{
#endif

	int __novak_thread_join(novak_thread_t thread, void **ptr);
	int __novak_thread_create(pthread_t *thread, pthread_attr_t *attr, void * (*routine)(void*), void *arg);
	int __novak_thread_cond_timewait(novak_cond_t *cond, novak_mutex_t *mutex, unsigned long microsecond);
#ifdef __cplusplus
}
#endif

#define novak_thread_create	__ortp_thread_create
#define novak_thread_join	__ortp_thread_join
#define novak_thread_exit	pthread_exit
#define novak_mutex_init	pthread_mutex_init
#define novak_mutex_lock	pthread_mutex_lock
#define novak_mutex_unlock	pthread_mutex_unlock
#define novak_mutex_destroy	pthread_mutex_destroy
#define novak_cond_init		pthread_cond_init
#define novak_cond_signal	pthread_cond_signal
#define novak_cond_broadcast	pthread_cond_broadcast
#define novak_cond_wait		pthread_cond_wait
#define novak_cond_timewait	__novak_thread_cond_timewait
//pthread_cond_timewait
#define novak_cond_destroy	pthread_cond_destroy
#endif

class NovakMutex{
public:
	NovakMutex();
	~NovakMutex();

	void Lock();
	void Unlock();
private:
	novak_mutex_t mutex;
};

class Thread
{
public:
	Thread();
	~Thread();
	static void* Run(void *pv) ;
	virtual int MainFun() = 0 ;

	bool IsActive(int timeout) ;
	bool Start() ;
	void Stop() ;
public:
	bool bExit ;
private:
	novak_thread_t m_pid ;
	novak_cond_t cond;
	novak_mutex_t mutex ;
};

#endif
