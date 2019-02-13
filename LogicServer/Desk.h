#include "LBase.h"

#include "User.h"
#include <map>
#include "LSingleton.h"

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
public:

	Desk();
	bool StartDesk(LUserPtr user1, LUserPtr user2);
	bool IsFull();
	bool IsEnd();
	bool IsCardType(Lint card);
	void FillDeskMsg(MsgDesk& send);
	std::shared_ptr<DeskUser>	GetDeskUser(int user_id);


	virtual bool SelectCard(Lint user_id, Lint card);
	


public:
	virtual void GetResult();

};

typedef std::shared_ptr<Desk>	LDeskPtr;

class NormalDesk : public Desk
{
public:
	NormalDesk();

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