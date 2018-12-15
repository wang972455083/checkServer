#include "ActiveManager.h"
#include "LTabFile.h"
#include "LTime.h"
#include "Work.h"
#include "LBase.h"
#include "Config.h"
#include "ActivityConfigTbl.h"
#include "NewZhuanPanActivityTbl.h"

bool ActiveManager::Init()
{
	if ( !m_RedisClient.initRedis(gConfig.GetRedisIp(), gConfig.GetRedisPort()) )
	{
		LLOG_DEBUG("redisClient init error");
		return false;
	}
	// 读取转盘配置
	ReadDrawActiveSet();

	//读取红包配置
	ReadRedPacketsActiveSet();
	/*// test
	{
		LTime cur;
		Json::Value root;   // 构建对象  
		root["BeginTime"] = cur.Secs();
		root["EndTime"] = cur.Secs() + 3600*24;
		root["Name"] = "转盘Desc";
		root["Desc"] = "转盘Desc";
		root["SpendType"] = 1;
		root["SpendCount"] = 3;

		Json::Value gift;
		gift["GiftName"] = "3房卡";
		gift["GiftType"] = 1;
		gift["GiftCount"] = 3;
		gift["GiftPercent"] = 0.5;
		gift["GiftTotalCount"] = 100;
		gift["GiftEnsure"] = true;
		gift["GiftResumeNextDay"] = true;
		root["Gift"].append( gift );

		gift["GiftName"] = "100房卡";
		gift["GiftType"] = 1;
		gift["GiftCount"] = 100;
		gift["GiftPercent"] = 0.5;
		gift["GiftTotalCount"] = 1000;
		gift["GiftEnsure"] = false;
		gift["GiftResumeNextDay"] = false;
		root["Gift"].append( gift );

		std::string strSet = root.toStyledString();
		SetDrawActiveConfig(strSet,ActivityID_Draw);
	}*/

	//加载分享活动数据
	Lstring strSet;
	if(m_RedisClient.readDataByString(ActivityShare_Config,strSet))
	{
		UpdateShareActiveSet(strSet,ActivityID_Share);
	}

	if(!ActivityConfigTbl::Instance().Init())
	{
		return false;
	}

	if(!NewZhuanPanActivityTbl::Instance().Init())
	{
		LLOG_DEBUG("NewZhuanPanActivityTbl init error");
		return false;
	}

	/*/test share active
	{
		Json::Value root;   // 构建对象 

		std::string desc = "活动已经终止!!!!";

		
		
		root["Name"] = "分享活动";
		root["BeginTime"] = 1483545600;
		root["EndTime"] = 1483935420;
		root["Id"] = ActivityID_Share;
		root["GiftType"] = 1;
		root["GiftCount"] = 3;
		root["GiftName"] = "房卡";
		root["Desc"] = string_To_UTF8(desc);

		std::string strSet = root.toStyledString();
		UpdateShareActiveSet(strSet,ActivityID_Share);
	}
	//*/
	return true;
}

bool ActiveManager::Final()
{
	
	return true;
}

void ActiveManager::SetDrawActiveConfig(const Lstring& strSet,int id)
{
	DrawSetClear();
	_UpdateDrawActiveSet( strSet, id );
	SaveDrawActiveSet();
	LLOG_ERROR( "ActiveManager::SetDrawActiveConfig BeginTime=%d EndTime=%d", m_Draw.m_BeginTime, m_Draw.m_EndTime );
}

void ActiveManager::DrawSetClear()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	m_Draw = ActivityDraw();
	m_RedisClient.delKey( ActivityDraw_Config );
	char key[64];
	// 库存
	for ( int i = 0; i < 20; ++i )
	{
		sprintf( key, "%s%d", ActivityDraw_RemainTotalCount_, i );
		m_RedisClient.delKey( key );
	}
	m_RedisClient.delKey( ActivityDraw_RemainTotalDate );
}

