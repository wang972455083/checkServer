
#include "ChessBlockFactory.h"
#include "Table/ChessBoardCfg.h"

void ChessBlockFactory::Init()
{
	
}

std::shared_ptr<ChessBlock> ChessBlockFactory::CreateChessBlock(ChessGame* pGame,int table_id, int color)
{
	std::shared_ptr<ChessBlock> pBlock = nullptr;

	ChessBoardTbl* pTbl = ChessBoardCfg::Instance().GetChessBoard(table_id);
	if (pTbl == nullptr)
		return pBlock;
	switch (pTbl->m_type)
	{
	case CHESS_JIANG:
		pBlock = std::shared_ptr<ChessJiang>();
		break;
	case CHESS_SHI:
		pBlock = std::shared_ptr<ChessShi>();
		break;
	case CHESS_XIANG:
		pBlock = std::shared_ptr<ChessXiang>();
		break;
	case CHESS_MA:
		pBlock = std::shared_ptr<ChessMa>();
		break;
	case CHESS_CHE:
		pBlock = std::shared_ptr<ChessChe>();
		break;
	case CHESS_PAO:
		pBlock = std::shared_ptr<ChessPao>();
		break;
	case CHESS_BING:
		pBlock = std::shared_ptr<ChessBing>();
		break;
	}

	if (pBlock == nullptr)
		return nullptr;

	pBlock->setTableId(table_id);
	pBlock->setColor(color);
	pBlock->setType(pTbl->m_type);
	pBlock->setHp(pTbl->m_min_hp);
	pBlock->setAttack(pTbl->m_min_attack);
	pBlock->setMaxHp(pTbl->m_max_hp);
	pBlock->setMaxAttack(pTbl->m_max_attack);
	pBlock->setCoinLevel(pTbl->m_min_coin_level);
	pBlock->setMaxCoinLevel(pTbl->m_max_coin_level);
	pBlock->setGame(pGame);

	return pBlock;
}

