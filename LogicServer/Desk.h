#pragma once

#include "LBase.h"

#include "User.h"
#include <map>
#include "LSingleton.h"

class Room;

class DeskUser
{
public:
	DeskUser();

	void Clear();

public:
	int m_user_id;
	CARD_TYPE m_select_card;

	CARD_TYPE m_tip_card;  //Ã· æµƒ≈∆

	int		  m_pos;

	bool	 m_robot;
};



class Desk
{

public:
	int						m_desk_id;
	
	std::vector<std::shared_ptr<DeskUser> > m_parters;
	int						m_star;
	int						m_cur_pos;
	int						m_select_left_time;
	int						m_type;
	Room*					m_room;
public:

	Desk();
	void SetRoom(Room* m_room);
	bool AddUser(int user_id, bool robot);

public :
	void Tick();
	int GetRandCard();
	
	bool StartDesk();
	bool IsFull();
	bool IsEnd();
	bool IsCardType(Lint card);
	void FillDeskMsg(MsgDesk& send);

	void TurnPos();

	std::shared_ptr<DeskUser>	GetDeskUser(int user_id);
	void DeskBrocast(LMsgSC& msg);
	bool EndDesk(std::map<int,int>& map_stars);


	virtual bool SelectCard(int user_id,int card);
	virtual void CheckTurnPos(std::shared_ptr<DeskUser> user);

	int GetWinner()
	{

	}

};

typedef std::shared_ptr<Desk>	LDeskPtr;

class NormalDesk : public Desk
{
public:
	//NormalDesk();

	void GetResult();
	//bool SelectCard(Lint user_id, Lint card);
};


;
class DeskFactory: public LSingleton<DeskFactory>
{
public :
	bool Init();
	bool Final();

	LDeskPtr GetDesk(DeskType type);
};