#include "LTime.h"
#include <boost/date_time/filetime_functions.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

#define DAY_SUM_SEC (24*60*60)

#define HOUR_SUM_SEC (60*60)

#define MIN_SUM_SEC (60)

#define SEC_SUM_MSEC (1000)

#define BEIJIN_TIME_ZONE (8*60*60)


LTime::LTime()
{
	Now();
}

LTime::LTime(const LTime& rt)
{
	memcpy(this,&rt,sizeof(*this));
}

void LTime::Delay(Lint time)
{
	_time += time*1000;
	time_t t = _time/SEC_SUM_MSEC+BEIJIN_TIME_ZONE;
#if _LINUX
	gmtime_r(&t, &_tm);
#else
	gmtime_s(&_tm,&t);
#endif
}

void LTime::Now()
{
	_time = LTimeTool::GetUTCStampInMilSecond();
	time_t t = _time/SEC_SUM_MSEC+BEIJIN_TIME_ZONE;
#if _LINUX
	gmtime_r(&t, &_tm);
#else
	gmtime_s(&_tm,&t);
#endif
}

void LTime::SetMSecs(Llong msecs)
{
	_time = msecs;
	time_t t = _time/SEC_SUM_MSEC+BEIJIN_TIME_ZONE;
#if _LINUX
	gmtime_r(&t, &_tm);
#else
	gmtime_s(&_tm,&t);
#endif
}

Llong LTime::MSecs()
{
	return _time;
}

void LTime::SetSecs(Lint secs)
{
	_time = (Llong)secs*(Llong)1000;
	time_t t = secs+BEIJIN_TIME_ZONE;
#if _LINUX
	gmtime_r(&t, &_tm);
#else
	gmtime_s(&_tm,&t);
#endif
}

Lint LTime::Secs()
{
	return (Lint) (_time/SEC_SUM_MSEC);
}

LTime& LTime::operator=(const LTime& rt)
{
	memcpy(this,&rt,sizeof(*this));
	return *this;
}

bool LTime::operator<(LTime const& rt)
{
	return _time < rt._time;
}

bool LTime::operator<=(LTime const& rt)
{
	return _time <= rt._time;
}

bool LTime::operator>(LTime const& rt)
{
	return _time > rt._time;
}

bool LTime::operator>=(LTime const& rt)
{
	return _time >= rt._time;
}

Lint LTime::GetYday()
{
	return _tm.tm_yday;
}

Lint LTime::GetMday()
{
	return _tm.tm_mday;
}

Lint LTime::GetWday()
{
	return _tm.tm_wday;
}

Lint LTime::GetYear()
{
	return _tm.tm_year+1900;
}

Lint LTime::GetMonth()
{
	return _tm.tm_mon;
}

Lint LTime::GetHour()
{
	return _tm.tm_hour;
}

Lint LTime::GetMin()
{
	return _tm.tm_min;
}

Lint LTime::GetSec()
{
	return _tm.tm_sec;
}

Lint LTime::GetMSec()
{
	return _time%1000;
}

Lint LTime::GetDate()
{
	char tmp[32];
	sprintf(tmp,"%04d%02d%02d",(_tm.tm_year + 1900),_tm.tm_mon+1,_tm.tm_mday);
	return atoi(tmp);
}

//////////////////////////////////////////////////////////////////////////
LDateTime::LDateTime()
	:_data(0)
{
	LTime curr;
	Lchar* tmp = (Lchar*)&_data;
	tmp[0] = (Lchar)curr.GetHour();
	tmp[1] = (Lchar)curr.GetMin();
	tmp[2] = (Lchar)curr.GetSec();
	tmp[3] = 0;
}

LDateTime::LDateTime(Lstring timestr)
{
	_data = 0;
}

LDateTime::LDateTime(Lint hour,Lint min,Lint sec)
{
	if(hour < 0 || hour > 23)
		hour = 0;
	if(min < 0 || min > 59)
		min = 0;
	if(sec < 0 || sec > 59)
		sec = 59;
	Lchar* tmp = (Lchar*)&_data;
	tmp[0] = (Lchar)hour;
	tmp[1] = (Lchar)min;
	tmp[2] = (Lchar)sec;
	tmp[3] = 0;
}

Lint LDateTime::GetHour()
{
	Lchar* tmp = (Lchar*)&_data;
	return tmp[0];
}

Lint LDateTime::GetMin()
{
	Lchar* tmp = (Lchar*)&_data;
	return tmp[1];
}

Lint LDateTime::GetSec()
{
	Lchar* tmp = (Lchar*)&_data;
	return tmp[2];
}

//////////////////////////////////////////////////////////////////////////
LWeekTime::LWeekTime()
	:_data(0)
{
	LTime curr;
	Lchar* tmp = (Lchar*)_data;

	tmp[0] = (Lchar)curr.GetWday();
	tmp[1] = (Lchar)curr.GetHour();
	tmp[2] = (Lchar)curr.GetMin();
	tmp[3] = (Lchar)curr.GetSec();
}

LWeekTime::LWeekTime(Lstring timestr)
{
	_data = 0;
}

