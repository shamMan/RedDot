
#include "Thread.h"


#if	!defined(_WIN32) && !defined(_WIN32_WCE)
int __ortp_thread_join(novak_thread_t thread, void **ptr){
	int err=pthread_join(thread,ptr);
	if (err!=0) {
	}
	return err;
}

int __ortp_thread_create(pthread_t *thread, pthread_attr_t *attr, void * (*routine)(void*), void *arg){
	pthread_attr_t my_attr;
	pthread_attr_init(&my_attr);
	if (attr)
		my_attr = *attr;
#ifdef ORTP_DEFAULT_THREAD_STACK_SIZE
	if (ORTP_DEFAULT_THREAD_STACK_SIZE!=0)
		pthread_attr_setstacksize(&my_attr, ORTP_DEFAULT_THREAD_STACK_SIZE);
#endif
	return pthread_create(thread, &my_attr, routine, arg);
}
// 1 for time out, 0 for cond
int __novak_thread_cond_timewait(novak_cond_t *cond, novak_mutex_t *mutex, unsigned long microsecond)
{
	timeval now;
	timespec to;
	gettimeofday(&now,NULL);  
	to.tv_sec   = now.tv_sec + microsecond / 1000;
	to.tv_nsec  = now.tv_usec * 1000 + (microsecond - ((microsecond / 1000) * 1000)) * 1000000;
	if (to.tv_nsec >= 1000000000)
	{
		to.tv_sec += to.tv_nsec / 1000000000;
		to.tv_nsec %= 1000000000;
	}
	return (pthread_cond_timedwait(cond, mutex, &to) == ETIMEDOUT);
}
#endif

#if	defined(_WIN32) || defined(_WIN32_WCE)

int win_mutex_init(novak_mutex_t *mutex, void *attr)
{
	*mutex=CreateMutex(NULL, FALSE, NULL);
	return 0;
}

int win_mutex_lock(novak_mutex_t * hMutex)
{
	WaitForSingleObject(*hMutex, INFINITE); /* == WAIT_TIMEOUT; */
	return 0;
}

int win_mutex_unlock(novak_mutex_t * hMutex)
{
	ReleaseMutex(*hMutex);
	return 0;
}

int win_mutex_destroy(novak_mutex_t * hMutex)
{
	CloseHandle(*hMutex);
	return 0;
}

typedef struct thread_param{
	void * (*func)(void *);
	void * arg;
}thread_param_t;

static unsigned WINAPI thread_starter(void *data){
	thread_param_t *params=(thread_param_t*)data;
	void *ret=params->func(params->arg);
	free(data);
	return (DWORD)ret;
}

#if defined WIN32
#    define _beginthreadex	CreateThread
#    define	_endthreadex	ExitThread
#endif

int win_thread_create(novak_thread_t *th, void *attr, void * (*func)(void *), void *data)
{
	thread_param_t *params=(thread_param_t*)malloc(sizeof(thread_param_t));
	params->func=func;
	params->arg=data;
	*th=(HANDLE)_beginthreadex( NULL, 0, (LPTHREAD_START_ROUTINE)thread_starter, params, 0, NULL);
	return 0;
}

int win_thread_join(novak_thread_t thread_h, void **unused)
{
	if (thread_h!=NULL)
	{
		WaitForSingleObject(thread_h, INFINITE);
		CloseHandle(thread_h);
	}
	return 0;
}

int win_thread_exit(novak_thread_t thread)
{
	CloseHandle(thread);
	return 0;
}

int win_cond_init(novak_cond_t *cond, void *attr)
{
	*cond=CreateEvent(NULL, FALSE, FALSE, NULL);
	return 0;
}

int win_cond_wait(novak_cond_t* hCond, novak_mutex_t * hMutex)
{
	//gulp: this is not very atomic ! bug here ?
	win_mutex_unlock(hMutex);
	WaitForSingleObject(*hCond, INFINITE);
	win_mutex_lock(hMutex);
	return 0;
}

int win_cond_signal(novak_cond_t * hCond)
{
	SetEvent(*hCond);
	return 0;
}

int win_cond_broadcast(novak_cond_t * hCond)
{
	win_cond_signal(hCond);
	return 0;
}

int win_cond_destroy(novak_cond_t * hCond)
{
	CloseHandle(*hCond);
	return 0;
}
// 1 for time out, 0 for hcond
int win_cond_timewait(novak_cond_t * hCond, novak_mutex_t * hMutex, unsigned long microsecond)
{
	//gulp: this is not very atomic ! bug here ?
	DWORD dwRet;
	win_mutex_unlock(hMutex);
	dwRet = WaitForSingleObject(*hCond, microsecond);
	win_mutex_lock(hMutex);
	return (dwRet ==  WAIT_TIMEOUT);
}
#endif


Thread::Thread()
{
	bExit = true ;
	novak_cond_init(&cond,NULL);
	novak_mutex_init(&mutex,NULL) ;
}
Thread::~Thread()
{
	Stop();
	novak_mutex_destroy(&mutex);
	novak_cond_destroy(&cond);
}
void* Thread::Run(void *pv)
{
	Thread *pthread= static_cast<Thread*>(pv);
	pthread->MainFun() ;
	return (void*)0 ;
}
bool Thread::Start()
{
	bExit = false ;
	int iRet = novak_thread_create(&m_pid,NULL,&Thread::Run,this);
	return (iRet == 0 );
}

bool Thread::IsActive(int timeout)
{
	novak_mutex_lock(&mutex);
	int iRet = novak_cond_timewait(&cond, &mutex,timeout);
	bool result = (iRet == 1) && (!bExit) ;
	novak_mutex_unlock(&mutex);	
	return result;
}


void Thread::Stop()
{
	if (bExit)
		return;
	novak_mutex_lock(&mutex);
	bExit = true ;
	novak_cond_signal(&cond) ;	
	novak_mutex_unlock(&mutex);
	
#if !defined(ANDROID) && !defined(IOS)
	void *retVal = NULL ;
	novak_thread_join(m_pid,&retVal);
#endif
}


NovakMutex::NovakMutex()
{
	novak_mutex_init(&mutex, NULL);
}
NovakMutex::~NovakMutex()
{
	novak_mutex_destroy(&mutex);
}

void NovakMutex::Lock()
{
	novak_mutex_lock(&mutex);
}
void NovakMutex::Unlock()
{
	novak_mutex_unlock(&mutex);
}
