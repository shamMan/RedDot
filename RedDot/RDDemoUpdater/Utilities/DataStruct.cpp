/****************************************************
 *  DataStruct.c
 *  Created on: 11-04-2007 9:27:12
 *  Original author: Eric Gao
 *  Modified by: Yitang Cheng
 ****************************************************/

#ifdef	WIN32
#include <Winsock.h>
#else
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include <sstream>
#include "Trace.h"
#include "DataStruct.h"


sockaddress::sockaddress()
	: ip_(0), port_(0)
{
}

sockaddress::sockaddress(unsigned short port, const std::string& ipstr)
	: port_(port)
{
	ip_ = ipstr2num(ipstr.c_str());
}

sockaddress::sockaddress(unsigned short port, unsigned int ip)
	: ip_(ip), port_(port)
{
}

sockaddress::sockaddress(unsigned short port, const char* ipstr)
	: port_(port)
{
	ip_ = ipstr2num(ipstr);
}

unsigned int sockaddress::ipstr2num(const char* ipstr)
{
	if (ipstr && ipstr[0] != 0) {
		struct hostent*	pht = gethostbyname(ipstr);
		if (pht) {
			unsigned int ip;
			memcpy(& ip, pht->h_addr_list[0], pht->h_length);
			return ntohl(ip);
		}
	}
	
	return 0;
}

const char* sockaddress::getipstr() const
{
	struct in_addr	addr;
	addr.s_addr = htonl(ip_);
	return inet_ntoa(addr);
}

bool sockaddress::operator <(const sockaddress that) const
{
	return ip_ < that.ip_
		|| (ip_ == that.ip_ && port_ < that.port_);
}

bool sockaddress::operator ==(const sockaddress that) const
{
	return ip_ == that.ip_ && port_ == that.port_;
}


std::ostream& operator << (std::ostream& os, const sockaddress& addr)
{
	os << addr.getipstr() << ":" << addr.port_;

	return os;
}


bool QMapID::operator <(const QMapID that) const
{
	return peerAddr_ < that.peerAddr_;
}


bool QMapID::operator ==(const QMapID that) const
{
	return peerAddr_ == that.peerAddr_;
}

ChannelStat::ChannelStat()
	: in_(0), out_(0)
{

}
