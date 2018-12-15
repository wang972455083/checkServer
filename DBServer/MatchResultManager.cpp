#include "MatchResultManager.h"
#include "LLog.h"
#include "LMsgS2S.h"
#include "DBSession.h"
#include "Work.h"
#include "Config.h"

bool MatchResultManager::Init()
{
	if(!m_dbsession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		return false;
	}

	LoadMatchResultInfo();
	return true;
}

bool MatchResultManager::Final()
{
	return true;
}

void MatchResultManager::LoadMatchResultInfo()
{
	//加载所有的玩家
	//DbTool tool;
	MYSQL* m = m_dbsession.GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("UserMatchInfo::LoadUserInfo mysql null");
		return;
	}

	Lint limitfrom = 0;
	Lint count = 500000;

	while(true)
	{
		std::stringstream ss;
		ss << "select userId, level, time, playState, playType FROM matchResult ORDER BY Id DESC LIMIT ";
		ss << limitfrom << "," << count;

		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("UserManager::LoadUserIdInfo sql error %s", mysql_error(m));
			break;
		}

		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		if(!row)
		{
			break;
		}

		while(row)
		{
			++limitfrom;
			MatchResultInfo* info = new MatchResultInfo();
			info->m_userId = atoi(*row++);
			info->m_season.m_level = atoi(*row++);
			info->m_season.m_time = atoi(*row++);
			info->m_season.m_playState = atoi(*row++);
			AddRecord(info);
			row = mysql_fetch_row(res);
		}
		mysql_free_result(res);
	}

	LLOG_ERROR("UserManager::LoadUserIdInfo user count %d", limitfrom);
}

void MatchResultManager::LoadMatchResultInfo(Lint serverID)
{
	MatchResultInfo info;
	std::map<Lint, MatchResultInfo*>::iterator it = m_userMatchInfoMap.begin();
	for(; it != m_userMatchInfoMap.end(); ++it)
	{	
		LMsgDb2MMatchResultInfo send;
		info.m_userId = it->second->m_userId;
		info.m_season.m_level = it->second->m_season.m_level;
		info.m_season.m_playState = it->second->m_season.m_playState;
		info.m_season.m_playType = it->second->m_season.m_playType;
		info.m_season.m_time = it->second->m_season.m_time;
		send.m_info.push_back(info);
		gWork.SendMsgToMatchServer(send);
	}
}

void MatchResultManager::AddRecord(MatchResultInfo* info)
{
	if (!m_userMatchInfoMap.count(info->m_userId))
	{
		m_userMatchInfoMap[info->m_userId] = info;
	}
}

void MatchResultManager::SaveResultInfo(MatchResultInfo &info)
{
	boost::mutex::scoped_lock l(m_mutexInsert);
	if (!m_userMatchInfoMap.count(info.m_userId))
	{
		MatchResultInfo* pInfo = new MatchResultInfo();
		pInfo->m_userId = info.m_userId;
		pInfo->m_season = info.m_season;
		m_userMatchInfoMap[pInfo->m_userId] = pInfo;

		MYSQL* m = m_dbsession.GetMysql();

		if (m == NULL)
		{
			LLOG_ERROR("UserMatchInfo::LoadUserInfo mysql null");
			return;
		}
		std::stringstream ss;
		ss << "insert into matchresult(userId, level, time, playState, playType) VALUES ( ";
		ss << "\'" << info.m_userId << "\',";
		ss << "\'" << info.m_season.m_level << "\',";
		ss << "\'" << info.m_season.m_time << "\',";
		ss << "\'" << info.m_season.m_playState << "\',";
		ss << "\'" << info.m_season.m_playType << "\');";

		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("UserManager::LoadUserIdInfo sql error %s", mysql_error(m));
			return;
		}
	}
	else
	{
		m_userMatchInfoMap[info.m_userId]->m_season = info.m_season;

		MYSQL* m = m_dbsession.GetMysql();

		if (m == NULL)
		{
			LLOG_ERROR("UserMatchInfo::LoadUserInfo mysql null");
			return;
		}
		std::stringstream ss;
		ss << "UPDATE matchresult set ";
		ss << "level = \'" << info.m_season.m_level << "\',";
		ss << "time = \'" << info.m_season.m_time << "\',";
		ss << "playState = \'" << info.m_season.m_playState << "\',";
		ss << "playType = \'" << info.m_season.m_playType << "\' ";
		ss << "where userId = \'" << info.m_userId << "\';";

		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("UserManager::LoadUserIdInfo sql error %s", mysql_error(m));
			return;
		}
	}
}