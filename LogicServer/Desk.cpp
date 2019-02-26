#include "Desk.h"
#include "RoomManager.h"

DeskUser::DeskUser()
{
	Clear();
}


void DeskUser::Clear()
{
	m_user_id = 0;
	m_select_card = CARD_NULL;
	m_tip_card = CARD_NULL;
	m_pos = 0;

	m_robot = false;
}


Desk::Desk(int type)
{
	m_desk_id = 0;
	m_star = 0;
	m_parters.clear();

	m_cur_pos = 0;
	m_select_left_time = 0;

	m_type = type;

	m_room = nullptr;
	m_xitong_card = 0;
	m_xitong_stars = 0;
}

Desk::~Desk()
{

}

void Desk::SetRoom(Room* room)
{
	m_room = room;
}

bool Desk::AddUser(int user_id,bool robot)
{
	if (IsFull())
		return false;

	std::shared_ptr<DeskUser>  desk_user = std::make_shared<DeskUser>();
	desk_user->m_user_id = user_id;
	desk_user->m_robot = robot;
	m_parters.push_back(desk_user);

	return true;
}

void Desk::Tick()
{
	if (m_select_left_time)
	{
		m_select_left_time--;
		
		//机器人在一半时间自动选择卡牌
		if (m_select_left_time == MAX_SELECT_TIME / 2)
		{
			for (int i = 0; i < m_parters.size(); ++i)
			{
				if (m_parters[i]->m_pos == m_cur_pos)
				{
					if (!IsCardType(m_parters[i]->m_select_card) && m_parters[i]->m_robot)
					{
						
						LMsgL2LAutoSelectCard* send = new LMsgL2LAutoSelectCard();
						send->m_user_id = m_parters[i]->m_user_id;
						send->m_room_id = m_room->GetRoomId();
						send->m_desk_id = m_desk_id;
						send->m_card = GetRandCard();

						gWork.Push(send);

						m_select_left_time = 0;
						
					}
				}
			}
		}

		//如果玩家一直没有选择卡牌，则在MAX_SELECT_TIME时间后自动选择卡牌
		if (m_select_left_time <= 0)
		{
			for (int i = 0; i < m_parters.size(); ++i)
			{
				if (m_parters[i]->m_pos == m_cur_pos)
				{
					if (!IsCardType(m_parters[i]->m_select_card) && !m_parters[i]->m_robot)
					{

						LMsgL2LAutoSelectCard* send = new LMsgL2LAutoSelectCard();
						send->m_user_id = m_parters[i]->m_user_id;
						send->m_room_id = m_room->GetRoomId();
						send->m_desk_id = m_desk_id;
						send->m_card = GetRandCard();

						gWork.Push(send);

						m_select_left_time = 0;

					}
				}
			}
		}
	}
}

int Desk::GetRandCard()
{
	int card = rand() % 3;
	card = card + 1;
	return card ;
}

vector<int> Desk::GetRandTwoCard()
{
	int rand_card = GetRandCard();

	vector<int>	cards;
	for (int i = 1; i < 4; ++i)
	{
		if (i != rand_card)
		{
			cards.push_back(i);
		}
	}

	return cards;
}



bool Desk::IsFull()
{
	return m_parters.size() == DESK_USER_CNT;
}

bool Desk::IsEnd()
{
	if (!IsFull())
		return false;

	for (auto it = m_parters.begin();it != m_parters.end();++it)
	{
		if ((*it)->m_select_card == CARD_NULL)
			return false;
	}

	return true;
}

bool Desk::IsCardType(Lint card)
{
	if (card <= CARD_NULL || card > CARD_SHI_TOU)
		return false;

	return true;
}

