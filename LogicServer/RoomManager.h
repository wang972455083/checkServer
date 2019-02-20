#pragma once
#include "LBase.h"
#include "LSingleton.h"
#include "LMsg.h"

#include <vector>

#include "Desk.h"









class Room
{
public:
	Room(int room_id);
	~Room();
	void Clear();

	bool AddUser(LUserPtr user);
	bool DelUser(Lint user_id);

	LUserPtr GetRoomUser(Lint user_id);

	void Tick();

	int GetRoomId()
	{
		return m_room_id;
	}

	bool IsInGame()
	{
		return m_room_status == RS_GAME;
	}
	
	bool IsFull()
	{
		return m_room_user.size() == ROOM_USER_CNT;
	}

	bool IsEmpty()
	{
		return m_room_user.size() == 0;
	}

	int GetRoomStatus()
	{
		return m_room_status;
	}

	std::vector<LUserPtr> GetAllUsers()
	{
		return m_room_user;
	}

public:
	void CheckStart();
	void StartGame();


	bool CheckRoomOver();
	void GameOver();
	void Broadcast(LMsgSC& msg);

	void CaculStar();

public:
	void	SendRoomUserModify(LUserPtr user, ROOMUSER_MODIFY type);

public:
	LDeskPtr			CreateDesk(int star);
	LDeskPtr			GetDesk(int desk_id);
	void				DestoryDesk(int desk_id);

	void				CheckDeskEnd(LDeskPtr desk);
	void				AddUserToDesk(LDeskPtr desk, LUserPtr user);

public:
	void				AutoCreateDesk(int ask_id, int opponent_id, int star);
	void				HanderCreateDeskRespon(LMsgC2SCreateDeskRespon* msg);
	void				HanderSelectCard(LMsgC2SSelectCard* msg);
	void				HanderAutoSelectCard(LMsgL2LAutoSelectCard* msg);
public:
	void				FillRoomInfo(int user_id,RoomMsg& msg);
private:
	Lint m_room_id;
	std::vector<LUserPtr>				m_room_user;
	std::map<Lint, LDeskPtr>			m_runing_desks;		//正在进行的桌子
	std::vector<LDeskPtr>				m_desks_record;		//结束的桌子的记录

	Lint							m_room_status;
	Lint							m_create_time; //创建房间的时间
	
	Lint							m_new_desk_id;	//创建下一个竞技的id
};

typedef std::shared_ptr<Room>	LRoomPtr;

class RoomManager : public LSingleton<RoomManager>
{
public:
	virtual	bool	Init();
	virtual	bool	Final();

public:
	void Tick();

public:
	LRoomPtr		GetRoomById(Lint room_id);
	LRoomPtr		CreateRoom(Lint room_id);

	bool			IsHaveRoom(int user_id);

	bool			AddUserToRoom(Lint room_id, Lint user_id);
	bool			AddUserToRoom(LRoomPtr room, LUserPtr user);

	void			DeleteUserToRoom(LRoomPtr room, Lint user_id);
	bool			DeleteRoom(Lint room_id);

private:
	std::map<Lint, LRoomPtr>	m_map_rooms;

	//玩家对应的桌子id
	std::map<Lint, Lint>		m_user_room;
};


#define gRoomManager   RoomManager::Instance()