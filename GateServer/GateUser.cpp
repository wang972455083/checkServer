#include "GateUser.h"
#include "LLog.h"
#include "LTime.h"
bool GateUserManager::Init()
{
	
	return true;
}

bool GateUserManager::Final()
{
	return true;
}


GUserPtr GateUserManager::GetUserById(Lint user_id)
{
	auto iter = m_gateUserIdMap.find(user_id);
	if (iter!= m_gateUserIdMap.end())
	{
		return iter->second;
	}

	return NULL;
}

GUserPtr GateUserManager::GetUserBySp(LSocketPtr sp)
{
	auto iter = m_gateUserSpMap.find(sp);
	if (iter != m_gateUserSpMap.end())
	{
		return iter->second;
	}

	return NULL;
}

void GateUserManager::AddUserToSpMap(GUserPtr user)
{
	
	m_gateUserSpMap[user->m_sp] = user;
}

void GateUserManager::AddUserToUidMap(GUserPtr user)
{

	m_gateUserIdMap[user->m_user_id] = user;
}

void GateUserManager::DelUser(GUserPtr user)
{
	if (nullptr == user)
		return;

	int user_id = user->getUserId();
	LSocketPtr sp = user->m_sp;
	if (sp)
	{
		auto it = m_gateUserSpMap.find(sp);
		if (it != m_gateUserSpMap.end())
		{
			m_gateUserSpMap.erase(it);
		}
	}

	
	auto iter1 = m_gateUserIdMap.find(user_id);
	
	if (iter1!=m_gateUserIdMap.end())
	{
		m_gateUserIdMap.erase(iter1);
	}
}

GUserPtr GateUserManager::CreateUser(LSocketPtr sp)
{
	GUserPtr user = std::make_shared<GateUser>();
	
	user->m_sp = sp;
	user->m_online = true;

	AddUserToSpMap(user);
	LLOG_DEBUG("GateUserManager::CreateUser AddUser ");
	return user;
}

void GateUserManager::Login(GUserPtr user, int user_id)
{
	user->m_user_id = user_id;

	AddUserToUidMap(user);
}



