#include "BaseCommonFunc.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/regex.hpp>
#include "LMsg.h"

#if _LINUX
#include <time.h> 
#include "IPFilter.h"
static bool created = false;
static boost::mutex lock;
#else
#include <windows.h>
#endif

void DisableIP(const std::string& ip)
{
#if _LINUX
	
	if(!created)
	{
		boost::lock_guard<boost::mutex> guard(lock);
		if(!created)
		{
			if(App_IPFilter::instance()->CreateIPFilterSocket() == 0)
			{
				created = true;
				App_IPFilter::instance()->SetIPFilterServer("127.0.0.1", 30000);
			}
		}
	}

	if(created)
	{
		App_IPFilter::instance()->SendFilterIPPacket(ip);
	}

#endif
}

static unsigned short bytecrc(unsigned short sum, unsigned char data)
{
	sum ^= data;

	for(int i =0; i <= 3; i++)
	{
		if((sum & 0x01) == 0)
			sum /= 2;
		else
			sum = (sum / 2) ^ 0x70B1;
	}
	return sum;
}

unsigned short CalcChecksum (unsigned char *data, int dataSize) 
{
    unsigned short sum = 0xFFFF;
	for(int i = 0; i < dataSize; i++)
		sum = bytecrc(sum, data[i]);

	return sum;
}

bool IsValidCellphoneNo(const std::string& no)
{
	const std::string expression = "^1[3|4|5|7|8]\\d{9}$";
	boost::regex reg(expression);
	return boost::regex_match(no, reg);
	return false;
}

Llong GetOSTickCount()
{
#if _LINUX
	struct timespec ts;  
	clock_gettime(CLOCK_MONOTONIC, &ts);  
	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#else
	return (Llong)::GetTickCount64();
#endif
}

LMsg* ParseInsideNetPacket(LBuffPtr recv, LSocketPtr s)
{
	int msgid = 0;
	recv->Read(msgid);
	LMsg* msg = LMsgFactory::Instance().CreateMsg(msgid);
	if(msg != NULL)
	{
		msg->Read(*recv.get());
		msg->m_sp = s;
	}
	else
	{
		LLOG_ERROR("ParseInsideNetPacket: error msgId=%d",msgid);
	}
	return msg;
}

Lint TransfromMsgId(Lint timestamp, Lint msgId)
{
	//Lint transedMsgId = (timestamp % 10000 + 1) * msgId;
	return msgId;
}

Lint InverseTransfromMsgId(Lint timestamp, Lint transedMsgId)
{
	//Lint msgId = timestamp > 0 ? (transedMsgId / (timestamp % 10000 + 1)) : 0;
	return transedMsgId;
}

Lint GetDistance(Lint cur_pos, Lint des_pos, Lint desk_user_count)
{
	int step = cur_pos - des_pos;
	return std::abs((step > 0? step - desk_user_count:step));
}

std::string& Trim(std::string &str)
{
	if (!str.empty())
	{
		str.erase(0, str.find_first_not_of(" "));
		str.erase(str.find_last_not_of(" ") + 1);
	}

	return str;
}

std::string& replace_all(std::string& str,const std::string& old_value,const std::string& new_value)
{     
	while(true)   {     
		std::string::size_type pos(0);
		if(   (pos=str.find(old_value))!=std::string::npos   )     
			str.replace(pos,old_value.length(),new_value);     
		else   break;     
	}     
	return   str;     
}

std::string& replace_all_distinct(std::string& str,const std::string& old_value,const std::string& new_value)
{     
	for(std::string::size_type pos(0);   pos!=std::string::npos;   pos+=new_value.length())   {     
		if(   (pos=str.find(old_value,pos))!=std::string::npos   )     
			str.replace(pos,old_value.length(),new_value);     
		else   break;     
	}     
	return   str;     
} 

std::string convertInt2String(int iValue)
{
	char chValue[100] = { 0 };
	sprintf(chValue, "%d", iValue);

	return std::string(chValue);
}

std::string convertUInt2String(unsigned int uValue)
{
	char chValue[100] = { 0 };
	sprintf(chValue, "%u", uValue);

	return std::string(chValue);
}

std::string convertLL2String(Llong lValue)
{
	char chValue[100] = { 0 };
	sprintf(chValue, "%lld", lValue);

	return std::string(chValue);
}