void ActiveManager::_UpdateDrawActiveSet(const Lstring& strSet,int id)
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);

	// 数据为空表示清除活动
	if ( strSet.empty() )
	{
		// 还原数据
		m_Draw = ActivityDraw();
		return;
	}

	Json::Reader reader(Json::Features::strictMode());
	Json::Value  value;
	if (!reader.parse(strSet, value))
	{
		LLOG_ERROR("ActiveManager::_UpdateDrawActiveSet parsr json error %s", reader.getFormatedErrorMessages().c_str());
		return;
	}

	// 还原数据
	m_Draw = ActivityDraw();

	m_Draw.m_strConfig = strSet;
	m_Draw.m_ActiveID = id;
	if ( !value["Name"].isNull() && value["Name"].isString() )
		m_Draw.m_ActiveName =  value["Name"].asString();
	else
		LLOG_ERROR("ActiveManager::_UpdateDrawActiveSet Name is error");

	if ( !value["Desc"].isNull() && value["Desc"].isString() )
		m_Draw.m_Desc =  value["Desc"].asString();
	else
		LLOG_ERROR("ActiveManager::_UpdateDrawActiveSet Desc is error");

	if ( !value["BeginTime"].isNull() && value["BeginTime"].isInt() )
		m_Draw.m_BeginTime = value["BeginTime"].asInt();
	else
		LLOG_ERROR("ActiveManager::_UpdateDrawActiveSet BeginTime is error");

	if ( !value["EndTime"].isNull() && value["EndTime"].isInt() )
		m_Draw.m_EndTime = value["EndTime"].asInt();
	else
		LLOG_ERROR("ActiveManager::_UpdateDrawActiveSet EndTime is error");

	//if ( !value["SpendType"].isNull() && value["SpendType"].isInt() )
	//	m_Draw.m_SpendType = value["SpendType"].asInt();
	//else
	//	LLOG_ERROR("ActiveManager::_UpdateDrawActiveSet SpendType is error");

	//if ( !value["SpendCount"].isNull() && value["SpendCount"].isInt() )
	//	m_Draw.m_SpendCount = value["SpendCount"].asInt();

	LTime cur;
	m_Draw.m_GiftTotalDate = cur.GetYday();

	const Json::Value& arrayObj = value["Gift"];
	for(unsigned int i = 0; i < arrayObj.size(); i++)
	{
		m_Draw.m_Gift.push_back( ActivityDrawGift() );
		ActivityDrawGift& gift = m_Draw.m_Gift.back();
		gift.m_GiftIndex = (Lint)m_Draw.m_Gift.size();

		if ( !arrayObj[i]["GiftName"].isNull() && arrayObj[i]["GiftName"].isString() )
			gift.m_GiftName = arrayObj[i]["GiftName"].asString();
		else
			LLOG_ERROR("ActiveManager::_UpdateDrawActiveSet GiftName is error");

		if ( !arrayObj[i]["GiftType"].isNull() && arrayObj[i]["GiftType"].isInt() )
			gift.m_GiftType = arrayObj[i]["GiftType"].asInt();
		else
			LLOG_ERROR("ActiveManager::_UpdateDrawActiveSet GiftType is error");

		if ( !arrayObj[i]["GiftCount"].isNull() && arrayObj[i]["GiftCount"].isInt() )
			gift.m_GiftCount = arrayObj[i]["GiftCount"].asInt();
		else
			LLOG_ERROR("ActiveManager::_UpdateDrawActiveSet GiftCount is error");

		if ( !arrayObj[i]["GiftPercent"].isNull() && arrayObj[i]["GiftPercent"].isInt() )
			gift.m_GiftPercent = arrayObj[i]["GiftPercent"].asInt();
		else
			LLOG_ERROR("ActiveManager::_UpdateDrawActiveSet GiftPercent is error");

		if ( gift.m_GiftPercent < 0 )
			gift.m_GiftPercent = 0;
		m_Draw.m_TotalGiftPercent += gift.m_GiftPercent;

		if ( !arrayObj[i]["GiftTotalCount"].isNull() && arrayObj[i]["GiftTotalCount"].isInt() )
			gift.m_GiftTotalCount = arrayObj[i]["GiftTotalCount"].asInt();
		else
			LLOG_ERROR("ActiveManager::_UpdateDrawActiveSet GiftTotalCount is error");

		// 库存
		gift.m_GiftRemainTotalCount = gift.m_GiftTotalCount;

		if ( !arrayObj[i]["GiftEnsure"].isNull() && arrayObj[i]["GiftEnsure"].isBool() )
			gift.m_GiftEnsure = arrayObj[i]["GiftEnsure"].asBool();

		if ( !arrayObj[i]["GiftResumeNextDay"].isNull() && arrayObj[i]["GiftResumeNextDay"].isBool() )
			gift.m_GiftResumeNextDay = arrayObj[i]["GiftResumeNextDay"].asBool();
		else
			LLOG_ERROR("ActiveManager::_UpdateDrawActiveSet GiftResumeNextDay is error");
	}
}

void ActiveManager::SaveDrawActiveSet()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);

	// 配置
	if(!m_RedisClient.writeDataByString(ActivityDraw_Config,m_Draw.m_strConfig))
	{
		LLOG_ERROR("ActiveManager::SaveDrawActiveSet writeDataByString Failed, key=ActivityDraw_Config" );
	}

	// 库存
	for ( auto it = m_Draw.m_Gift.begin(); it != m_Draw.m_Gift.end(); ++it )
	{
		char key[64];
		// 库存数量
		sprintf( key, "%s%d", ActivityDraw_RemainTotalCount_, it->m_GiftIndex );
		if(!m_RedisClient.writeDataByString(key,it->m_GiftRemainTotalCount))
		{
			LLOG_ERROR("ActiveManager::SaveDrawActiveSet writeDataByString Failed, key=%s", key );
		}
	}

	if(!m_RedisClient.writeDataByString(ActivityDraw_RemainTotalDate,m_Draw.m_GiftTotalDate))
	{
		LLOG_ERROR("ActiveManager::SaveDrawActiveSet writeDataByString Failed, key=%s", ActivityDraw_RemainTotalDate );
	}
}

void ActiveManager::ReadDrawActiveSet()
{
	Lstring strSet;
	if(m_RedisClient.readDataByString(ActivityDraw_Config,strSet))
	{
		_UpdateDrawActiveSet(strSet,ActivityID_Draw);

		if ( !strSet.empty() )
		{
			// 读取库存
			for ( auto it = m_Draw.m_Gift.begin(); it != m_Draw.m_Gift.end(); ++it )
			{
				char key[64];
				// 库存数量
				sprintf( key, "%s%d", ActivityDraw_RemainTotalCount_, it->m_GiftIndex );
				if(!m_RedisClient.readDataByString(key,it->m_GiftRemainTotalCount))
				{
					LLOG_ERROR("ActiveManager::ReadDrawActiveSet readDataByString Failed, key=%s", key );
				}
			}

			// 库存日期
			if(!m_RedisClient.readDataByString(ActivityDraw_RemainTotalDate,m_Draw.m_GiftTotalDate))
			{
				LLOG_ERROR("ActiveManager::ReadDrawActiveSet readDataByString Failed, key=%s", ActivityDraw_RemainTotalDate );
			}

		}
	}
}

