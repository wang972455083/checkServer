#ifndef _USER_MANAGER_H_
#define _USER_MANAGER_H_

#include "LSingleton.h"
#include "DUser.h"

class ActivityFenXiangInfo
{
public:
	Lint	m_userId;
	Lint	m_fetchCnt;
	Lint	m_lastFetchTime;

	ActivityFenXiangInfo()
	{
		m_userId = 0;
		m_fetchCnt = 0;
		m_lastFetchTime = 0;
	}
};

class UserManager:public LSingleton<UserManager>
{
public:
	
	virtual	bool		Init();
	virtual	bool		Final();

	DUser*				GetUserByOpenId(Lstring openid);

	void				DelUserByOpenId(Lstring openid);


	Lint				CreateUser(DUser* user);

	void				SaveUser(DUser* user);

	void				SaveUserLastLogin(DUser* user);

	void				LoadUserInfo();

	void				LoadUserIdInfo(Lint serverID);
	
	void				AddUser(DUser* user);
	static	Lint		GetRandInsertIDFromDB();

	void				LoadActivityInfo();
	void				AddFenXiangInfo(ActivityFenXiangInfo* info);
	bool				InstertFenXiangInfo(ActivityFenXiangInfo* info);
	void				SaveFenXiangInfo(ActivityFenXiangInfo* info);
	ActivityFenXiangInfo*	GetActivityFenXiangInfo(Lint userId);
	DUser*				GetUserById(Lint userId);
private:
	std::map<Lstring, DUser*>	m_userMap;
	std::map<Lint,ActivityFenXiangInfo*>	m_fenXiangInfo;
};

#define gUserManager UserManager::Instance()

#endif