LWeekTime::LWeekTime(Lint wday,Lint hour,Lint min,Lint sec)
{
	if(wday < 0 || wday > 6)
		wday = 0;
	if(hour < 0 || hour > 23)
		hour = 0;
	if(min < 0 || min > 59)
		min = 0;
	if(sec < 0 || sec > 59)
		sec = 0;

	Lchar* tmp = (Lchar*)_data;
	tmp[0] = (Lchar)hour;
	tmp[1] = (Lchar)min;
	tmp[2] = (Lchar)sec;
	tmp[3] = (Lchar)wday;
	_data = 0;
}

Lint LWeekTime::GetHour()
{
	Lchar* tmp = (Lchar*)&_data;
	return tmp[0];
}

Lint LWeekTime::GetMin()
{
	Lchar* tmp = (Lchar*)&_data;
	return tmp[1];
}

Lint LWeekTime::GetSec()
{
	Lchar* tmp = (Lchar*)&_data;
	return tmp[2];
}

Lint LWeekTime::GetWDay()
{
	Lchar* tmp = (Lchar*)&_data;
	return tmp[3];
}
//////////////////////////////////////////////////////////////////////////

Llong LTimeTool::GetUTCStampInMicSecond()
{
	boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::gregorian::Jan, 1));
	boost::posix_time::time_duration time_from_epoch = boost::posix_time::microsec_clock::universal_time() - epoch;
	return time_from_epoch.total_microseconds();
}

Llong LTimeTool::GetUTCStampInMilSecond()
{
	return GetUTCStampInMicSecond()/1000;
}

Lint LTimeTool::GetUTCStampInSecond()
{
	return (Lint)(GetUTCStampInMicSecond()/1000000);
}

Lstring LTimeTool::GetLocalTimeInString(LTime time)
{
	Lstring str;
	char tmp[32] = {0};
	sprintf(tmp,"[%02d-%02d-%02d %02d:%02d:%02d %03d]",time.GetYear(),time.GetMonth()+1,
		time.GetMday(),time.GetHour(),time.GetMin(),time.GetSec(),time.GetMSec());
	return Lstring(tmp);
}

Lstring LTimeTool::GetLocalDateInString(LTime time)
{
	char tmp[32] = {0};
	sprintf(tmp,"%02d-%02d-%02d",time.GetYear(),time.GetMonth()+1,time.GetMday());
	return Lstring(tmp);
}


bool LTimeTool::UpdateDay(LTime& time1,LTime& time2,LDateTime timepoint)
{
	Lint t1 = time1.Secs();
	Lint t2 = time2.Secs();
	Lint delay = timepoint.GetHour()*HOUR_SUM_SEC+timepoint.GetMin()*MIN_SUM_SEC+timepoint.GetSec();

	Lint days1 = (t1 - delay + BEIJIN_TIME_ZONE)/DAY_SUM_SEC;
	Lint days2 = (t2 - delay + BEIJIN_TIME_ZONE)/DAY_SUM_SEC;
	return days1 != days2;
}

Lint LTimeTool::IntervalDay(LTime& time1,LTime& time2,LDateTime timepoint)
{
	Lint t1 = time1.Secs();
	Lint t2 = time2.Secs();
	Lint delay = timepoint.GetHour()*HOUR_SUM_SEC+timepoint.GetMin()*MIN_SUM_SEC+timepoint.GetSec();

	Lint days1 = (t1 - delay + BEIJIN_TIME_ZONE)/DAY_SUM_SEC;
	Lint days2 = (t2 - delay + BEIJIN_TIME_ZONE)/DAY_SUM_SEC;
	return abs(days2-days1);
}

bool LTimeTool::UpdateWeek(LTime& time1,LTime& time2,LWeekTime daypoint)
{
	return false;
}

bool LTimeTool::TimeInDataTime(LTime& time,LDateTime timepoint1,LDateTime timepoint2)
{
	LTime curr;
	Lint days = (curr.Secs()+BEIJIN_TIME_ZONE)/DAY_SUM_SEC;
	Lint point0 = days*DAY_SUM_SEC;

	time_t t1 = point0 + timepoint1.GetHour()*HOUR_SUM_SEC+timepoint1.GetMin()*MIN_SUM_SEC+timepoint1.GetSec();
	time_t t2 = point0 + timepoint2.GetHour()*HOUR_SUM_SEC+timepoint2.GetMin()*MIN_SUM_SEC+timepoint2.GetSec();

	time_t t = time.Secs()+BEIJIN_TIME_ZONE;
	return t >= t1 && t <= t2;
}

bool LTimeTool::IsSameDay(LTime t1,LTime t2)
{
	if(t1.GetYear()==t2.GetYear()&&t1.GetYday()==t2.GetYday())
		return true;

	return false;
}

// 判断两个时间戳是否在同一天 [2017-5-22 willing]
bool LTimeTool::IsInSameDay(Lint nOneTimestamp, Lint nTwoTimestamp)
{
	return (Lint)(nOneTimestamp / (24 * 3600)) == (Lint)(nTwoTimestamp / (24 * 3600));
}