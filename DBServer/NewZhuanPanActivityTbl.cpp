#include "NewZhuanPanActivityTbl.h"
#include "LTabFile.h"
#include "LTime.h"
#include "LTool.h"
#include "Work.h"
#include<ctime>


NewZhuanPanActivityTbl::NewZhuanPanActivityTbl()
{
	Clear();
}

NewZhuanPanActivityTbl::~NewZhuanPanActivityTbl()
{
	Clear();
}

bool NewZhuanPanActivityTbl::Init()
{
	Clear();
	LoadNewZhuanPanTbl();
	LoadRewardTbl();
	return true;
}

bool NewZhuanPanActivityTbl::Final()
{
	return true;
}

bool NewZhuanPanActivityTbl::LoadNewZhuanPanTbl()
{
#if _LINUX
	const char* filename = "settings/NewZhuanPanConfig.txt";
#else
	const char* filename = "settings\\NewZhuanPanConfig.txt";
#endif
	Lint nIndex = 0;
	LTabFile tabFile;
	std::vector< std::string > lineItem;
	if (tabFile.Load(filename))
	{
		lineItem  = tabFile.getTapFileLine();
		m_bOpen							= atoi(lineItem[0].c_str());
		m_nMaxWinnerNeedCnt				= atoi(lineItem[1].c_str());
		m_nMaxWinnerDailyDrawMaxCnt		= atoi(lineItem[2].c_str());
		m_nCreateRoomNeedCnt			= atoi(lineItem[3].c_str());
		m_nCreateRoomDailyDrawMaxCnt	= atoi(lineItem[4].c_str());
		m_nContinueLoginNeedCnt			= atoi(lineItem[5].c_str());
		m_nMaxMoney						= atoi(lineItem[6].c_str());
		
		LLOG_DEBUG("LoadNewZhuanPanTbl Error!%d-%d-%d-%d-%d-%d",m_nMaxWinnerNeedCnt,
			m_nMaxWinnerDailyDrawMaxCnt,m_nCreateRoomNeedCnt,m_nCreateRoomDailyDrawMaxCnt,
			m_nContinueLoginNeedCnt,m_nMaxMoney);

		return true;
	}
	else
	{
		LLOG_ERROR("LoadNewZhuanPanTbl Error!");
	}

	return false;
}

bool NewZhuanPanActivityTbl::LoadRewardTbl()
{
#if _LINUX
	const char* filename = "settings/zhuanPanReward.txt";
#else
	const char* filename = "settings\\zhuanPanReward.txt";
#endif
	Lint nIndex = 0;
	LTabFile tabFile;
	std::vector< std::string > lineItem;
	if (tabFile.Load(filename))
	{
		lineItem  = tabFile.getTapFileLine();
		while (lineItem.size() >= 4 )
		{
			RewardCfg* pReward = new RewardCfg();
			
			pReward->m_rewardId = atoi(lineItem[0].c_str());
			pReward->m_redwardType = atoi(lineItem[1].c_str());
			pReward->m_redwardCount = atoi(lineItem[2].c_str());
			pReward->m_redwardWeight = atoi(lineItem[3].c_str());

			m_nMaxWeight += pReward->m_redwardWeight;
			if(pReward->m_redwardType == RT_CARD)
			{
				m_nMaxCardWeight += pReward->m_redwardWeight;
			}
			m_vecReward.push_back(pReward);


			lineItem.clear();
			lineItem  = tabFile.getTapFileLine();

			LLOG_DEBUG("NewZhuanPanActivityTbl Suceed!");
			
		}	
	
		return true;
	}
	else
	{
		LLOG_ERROR("NewZhuanPanActivityTbl Error!");
	}

	return false;
}

void NewZhuanPanActivityTbl::Clear()
{
	m_bOpen = false;
	m_nMaxWinnerNeedCnt			= 0;
	m_nMaxWinnerDailyDrawMaxCnt = 0;
	m_nCreateRoomNeedCnt		= 0;
	m_nCreateRoomDailyDrawMaxCnt = 0;
	m_nContinueLoginNeedCnt		= 0;
	m_nMaxMoney = 0;
	m_nMaxWeight = 0;
	m_nMaxCardWeight = 0;
	
	for(int i=0;i<m_vecReward.size();++i)
	{
		delete m_vecReward[i];
	}
	m_vecReward.clear();
}

RewardCfg* NewZhuanPanActivityTbl::GetReward(bool bOnlyCard)
{
	if(bOnlyCard)
	{
		int nKey = L_Rand(0,m_nMaxCardWeight);
		int nWeight = 0;
		for(int i=0;i<m_vecReward.size();++i)
		{
			if(m_vecReward[i]->m_redwardType == RT_CARD)
			{
				nWeight += m_vecReward[i]->m_redwardWeight;

				if(nKey<=nWeight)
					return m_vecReward[i];
			}
		}
	}
	else
	{
		int nKey = L_Rand(0,m_nMaxWeight);
		int nWeight = 0;
		for(int i=0;i<m_vecReward.size();++i)
		{
			{
				nWeight += m_vecReward[i]->m_redwardWeight;

				if(nKey<=nWeight)
					return m_vecReward[i];
			}
		}
	}

	return NULL;
}