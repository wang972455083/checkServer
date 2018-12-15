#include "ActivityConfigTbl.h"
#include "LTabFile.h"
#include "LTime.h"
#include "Work.h"
#include<ctime>


ActivityConfigTbl::ActivityConfigTbl()
{
	m_bZhuanPanRedpacketOpen = false;
	
}

ActivityConfigTbl::~ActivityConfigTbl()
{
	m_bZhuanPanRedpacketOpen = false;
	
}

bool ActivityConfigTbl::Init()
{
	LoadActivityConfg();
	return true;
}

bool ActivityConfigTbl::Final()
{
	return true;
}

bool ActivityConfigTbl::LoadActivityConfg()
{
#if _LINUX
	const char* filename = "settings/ActivityConfig.txt";
#else
	const char* filename = "settings\\ActivityConfig.txt";
#endif
	Lint nIndex = 0;
	LTabFile tabFile;
	std::vector< std::string > lineItem;
	if (tabFile.Load(filename))
	{
		lineItem  = tabFile.getTapFileLine();
		if(lineItem.size() < 2 || lineItem[0] != "ZhuanPanRedpacketActivityopen")
		{
			LLOG_ERROR("LoadActivityConfg Error!");
			return false;
		}
		m_bZhuanPanRedpacketOpen = atoi(lineItem[1].c_str());
		LLOG_DEBUG("LoadActivityConfg m_bOpen=%d", m_bZhuanPanRedpacketOpen);

		return true;
	}
	else
	{
		LLOG_ERROR("LoadActivityConfg Error!");
	}

	return false;
}

bool ActivityConfigTbl::IsZhuanPanRedpacketActivityOpen()
{
	return m_bZhuanPanRedpacketOpen;
}

