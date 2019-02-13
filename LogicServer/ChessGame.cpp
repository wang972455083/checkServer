
#include "ChessGame.h"
#include "ChessBlockFactory.h"
#include "ChessManager.h"
#include "Table/ChessBoardCfg.h"
#include "Work.h"
//#include "DeskManager.h"


ChessGame::ChessGame()
{
}


ChessGame::~ChessGame()
{
	
}

void ChessGame::Start(Desk* desk)
{
	m_desk = desk;

	if (m_desk)
	{
		Init();
	}
}


void ChessGame::Init()
{
	/*if (m_desk == nullptr)
		return;

	std::vector<LUserPtr> user = m_desk->GetAllUsers();
	if (user.size() != 2)
		return;

	
	m_map_fighters.clear();
	for (int i = 0; i < user.size(); ++i)
	{
		std::shared_ptr<ChessFighter> fighter = std::make_shared<ChessFighter>();
		fighter->m_user_id = user[i]->GetUserId();
		CHESS_COLOR color = CHESS_COLOR(CHESS_RED + i);
		
		m_map_fighters[color] = fighter;
	}
	
	for (int i = 0; i < POS_LIMIT_X; ++i)
	{
		for (int j = 0; j < POS_LIMIT_Y; j++)
		{
			m_Chess[i][j] = 0;
		}
	}

	m_Color = int(CHESS_RED);
	

	InitChessPos();*/
}

void ChessGame::InitChessPos()
{

	std::vector<Point>	vecPos;
	m_map_blocks.clear();

	int nId = 10000;
	for (int i=CHESS_JIANG; i<CHESS_MAX; ++i)
	{
		vecPos.clear();
		TYPE_CHESS type = TYPE_CHESS(i);
		vecPos = ChessManager::getInstance().GetPoints(CHESS_RED, type);
		int table_id = 1000 + i;

		for (size_t j = 0; j < vecPos.size(); ++j)
		{
			std::shared_ptr<ChessBlock> pRedBlock = ChessBlockFactory::getInstance().CreateChessBlock(this, table_id, CHESS_RED);
			pRedBlock->setPos(vecPos[j]);
			nId++;
			pRedBlock->setId(nId);
			m_map_blocks[nId] = pRedBlock;
			m_Chess[vecPos[j].m_posX][vecPos[j].m_posY] = nId;
		}
	}

	nId = 20000;
	for (int i = CHESS_JIANG; i < CHESS_MAX; ++i)
	{
		vecPos.clear();
		TYPE_CHESS type = TYPE_CHESS(i);
		vecPos = ChessManager::getInstance().GetPoints(CHESS_RED, type);
		int table_id = 1000 + i;

		for (size_t j = 0; j < vecPos.size(); ++j)
		{
			std::shared_ptr<ChessBlock> pBlackBlock = ChessBlockFactory::getInstance().CreateChessBlock(this, table_id, CHESS_BLACK);
			pBlackBlock->setPos(vecPos[j]);

			nId++;
			pBlackBlock->setId(nId);
			m_map_blocks[nId] = pBlackBlock;
			m_Chess[vecPos[j].m_posX][vecPos[j].m_posY] = nId;
		}
	}
}

void ChessGame::CheckMove(std::shared_ptr<ChessBlock>& pBlock, const Point& to_point)
{
	if(nullptr == pBlock)
	{
		return;
	}
		
	if (!IsChessPos(to_point))
		return;

	if(m_Chess[to_point.m_posX][to_point.m_posY])
		return;


	if (pBlock->getColor() != m_Color)
		return;


	if (pBlock->IsCanMove(to_point))
	{
		Move(pBlock, to_point);
		StepEnd();
	}
}

bool	ChessGame::IsChessPos(const Point& pt)
{
	if (pt.m_posX<0 || pt.m_posX>POS_LIMIT_X)
		return false;
	if (pt.m_posY<0 || pt.m_posY>POS_LIMIT_Y)
		return false;

	return true;
}

void ChessGame::StepEnd()
{
	m_map_fighters[(CHESS_COLOR)m_Color]->m_shou++;

	m_Color = m_Color % 2 + 1;
}

void  ChessGame::Move(std::shared_ptr<ChessBlock>& pBlock, const Point& pos)
{
	if (pBlock)
	{
		if (pBlock->getPos() == pos)
			return;
			
		m_Chess[pBlock->getPos().m_posX][pBlock->getPos().m_posY] = 0;
		m_Chess[pos.m_posX][pos.m_posY] = pBlock->getId();
		pBlock->setPos(pos);
	}
}

void ChessGame::Attack(const Point& from_point, const Point& to_point)
{
	
}

