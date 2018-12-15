#ifndef _DESK_MANAGER_H
#define _DESK_MANAGER_H

#include "LBase.h"
#include "LSingleton.h"
#include "LMsgS2S.h"



//只保存ID和logicserverid
class Desk
{
public:
	Desk()
	{
		Init();
	}

	void Init()
	{
		m_desk_id = 0;

		m_logic_server_id = 0;
	}
	
public:
	Lint m_desk_id;
	
	Lint m_logic_server_id;
};

typedef std::shared_ptr<Desk>  DeskPtr;


class DeskManager:public LSingleton<DeskManager>
{
public:
	virtual	bool	Init();
	virtual	bool	Final();

	//void			RecycleDeskId(LMsgL2LMGRecyleDeskID* msg);

	Lint			GetFreeDeskId();

	Lint			getServerIDbyDeskID(Lint nDeskID);

	void			RecycleDumpServer(Lint nLogicID);

	Lint			GetUserDeskID(Lint user_id);

	DeskPtr			CreateDesk(Lint desk_id);
	bool			DeleteDesk(int desk_id);
	DeskPtr			GetDesk(int desk_id);
	void			RecycleDeskID(Lint desk_id);  //回收desk_id
	void			AddUserToDesk(Lint desk_id, Lint user_id);
	void			DelUserToDesk(Lint desk_id, Lint user_id);
	
private:
	std::queue<Lint>	m_FreeDeskList;
	
	std::queue<Lint>	m_sharevideoId;

	std::map<Lint, Lint>		m_user_desk;
	//所有创建的桌子
	std::map<Lint, DeskPtr>		m_desks;
	
};
#define gDeskManager DeskManager::Instance()

#endif