bool ActiveManager::CheckDrawActiveValid()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);

	if(m_Draw.m_strConfig.empty())   //未配置分享活动，无效
	{
		return false;
	}

	LTime cur;
	return (cur.Secs() >= m_Draw.m_BeginTime) && (cur.Secs() <= m_Draw.m_EndTime);
}
bool ActiveManager::CheckUserActiveValid()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	return false;
}

int ActiveManager::GetDrawActiveId()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	return m_Draw.m_ActiveID;
}

int ActiveManager::GetDrawBeginTime()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	return m_Draw.m_BeginTime;
}

int ActiveManager::GetDrawEndTime()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	return m_Draw.m_EndTime;
}

int ActiveManager::GetDrawSpendType()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	return m_Draw.m_SpendType;
}

int ActiveManager::GetDrawSpendCount()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	return m_Draw.m_SpendCount;
}

ActivityDrawGift ActiveManager::GetDrawGift( ActivityDrawGift& fGift )
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);

	// 检查库存是否要恢复
	LTime cur;
	if ( m_Draw.m_GiftTotalDate != cur.GetYday() )
	{
		m_Draw.m_GiftTotalDate = cur.GetYday();
		for ( auto it = m_Draw.m_Gift.begin(); it != m_Draw.m_Gift.end(); ++it )
		{
			if ( it->m_GiftResumeNextDay )
			{
				it->m_GiftRemainTotalCount = it->m_GiftTotalCount;
			}
		}
		SaveDrawActiveSet();
	}

	Lint findindex = -1;
	Lint temp = 0;
	//设置随机因子
	Lint rand = L_Rand(0,m_Draw.m_TotalGiftPercent);
	for ( Lint i = 0; i < (Lint)m_Draw.m_Gift.size(); ++i )
	{
		if ( m_Draw.m_Gift[i].m_GiftPercent <= 0 )
		{
			continue;
		}
		temp += m_Draw.m_Gift[i].m_GiftPercent;
		if ( temp >= rand )
		{
			if ( m_Draw.m_Gift[i].m_GiftRemainTotalCount != 0 )	// 库存
			{
				findindex = i;
			}
			break;
		}
	}
	if ( findindex != -1 )
	{
		if ( m_Draw.m_Gift[findindex].m_GiftRemainTotalCount > 0 )
		{
			m_Draw.m_Gift[findindex].m_GiftRemainTotalCount--;
			// 保存库存数量
			char key[64];
			sprintf( key, "%s%d", ActivityDraw_RemainTotalCount_, m_Draw.m_Gift[findindex].m_GiftIndex );
			if(!m_RedisClient.writeDataByString(key,m_Draw.m_Gift[findindex].m_GiftRemainTotalCount))
			{
				LLOG_ERROR("ActiveManager::GetDrawGift readDataByString Failed, key=%s", key );
			}
		}
	}
	else
	{
		// 确保中的奖
		for ( Lint i = 0; i < (Lint)m_Draw.m_Gift.size(); ++i )
		{
			if ( m_Draw.m_Gift[i].m_GiftEnsure )
			{
				findindex = i;
				break;
			}
		}
	}

	if ( findindex != -1 )
	{
		m_Draw.m_GiftTime++;
		if ( m_Draw.m_GiftTime % 299 == 0 )
		{
			// 随机给一个配置库存为0的奖品
			int rand = L_Rand( 0, (Lint)m_Draw.m_Gift.size()-1 );

			for ( Lint i = 0; i < (Lint)m_Draw.m_Gift.size(); ++i )
			{
				if ( m_Draw.m_Gift[i].m_GiftTotalCount == 0 )
				{
					fGift = m_Draw.m_Gift[i];
					if ( rand <= (Lint)i )
					{
						break;
					}
				}
			}
		}
		return m_Draw.m_Gift[findindex];
	}
	else
	{
		return ActivityDrawGift();
	}
}

void ActiveManager::UpdateShareActiveSet(const Lstring& strSet,int id)
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);

	//先清空上一次配置
	gActiveManager.ShareSetClear();
		
	Json::Reader reader(Json::Features::strictMode());
	Json::Value value;
	try
	{
		if (reader.parse(strSet, value))
		{
			time_t stime = value["BeginTime"].asUInt();
			time_t etime = value["EndTime"].asUInt();
			int gifCount = value["GiftCount"].asUInt();
			int gifType = value["GiftType"].asUInt();

			if(!checkTimeValid(stime, etime))
			{
				LLOG_ERROR("#ActiveManager::Share load Config Time InValid !");
				return;
			}
			LLOG_DEBUG("ShareSet = %s",strSet.c_str());
			m_Share.m_strConfig = strSet;
			m_Share.m_BeginTime = (Lint)stime;
			m_Share.m_EndTime = (Lint)etime;
			m_Share.m_GiftCount = gifCount;
			m_Share.m_GiftType = gifType;
			m_Share.m_ActiveID = id;

			if(!m_RedisClient.writeDataByString(ActivityShare_Config,strSet))
			{
				LLOG_ERROR("UpdateShareActiveSet Failed!!");
			}
		}
	}
	catch (...)
	{
		LLOG_ERROR("exc exception -- json parse error!!!");
	}

	//LLOG_ERROR( "ActiveManager::UpdateShareActiveSet BeginTime=%d EndTime=%d", m_Share.m_BeginTime, m_Share.m_EndTime );
}

