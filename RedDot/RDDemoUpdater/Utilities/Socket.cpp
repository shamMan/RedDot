///////////////////////////////////////////////////////////
//  Socket.cpp
//  Implementation of the Class Socket
//  Created on:      09-04-2007 10:55:11
//  Original author: Yitang Cheng
// �޸�ԭ��iOS��̨socket���ܷ�����
// �����������ios�Դ���CFStreamCreatePairWithSocket����
///////////////////////////////////////////////////////////

#include <stdio.h>
#include <errno.h>
#ifdef WIN32
#define   IOC_VENDOR   0x18000000
#define   _WSAIOW(x,y)   (IOC_IN|(x)|(y))
#define   SIO_UDP_CONNRESET   _WSAIOW(IOC_VENDOR,12)
#define ex_errno WSAGetLastError()
#else
#include <netdb.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#define ex_errno errno
#endif
#include "Trace.h"
#include "Socket.h"
#include "Utils.h"

#ifdef WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

void initNetwork()
{
#ifdef WIN32
	unsigned short	wVersionRequested = MAKEWORD(2, 2);
	WSADATA	wsaData;
	int	err;

	err = WSAStartup(wVersionRequested, & wsaData);
	if (err != 0) {
		assert("WSAStartup() error!");
		exit(-1);
	}
#endif
}

Socket::Socket()
{
#ifdef TARGET_OS_IPHONE
    readStream = NULL;
    writeStream = NULL;
#endif
	invalidself();
}

Socket::Socket(SOCKET fd)
	: sock_(fd)
{
#ifdef TARGET_OS_IPHONE
    readStream = NULL;
    writeStream = NULL;
#endif
}

Socket::~Socket()
{
//#ifdef TARGET_OS_IPHONE
//    if (readStream != NULL)
//    {
//        CFReadStreamClose(readStream);
//        CFRelease(readStream);
//        readStream = NULL;
//    }
//    if (writeStream != NULL)
//    {
//        CFWriteStreamClose(writeStream);
//        CFRelease(writeStream);
//        writeStream = NULL;
//    }
//#endif
}


