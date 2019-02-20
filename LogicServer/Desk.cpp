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


Desk::Desk()
{
	m_desk_id = 0;
	m_star = 0;
	m_parters.clear();

	m_cur_pos = 0;
	m_select_left_time = 0;

	m_type = DT_COMMON;

	m_room = nullptr;
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

bool Desk::StartDesk()
{
	
	m_cur_pos++;
	m_select_left_time = MAX_SELECT_TIME;
	

	int nRand = rand() % 2;
	
	if (nRand)
	{
		for(int i=0;i<m_parters.size();++i)
		{
			m_parters[i]->m_pos = i + 1;
		}
	}
	else
	{
		for (int i = 0; i < m_parters.size(); ++i)
		{
			m_parters[i]->m_pos = m_parters.size()-i;
		}
	}
	

	LMsgS2CNoticeCreateDeskResult send;
	send.m_result = 0;
	FillDeskMsg(send.m_desk);

	DeskBrocast(send);

	return true;
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

bool Desk::EndDesk(std::map<int, int>& map_stars)
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
	else if (user1->m_select_card == CARD_BU && user2->m_select_card == CARD_SHI_TOU)
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
	else if (user1->m_select_card == CARD_SHI_TOU && user2->m_select_card == CARD_BU)
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
	else
	{
		if (user1->m_select_card > user2->m_select_card)
		{
			//user1
			send.m_winner = user1->m_user_id;

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
			//user2
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
	}

	for (int i = 0; i < send.m_results.size(); ++i)
	{
		map_stars[send.m_results[i].m_user_id] = send.m_results[i].m_stars;
	}

	DeskBrocast(send);

	return true;
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



//NormalDesk::NormalDesk()
//{

//}

void NormalDesk::GetResult()
{
	/*if (!IsEnd())
		return;

	Partener p1 = m_parters[0];
	Partener p2 = m_parters[1];

	if (p1.m_select_card == p2.m_select_card)
	{
		//平局
	}
	else
	{
		if (p1.m_select_card == CARD_BU && p2.m_select_card == CARD_SHI_TOU)
		{
			//p1赢
		}
		else if (p1.m_select_card == CARD_SHI_TOU && p2.m_select_card == CARD_BU)
		{
			//p2赢
		}
		else
		{
			if (p1.m_select_card > p2.m_select_card)
			{
				//p1赢
			}
			else
			{
				//p2赢
			}
		}
	}*/
}


bool DeskFactory::Init()
{
	return true;
}

bool DeskFactory::Final()
{
	return true;
}

LDeskPtr DeskFactory::GetDesk(DeskType type)
{

	switch (type)
	{
	case DT_COMMON:
		return std::make_shared<Desk>();
		break;
	}
	
	return nullptr;
}
