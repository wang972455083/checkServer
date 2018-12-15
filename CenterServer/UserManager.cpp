#include "UserManager.h"
#include "DbServer.h"
#include "LLog.h"
#include "LMsgS2S.h"
#include "Work.h"

bool UserManager::Init()
{
	LoadUserInfo();
	LoadActivityInfo();
	return true;
}

void UserManager::LoadUserIdInfo(Lint serverID)
{
	UserIdInfo info;
	std::map<Lstring, DUser*>::iterator it = m_userMap.begin();
	for(; it != m_userMap.end(); ++it)
	{	
		LMsgCe2LUserIdInfo send;
		info.m_id = it->second->m_usert.m_id;
		info.m_nike = it->second->m_usert.m_nike;
		info.m_headImageUrl = it->second->m_usert.m_headImageUrl;
		info.m_openId = it->second->m_usert.m_openid;
		info.m_sex = it->second->m_usert.m_sex;
		info.m_blockEndTime = it->second->m_usert.m_blockEndTime;
		send.m_info.push_back(info);
		gWork.SendMsgToLogic(send, serverID);
	}
}

void UserManager::LoadUserInfo()
{
	//加载所有的玩家
	//DbTool tool;
	MYSQL* m = gWork.GetDbSession().GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("UserManager::LoadUserIdInfo mysql null");
		return;
	}

	Lint limitfrom = 0;
	Lint count = 500000;

	while(true)
	{
		std::stringstream ss;
		//ss << "select Id,OpenId,Nike,Sex,Provice,City,Country,HeadImageUrl,UnionId,NumsCard1,NumsCard2,NumsCard3,LastLoginTime,RegTime,New,Gm,TotalCardNum,TotalPlayNum,last_relief_time,phoneNumber,serverID FROM user ORDER BY Id DESC LIMIT ";
		ss << "select u.Id,u.OpenId,u.Nike,u.Sex,u.Provice,u.City,u.Country,u.HeadImageUrl,u.UnionId,u.NumsCard1,u.NumsCard2,u.NumsCard3,";
		ss << "u.LastLoginTime,u.RegTime,u.New,u.Gm,u.TotalCardNum,u.TotalPlayNum,u.last_relief_time,u.phoneNumber, u.serverID, coalesce(bu.StartTime, 0), coalesce(bu.EndTime, 0)";
		ss << " FROM user as u left join block_user as bu on u.id=bu.UserID ORDER BY u.Id DESC LIMIT ";
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
			DUser* user = new DUser();
			user->m_usert.m_id = atoi(*row++);
			user->m_usert.m_openid = *row++;
			user->m_usert.m_nike = *row++;
			user->m_usert.m_sex = atoi(*row++);
			user->m_usert.m_provice = *row++;
			user->m_usert.m_city = *row++;
			user->m_usert.m_country = *row++;
			user->m_usert.m_headImageUrl = *row++;
			user->m_usert.m_unioid = *row++;
			user->m_usert.m_numOfCard1s = atoi(*row++);
			user->m_usert.m_numOfCard2s = atoi(*row++);
			user->m_usert.m_numOfCard3s = atoi(*row++);
			user->m_usert.m_lastLoginTime = atoi(*row++);
			user->m_usert.m_regTime = atoi(*row++);
			user->m_usert.m_new = atoi(*row++);
			user->m_usert.m_gm = atoi(*row++);
			user->m_usert.m_totalbuynum = atoi(*row++);
			user->m_usert.m_totalplaynum = atoi(*row++);
			user->m_usert.m_last_relief_time = atoi(*row++);
			user->m_usert.m_phoneNumber = *row++;
			user->m_usert.m_nAppId = atoi(*row++);
			user->m_usert.m_blockStartTime = atoi(*row++);
			user->m_usert.m_blockEndTime = atoi(*row++);

			AddUser(user);
			row = mysql_fetch_row(res);
		}
		mysql_free_result(res);
	}

	LLOG_ERROR("UserManager::LoadUserIdInfo user count %d", limitfrom);
}



