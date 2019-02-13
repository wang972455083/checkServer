#include "RoomManager.h"
#include "LMsgS2S.h"
#include "Work.h"



Room::Room(int room_id)
{
	Clear();
	m_room_id = room_id;

	LTime t;
	m_create_time = t.GetSec();
}

Room::~Room()
{
	Clear();
}

void Room::Clear()
{
	m_room_id = 0;
	
	m_room_status = 0;
	m_room_user.clear();
	m_room_status = RS_WAIT;
	m_new_desk_id = 0;
}

bool Room::AddUser(LUserPtr user)
{
	if (IsFull())
		return false;

	m_room_user.push_back(user);

	CheckStart();

	return true;
}

bool Room::DelUser(Lint user_id)
{
	for (auto it = m_room_user.begin(); it != m_room_user.end(); it++)
	{
		if ((*it)->GetUserId() == user_id)
		{
			it = m_room_user.erase(it);

			return true;
		}	
	}

	return false;
}

LUserPtr Room::GetRoomUser(Lint user_id)
{
	for (size_t i = 0; i < m_room_user.size(); i++)
	{
		if (m_room_user[i]->GetUserId() == user_id)
			return m_room_user[i];
	}

	return nullptr;
}

void Room::CheckStart()
{
	if (!IsFull())
		return ;

	StartGame();
}

void Room::StartGame()
{
	m_room_status = RS_GAME;

	LMsgL2LMQuickCreateRoomOpt send;
	send.m_result = 0;
	send.m_room_id = m_room_id;

	for (int i = 0; i < m_room_user.size(); ++i)
	{
		LUserPtr user = m_room_user[i];
		RoomUser msgUser;
		msgUser.m_user_id = user->GetUserId();
		msgUser.m_name = user->GetName();
		msgUser.m_head_icon = user->GetHeadIcon();
		msgUser.m_star = user->GetStar();

		send.m_users.push_back(msgUser);
	}

	gWork.SendToLogicManager(send);

}

void Room::CheckRoomOver()
{
	int not_zero_cnt = 0;
	for (int i = 0; i < m_room_user.size(); ++i)
	{
		if (m_room_user[i]->GetStar())
		{
			not_zero_cnt++;
			if (not_zero_cnt > 1)
			{
				return ;
			}
		}
	}


	
}

void Room::GameOver()
{
	//m_room_status = DS_WAIT;

	for (size_t i = 0; i < m_room_user.size(); ++i)
	{
		//m_room_user[i]->SetStatus(STATUS_NULL);
	}

	//m_game.reset();
}

void Room::Broadcast(LMsgSC& msg)
{
	for (int i = 0; i < m_room_user.size(); ++i)
	{
		std::shared_ptr<User> user = m_room_user[i];
		if (user)
		{
			user->Send(msg);
		}
	}	
}

void Room::CaculStar()
{

}

LDeskPtr	Room::CreateDesk(int star)
{

	if (star <= 0)
		return nullptr;

	m_new_desk_id++;
	LDeskPtr desk = std::make_shared<Desk>();
	desk->m_desk_id = m_new_desk_id;
	desk->m_star = star;

	return desk;
}

LDeskPtr Room::GetDesk(int desk_id)
{
	auto iter = m_runing_desks.find(desk_id);
	if (iter != m_runing_desks.end())
	{
		return iter->second;
	}

	return nullptr;
}

void Room::CheckDeskEnd(LDeskPtr desk)
{
	if (desk == nullptr)
		return;

	/*if (desk->IsEnd())
	{

		int winner = desk->GetWinner();
		for (int i = 0; i < desk->m_parters.size(); i++)
		{
			int user_id = desk->m_parters[i].m_user_id;
			LUserPtr user = GetRoomUser(user_id);
			if (user == nullptr)
				return;

			if (user_id == winner)
			{
				user->SetStar(user->GetStar() + desk->m_star);
			}
			else
			{
				user->SetStar(user->GetStar() - desk->m_star);
			}

			LMsgS2CDeskResult send;
			send.m_desk_id = desk->m_desk_id;
			send.m_winner = winner;
			
			user->Send(send);
		}
		m_desks_record.push_back(desk);

		//不是平局数据有改动
		if (winner)
		{
			LMsgS2CRoomInfoModify modify;
			for (int i = 0; i < desk->m_parters.size(); i++)
			{
				int user_id = desk->m_parters[i].m_user_id;
				std::shared_ptr<User> user = GetRoomUser(user_id);
				if (user == nullptr)
					return;

				RoomUser msgUser;
				msgUser.m_user_id = user->GetUserId();
				msgUser.m_name = user->GetName();
				msgUser.m_head_icon = user->GetHeadIcon();
				msgUser.m_star = user->GetStar();
				msgUser.m_desk_id = user->GetDeskId();

				modify.m_users.push_back(msgUser);


				MsgParter msgPart;
				msgPart.m_user_id = desk->m_parters[i].m_user_id;
				msgPart.m_card = desk->m_parters[i].m_card;
				modify.m_desk_info.m_parters.push_back(msgPart);
			}

			modify.m_desk_info.m_star = desk->m_star;
			
			Broadcast(modify);
		}
		

		auto iter = m_runing_desks.find(desk->m_desk_id);
		if (iter != m_runing_desks.end())
		{
			m_runing_desks.erase(iter);
		}

		if (winner)
		{

		}
	}*/
}

void Room::AddUserToDesk(LDeskPtr desk, LUserPtr user)
{
	//Partener part;
	//part.m_user_id = user->GetUserId();

	//desk->m_parters.push_back(part);
}


bool RoomManager::Init()
{
	return true;
}

bool RoomManager::Final()
{
	return true;
}


LRoomPtr RoomManager::GetRoomById(Lint room_id)
{
	auto it = m_map_rooms.find(room_id);
	if (it != m_map_rooms.end())
		return it->second;

	return nullptr;
}

LRoomPtr RoomManager::CreateRoom(Lint room_id)
{
	LRoomPtr room = std::make_shared<Room>(room_id);

	if (room == nullptr)
		return nullptr;

	m_map_rooms[room_id] = room;

	return room;
}

bool RoomManager::IsHaveRoom(int user_id)
{
	auto it = m_user_room.find(user_id);
	if (it != m_user_room.end())
		return true;

	return false;
}

bool RoomManager::AddUserToRoom(Lint room_id, Lint user_id)
{
	//m_user_room[user_id] = room_id;
	return true;
}

bool RoomManager::AddUserToRoom(LRoomPtr room, LUserPtr user)
{
	if (room == nullptr || user == nullptr)
		return false;

	if (room->AddUser(user))
	{
		m_user_room[user->GetUserId()] = room->GetRoomId();

		return true;
	}

	return false;
}

void RoomManager::DeleteUserToRoom(LRoomPtr room, Lint user_id)
{
	if (room->DelUser(user_id))
	{
		auto iter = m_user_room.find(user_id);
		if (iter != m_user_room.end())
		{
			m_user_room.erase(iter);
		}
	}
}

bool RoomManager::DeleteRoom(Lint room_id)
{
	auto it = m_map_rooms.find(room_id);
	if (it != m_map_rooms.end())
	{
		m_map_rooms.erase(it);

		return true;
	}

	return false;
}