Lstring ActiveManager::GetShareActiveSet()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	return m_Share.m_strConfig;
}

int ActiveManager::GetShareActiveId()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	return m_Share.m_ActiveID;
}

ActivityShare ActiveManager::GetShareSet()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	return m_Share;
}

bool ActiveManager::CheckShareActiveValid()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);

	if(m_Share.m_strConfig.empty())   //未配置分享活动，无效
	{
		return false;
	}

	LTime cur;
	return cur.Secs() <= m_Share.m_EndTime;  //在活动结束之前，有效
}

bool ActiveManager::CheckShareActiveStart()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);

	if(m_Share.m_strConfig.empty())   //未配置分享活动，无效
	{
		return false;
	}

	LTime cur;
	return (cur.Secs() >= m_Share.m_BeginTime) && (cur.Secs() <= m_Share.m_EndTime);  //在活动时间内，开启
}

void ActiveManager::ShareSetClear()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);

	m_Share = ActivityShare();
	m_RedisClient.delKey( ActivityShare_Config );
}

void ActiveManager::SetRedPacketsActiveConfig(const Lstring& strSet, int id)
{
	_UpdateRedPacketsActiveSet(strSet, id);
	LLOG_ERROR("ActiveManager::SetRedPacketsActiveConfig BeginTime=%d EndTime=%d", m_Draw.m_BeginTime, m_Draw.m_EndTime);
}

void ActiveManager::RedPacketsSetClear()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	m_RedPackets = ActivityRedPackets();
	m_RedisClient.delKey(ActivityRedPackets_Config);
	m_RedisClient.delKey(ActivityRedPackets_Config_Tomorrow);
	m_RedisClient.delKey(ActivityRedPackets_Remain);

	char key[64];
	// 库存
	for (int i = 0; i < 20; ++i)
	{
		sprintf(key, "%s%d", ActivityRedPackets_RemainTotalCount_, i);
		m_RedisClient.delKey(key);
	}
	m_RedisClient.delKey(ActivityRedPackets_RemainTotalDate);
}

void ActiveManager::_UpdateRedPacketsActiveSet(const Lstring& strSet, int id)
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);

	// 数据为空表示清楚活动
	if (strSet.empty())
	{
		// 还原数据
		m_RedPackets = ActivityRedPackets();
		m_RedisClient.delKey(ActivityRedPackets_Config);
		m_RedisClient.delKey(ActivityRedPackets_Config_Time);
		m_RedisClient.delKey(ActivityRedPackets_Config_Tomorrow);
		m_RedisClient.delKey(ActivityRedPackets_Remain);
		return;
	}

	Json::Reader reader(Json::Features::strictMode());
	Json::Value  value;
	if (!reader.parse(strSet, value))
	{
		LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet parsr json error %s", reader.getFormatedErrorMessages().c_str());
		return;
	}

	bool applyConfigNow = false;

	if (!value["ApplyConfigNow"].isNull() && value["ApplyConfigNow"].isBool())
		applyConfigNow = value["ApplyConfigNow"].asBool();
	else
		LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet Name is error");


	if (applyConfigNow)
	{
		RedPacketsSetClear();
		if (!m_RedisClient.writeDataByString(ActivityRedPackets_Config_Time, strSet))
		{
			LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet writeDataByString Failed, key=ActivityRedPackets_Config_Time");
		}

		// 还原数据
		m_RedPackets.m_strConfig = strSet;
		m_RedPackets.m_ActiveID = id;

		if (!value["BeginTime"].isNull() && value["BeginTime"].isInt())
			m_RedPackets.m_BeginTime = value["BeginTime"].asInt();
		else
			LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet BeginTime is error");

		if (!value["EndTime"].isNull() && value["EndTime"].isInt())
			m_RedPackets.m_EndTime = value["EndTime"].asInt();
		else
			LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet EndTime is error");

		if (!value["TotalCountLimit"].isNull() && value["TotalCountLimit"].isInt())
		{
			m_RedPackets.m_limit = value["TotalCountLimit"].asInt();
			m_RedPackets.m_left = m_RedPackets.m_limit;
		}
		else
			LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet EndTime is error");

		if (!value["TotalCountLimitResumeNextDay"].isNull() && value["TotalCountLimitResumeNextDay"].isBool())
			m_RedPackets.m_limiteResumeNextDay = value["TotalCountLimitResumeNextDay"].asBool();
		else
			LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet TotalCountLimitResumeNextDay is error");

		LTime cur;
		m_RedPackets.m_GiftTotalDate = cur.GetYday();

		const Json::Value& arrayObj = value["Gift"];
		for (unsigned int i = 0; i < arrayObj.size(); i++)
		{
			m_RedPackets.m_Gift.push_back(ActivityRedPacketsGift());
			ActivityRedPacketsGift& gift = m_RedPackets.m_Gift.back();
			gift.m_GiftIndex = (Lint)m_RedPackets.m_Gift.size();
			if (!arrayObj[i]["GiftIntervalBottom"].isNull() && arrayObj[i]["GiftIntervalBottom"].isInt())
				gift.m_GiftIntervalBottom = arrayObj[i]["GiftIntervalBottom"].asInt();//单位分
			else
				LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet GiftIntervalBottom is error");

			if (!arrayObj[i]["GiftIntervalTop"].isNull() && arrayObj[i]["GiftIntervalTop"].isInt())
				gift.m_GiftIntervalTop = arrayObj[i]["GiftIntervalTop"].asInt();
			else
				LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet GiftIntervalTop is error");

			if (!arrayObj[i]["GiftTotalCount"].isNull() && arrayObj[i]["GiftTotalCount"].isInt())
			{
				gift.m_GiftTotalCount = arrayObj[i]["GiftTotalCount"].asInt();
			}
			else
				LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet GiftTotalCount is error");
			// 库存
			gift.m_GiftRemainTotalCount = gift.m_GiftTotalCount;


			if (!arrayObj[i]["GiftResumeNextDay"].isNull() && arrayObj[i]["GiftResumeNextDay"].isBool())
				gift.m_GiftResumeNextDay = arrayObj[i]["GiftResumeNextDay"].asBool();
			else
				LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet GiftResumeNextDay is error");
		}

		SaveRedPacketsActiveSet();

	}
	else
	{

		//只生效时间，具体配置明天凌晨生效
		if (!value["BeginTime"].isNull() && value["BeginTime"].isInt())
			m_RedPackets.m_BeginTime = value["BeginTime"].asInt();
		else
			LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet BeginTime is error");

		if (!value["EndTime"].isNull() && value["EndTime"].isInt())
			m_RedPackets.m_EndTime = value["EndTime"].asInt();
		else
			LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet EndTime is error");

		if (!m_RedisClient.writeDataByString(ActivityRedPackets_Config_Time, strSet))
		{
			LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet writeDataByString Failed, key=ActivityRedPackets_Config_Time");
		}
		if (! m_RedisClient.writeDataByString(ActivityRedPackets_Config_Tomorrow, strSet))
		{
			LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet writeDataByString Failed, key=ActivityRedPackets_Config_Tomorrow");
		}
	}

}