bool UserManager::Final()
{
	return true;
}

void UserManager::SaveUser(DUser* user)
{
	//DbTool tool;
	MYSQL* m = gWork.GetDbSession().GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("UserManager::SaveUser mysql null");
		return;
	}

	//查询数据库
	std::stringstream ss;
	ss << "UPDATE user SET ";
	ss << "Nike='"<<  user->m_usert.m_nike <<"',";
	ss << "Sex='"<<  user->m_usert.m_sex <<"',";
	ss << "Provice='"<<  user->m_usert.m_provice <<"',";
	ss << "City='"<<  user->m_usert.m_city <<"',";
	ss << "Country='"<<  user->m_usert.m_country<<"',";
	ss << "HeadImageUrl='"<<  user->m_usert.m_headImageUrl<<"',";
	ss << "NumsCard1='"<<  user->m_usert.m_numOfCard1s<<"',";
	ss << "NumsCard2='"<<  user->m_usert.m_numOfCard2s<<"',";
	ss << "NumsCard3='"<<  user->m_usert.m_numOfCard3s<<"',";
	ss << "LastLoginTime='"<<  user->m_usert.m_lastLoginTime<<"',";
	ss << "TotalCardNum='" << user->m_usert.m_totalbuynum << "',";
	ss << "New='"<<  user->m_usert.m_new<< "',";
	ss << "last_relief_time='"<<  user->m_usert.m_last_relief_time<< "',";
	ss << "TotalPlayNum='"<<  user->m_usert.m_totalplaynum<< "',";
	ss << "phoneNumber='"<<  user->m_usert.m_phoneNumber<< "',";
	ss << "serverID='"<<  user->m_usert.m_nAppId<<"' WHERE UnionId='";
	ss << user->m_usert.m_unioid << "'";

	LLOG_DEBUG("UserManager::SaveUser sql =%s", ss.str().c_str());

	Lstring* sql = new Lstring(ss.str());
	gDbServer.Push(sql);
}


void UserManager::SaveUserLastLogin(DUser* user)
{
	//DbTool tool;
	MYSQL* m = gWork.GetDbSession().GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("UserManager::SaveUser mysql null");
		return;
	}

	//查询数据库
	std::stringstream ss;
	ss << "UPDATE user SET ";
	ss << "Nike='"<<  user->m_usert.m_nike <<"',";
	ss << "Sex='"<<  user->m_usert.m_sex <<"',";
	ss << "Provice='"<<  user->m_usert.m_provice <<"',";
	ss << "City='"<<  user->m_usert.m_city <<"',";
	ss << "Country='"<<  user->m_usert.m_country<<"',";
	ss << "HeadImageUrl='"<<  user->m_usert.m_headImageUrl<<"',";
	ss << "NumsCard1='"<<  user->m_usert.m_numOfCard1s<<"',";
	ss << "NumsCard2='"<<  user->m_usert.m_numOfCard2s<<"',";
	ss << "NumsCard3='"<<  user->m_usert.m_numOfCard3s<<"',";
	ss << "LastLoginTime='"<<  user->m_usert.m_lastLoginTime<<"',";
	ss << "Gm='" << user->m_usert.m_gm << "',";
	ss << "New='"<<  user->m_usert.m_new<< "',";
	ss << "TotalCardNum='" << user->m_usert.m_totalbuynum << "',";
	ss << "TotalPlayNum='" << user->m_usert.m_totalplaynum << "',";
	ss << "location='" << user->m_usert.m_location << "' ";
	ss << " WHERE UnionId='" << user->m_usert.m_unioid << "'";

	LLOG_DEBUG("UserManager::SaveUser sql =%s", ss.str().c_str());
	
	Lstring* sql = new Lstring(ss.str());
	gDbServer.Push(sql);
}

