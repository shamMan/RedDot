///////////////////////////////////////////////////////////
//  Socket.h
//  Implementation of the Class Socket
//  Created on:      09-04-2007 10:55:10
//  Original author: Yitang Cheng
//  @modify   darrenyu <darrenyu@audividi.com> 2013-07-22
///////////////////////////////////////////////////////////

#ifndef	SOCKET_H
#define	SOCKET_H

#ifdef	WIN32
#include <Winsock.h>

typedef int	socklen_t;
#else
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#ifdef TARGET_OS_IPHONE
#   include <CoreFoundation/CFBase.h>
#   include <CoreFoundation/CFStream.h>
#   include <CFNetwork/CFSocketStream.h>
#   include "Mutex.h"
#endif

typedef	int	SOCKET;
#define	INVALID_SOCKET		-1
#define	closesocket		close
#endif

#include "DataStruct.h"

void initNetwork();

class Socket
{
public:
	Socket();
	explicit Socket(SOCKET fd);
	virtual ~Socket();

	enum SHUT {RD/* = SD_RECEIVE*/, WR/* = SD_SEND*/, RDWR/* = SD_BOTH*/};

	int tcpconnect(unsigned short port, unsigned int ip);
	int tcplisten(unsigned short port, unsigned int ip = 0);
	int tcpisconnected();
	/**
	* ����һ��TCP����
	* @return	���󷵻�-1,��ʱ����0,�ɹ�����>0
	*/
	int accept(Socket& sock, sockaddress* psa = 0, int msec = 500);
	/**
	 * Read len bytes from Socket into the buffer starting at buf.
	 *
	 * @param len    the number of bytes want to read
	 * @param buf    points to a buffer where the message should be stored
	 * @return       Read len bytes message, readn() should return len;
	 *      	 Otherwise, -1 shall be returned.
	 */
	int readn(void* buf, size_t len, int msec = 500);
	int writen(const void* buf, size_t len, int msec = 500);
	/**
	 * Read up to len bytes from Socket into the buffer starting at buf.
	 *
	 * @param len    the number of bytes want to read
	 * @param buf    points to a buffer where the message should be stored
	 * @return       Read len bytes message, readn() should return len;
	 *               If no data are available to be read, then return 0;
	 *      	 Otherwise, -1 shall be returned.
	 */
	int read(void* buf, size_t len, int msec = 500);
	int peek(void* buf, size_t len);

	int udplisten(unsigned short port, unsigned int ip = 0);
	/**
	* 
	* @param len    ��Ϊָ��buf�ж����ֽڵ��������
	* @param buf    �ɵ����߷����ڴ�,�豣֤������len�ֽڳ�
	* @return       ����>0,��ʾ�ɹ���������,����ֵΪ���������ݳ���
	*       	����0,��ʾ�ض�ʱ����û�����ݶ���
	*       	���򷵻�-1
	*/
	int recvfrom(void* buf, size_t len, sockaddress& sa, int msec = 500);
	/**
	* 
	* @param len    ��Ϊָ��buf�ж����ֽڵ��������
	* @param buf    �ɵ����߷����ڴ�,�豣֤������len�ֽڳ�
	* @return       ����>0,��ʾ�ɹ���������,����ֵΪ���͵����ݳ���
	*       	���򷵻�-1
	*/
	int sendto(const void* buf, size_t len, const sockaddress& sa);

	int shutdown(SHUT shut);
	int close();
	/**
	* ���fd�Ŀɶ����
	* @param msec    ��Ϊselect��ʱ��ʱ��(��λΪ��)
	* @return	���󷵻�-1,��ʱ����0,�ɶ�����>0
	*/
	int checkfdread(int msec);
	/**
	* ���fd�Ŀ�д���
	* @param msec    ��Ϊselect��ʱ��ʱ��(��λΪ��)
	* @return	���󷵻�-1,��ʱ����0,�ɶ�����>0
	*/
	int checkfdwrite(int msec);
	int setreuseaddr(bool flag);
	int setnonblock(bool flag);
	int setnodelay(bool flag);
	int getsendbuff(int& size);
	int setsendbuff(int size);
	int getrecvbuff(int& size);
	int setrecvbuff(int size);
	int getpeeraddr(sockaddress& sa) const;
	int getsockaddr(sockaddress& sa) const;

	friend std::ostream& operator << (std::ostream& os, const Socket& sock);

	SOCKET gethandle() const;

private:
	void invalidself();
	int setsockbuff();
	SOCKET	sock_;
#ifdef TARGET_OS_IPHONE
	CFReadStreamRef readStream;
	CFWriteStreamRef writeStream;
    Mutex            mutex;
#endif
    
};


#endif	/*SOCKET_H*/

