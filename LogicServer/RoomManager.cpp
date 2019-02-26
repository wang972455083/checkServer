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
	m_new_desk_id = 1000;
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
	LUserPtr user = nullptr;

	for (auto it = m_room_user.begin(); it != m_room_user.end();)
	{
		if ((*it)->GetUserId() == user_id)
		{
			user = *it;

			if (user->GetDeskId())
				return  false;
			else
			{
				it = m_room_user.erase(it);
				break;
			}
		}
		else
		{
			++it;
		}
	}

	if (user)
	{
		LMsgS2CQuitRoom send;
		send.m_result = 0;
		send.m_user_id = user->GetUserId();
		user->Send(send);

		SendRoomUserModify(user, MODIFY_DEL_USER);
		

		return true;
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

void Room::Tick()
{
	for (auto it = m_runing_desks.begin(); it != m_runing_desks.end(); ++it)
	{
		it->second->Tick();
	}
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

bool Room::CheckRoomOver()
{
	LUserPtr win_user = nullptr;
	int count = 0;
	for (int i = 0; i < m_room_user.size(); ++i)
	{
		if (m_room_user[i]->GetStar())
		{
			win_user = m_room_user[i];
			count++;
			if (count > 1)
			{
				return false;
			}
		}
	}
	//Ó®¼Ò
	if (count == 1)
	{
		if (win_user)
		{
			LMsgS2CGameOver send;
			send.m_room_id = m_room_id;
			send.m_winner = win_user->GetUserId();
			send.m_stars = win_user->GetStar();

			Broadcast(send);

			return true;
		}
	}
	return false;
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

void Room::SendRoomUserModify(LUserPtr user, ROOMUSER_MODIFY type)
{
	if (user == nullptr)
		return;

	LMsgS2CRoomUserModify send;
	send.m_type = type;
	send.m_room_id = m_room_id;
	send.m_modify_user_id = user->GetUserId();
	send.m_stars = user->GetStar();
	send.m_desk_id = user->GetDeskId();

	Broadcast(send);
	
}

void Room::CaculStar()
{

}

LDeskPtr	Room::CreateDesk(int star)
{

	if (star <= 0)
		return nullptr;

	int desk_type = GetDeskType(star);
	LDeskPtr desk = DeskFactory::Instance().GetDesk(3);
	desk->SetRoom(this);
	desk->m_desk_id = m_new_desk_id;
	desk->m_star = star;

	m_new_desk_id++;

	m_runing_desks[desk->m_desk_id] = desk;

	return desk;
}

int Room::GetDeskType(int star)
{
	
	if (star >=3 )
	{
		int rd = rand() % 4;
		return rd+1;
	}
	else if(star>=2)
	{
		int rd = rand() % 3;
		return rd + 1;
	}
	else
	{
		int rd = rand() % 2;
		return rd + 1;
	}

	return 1;
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

	if (desk->IsEnd())
	{
		std::map<int, int> map_user_stars;
		if (desk->EndDesk(map_user_stars))
		{

			for (auto iter = map_user_stars.begin(); iter != map_user_stars.end(); ++iter)
			{

				LUserPtr user = GetRoomUser(iter->first);
				if (user == nullptr)
					continue;

				user->SetStar(user->GetStar() + iter->second);
				user->SetDeskId(0);

				SendRoomUserModify(user, MODIFY_UPDATE_USER);
			}

			DestoryDesk(desk->m_desk_id);

			CheckRoomOver();
		}

	}
}



void Room::AddUserToDesk(LDeskPtr desk, LUserPtr user)
{
	if (desk == nullptr || user == nullptr)
		return;

	if (user->GetDeskId())
		return;
	
	if (desk->AddUser(user->GetUserId(), user->IsRobot()))
	{
		user->SetDeskId(desk->m_desk_id);
		SendRoomUserModify(user, MODIFY_UPDATE_USER);

		if (desk->IsFull())
		{
			desk->StartDesk();
		}
	}

	return;
}

void Room::AutoCreateDesk(int ask_id, int opponent_id, int star)
{
	LUserPtr user = GetRoomUser(ask_id);
	if (user == nullptr)
		return;

	LUserPtr respon_user = GetRoomUser(opponent_id);
	if (respon_user == nullptr)
		return;

	if (user->GetStar() < star || respon_user->GetStar() < star)
	{
		LMsgS2CNoticeCreateDeskResult send;
		send.m_result = LMsgS2CNoticeCreateDeskResult::RS_STAR_ERROR;
		user->Send(send);
		return;
	}


	if (user->GetDeskId() || respon_user->GetDeskId())
	{
		LMsgS2CNoticeCreateDeskResult send;
		send.m_result = LMsgS2CNoticeCreateDeskResult::RS_IN_DESK;
		user->Send(send);
		return;
	}

	LDeskPtr desk = CreateDesk(star);
	if (desk == nullptr)
		return;

	AddUserToDesk(desk, user);
	AddUserToDesk(desk, respon_user);
}

void Room::HanderCreateDeskRespon(LMsgC2SCreateDeskRespon* msg)
{
	//Í¬Òâ
	
	LUserPtr user = GetRoomUser(msg->m_user_id);
	if (user == nullptr)
		return;

	LUserPtr respon_user = GetRoomUser(msg->m_opponent_id);
	if (respon_user == nullptr)
		return;

	if (user->GetStar() < msg->m_star || respon_user->GetStar() < msg->m_star)
	{
		LMsgS2CNoticeCreateDeskResult send;
		send.m_result = LMsgS2CNoticeCreateDeskResult::RS_STAR_ERROR;
		user->Send(send);
		return;
	}


	if (user->GetDeskId() || respon_user->GetDeskId())
	{
		LMsgS2CNoticeCreateDeskResult send;
		send.m_result = LMsgS2CNoticeCreateDeskResult::RS_IN_DESK;
		user->Send(send);
		return;
	}

	LDeskPtr desk = CreateDesk(msg->m_star);
	if (desk == nullptr)
		return;

	//desk->StartDesk(user, respon_user);
	AddUserToDesk(desk, user);
	AddUserToDesk(desk, respon_user);

}

void Room::HanderSelectCard(LMsgC2SSelectCard* msg)
{
	LUserPtr user = GetRoomUser(msg->m_user_id);
	if (user == nullptr)
		return;

	LDeskPtr desk = GetDesk(msg->m_desk_id);
	if (desk == nullptr)
		return;


	if (desk->SelectCard(msg->m_user_id,msg->m_card))
	{
		CheckDeskEnd(desk);
	}
}


void Room::HanderAutoSelectCard(LMsgL2LAutoSelectCard* msg)
{
	LUserPtr user = GetRoomUser(msg->m_user_id);
	if (user == nullptr)
		return;

	LDeskPtr desk = GetDesk(msg->m_desk_id);
	if (desk == nullptr)
		return;


	if (desk->SelectCard(msg->m_user_id,msg->m_card))
	{
		CheckDeskEnd(desk);
	}
}


void Room::DestoryDesk(int desk_id)
{
	auto iter = m_runing_desks.find(desk_id);
	if (iter != m_runing_desks.end())
	{
		m_runing_desks.erase(iter);
	}
}

void Room::FillRoomInfo(int user_id, RoomMsg& msg)
{
	LUserPtr user = GetRoomUser(user_id);
	if (nullptr == user)
		return;

	msg.m_room_id = m_room_id;
	for (int i = 0; i < m_room_user.size(); ++i)
	{
		LUserPtr user = m_room_user[i];
		RoomUser msg_user;
		msg_user.m_user_id = user->GetUserId();
		msg_user.m_name = user->GetName();
		msg_user.m_head_icon = user->GetHeadIcon();
		msg_user.m_star = user->GetStar();
		msg_user.m_desk_id = user->GetDeskId();

		msg.m_users.push_back(msg_user);
	}

	LDeskPtr desk = GetDesk(user->GetDeskId());
	if (desk)
	{
		desk->FillDeskMsg(msg.m_cur_desk);
	}
}

bool RoomManager::Init()
{
	return true;
}

bool RoomManager::Final()
{
	return true;
}

void RoomManager::Tick()
{
	for (auto iter = m_map_rooms.begin(); iter != m_map_rooms.end(); ++iter)
	{
		iter->second->Tick();
	}
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