DUser* UserManager::GetUserByOpenId(Lstring openid)
{
	if (m_userMap.count(openid))
	{
		LLOG_DEBUG("UserManager::GetUserByOpenId %s", openid.c_str());
		return m_userMap[openid];
	}

	LLOG_DEBUG("UserManager::GetUserByOpenId null %s", openid.c_str());
	return NULL;
}

DUser* UserManager::GetUserById(Lint userId)
{
	for (auto it = m_userMap.begin(); it != m_userMap.end(); it++)
	{
		if (it->second->m_usert.m_id == userId)
		{
			LLOG_DEBUG("UserManager::GetUserById %d", userId);
			return it->second;
		}
	}

	LLOG_DEBUG("UserManager::GetUserById null %d", userId);
	return NULL;
}

void UserManager::DelUserByOpenId(Lstring openid)
{
	if (m_userMap.count(openid))
	{
		m_userMap.erase(m_userMap.find(openid));
	}
}

Lint UserManager::CreateUser(DUser* user)
{
	//DbTool tool;
	MYSQL* m = gWork.GetDbSession().GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("UserManager::CreateUser mysql null");
		return 0;
	}

	//指定随机的id	
	Lint nInsertID = GetRandInsertIDFromDB();
	if (nInsertID == 0)
	{
		LLOG_ERROR("UserManager::RandInsertID==0");
		return 0;
	}

	std::stringstream ss;
	ss << "INSERT INTO user (Id,OpenId,Nike,Sex,Provice,City,Country,HeadImageUrl,UnionId,NumsCard1,NumsCard2,NumsCard3,LastLoginTime,RegTime,New,Gm,TotalCardNum,TotalPlayNum,phoneNumber,serverID) VALUES (";
	ss << "'" << nInsertID << "',";
	ss << "'" << user->m_usert.m_openid << "',";
	ss << "'" << user->m_usert.m_nike << "',";
	ss << "'" << user->m_usert.m_sex << "',";
	ss << "'" << user->m_usert.m_provice << "',";
	ss << "'" << user->m_usert.m_city << "',";
	ss << "'" << user->m_usert.m_country << "',";
	ss << "'" << user->m_usert.m_headImageUrl << "',";
	ss << "'" << user->m_usert.m_unioid << "',";
	ss << "'" << user->m_usert.m_numOfCard1s << "',";
	ss << "'" << user->m_usert.m_numOfCard2s << "',";
	ss << "'" << user->m_usert.m_numOfCard3s << "',";
	ss << "'" << user->m_usert.m_lastLoginTime<< "',";
	ss << "'" << user->m_usert.m_regTime << "',";
	ss << "'" << user->m_usert.m_new << "',";
	ss << "'" << user->m_usert.m_gm << "',";
	ss << "'" << user->m_usert.m_totalbuynum << "',";
	ss << "'" << user->m_usert.m_totalplaynum << "',";
	ss << "'" << user->m_usert.m_phoneNumber << "',";
	ss << "'" << user->m_usert.m_nAppId << "'";
	ss << ")";

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::CreateUser sql error %s", ss.str().c_str());
		return 0;
	}

	user->m_usert.m_id = (Lint)mysql_insert_id(m);
	return user->m_usert.m_id;
}

void UserManager::AddUser(DUser* user)
{
	if (!m_userMap.count(user->m_usert.m_unioid))
	{
		m_userMap[user->m_usert.m_unioid] = user;
	}
}