void ActiveManager::SaveRedPacketsActiveSet()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);

	// 配置
	if (!m_RedisClient.writeDataByString(ActivityRedPackets_Config, m_RedPackets.m_strConfig))
	{
		LLOG_ERROR("ActiveManager::SaveRedPacketsActiveSet writeDataByString Failed, key=ActivityDraw_Config");
	}

	// 库存
	for (auto it = m_RedPackets.m_Gift.begin(); it != m_RedPackets.m_Gift.end(); ++it)
	{
		char key[64];
		// 库存数量
		sprintf(key, "%s%d", ActivityRedPackets_RemainTotalCount_, it->m_GiftIndex);
		if (!m_RedisClient.writeDataByString(key, it->m_GiftRemainTotalCount))
		{
			LLOG_ERROR("ActiveManager::SaveRedPacketsActiveSet writeDataByString Failed, key=%s", key);
		}
	}

	if (!m_RedisClient.writeDataByString(ActivityRedPackets_Remain, m_RedPackets.m_left))
	{
		LLOG_ERROR("ActiveManager::SaveRedPacketsActiveSet writeDataByString Failed, key=%s", ActivityRedPackets_Remain);
	}

	if (!m_RedisClient.writeDataByString(ActivityRedPackets_RemainTotalDate, m_RedPackets.m_GiftTotalDate))
	{
		LLOG_ERROR("ActiveManager::SaveRedPacketsActiveSet writeDataByString Failed, key=%s", ActivityRedPackets_RemainTotalDate);
	}
}
void ActiveManager::ReadRedPacketsActiveSet()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	Lstring strSet;
	if (m_RedisClient.readDataByString(ActivityRedPackets_Config, strSet))
	{
		_ReloadRedPacketsActiveSet(strSet, ActivityID_Invite);
		LLOG_ERROR("ActiveManager::ReadRedPacketsActiveSet readDataByString ");
		if (!strSet.empty())
		{
			// 读取库存
			for (auto it = m_RedPackets.m_Gift.begin(); it != m_RedPackets.m_Gift.end(); ++it)
			{
				char key[64];
				// 库存数量
				sprintf(key, "%s%d", ActivityRedPackets_RemainTotalCount_, it->m_GiftIndex);
				if (!m_RedisClient.readDataByString(key, it->m_GiftRemainTotalCount))
				{
					LLOG_ERROR("ActiveManager::ReadRedPacketsActiveSet readDataByString Failed, key=%s", key);
				}
			}
			if (!m_RedisClient.readDataByString(ActivityRedPackets_Remain, m_RedPackets.m_left))
			{
				LLOG_ERROR("ActiveManager::ReadRedPacketsActiveSet readDataByString Failed, key=%s", ActivityRedPackets_RemainTotalDate);
			}

			// 库存日期
			if (!m_RedisClient.readDataByString(ActivityRedPackets_RemainTotalDate, m_RedPackets.m_GiftTotalDate))
			{
				LLOG_ERROR("ActiveManager::ReadRedPacketsActiveSet readDataByString Failed, key=%s", ActivityRedPackets_RemainTotalDate);
			}
		}
	}
	if (m_RedisClient.readDataByString(ActivityRedPackets_Config_Time, strSet))
	{
		_ReloadRedPacketsActiveTime(strSet);
	}
}

