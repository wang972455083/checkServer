#include "DeskManager.h"
#include "Work.h"
#include "LLog.h"

#include "Config.h"

bool DeskManager::Init()
{
	//初始所有的桌子号
	
	std::vector<Lint> deskVector;
	for (Lint desk_id = 100000; desk_id < 999999; ++desk_id)
	{
		deskVector.push_back(desk_id);
	}

	//生成随机队列
	Lint nSize = deskVector.size();
	while(nSize > 0)
	{
		Lint seed1 = L_Rand(0, nSize - 1);
		m_FreeDeskList.push(deskVector[seed1]);
		m_sharevideoId.push(deskVector[seed1]);
		deskVector[seed1] = deskVector[nSize - 1];
		nSize --;
	}

	for (int i = 0; i < deskVector.size(); ++i)
	{
		m_FreeDeskList.push(deskVector[i]);
	}

	return true;
}

bool DeskManager::Final()
{
	return true;
}

Lint DeskManager::GetFreeDeskId()
{
	Lint desk_id = 0;
	Lint nDeskSize = m_FreeDeskList.size();
	LLOG_ERROR("DeskManager::GetFreeDeskId size=%d", nDeskSize);
	if (nDeskSize > 0)
	{
		desk_id = m_FreeDeskList.front();
		m_FreeDeskList.pop();
		
	}
	return desk_id;
}





Lint DeskManager::getServerIDbyDeskID(Lint nDeskID)
{
	/*auto desk = m_DeskLogicID.find(nDeskID);
	if (desk != m_DeskLogicID.end())
	{
		return desk->second;
	}*/
	return 0;
}

void DeskManager::RecycleDumpServer(Lint nLogicID)
{
	/*auto desk = m_DeskLogicID.begin();
	for (; desk != m_DeskLogicID.end();)
	{
		if (desk->second == nLogicID)
		{
			m_FreeDeskList.push(desk->first);
			desk = m_DeskLogicID.erase(desk);
		}
		else
		{
			++ desk;
		}
	}*/
}

Lint	DeskManager::GetUserDeskID(Lint user_id)
{
	int desk_id = 0;
	auto it = m_user_desk.find(user_id);
	if (it != m_user_desk.end())
	{
		desk_id = it->second;
	}

	return desk_id;
}

DeskPtr DeskManager::CreateDesk(Lint desk_id)
{
	DeskPtr desk = std::make_shared<Desk>();
	desk->m_desk_id = desk_id;
	m_desks[desk_id] = desk;
	
	return desk;
}


bool DeskManager::DeleteDesk(int desk_id)
{
	auto it = m_desks.find(desk_id);

	if (it != m_desks.end())
	{
		m_desks.erase(it);

		RecycleDeskID(desk_id);

		return true;
	}

	return false;
}

DeskPtr DeskManager::GetDesk(int desk_id)
{
	auto it = m_desks.find(desk_id);

	if (it != m_desks.end())
		return it->second;

	return nullptr;
}

void DeskManager::RecycleDeskID(Lint desk_id)
{
	m_FreeDeskList.push(desk_id);
}

void DeskManager::AddUserToDesk(Lint desk_id, Lint user_id)
{
	m_user_desk[user_id] = desk_id;
}

void DeskManager::DelUserToDesk(Lint desk_id, Lint user_id)
{
	auto it = m_user_desk.find(user_id);
	if(it != m_user_desk.end())
	{
		m_user_desk.erase(it);
	}
}