Lint UserManager::GetRandInsertIDFromDB()
{
	MYSQL* m = gWork.GetDbSession().GetMysql();
	if (m == NULL)
	{
		LLOG_ERROR("UserManager::GetRandInsertIDFromDB mysql null");
		return 0;
	}
	////////////////////////////////////////////////////////
	Lint nid = 0;
	Lint nInsertID = 0;
	static Lstring  SQLSelect = "select Id, UserId from user_id_table order by Id  limit 1";
	if (mysql_real_query(m, SQLSelect.c_str(), SQLSelect.size()))
	{
		LLOG_ERROR("GetRandInsertIDFromDB1  error: %s  Sql: %s", mysql_error(m), SQLSelect.c_str());
		return 0;
	}

	MYSQL_RES* res = mysql_store_result(m);
	if (!res)
	{
		LLOG_ERROR("GetRandInsertIDFromDB2  error: %s ", mysql_error(m));
		return 0;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		mysql_free_result(res);
		LLOG_ERROR("GetRandInsertIDFromDB3  user_id_table Empty!");
		return 0;
	}
	nid = atoi(*row++);
	nInsertID = atoi(*row);
	mysql_free_result(res);
	std::stringstream ss;
	ss << "delete from user_id_table where Id = " << nid;
	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("GetRandInsertIDFromDB4 sql error: %s  Sql: %s", mysql_error(m), ss.str().c_str());
		return 0;
	}
	return nInsertID;
}

void UserManager::LoadActivityInfo()
{
	MYSQL* m = gWork.GetDbSession().GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("UserManager::LoadUserIdInfo mysql null");
		return;
	}

	Lint limitfrom = 0;
	Lint count = 500000;

	while(true)
	{
		std::stringstream ss;
		ss << "select Id,userid,fetchCnt,lastFetchTime FROM Activity ORDER BY Id DESC LIMIT ";
		ss << limitfrom << "," << count;

		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("UserManager::LoadActivityInfo sql error %s", mysql_error(m));
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
			ActivityFenXiangInfo* info = new ActivityFenXiangInfo();
			int id = atoi(*row++);
			info->m_userId = atoi(*row++);
			info->m_fetchCnt = atoi(*row++);
			info->m_lastFetchTime = atoi(*row++);

			m_fenXiangInfo[info->m_userId]=info;
			row = mysql_fetch_row(res);
		}
		mysql_free_result(res);
	}

	LLOG_ERROR("UserManager::LoadActivityInfo user count %d", limitfrom);
}

void UserManager::AddFenXiangInfo(ActivityFenXiangInfo* info)
{
	if(NULL == info)
		return ;

	m_fenXiangInfo[info->m_userId]=info;
}

bool UserManager::InstertFenXiangInfo(ActivityFenXiangInfo* info)
{
	if(NULL == info)
		return false;

	MYSQL* m = gWork.GetDbSession().GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("UserManager::CreateUser mysql null");
		return false;
	}

	std::stringstream ss;
	ss << "INSERT INTO Activity (userid,fetchCnt,lastFetchTime) VALUES (";
	ss << "'" << info->m_userId << "',";
	ss << "'" << info->m_fetchCnt << "',";
	ss << "'" << info->m_lastFetchTime << "'";
	ss << ")";

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::CreateUser sql error %s", ss.str().c_str());
		return false;
	}

	AddFenXiangInfo(info);
	return true;
}

void UserManager::SaveFenXiangInfo(ActivityFenXiangInfo* info)
{
	MYSQL* m = gWork.GetDbSession().GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("UserManager::SaveUser mysql null");
		return;
	}

	//查询数据库
	std::stringstream ss;
	ss << "UPDATE Activity SET ";
	ss << "userid='"<<  info->m_userId <<"',";
	ss << "fetchCnt='"<< info->m_fetchCnt <<"',";
	ss << "lastFetchTime='"<<  info->m_lastFetchTime <<"'";
	ss << " WHERE userid='" << info->m_userId << "'";
	
	LLOG_DEBUG("UserManager::SaveUser sql =%s", ss.str().c_str());

	Lstring* sql = new Lstring(ss.str());
	gDbServer.Push(sql);
}

ActivityFenXiangInfo* UserManager::GetActivityFenXiangInfo(Lint userId)
{
	std::map<Lint,ActivityFenXiangInfo*>::iterator iter = m_fenXiangInfo.find(userId);
	if(iter != m_fenXiangInfo.end())
		return iter->second;

	return NULL;
}