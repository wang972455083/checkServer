#include "ActiveShopCfg.h"
#include "DBC.h"
#include "string-util.h"
#include "distribution.h"
#include <time.h>

const char* ActiveShopCfg::szGiftFile		= "ActiveShop.dat";

/////////////////////////////////////////////////////////////////////////////////////////

bool ActiveShopTbl::IsCurrentTime( ) const
{
	if( m_tBeginTime == 0 && m_tEndTime == 0 )
	{
		return true;
	}

	time_t now = time(NULL);

	if( now >= m_tBeginTime && now <= m_tEndTime )
	{
		return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
ActiveShopCfg::ActiveShopCfg()
{
	ClearTbl();
}

ActiveShopCfg::~ActiveShopCfg()
{
	ClearTbl();
}

bool ActiveShopCfg::LoadTbl()
{
	ClearTbl();

	DBCFile fileDBC(0);
	fileDBC.OpenFromTXT(szGiftFile);
	int nRow = fileDBC.GetRecordsNum();
	for (int i = 0; i < nRow; i++)
	{
		bool bUsing = (fileDBC.Search_Posistion(i, E_ACTIVE_SHOP_USEING	)->iValue == 0) ? false : true;
		ActiveShopTbl* pTbl = new ActiveShopTbl();
		pTbl->m_nID = fileDBC.Search_Posistion(i, E_ACTIVE_SHOP_ID	)->iValue;

		pTbl->m_tBeginTime = GetDayTimeByStr(fileDBC.Search_Posistion(i,E_ACTIVE_SHOP_TIME_BG_D)->pString
			,fileDBC.Search_Posistion(i,E_ACTIVE_SHOP_TIME_BG_T)->pString);
		pTbl->m_tEndTime = GetDayTimeByStr(fileDBC.Search_Posistion(i,E_ACTIVE_SHOP_TIME_ED_D)->pString
			,fileDBC.Search_Posistion(i,E_ACTIVE_SHOP_TIME_ED_T)->pString);

		pTbl->m_nRewardID		= fileDBC.Search_Posistion(i, E_ACTIVE_SHOP_REWARD_ID )->iValue;
		pTbl->m_nActiveFlag	= fileDBC.Search_Posistion(i, E_ACTIVE_SHOP_ACTIVE_FLAG )->iValue;

		pTbl->m_nType	= fileDBC.Search_Posistion(i, E_ACTIVE_SHOP_TYEP )->iValue;
		pTbl->m_nBuyCnt	= fileDBC.Search_Posistion(i, E_ACTIVE_SHOP_BUY_CNT )->iValue;
		pTbl->m_nCost	= fileDBC.Search_Posistion(i, E_ACTIVE_SHOP_COST )->iValue;

		m_mapGift[pTbl->m_nID] = pTbl;
	}
	printf("Load %s cnt:%d\n", szGiftFile, nRow);
	return true;

}

void ActiveShopCfg::ClearTbl()
{
	map<int,ActiveShopTbl*>::iterator iter;
	for(iter = m_mapGift.begin(); iter != m_mapGift.end();iter++)
	{
		delete iter->second;
	}
	m_mapGift.clear();
}


const ActiveShopTbl* ActiveShopCfg::GetActiveShopTbl(int nGiftID)
{
	map<int,ActiveShopTbl*>::iterator iter;
	iter = m_mapGift.find(nGiftID);
	if(iter!=m_mapGift.end())
		return iter->second;
	return NULL;
}

const ActiveShopTbl* ActiveShopCfg::GetActiveShopTblByNum( int nNum )
{
	if ( nNum >= (int)m_mapGift.size() )
	{
		return NULL;
	}
	map<int,ActiveShopTbl*>::iterator iter;
	iter = m_mapGift.begin();

	for ( int i = 0; i < nNum; i++ )
	{
		iter++;
	}
	return iter->second;
}

const ActiveShopTbl* ActiveShopCfg::GetActiveShopTblByType( int nType )
{
	map<int,ActiveShopTbl*>::iterator iter;

	for ( iter = m_mapGift.begin(); iter != m_mapGift.end(); iter++ )
	{
		if ( iter->second == NULL )
		{
			continue;
		}

		if ( iter->second->m_nActiveFlag == nType && iter->second->IsCurrentTime() )
		{
			return iter->second;
		}
	}
	return NULL;
}