bool ActiveManager::CheckRedPacketActiveValid()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	if (m_RedPackets.m_strConfig.empty())   //未配置分享活动，无效
	{
		return false;
	}

	LTime cur;
	return (cur.Secs() >= m_RedPackets.m_BeginTime) && (cur.Secs() <= m_RedPackets.m_EndTime);
}

int ActiveManager::GetRedPacketsActiveId()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	return m_RedPackets.m_ActiveID;
}
int ActiveManager::GetRedPacketsBeginTime()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	return m_RedPackets.m_BeginTime;
}
int ActiveManager::GetRedPacketsEndTime()
{
	return m_RedPackets.m_EndTime;
}

Lint ActiveManager::getRestTimes(Lint userid)
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	char canDrawCountTime[128] = { 0 };
	char alreadyDrawedCountTime[128] = { 0 };
	char canDrawCount[128] = { 0 };
	char alreadyDrawedCount[128] = { 0 };
	sprintf(canDrawCountTime, "ActivityLastUpdateCanDrawCountTime_userid_%d", userid);
	sprintf(alreadyDrawedCountTime, "ActivityLastUpdateAlreadyDrawedCountTime_userid_%d", userid);
	sprintf(canDrawCount, "ActivityCanDrawCount_userid_%d", userid);
	sprintf(alreadyDrawedCount, "ActivityAlreadyDrawedCount_userid_%d", userid);


	if (isRedisTimeNotToday(std::string(alreadyDrawedCountTime)))
	{
		if (isRedisTimeNotToday(std::string(canDrawCountTime)))
		{
			return 0;
		}
		else
		{
			Lint count = 0;

			if (m_RedisClient.readDataByString(std::string(canDrawCount), count))
			{
				return count;
			}
			else
				return 0;
		}
	}
	else
	{
		Lint count = 0, count2 = 0;
		if (m_RedisClient.readDataByString(std::string(canDrawCount), count) && m_RedisClient.readDataByString(std::string(alreadyDrawedCount), count2))
		{
			return count - count2;
		}
		else
			return 0;
	}
	return 0;
}

void ActiveManager::updateRedisKeyTime(const Lstring& key)
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	LTime cur;
	bool  ret = m_RedisClient.writeDataByString(key, cur.Secs());
	if (ret)
	{
		m_RedisClient.setKeyTTLTime(key, (24 * 60 * 60));
	}
}

void ActiveManager::increaseRedisKeyValue(const Lstring& key)
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	int count = 0;
	bool ret = m_RedisClient.readDataByString(key, count);
	if (ret)
	{
		count += 1;
		m_RedisClient.writeDataByString(key, count);
	}
}

bool ActiveManager::redisWriteDataByString(const std::string& strKey, int iWrite)
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	return m_RedisClient.writeDataByString(strKey, iWrite);
}

bool ActiveManager::redisReadDataByString(const std::string& strKey, int& iRead)
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	return m_RedisClient.readDataByString(strKey,iRead);
}

bool  ActiveManager::redisSetKeyTTLTime(const std::string& strKey, unsigned int tim_secs)
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	return m_RedisClient.setKeyTTLTime(strKey,tim_secs);
}

bool ActiveManager::redisReadDataByList(const std::string& strKey, unsigned int iFromIndex, unsigned int iReadCount, std::list<std::string>& listRead)
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	return m_RedisClient.readDataByList(strKey,iFromIndex,iReadCount,listRead);
}

bool ActiveManager::redisWriteDataByList(const std::string& strKey, const std::string& strWrite, unsigned int limit)
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	return m_RedisClient.writeDataByList(strKey, strWrite, limit);
}

bool ActiveManager::isRedisTimeNotToday(const Lstring& key)
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	bool suc = false;
	int time = 0;
	bool ret = m_RedisClient.readDataByString(key, time);
	if (ret)
	{
		if (time != 0)
		{
			LTime lastTime;
			int curDay = lastTime.GetDate();
			lastTime.SetSecs(time);
			int lastDay = lastTime.GetDate();
			if (curDay != lastDay)
			{
				suc = true;
			}
		}
		else
		{
			suc = true;
		}
	}

	return  suc;
}