int Socket::tcpconnect(unsigned short port, unsigned int ip)
{
	struct sockaddr_in	addr;

	if ((sock_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		NLERROR("Socket(SOCK_STREAM)");
		invalidself();
		return -1;
	}

	if (0 != setnonblock(true)) {
		NLERROR("setnonblock(true) error");
		close();
		return -1;
	}

	if (0 != setsockbuff()) {
		NLERROR("setsockbuff() error");
		close();
		return -1;
	}

	memset((char *) & addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons(port);
	if (0 != connect(sock_,
			(struct sockaddr *) & addr,
			sizeof(struct sockaddr))) {
#ifdef	WIN32
		if (WSAEWOULDBLOCK != GetLastError()) {
#else
		if (EINPROGRESS != errno) {
#endif
			NLERROR("connect(destip = " << ip
				<< ", destport = " << port << ")");
			close();
			return -1;
		}
	}

	if (checkfdwrite(5000) <= 0) {
		NLERROR("connect(" << ip << ":" << port << "): timeout or error");
		close();
		return -1;
	}
	
	int	valopt;
	socklen_t	lon = sizeof(int); 
	if (getsockopt(sock_, SOL_SOCKET, SO_ERROR, (char*)(&valopt), &lon) < 0) { 
		NLERROR("getsockopt()");
		close();
		return -1;
	} 
	// Check the value returned... 
	if (valopt) { 
		NLERROR("getsockopt(SO_ERROR): valopt = " << valopt);
		close();
		return -1;
	} 

	if (0 != setnonblock(false)) {
		NLERROR("setnonblock(false)");
		close();
		return -1;
	}

	if (0 != setnodelay(true)) {
		close();
		return -1;
	}
#ifdef TARGET_OS_IPHONE
        if (kCFCoreFoundationVersionNumber > kCFCoreFoundationVersionNumber10_6) { /*I.E is >=OS4*/
            
            if (readStream != NULL)
            {
                CFReadStreamClose(readStream);
                CFRelease(readStream);
                readStream = NULL;
            }
            if (writeStream != NULL)
            {
                CFWriteStreamClose(writeStream);
                CFRelease(writeStream);
                writeStream = NULL;
            }
            CFStreamCreatePairWithSocket(kCFAllocatorDefault, sock_,
                                         &readStream, &writeStream);
            if (readStream != NULL)
                CFReadStreamSetProperty(readStream, kCFStreamNetworkServiceType, kCFStreamNetworkServiceTypeVoIP);
            if (writeStream != NULL)
                CFWriteStreamSetProperty(writeStream, kCFStreamNetworkServiceType, kCFStreamNetworkServiceTypeVoIP);
            if (CFReadStreamOpen (readStream)) {
                printf("tcpconnect CFReadStreamOpen success\n");
            }
            if (CFWriteStreamOpen (writeStream)) {
                printf("tcpconnect CFWriteStreamOpen success\n");
            }

        }
#endif

	return 0;
}


int	Socket::tcplisten(unsigned short port, unsigned int ip)
{
	struct sockaddr_in	addr;

	if ((sock_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		NLERROR("Socket(SOCK_STREAM)");
		invalidself();
		return -1;
	}

	int	reuse = 1;

	if (setsockopt(sock_, SOL_SOCKET,
		SO_REUSEADDR,
		(char *) & reuse,
		sizeof(reuse)) < 0) {
		NLERROR("setsockopt(sock = " << sock_ << ", SO_REUSEADDR)");
		close();
		return -1;
	}

	if (0 != setsockbuff()) {
		NLERROR("setsockbuff() error");
		close();
		return -1;
	}

	memset((char *) & addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(0 == ip ? INADDR_ANY : ip);
	addr.sin_port = htons(port);

	if (0 != bind(sock_,
			(struct sockaddr *) & addr,
			sizeof(struct sockaddr))) {
		NLERROR("bind(sock = " << sock_ << ", port = " << port << ")");
		close();
		return -1;
	}
	else
	if (0 != listen(sock_, 5)) {
		NLERROR("listen(sock = " << sock_ << ")");
		close();
		return -1;
	}

	if (0 != setnodelay(true)) {
		close();
		return -1;
	}

	return 0;
}


/**
* 
* @return	
*/
int	Socket::accept(Socket&	sock, sockaddress* psa, int msec)
{

	struct sockaddr_in addr;
	socklen_t	fromlen = sizeof(addr);
	int	ret = -1;

	ret = checkfdread(msec);
	if (-1 == ret) {
		NLERROR("accept(sock = " << sock_ << ") checkfdread()");
		return -1;
	}
	else {
		if (0 == ret) {
			//timeout
			NLTRACE("accept(sock = " << sock_ << ") timeout(=" << msec << ")!");
			return 0;
		}
	}

	ret = -1;
	if (INVALID_SOCKET
		!= (sock.sock_ = ::accept(sock_,
					(struct sockaddr *) & addr,
					& fromlen))) {
        
#ifdef TARGET_OS_IPHONE
        if (kCFCoreFoundationVersionNumber > kCFCoreFoundationVersionNumber10_6) { /*I.E is >=OS4*/
            
            if (sock.readStream != NULL)
            {
                CFReadStreamClose(sock.readStream);
                CFRelease(sock.readStream);
                sock.readStream = NULL;
            }
            if (sock.writeStream != NULL)
            {
                CFWriteStreamClose(sock.writeStream);
                CFRelease(sock.writeStream);
                sock.writeStream = NULL;
            }
            CFStreamCreatePairWithSocket(kCFAllocatorDefault, sock.sock_,
                                         &sock.readStream, &sock.writeStream);
            if (sock.readStream != NULL)
                CFReadStreamSetProperty(sock.readStream, kCFStreamNetworkServiceType, kCFStreamNetworkServiceTypeVoIP);
            if (sock.writeStream != NULL)
                CFWriteStreamSetProperty(sock.writeStream, kCFStreamNetworkServiceType, kCFStreamNetworkServiceTypeVoIP);
            if (CFReadStreamOpen (sock.readStream)) {
                printf("accept CFReadStreamOpen success\n");
            }
            if (CFWriteStreamOpen (sock.writeStream)) {
                printf("accept CFWriteStreamOpen success\n");
            }
            
        }
#endif
        
        
		if (psa) {
			psa->ip_ = ntohl(addr.sin_addr.s_addr);
			psa->port_ = ntohs(addr.sin_port);
		}
		NLDEBUG("accept(sock = " << sock_ << ")");
		ret = 1;
	}
#ifndef WIN32
	else
    {
        if (EINTR == errno || EPIPE == errno) {
            NLDEBUG("accept(sock = " << sock_ << ") catch a signal.");
            return 0;
        }
        printf("accept error:%d\n",errno);
    }
#endif

	return ret;
}


/**
* ¼ì²éfdµÄ¿É¶ÁÇé¿ö
* @param msec    ×÷Îªselect³¬Ê±µÄÊ±¼ä(µ¥Î»ÎªÃë)
* @return	´íÎó·µ»Ø-1,³¬Ê±·µ»Ø0,¿É¶Á·µ»Ø>0
*/
int Socket::checkfdread(int msec)
{
	fd_set	rfdset;
	struct timeval	timeout;
	SOCKET	sock = sock_;
	int	ret = -1;

	NLTRACE("checkfdread(" << sock << ", msec = " << msec << ")");
	if (INVALID_SOCKET == sock) {
		return ret;
	}

	while (1) {
	FD_ZERO(& rfdset);
	FD_SET(sock, & rfdset);
	timeout.tv_sec = msec / 1000;
	timeout.tv_usec = (msec % 1000) * 1000;

#ifdef	WIN32
	ret = select(0, & rfdset, 0, 0, & timeout);
#else
	ret = select(sock + 1, & rfdset, 0, 0, & timeout);
		if (-1 == ret && (EINTR == errno || EPIPE == errno)) {
			NLTRACE("SKIP: select(sock = " << sock_ << ")");
			continue;
	}
#endif
		break;
	}

	return ret;
}

/**
* ¼ì²éfdµÄ¿ÉÐ´Çé¿ö
* @param msec    ×÷Îªselect³¬Ê±µÄÊ±¼ä(µ¥Î»ÎªÃë)
* @return	´íÎó·µ»Ø-1,³¬Ê±·µ»Ø0,¿É¶Á·µ»Ø>0
*/
int Socket::checkfdwrite(int msec)
{
	fd_set	wfdset;
	struct timeval	timeout;
	SOCKET	sock = sock_;

	NLTRACE("checkfdwrite(" << sock << ", msec = " << msec << ")");
	if (INVALID_SOCKET == sock) {
		return -1;
	}

	FD_ZERO(& wfdset);
	FD_SET(sock, & wfdset);
	timeout.tv_sec = msec / 1000;
	timeout.tv_usec = (msec % 1000) * 1000;

	int	ret = 0;
#ifdef	WIN32
	ret = select(0, 0, & wfdset, 0, & timeout);
#else
	ret = select(sock + 1, 0, & wfdset, 0, & timeout);
#endif
	return ret;
}

/**
 * Read len bytes from Socket into the buffer starting at buf.
 *
 * @param len    the number of bytes want to read
 * @param buf    points to a buffer where the message should be stored
 * @return       Read len bytes message, readn() should return len;
 *               If timeout, then return 0;
 *      	 Otherwise, -1 shall be returned.
 */
int Socket::readn(void* buf, size_t len, int msec)
{
	int	nleft = (int)len;
	int	nlen, ret;
	struct timeval	btime, etime;
	
	gettimeofday(&btime, 0);
	while (nleft > 0) {
		ret = checkfdread(100);
		if (-1 == ret) {
			NLERROR("readn(sock = " << sock_ << ") checkfdread()");
			break;
		}
		else {
			if (0 == ret) {
				//timeout
				gettimeofday(&etime, 0);
				if (difftime(btime, etime) * 1000 > msec) {
					NLDEBUG("readn(sock = " << sock_ << ") timeout(=" << msec << ")!");
					return 0;
				}
				NLTRACE("difftime() = " << difftime(btime, etime));

				continue;
			}
		}

		nlen = recv(sock_, (char*)buf + (len - nleft), nleft, 0);
		if (0 == nlen) {
			NLERROR("peer(" << sock_ << ") is closed.");
			return -1;
		}
		else {
			if (nlen < 0) {
				NLERROR("recv(sock = " << sock_ << ")");
#ifndef WIN32
				if (EINTR == errno || EPIPE == errno) {
					continue;
				}
#endif
				break;
			}
		}
		nleft -= nlen;
	}

	if (0 == nleft) {
		return (int)len;
	}

	return -1;
}

/**
 * Read up to len bytes from Socket into the buffer starting at buf.
 *
 * @param len    the number of bytes want to read
 * @param buf    points to a buffer where the message should be stored
 * @return       Read len bytes message, readn() should return len;
 *               If no data are available to be read, then return 0;
 *      	 Otherwise, -1 shall be returned.
 */
int Socket::read(void* buf, size_t len, int msec)
{
	int	nlen, ret;

	NLTRACE("read(sock = " << sock_ << ")");
	ret = checkfdread(msec);
	if (-1 == ret) {
		NLERROR("read(sock = " << sock_ << ") checkfdread()");
		return -1;
	}
	else {
		if (0 == ret) {
			//timeout
			NLTRACE("read(sock = " << sock_ << ") timeout(=" << msec << ")!");
			return 0;
		}
	}

	nlen = recv(sock_, (char*)buf, (int)len, 0);
	if (0 == nlen) {
		NLERROR("recv(sock = " << sock_ << "): ret = " << nlen << ", is closed.");
		return -1;
	}
	else {
		if (nlen < 0) {
			NLERROR("recv(sock = " << sock_ << ")");
#ifndef WIN32
			if (EINTR == errno || EPIPE == errno)
				nlen = 0;
#endif
		}
	}

	return nlen;
}

int Socket::peek(void* buf, size_t len)
{
	int	nlen;

	nlen = recv(sock_, (char*)buf, (int)len, MSG_PEEK);
	if (0 == nlen) {
		NLERROR("recv(sock = " << sock_ << "): ret = " << nlen << ", is closed.");
		return -1;
	}
	else {
		if (nlen < 0) {
			NLERROR("recv(sock = " << sock_ << ")");
#ifndef WIN32
			if (EINTR == errno || EPIPE == errno)
				nlen = 0;
#endif
		}
	}
	NLTRACE("peek(sock = " << sock_ << "): return " << nlen);

	return nlen;
}


/**
*
* @param len    ×÷ÎªÖ¸¶¨Ð´¶à³¤µÄÊäÈë²ÎÊý
* @param buf    ÓÉµ÷ÓÃÕß·ÖÅäÄÚ´æ,Ðè±£Ö¤ÖÁÉÙÓÐlen×Ö½Ú³¤
* @return       ·µ»Ø>0,´ËÊ±·µ»ØÖµÒ»¶¨ÎªÊäÈëµÄlen,±íÊ¾³É¹¦Ð´len³¤¶ÈÊý¾Ý
*       	·ñÔò·µ»Ø-1,±íÊ¾ÌØ¶¨Ê±¼äÄÚÃ»ÓÐ³É¹¦Ð´³ölen³¤¶ÈµÄÊý¾Ý
*/
int Socket::writen(const void* buf, size_t len, int msec)
{
	int	nleft = (int)len;
	int	nlen = 0, ret;
	struct timeval	btime, etime;

	gettimeofday(&btime, 0);
	while (nleft > 0) {
		ret = checkfdwrite(100);
		if (-1 == ret) {
			NLERROR(
				"writen(sock = "
				<< sock_
				<< ") checkfdread()");
			break;
		}
		else {
			if (0 == ret) {
				//timeout
				gettimeofday(&etime, 0);
				if (difftime(btime, etime) * 1000 > msec) {
					NLDEBUG("writen(sock = " << sock_ << ") timeout(=" << msec << ")!");
					break;
				}
				NLTRACE("difftime() = " << difftime(btime, etime));
			}
		}

		nlen = send(sock_, (char*)buf + (len - nleft), nleft, 0);
		if (nlen < 0) {
#ifndef WIN32
			if (EINTR == errno || EPIPE == errno) {
				NLERROR("send(sock = " << sock_ << ")");
				continue;
			}
#endif
			break;
		}
		nleft -= nlen;
	}

	if (0 == nleft) {
		return (int)len;
	}

	return -1;
}


int Socket::udplisten(unsigned short port, unsigned int ip)
{
	struct sockaddr_in	addr;

	if ((sock_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		NLERROR("Socket(SOCK_DGRAM)");
		return -1;
	}

	memset((char *) & addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(0 == ip ? INADDR_ANY : ip);
	addr.sin_port = htons(port);

	if (0 != bind(sock_,
			(struct sockaddr *) & addr,
			sizeof(struct sockaddr))) {
		NLERROR("bind(sock = " << sock_ << ", port = " << port << ")");
		close();
		return -1;
	}

#ifdef WIN32
	DWORD	dwBytesReturned = 0;
	DWORD	bNewBehavior = FALSE;
	DWORD	status;

	status = ioctlsocket(sock_, SIO_UDP_CONNRESET,
		&bNewBehavior);
#endif

	if (0 != setsockbuff()) {
		NLERROR("setsockbuff() error");
		close();
		return -1;
	}
    
	return 0;
}

/**
*
* @param len    ×÷ÎªÖ¸¶¨bufÓÐ¶àÉÙ×Ö½ÚµÄÊäÈë²ÎÊý
* @param buf    ÓÉµ÷ÓÃÕß·ÖÅäÄÚ´æ,Ðè±£Ö¤ÖÁÉÙÓÐlen×Ö½Ú³¤
* @return       ·µ»Ø>0,±íÊ¾³É¹¦¶Áµ½Êý¾Ý,·µ»ØÖµÎª¶Áµ½µÄÊý¾Ý³¤¶È
*       	·µ»Ø0,±íÊ¾ÌØ¶¨Ê±¼äÄÚÃ»ÓÐÊý¾Ý¶Áµ½
*       	·ñÔò·µ»Ø-1
*/
int Socket::recvfrom(void* buf, size_t len, sockaddress& sa, int msec)
{
	struct sockaddr_in	addr;
	socklen_t	fromlen = sizeof(addr);
	int	ret = -1, nlen = -1;

	ret = checkfdread(msec);
	if (-1 == ret) {
		NLERROR("recvfrom(sock = " << sock_ << ") checkfdread()");
		return -1;
	}
	else {
		if (0 == ret) {
			//timeout
			NLTRACE("recvfrom(sock = " << sock_ << ") timeout(=" << msec << ")!");
			return 0;
		}
	}
	if ((nlen = ::recvfrom(sock_,
			(char*)buf,
			(int)len,
			0,
			(struct sockaddr *) & addr,
			& fromlen))
		>= 0) {
		NLTRACE("recvfrom(sock = " << sock_ << "): len =" << nlen);
		sa.ip_ = ntohl(addr.sin_addr.s_addr);
		sa.port_ = ntohs(addr.sin_port);
		NLDEBUG("recvfrom(sock = " << sock_ << "): len = " << nlen << ", " << sa);
	}
	else {
#ifdef	WIN32
		if (WSAECONNRESET == GetLastError())
#else
		if (EINTR == errno || EPIPE == errno)
#endif
		{
			NLERROR("SKIP: recvfrom(sock = " << sock_ << ")");
			nlen = 0;
		}
		else {
			NLERROR("recvfrom(sock = " << sock_ << ")");
			nlen = -1;
		}
	}

	return nlen;
}


/**
*
* @param len    ×÷ÎªÖ¸¶¨bufÓÐ¶àÉÙ×Ö½ÚµÄÊäÈë²ÎÊý
* @param buf    ÓÉµ÷ÓÃÕß·ÖÅäÄÚ´æ,Ðè±£Ö¤ÖÁÉÙÓÐlen×Ö½Ú³¤
* @return       ·µ»Ø>0,±íÊ¾³É¹¦·¢ËÍÊý¾Ý,·µ»ØÖµÎª·¢ËÍµÄÊý¾Ý³¤¶È
*       	·ñÔò·µ»Ø-1
*/
int Socket::sendto(const void* buf, size_t len, const sockaddress& sa)
{
	struct sockaddr_in	addr;
	socklen_t	fromlen = sizeof(addr);
	int	ret = -1;

	addr.sin_family = PF_INET;
	addr.sin_addr.s_addr = htonl(sa.ip_);
	addr.sin_port = htons(sa.port_);
	if ((ret = ::sendto(sock_,
			(const char*)buf,
			(int)len,
			0,
			(struct sockaddr *) & addr,
			fromlen))
		>= 0) {
		NLTRACE("sendto(sock = " << sock_ << "): len =" << len);
	}
	else {
#ifdef	WIN32
		if (WSAECONNRESET == GetLastError() || WSAEHOSTUNREACH == GetLastError())
#else
		if (EINTR == errno || EPIPE == errno)
#endif
		{
			NLERROR("SKIP: sendto(sock = " << sock_ << ")");
			ret = 0;
		}
		else {
			NLERROR("sendto(sock = " << sock_ << ")");
			ret = -1;
		}
	}
	NLDEBUG("sendto(sock = " << sock_ << "): len = " << len << ", " << sa);

	return ret;
}

int Socket::setreuseaddr(bool flag)
{
	int	reuse = 1;

	if (setsockopt(sock_, SOL_SOCKET,
		SO_REUSEADDR,
		(char *) & reuse,
		sizeof(reuse)) < 0) {
		NLERROR("setsockopt(sock = " << sock_ << ", SO_REUSEADDR)");
		return -1;
	}

	return 0;
}

int Socket::setnonblock(bool flag)
{
	int	val = flag ? 1 : 0;

#ifdef WIN32
	if (0 != ioctlsocket(sock_, FIONBIO, (u_long *) & val)) {
		return -1;
	}
#else
	if (ioctl(sock_, FIONBIO, & val) < 0) {
		return -1;
	}
#endif

	return 0;
}


int Socket::setnodelay(bool flag)
{
	int	val = flag ? 1 : 0;

	if (setsockopt(sock_, IPPROTO_TCP, TCP_NODELAY, (const char*)&val, sizeof(val)) < 0) {
		NLERROR("setsockopt(sock = " << sock_ << ", TCP_NODELAY)");
		return -1;
	}

	return 0;
}

int Socket::getsendbuff(int& size)
{
	int	optlen = sizeof(int);
	int&	optval = size;
#ifdef WIN32
	if (getsockopt(sock_, SOL_SOCKET, SO_SNDBUF, (char*)&optval, &optlen) < 0) {
#else
        if (getsockopt(sock_, SOL_SOCKET, SO_SNDBUF, (char*)&optval, (socklen_t*)&optlen) < 0) {
#endif
		NLERROR("getsockopt(sock = " << sock_ << ", sendbuf = " << size << ", SO_SNDBUF)");
		return -1;
	}

	return 0;
}

int Socket::setsendbuff(int size)
{
	int	optlen = sizeof(int);
	int	optval = size;

#ifdef WIN32
	if (setsockopt(sock_, SOL_SOCKET, SO_SNDBUF, (const char*)&optval, optlen) < 0) {
#else
        if (setsockopt(sock_, SOL_SOCKET, SO_SNDBUF, (const char*)&optval, (socklen_t)optlen) < 0) {
#endif
		NLERROR("setsockopt(sock = " << sock_ << ", sendbuf = " << size << ", SO_SNDBUF)");
		return -1;
	}

	return 0;
}

int Socket::getrecvbuff(int& size)
{
	int	optlen = sizeof(int);
	int&	optval = size;
#ifdef WIN32
	if (getsockopt(sock_, SOL_SOCKET, SO_RCVBUF, (char*)&optval, &optlen) < 0) {
#else
        if (getsockopt(sock_, SOL_SOCKET, SO_RCVBUF, (char*)&optval, (socklen_t*)&optlen) < 0) {
#endif
		NLERROR("getsockopt(sock = " << sock_ << ", sendbuf = " << size << ", SO_RCVBUF)");
		return -1;
	}

	return 0;
}

int Socket::setrecvbuff(int size)
{
	int	optlen = sizeof(int);
	int	optval = size;

	if (setsockopt(sock_, SOL_SOCKET, SO_RCVBUF, (const char*)&optval, optlen) < 0) {
		NLERROR("setsockopt(sock = " << sock_ << ", sendbuf = " << size << ", SO_RCVBUF)");
		return -1;
	}

	return 0;
}

int Socket::shutdown(SHUT shut)
{
	return ::shutdown(sock_, shut);
}

int Socket::close()
{
	if (INVALID_SOCKET != sock_) {
		NLDEBUG("close(sock = " << sock_ << ").");
		::closesocket(sock_);
        
#ifdef TARGET_OS_IPHONE
        // 需要加锁，其他线程也会使用
        LockHelper<Mutex> lock(mutex);
        if (readStream != NULL)
        {
            CFReadStreamClose(readStream);
            CFRelease(readStream);
            readStream = NULL;
        }
        if (writeStream != NULL)
        {
            CFWriteStreamClose(writeStream);
            CFRelease(writeStream);
            writeStream = NULL;
        }
#endif
		invalidself();
	}

	return 0;
}

void Socket::invalidself()
{
	sock_ = INVALID_SOCKET;
}

int Socket::setsockbuff()
{
	 //TODO: Õâ¸ö²ÎÊýÐèÒª¸ù¾ÝÊµ¼Ê×´¿öµ÷Õû,windows 128K, android, 32K
	int sndbuff = 32 * 1024;
	if (0 != setsendbuff(sndbuff)) {
		NLERROR("setsendbuff(" << sndbuff << ") error");
		return -1;
	}

	int rcvbuff = 32 * 1024;
	if (0 != setrecvbuff(rcvbuff)) {
		NLERROR("setrecvbuff(" << rcvbuff << ") error");
		return -1;
	}

	return 0;
}

int Socket::getpeeraddr(sockaddress& sa) const
{
	struct sockaddr_in	addr;
	socklen_t	len = sizeof(addr);
	int	ret = -1;

	if (0 == (ret = getpeername(sock_,
				(struct sockaddr *) & addr,
				& len))) {
		sa.ip_ = ntohl(addr.sin_addr.s_addr);
		sa.port_ = ntohs(addr.sin_port);
	}

	return ret;
}


int Socket::getsockaddr(sockaddress& sa) const
{
	struct sockaddr_in	addr;
	socklen_t	len = sizeof(addr);
	int	ret = -1;

	if (0 == (ret = getsockname(sock_,
				(struct sockaddr *) & addr,
				& len))) {
		sa.ip_ = ntohl(addr.sin_addr.s_addr);
		sa.port_ = ntohs(addr.sin_port);
	}

	return ret;
}

SOCKET Socket::gethandle() const
{
	return sock_;
}

std::ostream& operator << (std::ostream& os, const Socket& sock)
{
	os << "[sock = " << sock.gethandle() << "]";

	return os;
}

#define SOCKET_TIMEOUT 50

int Socket::tcpisconnected()
{
	int res;
	struct timeval tv;
	fd_set wrset;
	int valopt;
	socklen_t sock_len;
	if (sock_ == INVALID_SOCKET)
		return -1;

	tv.tv_sec = SOCKET_TIMEOUT / 1000;
	tv.tv_usec = (SOCKET_TIMEOUT % 1000) * 1000;

	FD_ZERO(&wrset);
	FD_SET(sock_, &wrset);

	res = select((int)sock_ + 1, NULL, &wrset, NULL, &tv);
	if (res > 0) {
		sock_len = sizeof(int);
		if (getsockopt(sock_, SOL_SOCKET, SO_ERROR, (char *) (&valopt), &sock_len) == 0) {
				if (valopt) {
					NLERROR("Cannot connect socket node / " << strerror(ex_errno) << "[" << ex_errno <<"]\n");
					return -1;
				} else {
					return 0;
				}
		} else {
			NLERROR("Cannot connect socket node / error in getsockopt " << strerror(ex_errno) << "[" << ex_errno <<"]\n");
			return -1;
		}
	} else if (res < 0) {
		NLERROR("Cannot connect socket node / error in select " << strerror(ex_errno) << "[" << ex_errno <<"]\n");
		return -1;
	} else {
		NLERROR("Cannot connect socket node / select timeout (" <<  SOCKET_TIMEOUT << "ms)\n");
		return 1;
	}
}