void Desk::FillDeskMsg(MsgDesk& send)
{
	send.m_star = m_star;
	send.m_desk_id = m_desk_id;
	send.m_cur_pos = m_cur_pos;
	
	for (int i = 0; i < m_parters.size(); ++i)
	{
		MsgDeskUser desk_user;
		desk_user.m_user_id = m_parters[i]->m_user_id;
		desk_user.m_pos = m_parters[i]->m_pos;
		desk_user.m_select_card = m_parters[i]->m_select_card;
		desk_user.m_tip_card = m_parters[i]->m_tip_card;

		send.m_desk_users.push_back(desk_user);
	}

	send.m_select_left_time = m_select_left_time;
	send.m_type = m_type;
}

std::shared_ptr<DeskUser>	Desk::GetDeskUser(int user_id)
{
	for (int i = 0; i < m_parters.size(); ++i)
	{
		if (m_parters[i]->m_user_id == user_id)
			return m_parters[i];
	}

	return nullptr;
}

bool Desk::StartDesk()
{

	m_cur_pos++;
	m_select_left_time = MAX_SELECT_TIME;


	int nRand = rand() % 2;

	if (nRand)
	{
		for (int i = 0; i < m_parters.size(); ++i)
		{
			m_parters[i]->m_pos = i + 1;
		}
	}
	else
	{
		for (int i = 0; i < m_parters.size(); ++i)
		{
			m_parters[i]->m_pos = m_parters.size() - i;
		}
	}

	SetTipCard();

	LMsgS2CNoticeCreateDeskResult send;
	send.m_result = 0;
	FillDeskMsg(send.m_desk);

	DeskBrocast(send);

	return true;
}

bool Desk::SelectCard(int user_id, int card)
{
	if (m_room == nullptr)
		return false;

	LMsgS2CSelectCard send;
	send.m_room_id = m_room->GetRoomId();
	send.m_desk_id = m_desk_id;
	send.m_card = card;
	send.m_operator_user_id = user_id;

	std::shared_ptr<DeskUser> desk_user = GetDeskUser(user_id);
	if (desk_user == nullptr)
	{
		return false;
	}

	int result = 0;
	do
	{
		if (m_cur_pos != desk_user->m_pos)
		{
			result =  LMsgS2CSelectCard::RT_POS_ERROR;
			break;
		}
			

		if (IsCardType(desk_user->m_select_card))
		{
			result = LMsgS2CSelectCard::RT_CARD_ERROR;
			break;
		}
			

		if (!IsCardType(card))
		{
			result = LMsgS2CSelectCard::RT_CARD_ERROR;
			break;
		}
		
		desk_user->m_select_card = CARD_TYPE(card);
		
	} while (0);
	
	if (result == 0)
	{

		send.m_result = result;

		DeskBrocast(send);

		CheckTurnPos(desk_user);
	}
	else
	{
		LUserPtr user = m_room->GetRoomUser(user_id);
		if (user)
		{
			user->Send(send);
		}
	}


	return (result == 0);
}



void Desk::CheckTurnPos(std::shared_ptr<DeskUser> user)
{
	if (m_cur_pos >= MAX_POS)
		return;

	if (user->m_pos != m_cur_pos)
		return;

	if (user->m_select_card == CARD_NULL)
		return;

	TurnPos();
}

void Desk::DeskBrocast(LMsgSC& msg)
{
	for (int i = 0; i < m_parters.size(); ++i)
	{
		if (m_room)
		{
			LUserPtr user = m_room->GetRoomUser(m_parters[i]->m_user_id);
			if (user)
			{
				user->Send(msg);
			}
		}
	}
}

void Desk::TurnPos()
{
	m_cur_pos++;
	m_select_left_time = MAX_SELECT_TIME;


	LMsgS2CTurnDeskPos send;
	send.m_desk_id = m_desk_id;
	send.m_cur_pos = m_cur_pos;
	send.m_left_select_time = m_select_left_time;

	DeskBrocast(send);
}

int Desk::GetLoseCardByTargetCatd(int target_card)
{
	switch (target_card)
	{
	case CARD_BU:
		return CARD_SHI_TOU;
		break;
	case CARD_SHI_TOU:
		return CARD_JIAN_DAO;
		break;
	case CARD_JIAN_DAO:
		return CARD_BU;
	}

	return 0;
}