Money ActiveManager::GetRedPacketGift()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	// 检查库存是否要恢复
	LTime cur;
	if (m_RedPackets.m_GiftTotalDate != cur.GetYday())
	{
		m_RedPackets.m_GiftTotalDate = cur.GetYday();
		for (auto it = m_RedPackets.m_Gift.begin(); it != m_RedPackets.m_Gift.end(); ++it)
		{
			if (it->m_GiftResumeNextDay)
			{
				it->m_GiftRemainTotalCount = it->m_GiftTotalCount;
			}
		}
		if (m_RedPackets.m_limiteResumeNextDay)
		{
			m_RedPackets.m_left = m_RedPackets.m_limit;
		}
		SaveRedPacketsActiveSet();
	}

	Lint findindex = -1;
	Lint temp = 0;
	Lint remaincount = GetRaminGiftCount();
	Lint rand = L_Rand(1, GetRaminGiftCount());
	for (Lint i = 0; i < (Lint)m_RedPackets.m_Gift.size(); ++i)
	{
		temp += m_RedPackets.m_Gift[i].m_GiftRemainTotalCount;
		if (rand <= temp)
		{
			if (m_RedPackets.m_Gift[i].m_GiftRemainTotalCount != 0)	// 库存
			{
				findindex = i;
			}
			break;
		}

	}
	if (findindex != -1)
	{
		if (m_RedPackets.m_Gift[findindex].m_GiftRemainTotalCount > 0)
		{
			Lint money = L_Rand(m_RedPackets.m_Gift[findindex].m_GiftIntervalBottom, m_RedPackets.m_Gift[findindex].m_GiftIntervalTop);
			money = (money / 10) * 10;

			if (m_RedPackets.m_left >= money && money!=0)
			{
				m_RedPackets.m_Gift[findindex].m_GiftRemainTotalCount--;
				// 保存库存数量
				char key[64];
				sprintf(key, "%s%d", ActivityRedPackets_RemainTotalCount_, m_RedPackets.m_Gift[findindex].m_GiftIndex);
				if (!m_RedisClient.writeDataByString(key, m_RedPackets.m_Gift[findindex].m_GiftRemainTotalCount))
				{
					LLOG_ERROR("ActiveManager::GetRedPacketGift ActivityRedPackets_RemainTotalCount_ Failed, key=%s", key);
				}
				m_RedPackets.m_left -= money;

				if (!m_RedisClient.writeDataByString(ActivityRedPackets_Remain, m_RedPackets.m_left))
				{
					LLOG_ERROR("ActiveManager::GetRedPacketGift ActivityRedPackets_Remain Failed, key=%s", key);
				}
				return Money(money, findindex);
			}
			else
			{
				return Money(0, 0);
			}

		}
	}
	return Money(0, 0);
}

Lint ActiveManager::GetRaminGiftCount()
{
	Lint ret = 0;
	for (Lint i = 0; i < m_RedPackets.m_Gift.size(); ++i)
	{
		ret += m_RedPackets.m_Gift[i].m_GiftRemainTotalCount;
	}
	return ret;
}

void ActiveManager::ReturnRedPacketBack(Lint index, Lint money, Lint invitee, Lint type)
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	m_RedPackets.m_Gift[index].m_GiftRemainTotalCount++;
	m_RedPackets.m_left += money;
	if (!m_RedisClient.writeDataByString(ActivityRedPackets_Remain, m_RedPackets.m_left))
	{
		LLOG_ERROR("ActiveManager::ReturnRedPacketBack writeDataByString Failed, key=%s", ActivityRedPackets_RemainTotalDate);
	}
	char key[64];
	sprintf(key, "%s%d", ActivityRedPackets_RemainTotalCount_, m_RedPackets.m_Gift[index].m_GiftIndex);
	if (!m_RedisClient.writeDataByString(key, m_RedPackets.m_Gift[index].m_GiftRemainTotalCount))
	{
		LLOG_ERROR("ActiveManager::ReturnRedPacketBack writeDataByString Failed, key=%s", key);
	}

	gWork.SetRedActivityFalse(invitee, type);
}

