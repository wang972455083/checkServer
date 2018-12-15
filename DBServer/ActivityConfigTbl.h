#ifndef _ACTIVITY_CONFIG_TBL_H
#define _ACTIVITY_CONFIG_TBL_H

#include<utility>
#include "LSingleton.h"

class ActivityConfigTbl:public LSingleton<ActivityConfigTbl>
{
public:
	ActivityConfigTbl();
	~ActivityConfigTbl();

	virtual bool Init();
	virtual bool Final();

public:
	bool LoadActivityConfg();
	bool IsZhuanPanRedpacketActivityOpen();
	
private:
	bool	m_bZhuanPanRedpacketOpen;	//转盘红包活动开关
};

#endif