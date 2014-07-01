#ifndef TEST_GPS_COM_READ_THREAD_H_
#define TEST_GPS_COM_READ_THREAD_H_

//#include "../../SRC_Communication/System/Thread.h"
#include <windows.h>
#include "GPS_Info.h"
#include <vector>
#include <set>

using namespace std;

namespace IS_GIS
{
class GPS;
class ComReadThread : public IS_GIS::Thread
{
	struct GPS_Sentence
	{
		vector<string> m_words;
		size_t wordCount() const
		{	return m_words.size();	}
		string & operator[](size_t index)
		{	return m_words[index];	}
		void clear()
		{	m_words.clear();	}
	};

	HANDLE m_com_hdl;
	bool m_exit_tag;
	GPS_Info m_info;

	//loujt,09-04-10
	bool m_overwrite_saInfo;
	set<int> m_valid_sa_number;
	Satellites_Info sa_info;

	GPS *m_parent_gps;

	bool isMarkedStop();
	//return the length of last sentence
	int parse(char *buf, int sz);
private:
	int parse(char *buf, int sz, vector<GPS_Sentence> *result);
	int readSentence(char *buf, int sz, GPS_Sentence *result);
	void updateGPSInfo(GPS_Sentence &sentence);
	void updateGPGLL(GPS_Sentence &sentence);
	void updateGPRMC(GPS_Sentence &sentence);
	void updateGPGSV(GPS_Sentence &sentence);

	double str2Lat(const string &word);
	double str2Lon(const string &word);
	double str2Speed(const string &word);
	double str2Heading(const string &word);
	int  str2Number(const string &word);
	GPS_Info::UTC_Time str2Time(const string &word);
	GPS_Info::UTC_Date str2Date(const string &word);
public:
	ComReadThread();
	void setWriteTarget(GPS *gps)
	{	m_parent_gps = gps;	}
	void setCOMPort(HANDLE com_hdl)
	{	m_com_hdl = com_hdl;	}
	void markStop();
	int run();
};
}
#endif //TEST_GPS_COM_READ_THREAD_H_