bool ChessGame::IsHaveOneBlock(const Point& from_point, const Point& to_point)
{
	//x轴相同，Y轴不同
	if (to_point.m_posX == from_point.m_posX && to_point.m_posY != from_point.m_posY)
	{
		int min = from_point.m_posY;
		int max = to_point.m_posY;
		if (max < min)
		{
			ChessManager::getInstance().Swap(min, max);
		}

		int nCnt = 0;
		int x = to_point.m_posX;
		for (int y = min+1; y < max, y <= POS_LIMIT_Y; ++y)
		{
			if (m_Chess[x][y])
			{
				nCnt++;
			}
		}

		if (nCnt == 1)
			return true;
	}
	else if (to_point.m_posY == from_point.m_posY && to_point.m_posX != from_point.m_posX)
	{
		int min = from_point.m_posX;
		int max = to_point.m_posX;
		if (max < min)
		{
			ChessManager::getInstance().Swap(min, max);
		}

		int nCnt = 0;
		int y = to_point.m_posY;
		for (int x= min+1; x < max, x <= POS_LIMIT_X; ++x)
		{
			if (m_Chess[x][y])
			{
				nCnt++;
			}
		}

		if (nCnt == 1)
			return true;
	}

	return false;
}

bool ChessGame::DestoryBolck(std::shared_ptr<ChessBlock> pBlock)
{
	if (pBlock == nullptr)
		return false;

	auto it = m_map_blocks.find(pBlock->getId());
	if (it != m_map_blocks.end())
		m_map_blocks.erase(it);

	m_Chess[pBlock->getPos().m_posX][pBlock->getPos().m_posY] = 0;

	if (pBlock->getType() == CHESS_JIANG)
	{
		GameOver();
	}

	return true;
}

void ChessGame::GameOver()
{
	/*if (m_desk == nullptr)
		return;

	int winner_id = m_map_fighters[(CHESS_COLOR)m_Color]->m_user_id;

	for (auto it = m_map_fighters.begin(); it != m_map_fighters.end(); ++it)
	{
		int user_id = it->second->m_user_id;

		LUserPtr user = m_desk->GetDeskUser(user_id);
		if (user)
		{
			GateInfo* gate_server = gWork.GetGateInfoById(user->GetGateServerId());
			if (gate_server)
			{
				LMsgS2CChessResult send;
				send.m_user_id = user_id;
				send.m_winner = winner_id;

				gWork.SendMessageToUser(gate_server->m_sp, user_id, send);
			}
		}
	}*/
}

bool ChessGame::IsHaveBlock(const Point& pos)
{
	return m_Chess[pos.m_posX][pos.m_posY];
}

CHESS_COLOR ChessGame::GetUserColor(Lint user_id)
{
	for (auto it = m_map_fighters.begin(); it != m_map_fighters.end(); ++it)
	{
		if (it->second->m_user_id == user_id)
			return it->first;
	}

	return CHESS_ERROR_COLOR;
}

std::shared_ptr<ChessBlock> ChessGame::GetBlockById(Lint block_id)
{
	auto it = m_map_blocks.find(block_id);
	if (it != m_map_blocks.end())
		return it->second;

	return nullptr;
}

void ChessGame::FillBlockMsg(BlockMsg& msg, const std::shared_ptr<ChessBlock> block)
{
	if (block == nullptr)
		return;

	msg.m_id = block->getId();
	msg.m_table_id = block->getTableId();
	msg.m_color = block->getColor();
	msg.m_type = block->getType();
	msg.m_hp = block->getHp();
	msg.m_max_hp = block->getMaxHp();
	msg.m_attack = block->getAttack();
	msg.m_max_attack = block->getMaxAttack();
	msg.m_coin_level = block->getCoinLevel();
	msg.m_max_coin_level = block->getMaxCoinLevel();
	msg.m_pos.m_posX = block->getPos().m_posX;
	msg.m_pos.m_posY = block->getPos().m_posY;
}


//消息处理函数
void ChessGame::HanderChessMove(LMsgC2SChessMove* msg)
{
	LMsgS2CChessMove send;
	send.m_user_id = msg->m_user_id;
	send.m_move_id = msg->m_move_id;

	int result = 0;
	do
	{
		int user_id = msg->m_user_id;
		if (GetUserColor(user_id) != m_Color)
		{
			result = 1;
			break;
		}

		std::shared_ptr<ChessBlock> pBlock = GetBlockById(msg->m_move_id);

		if (nullptr == pBlock)
		{
			result = 2;
			break;
		}

		Point to_point(msg->m_to_point.m_posX, msg->m_to_point.m_posY);
		if (!IsChessPos(to_point))
		{
			result = 3;
			break;
		}

		if (m_Chess[to_point.m_posX][to_point.m_posY])
		{
			result = 4;
			break;
		}


		if (pBlock->getColor() != m_Color)
		{
			result = 5;
			break;
		}


		if (pBlock->IsCanMove(to_point))
		{
			Move(pBlock, to_point);
			//StepEnd();
		}
		else
		{
			result = 3;
			break;
		}


	} while (0);

	send.m_errorCode = result;
	if (result == 0)
	{
		send.m_to_point = msg->m_to_point;
		ClearAttackData();
	}

	gWork.SendMessageToUser(msg->m_sp, msg->m_user_id, send);
}

