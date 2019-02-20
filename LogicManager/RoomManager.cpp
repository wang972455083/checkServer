#include "RoomManager.h"
#include "Work.h"
#include "LLog.h"

#include "Config.h"

bool RoomManager::Init()
{
	//初始所有的桌子号
	
	std::vector<Lint> RoomVector;
	for (Lint Room_id = 100000; Room_id < 999999; ++Room_id)
	{
		RoomVector.push_back(Room_id);
	}

	//生成随机队列
	Lint nSize = RoomVector.size();
	while(nSize > 0)
	{
		Lint seed1 = L_Rand(0, nSize - 1);
		m_FreeRoomList.push(RoomVector[seed1]);
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
	auto it = m_Rooms.find(nRoomID);
	if (it != m_Rooms.end())
	{
		return it->second->m_logic_server_id;
	}
	return 0;
}

void RoomManager::RecycleDumpServer(Lint nLogicID)
{
	for (auto it = m_Rooms.begin(); it != m_Rooms.end(); )
	{
		if (it->second->m_logic_server_id == nLogicID)
		{
			RecycleRoomID(it->first);

			it = m_Rooms.erase(it);
		}
		else
		{
			++it;
		}
	}
}


RoomPtr RoomManager::CreateRoom(Lint room_id,int logic_server_id)
{
	RoomPtr room = std::make_shared<Room>();
	room->m_room_id = room_id;
	room->m_logic_server_id = logic_server_id;
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