void Desk::SetTipCard()
{

}


bool TypeOneDesk::EndDesk(std::map<int, int>& map_stars)
{
	if (m_room == nullptr)
		return false;

	std::shared_ptr<DeskUser> user1 = m_parters[0];
	std::shared_ptr<DeskUser> user2 = m_parters[1];

	LMsgS2CDeskOverResult send;
	send.m_desk_id = m_desk_id;

	if (user1->m_select_card == user2->m_select_card)
	{
		//平局
		send.m_winner = 0;

		MsgUserResult result;
		result.m_user_id = user1->m_user_id;
		result.m_select_card = user1->m_select_card;
		result.m_stars = 0;
		send.m_results.push_back(result);



		result.m_user_id = user2->m_user_id;
		result.m_select_card = user2->m_select_card;
		result.m_stars = 0;
		send.m_results.push_back(result);


	}
	else if(GetLoseCardByTargetCatd(user1->m_select_card) == user2->m_select_card)
	{
		send.m_winner = user1->m_user_id;

		int star = 0;

		MsgUserResult result;
		result.m_user_id = user1->m_user_id;
		result.m_select_card = user1->m_select_card;
		result.m_stars = m_star;
		send.m_results.push_back(result);

		result.m_user_id = user2->m_user_id;
		result.m_select_card = user2->m_select_card;
		result.m_stars = -m_star;
		send.m_results.push_back(result);
	}
	else 
	{
		send.m_winner = user2->m_user_id;

		MsgUserResult result;
		result.m_user_id = user2->m_user_id;
		result.m_select_card = user2->m_select_card;
		result.m_stars = m_star;

		send.m_results.push_back(result);

		result.m_user_id = user1->m_user_id;
		result.m_select_card = user1->m_select_card;
		result.m_stars = -m_star;
		send.m_results.push_back(result);
	}
	

	for (int i = 0; i < send.m_results.size(); ++i)
	{
		map_stars[send.m_results[i].m_user_id] = send.m_results[i].m_stars;
	}

	DeskBrocast(send);

	return true;
}


void TypeSecondDesk::SetTipCard()
{
	//系统为位置为1的玩家提示一张牌
	m_xitong_card = GetRandCard();
	for (int i = 0; i < m_parters.size(); ++i)
	{
		if (m_parters[i]->m_pos == POS_1)
		{
			m_parters[i]->m_tip_card = (CARD_TYPE)m_xitong_card;
		}
	}
}

bool TypeSecondDesk::EndDesk(std::map<int, int>& map_stars)
{
	if (!IsFull())
		return false;

	std::shared_ptr<DeskUser> user_pos1 = m_parters[0];
	std::shared_ptr<DeskUser> user_pos2 = m_parters[1];
	if (user_pos1->m_pos != POS_1)
	{
		user_pos1 = m_parters[1];
		user_pos2 = m_parters[0];
	}
	
	LMsgS2CDeskOverResult send;
	send.m_desk_id = m_desk_id;

	if (user_pos1->m_select_card == user_pos2->m_select_card)
	{
		//平局
		send.m_winner = 0;

		MsgUserResult result;
		result.m_user_id = user_pos1->m_user_id;
		result.m_select_card = user_pos1->m_select_card;
		result.m_stars = 0;
		send.m_results.push_back(result);



		result.m_user_id = user_pos2->m_user_id;
		result.m_select_card = user_pos2->m_select_card;
		result.m_stars = 0;
		send.m_results.push_back(result);


	}
	else if (GetLoseCardByTargetCatd(user_pos1->m_select_card) == user_pos2->m_select_card)
	{
		send.m_winner = user_pos1->m_user_id;

		int star = m_star;
		if (user_pos1->m_select_card == user_pos1->m_tip_card)
		{
			star += m_xitong_stars;
		}

		MsgUserResult result;
		result.m_user_id = user_pos1->m_user_id;
		result.m_select_card = user_pos1->m_select_card;
		result.m_stars = m_star;
		send.m_results.push_back(result);

		result.m_user_id = user_pos2->m_user_id;
		result.m_select_card = user_pos2->m_select_card;
		result.m_stars = -m_star;
		send.m_results.push_back(result);
	}
	else
	{
		send.m_winner = user_pos2->m_user_id;

		MsgUserResult result;
		result.m_user_id = user_pos2->m_user_id;
		result.m_select_card = user_pos2->m_select_card;
		result.m_stars = m_star;

		send.m_results.push_back(result);

		result.m_user_id = user_pos1->m_user_id;
		result.m_select_card = user_pos1->m_select_card;
		result.m_stars = -m_star;
		send.m_results.push_back(result);
	}


	for (int i = 0; i < send.m_results.size(); ++i)
	{
		map_stars[send.m_results[i].m_user_id] = send.m_results[i].m_stars;
	}

	DeskBrocast(send);

	return true;
}