void ChessGame::HanderChessAttack(LMsgC2SChessAttack* msg)
{
	LMsgS2CChessAttack send;
	send.m_user_id = msg->m_user_id;

	int result = 0;
	do
	{
		int user_id = msg->m_user_id;
		if (GetUserColor(user_id) != m_Color)
		{
			result = 1;
			break;
		}

		std::shared_ptr<ChessBlock> attack_block = GetBlockById(msg->m_attack_id);
		if (attack_block == nullptr)
		{
			result = 2;
			break;
		}

		std::shared_ptr<ChessBlock> get_damage_block = GetBlockById(msg->m_get_damage_id);
		if (get_damage_block == nullptr)
		{
			result = 2;
			break;
		}

		if (attack_block->getColor() != m_Color)
		{
			result = 3;
			break;
		}

		if (attack_block->getColor() == get_damage_block->getColor())
		{
			result = 3;
			break;
		}

		if (!attack_block->IsCanAttack(get_damage_block))
		{
			result = 4;
			break;
		}


		int damage = attack_block->getAttack();
		get_damage_block->GetDamage(damage);

		//如果吃掉，就销毁被吃的棋子，如果没有，则两个棋子都不动
		if (get_damage_block->getHp() == 0)
		{
			DestoryBolck(get_damage_block);

			Move(attack_block, get_damage_block->getPos());
		}

		FillBlockMsg(send.m_attack_block, attack_block);
		FillBlockMsg(send.m_get_damage_block, get_damage_block);

		AddAttackData(attack_block->getId(), get_damage_block->getPos());

	} while (0);

	send.m_errorCode = result;

	gWork.SendMessageToUser(msg->m_sp, msg->m_user_id, send);
}

bool ChessGame::CostCoin(CHESS_COLOR color, int cost)
{
	if (m_map_fighters[color]->m_money >= cost)
	{
		m_map_fighters[color]->m_money = m_map_fighters[color]->m_money - cost;
		return true;
	}

	return false;
}

void ChessGame::HanderChessUpgrade(LMsgC2SChessUpgrade* msg)
{
	LMsgS2CChessUpgrade send;
	send.m_user_id = msg->m_user_id;

	int result = 0;
	do
	{
		int user_id = msg->m_user_id;
		if (GetUserColor(user_id) != m_Color)
		{
			result = 1;
			break;
		}

		std::shared_ptr<ChessBlock> pBlock = GetBlockById(msg->m_block_id);

		if (nullptr == pBlock)
		{
			result = 2;
			break;
		}

		if (pBlock->getColor() != m_Color)
		{
			result = 3;
			break;
		}

		ChessBoardTbl* pTbl = ChessBoardCfg::Instance().GetChessBoard(pBlock->getTableId());
		if (nullptr == pTbl)
		{
			result = 4;
			break;
		}

		switch (msg->m_protery)
		{
		case BP_HP:
		{
			if (pBlock->getHp() >= pBlock->getMaxHp())
			{
				result = 5;
				break;
			}

			if (!CostCoin((CHESS_COLOR)m_Color, pTbl->m_upgrade_hp_cost))
			{ 
				result = 6;
				break;
			}

			pBlock->setHp(pBlock->getHp() + 1);

		}
		break;
		case BP_ATTACK:
		{
			if (pBlock->getAttack() >= pBlock->getMaxAttack())
			{
				result = 5;
				break;
			}

			if (!CostCoin((CHESS_COLOR)m_Color, pTbl->m_upgrad_attack_cost))
			{
				result = 6;
				break;
			}

			pBlock->setAttack(pBlock->getAttack() + 1);
		}
		break;
		case BP_COIN:
		{
			if (pBlock->getCoinLevel() >= pBlock->getMaxCoinLevel())
			{
				result = 5;
				break;
			}

			if (!CostCoin((CHESS_COLOR)m_Color, pTbl->m_upgrade_coin_level_cost[pBlock->getCoinLevel()]))
			{
				result = 6;
				break;
			}

			pBlock->setCoinLevel(pBlock->getCoinLevel() + 1);
		}
		break;
		}

		if (result == 0)
		{
			FillBlockMsg(send.m_block, pBlock);
		}

	} while (0);

	
	send.m_errorCode = result;
	gWork.SendMessageToUser(msg->m_sp, msg->m_user_id, send);
}

void ChessGame::AddAttackData(Lint attack_block_id, Point get_damage_pos)
{
	m_attack_data.m_attack_block_id = attack_block_id;
	m_attack_data.m_get_damage_pos = get_damage_pos;
}

void ChessGame::ClearAttackData()
{
	m_attack_data.Clear();
}

void ChessGame::SendAllChessInfoToUser(Lint user_id)
{
	CHESS_COLOR user_color = GetUserColor(user_id);
	if (user_color != CHESS_ERROR_COLOR)
	{
		LMsgS2CChessInfo send;
		send.m_user_id = user_id;
		send.m_user_color = user_color;
		send.m_cur_desk_color = m_Color;

		for (auto it = m_map_blocks.begin(); it != m_map_blocks.end(); ++it)
		{
			BlockMsg block_msg;
			FillBlockMsg(block_msg, it->second);
			send.m_chess_boards.push_back(block_msg);
		}
	}
}



