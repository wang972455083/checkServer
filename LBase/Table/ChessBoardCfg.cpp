#include "ChessBoardCfg.h"

#include "DBC.h"
#include "string-util.h"
#include "distribution.h"
#include <time.h>

const char* ChessBoardCfg::szChessBoardFile = "table/chessboard.dat";




ChessBoardTbl::ChessBoardTbl()
{
	m_id = 0;
	m_type = 0;
	m_min_hp = 0;
	m_max_hp = 0;
	m_min_attack = 0;
	m_max_attack = 0;

	m_min_coin_level = 0;
	m_max_coin_level = 0;

	m_upgrade_hp_cost = 0;
	m_upgrad_attack_cost = 0;
	m_upgrade_coin_level_cost.clear();
	m_grade = 0;
}


ChessBoardTbl::~ChessBoardTbl()
{
}

bool ChessBoardCfg::Init()
{
	return true;
}

bool ChessBoardCfg::Final()
{
	ClearTbl();
	return true;
}

bool ChessBoardCfg::LoadTbl()
{
	ClearTbl();

	DBCFile fileDBC(0);
	fileDBC.OpenFromTXT(szChessBoardFile);
	int nRow = fileDBC.GetRecordsNum();
	for (int i = 0; i < nRow; i++)
	{
		//bool bUsing = (fileDBC.Search_Posistion(i, E_ACTIVE_SHOP_USEING)->iValue == 0) ? false : true;
		ChessBoardTbl* pTbl = new ChessBoardTbl();
		pTbl->m_id = fileDBC.Search_Posistion(i, 1)->iValue;

		//pTbl->m_tBeginTime = GetDayTimeByStr(fileDBC.Search_Posistion(i, E_ACTIVE_SHOP_TIME_BG_D)->pString
		//	, fileDBC.Search_Posistion(i, E_ACTIVE_SHOP_TIME_BG_T)->pString);
		pTbl->m_type = fileDBC.Search_Posistion(i, 2)->iValue;
		pTbl->m_min_hp = fileDBC.Search_Posistion(i, 3)->iValue;
		pTbl->m_max_hp = fileDBC.Search_Posistion(i, 4)->iValue;
		pTbl->m_min_attack = fileDBC.Search_Posistion(i, 5)->iValue;
		pTbl->m_max_attack = fileDBC.Search_Posistion(i, 6)->iValue;
		pTbl->m_min_coin_level = fileDBC.Search_Posistion(i, 7)->iValue;
		pTbl->m_max_coin_level = fileDBC.Search_Posistion(i, 8)->iValue;
		pTbl->m_upgrade_hp_cost = fileDBC.Search_Posistion(i, 9)->iValue;
		pTbl->m_upgrad_attack_cost = fileDBC.Search_Posistion(i, 10)->iValue;

		string strlevel_cost = fileDBC.Search_Posistion(i, 11)->pString;
		tokenize_as_int(strlevel_cost, pTbl->m_upgrade_coin_level_cost, ",");

		pTbl->m_grade = fileDBC.Search_Posistion(i, 12)->iValue;

		m_mapBoards[pTbl->m_id] = pTbl;
	}
	printf("Load %s cnt:%d\n", szChessBoardFile, nRow);
	return true;

}

void ChessBoardCfg::ClearTbl()
{
	
	for (auto iter = m_mapBoards.begin(); iter != m_mapBoards.end(); iter++)
	{
		delete iter->second;
	}
	m_mapBoards.clear();
}


ChessBoardTbl* ChessBoardCfg::GetChessBoard(int chess_id)
{
	auto it = m_mapBoards.find(chess_id);
	if (it != m_mapBoards.end())
	{
		return it->second;
	}

	return nullptr;
}


