#include "UserManager.h"

User::User(int user_id)
{
	m_user_id = user_id;

	m_room_id = 0;
	m_user_sp = nullptr;

	m_quick_start = false;
}

User::~User()
{

}

Lint User::GetUserId()
{
	return m_user_id;
}

void User::SetUserSp(LSocketPtr sp)
{
	m_user_sp = sp;
}

LSocketPtr User::GetUserSp()
{
	return m_user_sp;
}

void User::SetName(Lstring name)
{
	m_name = name;
}

Lstring User::GetName()
{
	return m_name;
}

void User::SetHeadIcon(Lstring head_icon)
{
	m_head_icon = head_icon;
}

Lstring User::GetHeadIcon()
{
	return m_head_icon;
}

bool UserManager::Init()
{
	return true;
}

bool UserManager::Final()
{
	return false;
}

void UserManager::AddUser(LUserPtr user)
{
	m_map_users[user->GetUserId()] = user;
}

void UserManager::DeleteUser(int user_id)
{
	auto iter = m_map_users.find(user_id);

	if (iter != m_map_users.end())
	{
		m_map_users.erase(iter);
	}
}

LUserPtr UserManager::GetUserById(int user_id)
{
	auto iter = m_map_users.find(user_id);

	if (iter != m_map_users.end())
	{
		return iter->second;
	}

	return nullptr;
}


