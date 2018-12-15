#pragma once
#include "SingleBase.h"
#include "ChessBlock.h"
#include "ChessGame.h"


class ChessBlockFactory : public SingleBase<ChessBlockFactory>
{
public:
	void Init();
	std::shared_ptr<ChessBlock> CreateChessBlock(ChessGame* pGame,int table_id, int color);
};

