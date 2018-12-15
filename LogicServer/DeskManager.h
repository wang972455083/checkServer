#pragma once
#include "LBase.h"
#include "LSingleton.h"
//#include "ChessGame.h"
#include "LMsg.h"

#include <vector>
#include "User.h"

#include "ChessGame.h"

class Desk
{
public:
	Desk();
	~Desk();
	void Clear();

	bool AddUser(LUserPtr user);
	bool DelUser(Lint user_id);

	LUserPtr GetDeskUser(Lint user_id);

	bool IsInChess()
	{
		return m_desk_status == DS_CHESS;
	}
	
	bool IsFull()
	{
		return m_desk_user.size() == DESK_FULL_USER_CNT;
	}

	bool IsEmpty()
	{
		return m_desk_user.size() == 0;
	}

	int GetDeskStatus()
	{
		return m_desk_status;
	}

	std::vector<LUserPtr> GetAllUsers()
	{
		return m_desk_user;
	}

public:
	void CheckStart();
	void StartGame();

	void GameOver();

public:
	void HanderChessMove(LMsgC2SChessMove* msg);
	void HanderChessAttack(LMsgC2SChessAttack* msg);
	void HanderChessUpgrade(LMsgC2SChessUpgrade* msg);
public:
	Lint m_desk_id;
	Lint m_desk_type;
	Lint m_cost;

private:

	std::vector<LUserPtr>			m_desk_user;
	std::shared_ptr<ChessGame>		m_game;
	Lint							m_desk_status;
};

typedef std::shared_ptr<Desk>	LDeskPtr;

class DeskManager : public LSingleton<DeskManager>
{
public:
	virtual	bool	Init();
	virtual	bool	Final();

public:
	LDeskPtr		GetDeskById(Lint desk_id);
	LDeskPtr		CreateDesk(Lint desk_id);

	bool			IsHaveDesk(int user_id);

	bool			AddUserToDesk(Lint desk_id, Lint user_id);
	bool			AddUserToDesk(LDeskPtr desk, LUserPtr user);

	void			DeleteUserToDesk(LDeskPtr desk, Lint user_id);
	bool			DeleteDesk(Lint desk_id);

private:
	std::map<Lint, LDeskPtr>	m_map_desks;

	//玩家对应的桌子id
	std::map<Lint, Lint>		m_user_desk;
};


#define gDeskManager   DeskManager::Instance()