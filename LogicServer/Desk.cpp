#include "Desk.h"


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
}


Desk::Desk()
{
	m_desk_id = 0;
	m_star = 0;
	m_parters.clear();

	m_cur_pos = 0;
	m_select_left_time = 0;

	m_type = DT_COMMON;
}

bool Desk::StartDesk(LUserPtr user1, LUserPtr user2)
{
	if (user1 == nullptr || user2 == nullptr)
		return false;

	if (!m_parters.empty())
		return false;

	user1->SetDeskId(m_desk_id);
	user2->SetDeskId(m_desk_id);
	

	std::shared_ptr<DeskUser>  desk_user1 = std::make_shared<DeskUser>();
	desk_user1->m_user_id = user1->GetUserId();
	m_parters.push_back(desk_user1);

	std::shared_ptr<DeskUser>  desk_user2 = std::make_shared<DeskUser>();
	desk_user2->m_user_id = user2->GetUserId();
	m_parters.push_back(desk_user2);

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
		int pos = m_parters.size();
		for (int i = 0; i < m_parters.size(); ++i)
		{
			m_parters[i]->m_pos = pos - i;
		}
	}
	
	m_cur_pos = 1;
	m_select_left_time = 15;

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

bool Desk::SelectCard(Lint user_id, Lint card)
{
	std::shared_ptr<DeskUser> desk_user = GetDeskUser(user_id);
	if (desk_user == nullptr)
		return false;

	if (m_cur_pos != desk_user->m_pos)
		return false;

	if (desk_user->m_select_card != CARD_NULL)
		return false;

	if (!IsCardType(card))
		return false;



	return false;
}

void Desk::GetResult()
{

}

NormalDesk::NormalDesk()
{

}

void NormalDesk::GetResult()
{
	/*if (!IsEnd())
		return;

	Partener p1 = m_parters[0];
	Partener p2 = m_parters[1];

	if (p1.m_select_card == p2.m_select_card)
	{
		//ƽ��
	}
	else
	{
		if (p1.m_select_card == CARD_BU && p2.m_select_card == CARD_SHI_TOU)
		{
			//p1Ӯ
		}
		else if (p1.m_select_card == CARD_SHI_TOU && p2.m_select_card == CARD_BU)
		{
			//p2Ӯ
		}
		else
		{
			if (p1.m_select_card > p2.m_select_card)
			{
				//p1Ӯ
			}
			else
			{
				//p2Ӯ
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