void TypeThirdDesk::SetTipCard()
{
	//系统为每个玩家提示一张牌，并且相同
	m_xitong_card = GetRandCard();
	for (int i = 0; i < m_parters.size(); ++i)
	{
		m_parters[i]->m_tip_card = (CARD_TYPE)m_xitong_card;
		
	}
}

bool TypeThirdDesk::EndDesk(std::map<int, int>& map_stars)
{
	if (!IsFull())
		return false;

	std::shared_ptr<DeskUser> user1 = m_parters[0];
	std::shared_ptr<DeskUser> user2 = m_parters[1];


	LMsgS2CDeskOverResult send;
	send.m_desk_id = m_desk_id;

	if (user1->m_select_card == user2->m_select_card)
	{
		//平局
		send.m_winner = 0;

		int star = 0;
		if (user1->m_select_card == m_xitong_card)
		{
			star = m_star / 2;
		}
		else
		{
			star = -(m_star / 2);
		}

		MsgUserResult result;
		result.m_user_id = user1->m_user_id;
		result.m_select_card = user1->m_select_card;
		result.m_stars = star;
		send.m_results.push_back(result);



		result.m_user_id = user2->m_user_id;
		result.m_select_card = user2->m_select_card;
		result.m_stars = star;
		send.m_results.push_back(result);


	}
	else if (GetLoseCardByTargetCatd(user1->m_select_card) == user2->m_select_card)
	{
		send.m_winner = user1->m_user_id;

		int star = m_star;
		if (user1->m_select_card == m_xitong_card || user2->m_select_card == m_xitong_card)
		{
			star += m_xitong_stars;
		}

		MsgUserResult result;
		result.m_user_id = user1->m_user_id;
		result.m_select_card = user1->m_select_card;
		result.m_stars = star;
		send.m_results.push_back(result);

		result.m_user_id = user2->m_user_id;
		result.m_select_card = user2->m_select_card;
		result.m_stars = -m_star;
		send.m_results.push_back(result);
	}
	else
	{
		send.m_winner = user2->m_user_id;

		int star = m_star;
		if (user1->m_select_card == m_xitong_card || user2->m_select_card == m_xitong_card)
		{
			star += m_xitong_stars;
		}

		MsgUserResult result;
		result.m_user_id = user2->m_user_id;
		result.m_select_card = user2->m_select_card;
		result.m_stars = star;

		send.m_results.push_back(result);

		result.m_user_id = user1->m_user_id;
		result.m_select_card = user1->m_select_card;
		result.m_stars = -m_star;
		send.m_results.push_back(result);
	}


	for (int i = 0; i < send.m_results.size(); ++i)
	{
		map_stars[send.m_results[i].m_user_id] = send.m_results[i].m_stars;
	}

	DeskBrocast(send);

	return true;
}


void TypeFourthDesk::SetTipCard()
{
	//系统为每个玩家提示一张牌，并且相同
	vector<int> cards = GetRandTwoCard();
	if (cards.size() != m_parters.size())
		return;

	for (int i = 0; i < m_parters.size(); ++i)
	{
		m_parters[i]->m_tip_card = (CARD_TYPE)cards[i];

	}
}

