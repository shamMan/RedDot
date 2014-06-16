/****************************************************
 *  DataStruct.h
 *  Created on: 11-04-2007 9:27:12
 *  Implementation of the Class ConfigureData
 *  Original author: Eric Gao
 *  Modified by: Yitang Cheng
 ****************************************************/

#ifndef	DATASTRUCT_H
#define DATASTRUCT_H

#include <string>
#include <sstream>
#include "Trace.h"

#define VIDINOW			"VIDINOW"


enum ConnectionType {
	MINCONNECTIONTYPE,
	MINIGK,
	H225,
	H245,
	T120,
	AUDIO,
	DATA,
	CHAT,
	VIDEO,
	UNKNOWN,
	MAXCONNECTIONTYPE
};

enum BindType {
	MINBINDTYPE,
	TCP2TCP2TCP,
	UDP2UDP2UDP,
	UDP2TCP2UDP,
	MAXBINDTYPE
};

enum ProxyType {
	MINPROXYTYPE,
	HTTPTUNNEL,
	HTTPPROXY,
	SOCK4PROXY,
	SOCK5PROXY,
	NOPROXY,
	MAXPROXYTYPE
};

struct sockaddress
{
	sockaddress();
	sockaddress(unsigned short port, const std::string& ipstr);
	sockaddress(unsigned short port, unsigned int ip);
	sockaddress(unsigned short port, const char* ipstr = 0);
	const char* getipstr() const;

	bool operator <(const sockaddress that) const;
	bool operator ==(const sockaddress that) const;
	friend std::ostream& operator << (std::ostream& os, const sockaddress& addr);

	unsigned int ip_;
	unsigned short	port_;
private:
	unsigned int ipstr2num(const char* ipstr);
};

struct QMapID
{
	bool operator ==(const QMapID that) const;
	bool operator <(const QMapID that) const;
	sockaddress	peerAddr_;
};

#ifdef	TC
struct ProxyInfo
{
	ProxyInfo()
		:type_(NOPROXY)
	{}

	ProxyType	type_;
	sockaddress	addr_;
	std::string	user_;
	std::string	passwd_;
};
#else
struct WebInfo
{
	sockaddress	httpAddr_;
	sockaddress	httpsAddr_;
};
#endif

struct BindInfo
{
	BindType	type_;
	sockaddress	addr_;
};

struct ChannelStat 
{
	ChannelStat();

	size_t	in_;
	size_t	out_;
};

#ifdef WIN32
#define PACK_ATTRIBUTE
#pragma pack(push)
#pragma pack(1)
#else
#define PACK_ATTRIBUTE __attribute__((packed))
#endif

struct udp_header_t
{
	char	headid[8];
	unsigned int	destip;
	unsigned short	destport;
}
PACK_ATTRIBUTE;

#ifdef WIN32
#pragma pack(pop)
#endif

#endif /*DATASTRUCT_H*/

