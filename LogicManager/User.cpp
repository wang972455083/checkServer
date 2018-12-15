#include "UserManager.h"

User::User()
{
	Clear();
}

User::~User()
{
	Clear();
}

void User::Clear()
{
	m_user_id = 0;
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




std::vector<Lint>&	User::GetInitChessBoard()
{
	return m_init_chess;
}

void User::SetInitChessBoard(std::vector<Lint> init_chess)
{
	m_init_chess = init_chess;
}