bool TypeFourthDesk::EndDesk(std::map<int, int>& map_stars)
{
	if (!IsFull())
		return false;

	std::shared_ptr<DeskUser> user1 = m_parters[0];
	std::shared_ptr<DeskUser> user2 = m_parters[1];


	LMsgS2CDeskOverResult send;
	send.m_desk_id = m_desk_id;

	if (user1->m_select_card == user2->m_select_card)
	{
		//平局
		send.m_winner = 0;

		int star1 = 0;
		int star2 = 0;
		if (user1->m_select_card == user1->m_tip_card)
		{
			star1 = m_xitong_stars / 2;
		}
		else if(user2->m_select_card == user2->m_tip_card)
		{
			star2 = m_xitong_stars / 2;
		}

		MsgUserResult result;
		result.m_user_id = user1->m_user_id;
		result.m_select_card = user1->m_select_card;
		result.m_stars = star1;
		send.m_results.push_back(result);



		result.m_user_id = user2->m_user_id;
		result.m_select_card = user2->m_select_card;
		result.m_stars = star2;
		send.m_results.push_back(result);


	}
	else if (GetLoseCardByTargetCatd(user1->m_select_card) == user2->m_select_card)
	{
		send.m_winner = user1->m_user_id;

		int star1 = 0;
		int star2 = 0;

		if (user1->m_select_card == user1->m_tip_card && user2->m_select_card == user2->m_tip_card)
		{
			star1 = m_xitong_card;
			star2 = m_xitong_card / 2;
		}
		else if (user1->m_select_card != user1->m_tip_card && user2->m_select_card != user2->m_tip_card)
		{
			star1 = m_star;
			star2 = -m_star;
		}
		else
		{
			star1 = m_star + m_xitong_card / 2;
			star2 = -m_star;
		}

		MsgUserResult result;
		result.m_user_id = user1->m_user_id;
		result.m_select_card = user1->m_select_card;
		result.m_stars = star1;
		send.m_results.push_back(result);

		result.m_user_id = user2->m_user_id;
		result.m_select_card = user2->m_select_card;
		result.m_stars = star2;
		send.m_results.push_back(result);
	}
	else
	{
		send.m_winner = user2->m_user_id;

		int star1 = 0;
		int star2 = 0;

		if (user1->m_select_card == user1->m_tip_card && user2->m_select_card == user2->m_tip_card)
		{
			star2 = m_xitong_card;
			star1 = m_xitong_card / 2;
		}
		else if (user1->m_select_card != user1->m_tip_card && user2->m_select_card != user2->m_tip_card)
		{
			star2 = m_star;
			star1 = -m_star;
		}
		else
		{
			star2 = m_star + m_xitong_card / 2;
			star1 = -m_star;
		}

		MsgUserResult result;
		result.m_user_id = user2->m_user_id;
		result.m_select_card = user2->m_select_card;
		result.m_stars = star2;
		send.m_results.push_back(result);

		result.m_user_id = user1->m_user_id;
		result.m_select_card = user1->m_select_card;
		result.m_stars = star1;
		send.m_results.push_back(result);
	}


	for (int i = 0; i < send.m_results.size(); ++i)
	{
		map_stars[send.m_results[i].m_user_id] = send.m_results[i].m_stars;
	}

	DeskBrocast(send);

	return true;
}


bool DeskFactory::Init()
{
	return true;
}

bool DeskFactory::Final()
{
	return true;
}

LDeskPtr DeskFactory::GetDesk(int  type)
{

	switch (type)
	{
	case DT_FIRST_TYPE:
		return std::make_shared<TypeOneDesk>();
		break;
	case DT_SECOND_TYPE:
		return std::make_shared<TypeSecondDesk>();
		break;
	case DT_THIRD_TYPE:
		return std::make_shared<TypeThirdDesk>();
		break;
	case DT_FOURTH_TYPE:
		return std::make_shared<TypeFourthDesk>();
		break;
	}
	
	return std::make_shared<TypeOneDesk>();;
}
