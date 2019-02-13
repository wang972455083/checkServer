#ifndef _ROOM_MANAGER_H
#define _ROOM_MANAGER_H

#include "LBase.h"
#include "LSingleton.h"
#include "LMsgS2S.h"



//ֻ����ID��logicserverid
class Room
{
public:
	Room()
	{
		Init();
	}

	void Init()
	{
		m_room_id = 0;

		m_logic_server_id = 0;
	}
	
public:
	Lint m_room_id;
	
	Lint m_logic_server_id;
};

typedef std::shared_ptr<Room>  RoomPtr;


class RoomManager:public LSingleton<RoomManager>
{
public:
	virtual	bool	Init();
	virtual	bool	Final();

	//void			RecycleRoomId(LMsgL2LMGRecyleRoomID* msg);

	Lint			GetFreeRoomId();

	Lint			getServerIDbyRoomID(Lint nRoomID);

	void			RecycleDumpServer(Lint nLogicID);

	Lint			GetUserRoomID(Lint user_id);

	RoomPtr			CreateRoom(Lint Room_id);
	bool			DeleteRoom(int Room_id);
	RoomPtr			GetRoom(int Room_id);
	void			RecycleRoomID(Lint Room_id);  //����Room_id
	void			AddUserToRoom(Lint Room_id, Lint user_id);
	void			DelUserToRoom(Lint Room_id, Lint user_id);
	
private:
	std::queue<Lint>	m_FreeRoomList;
	
	std::queue<Lint>	m_sharevideoId;

	std::map<Lint, Lint>		m_user_Room;
	//���д���������
	std::map<Lint, RoomPtr>		m_Rooms;
	
};
#define gRoomManager RoomManager::Instance()

#endif