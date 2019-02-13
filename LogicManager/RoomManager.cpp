#include "RoomManager.h"
#include "Work.h"
#include "LLog.h"

#include "Config.h"

bool RoomManager::Init()
{
	//��ʼ���е����Ӻ�
	
	std::vector<Lint> RoomVector;
	for (Lint Room_id = 100000; Room_id < 999999; ++Room_id)
	{
		RoomVector.push_back(Room_id);
	}

	//�����������
	Lint nSize = RoomVector.size();
	while(nSize > 0)
	{
		Lint seed1 = L_Rand(0, nSize - 1);
		m_FreeRoomList.push(RoomVector[seed1]);
		m_sharevideoId.push(RoomVector[seed1]);
		RoomVector[seed1] = RoomVector[nSize - 1];
		nSize --;
	}

	for (int i = 0; i < RoomVector.size(); ++i)
	{
		m_FreeRoomList.push(RoomVector[i]);
	}

	return true;
}

bool RoomManager::Final()
{
	return true;
}

Lint RoomManager::GetFreeRoomId()
{
	Lint Room_id = 0;
	Lint nRoomSize = m_FreeRoomList.size();
	LLOG_ERROR("RoomManager::GetFreeRoomId size=%d", nRoomSize);
	if (nRoomSize > 0)
	{
		Room_id = m_FreeRoomList.front();
		m_FreeRoomList.pop();
		
	}
	return Room_id;
}





Lint RoomManager::getServerIDbyRoomID(Lint nRoomID)
{
	/*auto Room = m_RoomLogicID.find(nRoomID);
	if (Room != m_RoomLogicID.end())
	{
		return Room->second;
	}*/
	return 0;
}

void RoomManager::RecycleDumpServer(Lint nLogicID)
{
	/*auto Room = m_RoomLogicID.begin();
	for (; Room != m_RoomLogicID.end();)
	{
		if (Room->second == nLogicID)
		{
			m_FreeRoomList.push(Room->first);
			Room = m_RoomLogicID.erase(Room);
		}
		else
		{
			++ Room;
		}
	}*/
}

Lint	RoomManager::GetUserRoomID(Lint user_id)
{
	int Room_id = 0;
	auto it = m_user_Room.find(user_id);
	if (it != m_user_Room.end())
	{
		Room_id = it->second;
	}

	return Room_id;
}

RoomPtr RoomManager::CreateRoom(Lint room_id)
{
	RoomPtr room = std::make_shared<Room>();
	room->m_room_id = room_id;
	m_Rooms[room_id] = room;
	
	return room;
}


bool RoomManager::DeleteRoom(int Room_id)
{
	auto it = m_Rooms.find(Room_id);

	if (it != m_Rooms.end())
	{
		m_Rooms.erase(it);

		RecycleRoomID(Room_id);

		return true;
	}

	return false;
}

RoomPtr RoomManager::GetRoom(int Room_id)
{
	auto it = m_Rooms.find(Room_id);

	if (it != m_Rooms.end())
		return it->second;

	return nullptr;
}

void RoomManager::RecycleRoomID(Lint Room_id)
{
	m_FreeRoomList.push(Room_id);
}

void RoomManager::AddUserToRoom(Lint Room_id, Lint user_id)
{
	m_user_Room[user_id] = Room_id;
}

void RoomManager::DelUserToRoom(Lint Room_id, Lint user_id)
{
	auto it = m_user_Room.find(user_id);
	if(it != m_user_Room.end())
	{
		m_user_Room.erase(it);
	}
}



