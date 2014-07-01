//#include "stdafx.h"
#include "CommReadThread.h"
#include "GPS.h"
#include "../Common/Util/ErrorInfo.h"
#include <time.h>
//#include <fstream>
#include <algorithm>
using namespace GIS_Embedded;


namespace IS_GIS
{

ComReadThread::ComReadThread()
{	
	m_com_hdl = NULL;
	m_exit_tag = false;
	m_parent_gps = NULL;

	m_overwrite_saInfo = false;
}

bool ComReadThread::isMarkedStop()
{
	bool ret;
	lock();
		ret = m_exit_tag;
	unlock();
	return ret;
}

void ComReadThread::markStop()
{
	lock();
		m_exit_tag = true;
	unlock();
}

int ComReadThread::run()
{
	char buffer[5100];//loujt,09-03-05,buffer[4100],assert last_sentenct_l<1000
	int last_sentence_l = 0;
	unsigned long readsum;

	// 读取数据。
	OVERLAPPED o;
	DWORD dwEvtMask;
	// Create an event object for use in WaitCommEvent. 

	if(!m_parent_gps)
	{
		assert(0);
		return -1;
	}

	o.Offset = o.OffsetHigh = 0;
	o.hEvent = CreateEvent(
		NULL,   // no security attributes 
		TRUE,  // auto reset event 
		FALSE,  // not signaled 
		NULL    // no name 
		);

	assert(o.hEvent);
	
	while( 1 )
	{
		if(isMarkedStop())
		{
			break;
		}

		dwEvtMask = 0;
		BOOL result;
		DWORD dwErrorFlags;
		COMSTAT ComStat;

		result = ::WaitCommEvent(m_com_hdl, &dwEvtMask, NULL);//&o);


		//{
		//	ofstream ofs;
		//	ofs.open(L"/SD/test_log.txt", ios_base::app);

		//	//time_t current_time;
		//	//time(&current_time);
		//	
		//	//ofs << current_time << endl;
		//	ofs << dwEvtMask << endl;

		//	ofs.close();
		//}

		DWORD errornum;
		errornum = ::GetLastError();
		::ClearCommError(m_com_hdl, &dwErrorFlags, &ComStat); 		

		if( result == TRUE || errornum == ERROR_IO_PENDING )
		{
			if( (dwEvtMask & EV_RXCHAR) == EV_RXCHAR && ComStat.cbInQue > 0 )
			{
				readsum = 4096;
				result = ::ReadFile(m_com_hdl,buffer+last_sentence_l,readsum,&readsum, /*&o*/NULL );
				readsum += last_sentence_l;
				//if ( !result && GetLastError() == ERROR_IO_PENDING )
				//	result = GetOverlappedResult( m_com_hdl, &o, &readsum, TRUE );

		/*		{
					wchar_t buf[100];
					wsprintf(buf, L"readsome %d", readsum);
					::MessageBox(NULL, buf, L"", MB_OK);
				}*/

				if( result == TRUE && readsum > 0 && readsum <= 4096+last_sentence_l)
				{
					buffer[readsum] = '\0';

					//{
					//	ofstream ofs2;
					//	ofs2.open(L"test_log.txt",ios_base::app);//StorageCard
					//	ofs2<<endl<<endl<<"before handle"<<endl;
					//	ofs2<< buffer+last_sentence_l << endl;
					//	ofs2<<endl<<endl<<"after handle"<<endl;
					//	ofs2<< buffer << endl;
					//	ofs2.close();
					//}

//					myCtrl->OnRead_COM( readsum, buffer );

					last_sentence_l = parse(buffer, readsum);
					memcpy(buffer,buffer+(readsum-last_sentence_l),last_sentence_l);

				}
				else 
				{
					//ofstream ofs3;
					//ofs3.open(L"/SDMMC/test_log3.txt",ios_base::app);
					//ofs3<< result<< errornum << endl;

					//ofs3.close();
				}
			}
			//else
			//{
			//	//ofstream ofs4;
			//	//	ofs4.open(L"/SD/test_log4.txt",ios_base::app);
			//	//	ofs4<< dwEvtMask << ", " << ComStat.cbInQue<<endl;

			//	//	ofs4.close();
			//}
		}
		// if the COM port is read too fast, sentences will be incomplete.

		Sleep(300);
	}

	::CloseHandle( o.hEvent );
//	::CloseHandle( m_com_hdl );
	
	//::ExitThread(0);

	return 0;
}

//return the length of last sentence
int ComReadThread::parse(char *data, int readsum)
{
/* 数据格式如下，再做进一步处理。
$GPGLL,3021.1274,N,12015.6649,E,123012.125,A*3B
$GPGGA,123012.12,3021.1274,N,12015.6649,E,1,04,2.0,-0007,M,,,,*37
$GPRMB,A,0.00,R,SIM001,SIM002,3022.3652,N,12015.6670,E,001.2,000.,021.7,V*1D
$GPRMC,123012.12,A,3021.1274,N,12015.6649,E,21.7,358.6,240501,05.,W*7B
$GPAPB,A,A,0.0,R,N,,,4.5,M,SIM002,4.6,M,,,*3C
$GPGSA,M,3,01,02,03,04,,,,,,,,,2.0,2.0,2.0*38
$GPGSV,3,1,12,18,83,097,,23,73,120,,21,51,322,,05,45,118,*7A
$GPGSV,3,2,12,09,43,040,,15,34,216,,30,33,159,,14,28,297,*7A
$GPGSV,3,3,12,17,24,194,,29,20,292,,26,07,083,,25,05,232,*7F
//*/
	//vector<GPS_Sentence> v_sent;
	//parse(data, readsum, &v_sent);
	vector<GPS_Sentence> v_sent;
	int last_sentence_p;
	last_sentence_p = parse(data, readsum, &v_sent);

	for(vector<GPS_Sentence>::iterator it = v_sent.begin(); it != v_sent.end(); ++ it)
	{
		updateGPSInfo(*it);
	}
	m_parent_gps->write(m_info);
	
	if(m_overwrite_saInfo)//loujt,完整得更新，包括重置从有到无信号的卫星
		m_parent_gps->write(sa_info);

	if(last_sentence_p < 0) //last_sentence_p could be -1
		return 0;

	return readsum - last_sentence_p; 
/*	char m_buffer[1024];
	m_buffer[0] = '\0';
	long m_bufid = 1;

	long i = 0;
	while( i < readsum )
	{
		if( m_buffer[0] == '$' ) // 读后面的数据。 
		{
			while( i < readsum && data[i] != '$' )
				m_buffer[m_bufid++] = data[i++];
			if( i == readsum )
				return;
			
		//	wchar_t buf[100];
		//	for(int j = 0; j < strlen(m_buffer) + 1; j ++)
		//	{
		//		buf[j] = m_buffer[j];
		//	}
		//	::MessageBox(NULL, buf, L"", MB_OK);

			// 读到'$'，处理数据，暂时只处理GPGLL
			if( strncmp( m_buffer, "$GPGLL", 6 ) == 0 )
			{
				double lon, lat;
				char * p = m_buffer;
				while( *p != ',' )
					p++;
				p++;
				lat = atof( p );
				while( *p != ',' )
					p++;
				p++;
				while( *p != ',' )
					p++;
				p++;
				lon = atof( p );
				//FireReceivePos( lon, lat );
				m_info.latitude = lat;
				m_info.longitude = lon;

				//{
				//	wchar_t buf[100];
				//	wsprintf(buf, L"read: %f, %d", m_info.latitude, m_info.longitude);
				//	::MessageBox(NULL, buf, L"", MB_OK);
				//}
			}

			m_parent_gps->write(m_info);

			m_bufid = 1;
			m_buffer[0] = '$';
			i++;
		}
		else
		{
			// 寻找开头。
			while( i < readsum && data[i] != '$' )
				i++;
			if( i == readsum )
				return;

			m_buffer[0] = '$';
			m_bufid = 1;
			i++;
		}
	}

	m_parent_gps->write(m_info);

	return;
	*/
}

int ComReadThread::parse(char *buf, int sz, std::vector<GPS_Sentence> *result)
{
	int last_sentence_p = -1;  //as the return value(the index of the last '$')
	result->resize(0);
	if(!buf || sz < 1)
		return last_sentence_p;

	char *p = buf;
	while(p - buf < sz)
	{
		if(*p == '$')	// a sentence start here
		{
			last_sentence_p = p-buf;

			GPS_Sentence sentence;
			p += readSentence(p, buf + sz - p, &sentence);
			result->push_back(sentence);
		}
		else
		{
			p ++;
		}
	}
	
	return last_sentence_p;
}

int ComReadThread::readSentence(char *_buf, int sz, GPS_Sentence *result)
{
	assert(*_buf == '$');

	result->m_words.clear();

	char *p = _buf;
	vector<char> buf;
	int ret = 0;
	bool valid = false;

	// copy a sentence string
	do
	{
		buf.push_back(*p);
		p ++;
	}
	while(p - _buf < sz && *p != '$');
	ret = p - _buf;

	// check if a valid sentence
	for(p = &buf[0]; p - &buf[0] < ret; p ++)
	{
		if(*p == '*') // check sum
		{
			p ++;
			if(p - &buf[0] >= ret)
			{
				valid = false;
				break;
			}
			else
			{
				valid = true;
				break;	// without really using the checksum
			}
		}
	}
	if(!valid)
		return ret;

	// seperate to words 
	string word;
	word.clear();

	for(p = &buf[0]; p - &buf[0] < ret; p ++)
	{
		if(*p != ',' && !isspace(*p))
		{
			word.push_back(*p);
		}
		else if(*p == ',')
		{
			result->m_words.push_back(word);
			word.clear();
		}
		else{}
	}
	result->m_words.push_back(word);

	return ret;
}
/*
$GPGLL,3021.1274,N,12015.6649,E,123012.125,A,*3B
$GPGGA,123012.12,3021.1274,N,12015.6649,E,1,04,2.0,-0007,M,,,,*37
$GPRMB,A,0.00,R,SIM001,SIM002,3022.3652,N,12015.6670,E,001.2,000.,021.7,V*1D
$GPRMC,123012.12,A,3021.1274,N,12015.6649,E,21.7,358.6,240501,05.,W*7B
$GPAPB,A,A,0.0,R,N,,,4.5,M,SIM002,4.6,M,,,*3C
$GPGSA,M,3,01,02,03,04,,,,,,,,,2.0,2.0,2.0*38
$GPGSV,3,1,12,18,83,097,,23,73,120,,21,51,322,,05,45,118,*7A
$GPGSV,3,2,12,09,43,040,,15,34,216,,30,33,159,,14,28,297,*7A
$GPGSV,3,3,12,17,24,194,,29,20,292,,26,07,083,,25,05,232,*7F
*/
void ComReadThread::updateGPSInfo(GPS_Sentence &sentence)
{
	if(sentence.m_words.size() < 1)
		return;

	if(sentence[0].compare("$GPGLL") == 0)
	{
		updateGPGLL(sentence);
	}
	if(sentence[0].compare("$GPRMC") == 0)
	{
		updateGPRMC(sentence);
	}
	if(sentence[0].compare("$GPGSV")==0)
	{
		updateGPGSV(sentence);
	}
}

void ComReadThread::updateGPGLL(GPS_Sentence &sentence)
{
	//{
	//	ofstream ofs;
	//	ofs.open(L"/SD/test_log.txt", ios_base::app);

	//	for(size_t i = 0; i < sentence.wordCount(); i ++)
	//		ofs << sentence[i].c_str() << ",";
	//	ofs << endl;
	//	ofs.close();
	//}



	if(sentence.wordCount() < 5) //!= 8)
		return;

	string lat_word = sentence[1];
	string lon_word = sentence[3];

	m_info.latitude = str2Lat(lat_word);
	m_info.longitude = str2Lon(lon_word);
}

void ComReadThread::updateGPRMC(ComReadThread::GPS_Sentence &sentence)
{
	//{
	//	ofstream ofs;
	//	ofs.open(L"/SD/test_log.txt", ios_base::app);

	//	for(size_t i = 0; i < sentence.wordCount(); i ++)
	//		ofs << sentence[i].c_str() << "," ;
	//	ofs << endl;
	//	ofs.close();
	//}

	if(sentence.wordCount() < 10) // != 12
		return;

	string time_word = sentence[1];
	string lat_word = sentence[3];
	string lon_word = sentence[5];
	string speed_word = sentence[7];
	string heading_word = sentence[8];
	string date_word = sentence[9];

	m_info.latitude = str2Lat(lat_word);
	m_info.longitude = str2Lon(lon_word);
	m_info.speed = str2Speed(speed_word);
	m_info.heading = str2Heading(heading_word);

	if(time_word.size() >= 6)
		m_info.utc_time = str2Time(time_word);
	if(date_word.size() >= 6)
		m_info.utc_date = str2Date(date_word);
}
//
//$GPGSV，(1)，(2)，(3)，(4)，(5)，(6)，(7)，…(4),(5)，(6)，(7)*hh(CR)(LF) 
//(1)总的GSV语句电文数；6
//(2)当前GSV语句号:1; 
//(3)可视卫星总数:23; 
//(4)卫星号:01; 
//(5)仰角(00～90度):32度; 
//(6)方位角(000～359度):0度; 
//(7)信噪比(00～99dB):0dB(后面依次为第10，16，17号卫星的信息); 
//$GPGSV,6,1,23,	1,1,32,,	2,5,,,		3,5,,,		4,5,,*7E
//$GPGSV,6,2,23,	5,5,,,		6,5,,,		7,31,181,,	8,5,210,*4C
//$GPGSV,6,3,23,	11,54,47,,	13,78,331,,	14,5,,,		15,5,,*42
//$GPGSV,6,4,23,	17,22,317,,	18,5,,,		19,59,161,,	20,25,358,*7C
//$GPGSV,6,5,23,	23,73,341,,	24,5,,,		25,42,165,,	26,5,,*79
//$GPGSV,6,6,23,	28,10,261,,	30,5,,,		32,20,12,*70
void ComReadThread::updateGPGSV(ComReadThread::GPS_Sentence &sentence)
{
	try
	{
		int gsv_sentence_sum = str2Number(sentence[1]);
		int cur_gsv_sentence_index = str2Number(sentence[2]);

		if(gsv_sentence_sum == cur_gsv_sentence_index)//完整的GSV解析完后统一更新
			m_overwrite_saInfo = true;
		else
			m_overwrite_saInfo = false;
	 
		sa_info.Total_Number=str2Number(sentence[3]);
		if(cur_gsv_sentence_index ==1)
		{
			m_valid_sa_number.clear();
			sa_info.Valid_Number = 0;
		}

		for (int i = 0; i < 4; i++) //每条电文最多含有4颗卫星信息
		{
			if(((i+1)*4+3)>sentence.wordCount()-1)
				break;

			if(sentence[(i+1)*4+3].compare("") ==0
				  || sentence[(i+1)*4+2].compare("") ==0
				  || sentence[(i+1)*4+1].compare("") ==0 )
				  continue;

			int sa_number = str2Number(sentence[(i+1)*4]);
			if(m_valid_sa_number.find(sa_number) != m_valid_sa_number.end())
				continue;
			else
				m_valid_sa_number.insert(sa_number);

			assert(sa_info.Valid_Number<32);

			SAInfo& sa_valid=sa_info.Satellite[sa_info.Valid_Number];

			sa_valid.Number = sa_number;
			sa_valid.height_angle =atof(sentence[(i+1)*4+1].c_str());
			sa_valid.direction_angle =atof(sentence[(i+1)*4+2].c_str());
			sa_valid.Strength = str2Number(sentence[(i+1)*4+3]);

			sa_info.Valid_Number++;
		}
		
		if(gsv_sentence_sum == cur_gsv_sentence_index)
		{
			if(sa_info.Valid_Number >=1)
				sa_info.Gps_Valid = true;
			else
				sa_info.Gps_Valid = false;
		}
	
	}
	catch(exception e)
	{
	//	cout<<"error"<<endl;
	}

	//
	//try
	//{
	//	 int count=0;
	//	 count=str2Number(sentence[1]);
	//	 if(count > 24)
	//		 count = 24;
	//	
	// 
	//	sa_info.Total_Number=str2Number(sentence[3]);
	//
	//	for (int i = 0; i < count; i++)
	//	{
	//		  if(((i+1)*4+3)>sentence.wordCount()-1)
	//			  break;

	//	      SAInfo temp=SAInfo();
	//		 
	//		  temp.Number =str2Number(sentence[(i+1)*4]);
	//		  temp.height_angle =atof(sentence[(i+1)*4+1].c_str());
	//		  temp.direction_angle =atof(sentence[(i+1)*4+2].c_str());
	//	
	//		 if(sentence[(i+1)*4+3].compare("")==0)
	//			 temp.Strength = 0;
	//		 else
	//		     temp.Strength = str2Number(sentence[(i+1)*4+3]);
	//		
	//		 sa_info.Update(temp);
	//	  
	//	 }
	//	
	//
	//}
	//catch(exception e)
	//{
	////	cout<<"error"<<endl;
	//}
	
}

double ComReadThread::str2Lat(const string &word)
{
	if(word.size() < 2)
		return 0.0;

	double degree = atof(word.substr(0, 2).c_str()); // assume > 10 degree
	double cent = atof(word.substr(2).c_str());
	return degree + cent/60;	// assume north
}

double ComReadThread::str2Lon(const string &word)
{
	if(word.size() < 3)
		return 0.0;
	double degree = atof(word.substr(0, 3).c_str()); // assume > 100 degree
	double cent = atof(word.substr(3).c_str());
	return degree + cent/60; // assume east
}

double ComReadThread::str2Speed(const std::string &word)
{
	return atof(word.c_str()) * 1.852;
}

double ComReadThread::str2Heading(const std::string &word)
{
	return atof(word.c_str());
}
int  ComReadThread::str2Number(const string &word)
{
	return atoi(word.c_str());

}
GPS_Info::UTC_Time ComReadThread::str2Time(const std::string &word)
{
	GPS_Info::UTC_Time time;

	time.hh = time.mm = time.ss = 0;

	if(word.size() < 6)
		return time;

	time.hh = atol(word.substr(0, 2).c_str());
	time.mm = atol(word.substr(2, 2).c_str());
	time.ss = atol(word.substr(4, 2).c_str());

	return time;
}

GPS_Info::UTC_Date ComReadThread::str2Date(const std::string &word)
{
	GPS_Info::UTC_Date date;
	date.dd = date.mm = date.yy = 0;

	if(word.size() < 6)
		return date;

	date.dd = atol(word.substr(0, 2).c_str());
	date.mm = atol(word.substr(2, 2).c_str());
	date.yy = atol(word.substr(4, 2).c_str());

	return date;
}

}