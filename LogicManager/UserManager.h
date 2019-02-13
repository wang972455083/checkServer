#pragma once

#include "LBase.h"
#include "LSingleton.h"

#include "LMsg.h"
#include <map>


class User
{
public:
	User(int user_id);
	~User();

	Lint GetUserId();
	void SetRoomId(int room_id);
	Lint GetRoomId();

	void SetUserSp(LSocketPtr sp);
	LSocketPtr GetUserSp();

	void SetName(Lstring name);
	Lstring GetName();

	void SetHeadIcon(Lstring head_icon);
	Lstring GetHeadIcon();

	void SetQuickStartStatus(bool quick_start)
	{
		m_quick_start = quick_start;
	}

	bool IsQuickStart()
	{
		return m_quick_start;
	}

private:
	Lint		m_user_id;
	Lint		m_room_id;
	Lstring		m_name;
	Lstring		m_head_icon;

	LSocketPtr	m_user_sp;

	bool		m_quick_start;
};

typedef  std::shared_ptr<User>   LUserPtr;

class UserManager :public LSingleton<UserManager>
{
public:
	virtual bool Init();
	virtual bool Final();

	void		 AddUser(LUserPtr user);
	void		 DeleteUser(int user_id);
	LUserPtr		 GetUserById(int user_id);

private:
	std::map<Lint, LUserPtr>    m_map_users;
};

#define gUserManager UserManager::Instance()