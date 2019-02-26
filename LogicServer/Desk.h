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

	CARD_TYPE m_tip_card;  //提示的牌

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
	int						m_xitong_card;
	int						m_xitong_stars;  //系统奖励
public:

	Desk(int type);
	virtual ~Desk();
	void SetRoom(Room* m_room);
	bool AddUser(int user_id, bool robot);

public :
	void Tick();
	int GetRandCard();  
	vector<int> GetRandTwoCard();
	bool IsCardType(Lint card);
	void FillDeskMsg(MsgDesk& send);
	void DeskBrocast(LMsgSC& msg);
	std::shared_ptr<DeskUser>	GetDeskUser(int user_id);
	bool IsFull();
	void TurnPos();
	bool IsEnd();
	bool SelectCard(int user_id, int card);
	void CheckTurnPos(std::shared_ptr<DeskUser> user);
	int GetDeskType() { return m_type; }
	int GetLoseCardByTargetCatd(int target_card);
	bool StartDesk();
public:


	virtual void SetTipCard();
	virtual bool EndDesk(std::map<int, int>& map_stars) = 0;
};

typedef std::shared_ptr<Desk>	LDeskPtr;

class TypeOneDesk : public Desk
{
public:
	TypeOneDesk() :Desk(DT_FIRST_TYPE)
	{

	}
	~TypeOneDesk()
	{

	}


	bool EndDesk(std::map<int, int>& map_stars);
};

class TypeSecondDesk : public Desk
{
public:
	TypeSecondDesk() :Desk(DT_SECOND_TYPE)
	{
		m_xitong_stars = m_star;
	}
	~TypeSecondDesk()
	{

	}

	void SetTipCard();
	bool EndDesk(std::map<int, int>& map_stars);

private:
	int m_xitong_stars;  //系统奖励
};


class TypeThirdDesk : public Desk
{
public:
	TypeThirdDesk() :Desk(DT_THIRD_TYPE)
	{
		m_xitong_stars = m_star;
	}
	~TypeThirdDesk()
	{

	}

	void SetTipCard();
	bool EndDesk(std::map<int, int>& map_stars);

};

class TypeFourthDesk : public Desk
{
public:
	TypeFourthDesk() :Desk(DT_FOURTH_TYPE)
	{
		m_xitong_stars = m_star;
	}
	~TypeFourthDesk()
	{

	}

	void SetTipCard();
	bool EndDesk(std::map<int, int>& map_stars);
	
};
;
class DeskFactory: public LSingleton<DeskFactory>
{
public :
	bool Init();
	bool Final();

	LDeskPtr GetDesk(int type);
};