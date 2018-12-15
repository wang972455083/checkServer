#include "User.h"

User::User(int user_id,Lstring name,Lstring head_icon)
{
	m_user_id = user_id;
	m_name = name;
	m_head_icon = head_icon;

	m_status = 0;
	m_gate_id = 0;
	m_init_chess.clear();
}

User::~User()
{
	Clear();
}

void User::Clear()
{
	m_user_id = 0;
	m_name = "";

	m_status = 0;
	m_init_chess.clear();
}

Lint User::GetUserId()
{
	return m_user_id;
}


void User::SetUserId(Lint user_id)
{
	m_user_id = user_id;
}



Lint User::GetStatus()
{
	return m_status;
}

void User::SetStatus(Lint status)
{
	m_status = status;
}