void ActiveManager::_ReloadRedPacketsActiveSet(const Lstring& strSet, int id)
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);

	Json::Reader reader(Json::Features::strictMode());
	Json::Value  value;
	if (!reader.parse(strSet, value))
	{
		LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet parsr json error %s", reader.getFormatedErrorMessages().c_str());
		return;
	}

	// 还原数据
	m_RedPackets = ActivityRedPackets();

	m_RedPackets.m_strConfig = strSet;
	m_RedPackets.m_ActiveID = id;
	if (!value["Name"].isNull() && value["Name"].isString())
		m_RedPackets.m_ActiveName = value["Name"].asString();
	else
		LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet Name is error");

	if (!value["Desc"].isNull() && value["Desc"].isString())
		m_RedPackets.m_Desc = value["Desc"].asString();
	else
		LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet Desc is error");

	if (!value["BeginTime"].isNull() && value["BeginTime"].isInt())
		m_RedPackets.m_BeginTime = value["BeginTime"].asInt();
	else
		LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet BeginTime is error");

	if (!value["EndTime"].isNull() && value["EndTime"].isInt())
		m_RedPackets.m_EndTime = value["EndTime"].asInt();
	else
		LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet EndTime is error");

	if (!value["TotalCountLimit"].isNull() && value["TotalCountLimit"].isInt())
	{
		m_RedPackets.m_limit = value["TotalCountLimit"].asInt(); 
		m_RedPackets.m_left = m_RedPackets.m_limit;
	}
	else
		LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet EndTime is error");

	if (!value["TotalCountLimitResumeNextDay"].isNull() && value["TotalCountLimitResumeNextDay"].isBool())
		m_RedPackets.m_limiteResumeNextDay = value["TotalCountLimitResumeNextDay"].asBool();
	else
		LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet TotalCountLimitResumeNextDay is error");

	LTime cur;
	m_RedPackets.m_GiftTotalDate = cur.GetYday();

	const Json::Value& arrayObj = value["Gift"];
	for (unsigned int i = 0; i < arrayObj.size(); i++)
	{
		m_RedPackets.m_Gift.push_back(ActivityRedPacketsGift());
		ActivityRedPacketsGift& gift = m_RedPackets.m_Gift.back();
		gift.m_GiftIndex = (Lint)m_RedPackets.m_Gift.size();
		if (!arrayObj[i]["GiftIntervalBottom"].isNull() && arrayObj[i]["GiftIntervalBottom"].isInt())
			gift.m_GiftIntervalBottom = arrayObj[i]["GiftIntervalBottom"].asInt();//单位分
		else
			LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet GiftIntervalBottom is error");

		if (!arrayObj[i]["GiftIntervalTop"].isNull() && arrayObj[i]["GiftIntervalTop"].isInt())
			gift.m_GiftIntervalTop = arrayObj[i]["GiftIntervalTop"].asInt(); 
		else
			LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet GiftIntervalTop is error");

		if (!arrayObj[i]["GiftTotalCount"].isNull() && arrayObj[i]["GiftTotalCount"].isInt())
		{
			gift.m_GiftTotalCount = arrayObj[i]["GiftTotalCount"].asInt();
		}
		else
			LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet GiftTotalCount is error");
		// 库存
		gift.m_GiftRemainTotalCount = gift.m_GiftTotalCount;


		if (!arrayObj[i]["GiftResumeNextDay"].isNull() && arrayObj[i]["GiftResumeNextDay"].isBool())
			gift.m_GiftResumeNextDay = arrayObj[i]["GiftResumeNextDay"].asBool();
		else
			LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet GiftResumeNextDay is error");
	}
}
void ActiveManager::_ReloadRedPacketsActiveTime(const Lstring& strSet)
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);

	// 数据为空表示清楚活动
	if (strSet.empty())
	{
		// 还原数据
		m_RedPackets = ActivityRedPackets();
		return;
	}

	Json::Reader reader(Json::Features::strictMode());
	Json::Value  value;
	if (!reader.parse(strSet, value))
	{
		LLOG_ERROR("ActiveManager::_ReloadRedPacketsActiveTime parsr json error %s", reader.getFormatedErrorMessages().c_str());
		return;
	}

	if (!value["BeginTime"].isNull() && value["BeginTime"].isInt())
		m_RedPackets.m_BeginTime = value["BeginTime"].asInt();
	else
		LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet BeginTime is error");

	if (!value["EndTime"].isNull() && value["EndTime"].isInt())
		m_RedPackets.m_EndTime = value["EndTime"].asInt();
	else
		LLOG_ERROR("ActiveManager::_UpdateRedPacketsActiveSet EndTime is error");

}

void ActiveManager::ReloadRedPacketActivitySetTomorrow()
{
	boost::recursive_mutex::scoped_lock l(m_Mutex);
	Lstring strSet;
	if (m_RedisClient.readDataByString(ActivityRedPackets_Config_Tomorrow, strSet) && !strSet.empty())
	{
		_ReloadRedPacketsActiveSet(strSet, ActivityID_Invite);
		SaveRedPacketsActiveSet();
		//m_RedisClient.writeDataByString(ActivityRedPackets_Config, strSet); 
		m_RedisClient.writeDataByString(ActivityRedPackets_Config_Time, strSet);
		LLOG_ERROR("ActiveManager::ReloadActivitySetTomorrow readDataByString ");
		m_RedisClient.delKey(ActivityRedPackets_Config_Tomorrow);
	}

	LTime cur;
	m_RedPackets.m_GiftTotalDate = cur.GetYday();
	for (auto it = m_RedPackets.m_Gift.begin(); it != m_RedPackets.m_Gift.end(); ++it)
	{
		if (it->m_GiftResumeNextDay)
		{
			it->m_GiftRemainTotalCount = it->m_GiftTotalCount;
		}
	}
	if (m_RedPackets.m_limiteResumeNextDay)
	{
		m_RedPackets.m_left = m_RedPackets.m_limit;
	}
	SaveRedPacketsActiveSet();

}

void ActiveManager::UpdateZhuanPanRedpacketMap(Lint userId,ActivityZhuanPanRedpacket* pData)
{
	if(pData)
	{
		m_mapZhuanPanRedpacket[userId]=pData;
	}
}

ActivityZhuanPanRedpacket* ActiveManager::GetZhuanPanRedpacketData(Lint userId)
{
	std::map<Lint,ActivityZhuanPanRedpacket*>::iterator iter = m_mapZhuanPanRedpacket.find(userId);
	if(iter != m_mapZhuanPanRedpacket.end())
		return iter->second;

	return NULL;
}

bool ActiveManager::IsZhuanPanRedpacketOpen()
{
	return ActivityConfigTbl::Instance().IsZhuanPanRedpacketActivityOpen();
}

void ActiveManager::InsertNewZhuanPanActivityMap(Lint userId,NewZhuanPanActivity* pData)
{
	if(pData)
	{
		if(!m_mapNewZhuanPanActivity.count(userId))
		{
			m_mapNewZhuanPanActivity[userId]=pData;
		}
	}
}

NewZhuanPanActivity* ActiveManager::GetNewZhuanPanActivityData(Lint userId)
{
	std::map<Lint,NewZhuanPanActivity*>::iterator iter = m_mapNewZhuanPanActivity.find(userId);
	if(iter != m_mapNewZhuanPanActivity.end())
		return iter->second;

	return NULL;
}

