#include "DeskManager.h"

Desk::Desk()
{
	Clear();
}

Desk::~Desk()
{
	Clear();
}

void Desk::Clear()
{
	m_desk_id = 0;
	m_desk_type = 0;
	m_cost = 0;
	m_desk_status = 0;
	m_desk_user.clear();

	//if (m_game == nullptr)
	//{
		//m_game = std::shared_ptr<ChessGame>();
	//}

	//m_game.Clear();
}

bool Desk::AddUser(LUserPtr user)
{
	if (IsFull())
		return false;

	//LUserPtr user = std::shared_ptr<User>();
	//user->SetUserId(user_id);
	
	m_desk_user.push_back(user);

	return true;
}

bool Desk::DelUser(Lint user_id)
{
	for (auto it = m_desk_user.begin(); it != m_desk_user.end(); it++)
	{
		if ((*it)->GetUserId() == user_id)
			it = m_desk_user.erase(it);
	}

	return true;
}

LUserPtr Desk::GetDeskUser(Lint user_id)
{
	for (size_t i = 0; i < m_desk_user.size(); i++)
	{
		if (m_desk_user[i]->GetUserId() == user_id)
			return m_desk_user[i];
	}

	return nullptr;
}

void Desk::CheckStart()
{
	if (!IsFull())
		return ;

	for (auto it = m_desk_user.begin(); it != m_desk_user.end(); it++)
	{
		if ((*it)->GetStatus() != STATUS_READY)
			return;
	}

	//¿ªÊ¼Æå¾Ö
	StartGame();
}

void Desk::StartGame()
{
	if (m_game == nullptr)
		m_game = std::shared_ptr<ChessGame>();

	if (m_game == nullptr)
		return;

	m_game->Start(this);

	m_desk_status = DS_CHESS;
}

void Desk::GameOver()
{
	m_desk_status = DS_WAIT;

	for (size_t i = 0; i < m_desk_user.size(); ++i)
	{
		m_desk_user[i]->SetStatus(STATUS_NULL);
	}

	m_game.reset();
}

void Desk::HanderChessMove(LMsgC2SChessMove* msg)
{
	if (m_game)
	{
		m_game->HanderChessMove(msg);
	}
}

void Desk::HanderChessAttack(LMsgC2SChessAttack* msg)
{
	if (m_game)
	{
		m_game->HanderChessAttack(msg);
	}
}

void Desk::HanderChessUpgrade(LMsgC2SChessUpgrade* msg)
{
	if (m_game)
	{
		m_game->HanderChessUpgrade(msg);
	}
}


bool DeskManager::Init()
{
	return true;
}

bool DeskManager::Final()
{
	return true;
}


LDeskPtr DeskManager::GetDeskById(Lint desk_id)
{
	auto it = m_map_desks.find(desk_id);
	if (it != m_map_desks.end())
		return it->second;

	return nullptr;
}

LDeskPtr DeskManager::CreateDesk(Lint desk_id)
{
	LDeskPtr desk = std::make_shared<Desk>();

	if (desk == nullptr)
		return nullptr;

	desk->m_desk_id = desk_id;

	m_map_desks[desk_id] = desk;

	return desk;
}

bool DeskManager::IsHaveDesk(int user_id)
{
	auto it = m_user_desk.find(user_id);
	if (it != m_user_desk.end())
		return true;

	return false;
}

bool DeskManager::AddUserToDesk(Lint desk_id, Lint user_id)
{
	//m_user_desk[user_id] = desk_id;
	return true;
}

bool DeskManager::AddUserToDesk(LDeskPtr desk, LUserPtr user)
{
	if (desk == nullptr || user == nullptr)
		return false;

	if (desk->AddUser(user))
	{
		m_user_desk[user->GetUserId()] = desk->m_desk_id;

		return true;
	}

	return false;
}

void DeskManager::DeleteUserToDesk(LDeskPtr desk, Lint user_id)
{
	if (desk->DelUser(user_id))
	{
		auto iter = m_user_desk.find(user_id);
		if (iter != m_user_desk.end())
		{
			m_user_desk.erase(iter);
		}
	}
}

bool DeskManager::DeleteDesk(Lint desk_id)
{
	auto it = m_map_desks.find(desk_id);
	if (it != m_map_desks.end())
	{
		m_map_desks.erase(it);

		return true;
	}

	return false;
}


