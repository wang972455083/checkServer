
#include "ChessBlock.h"
#include "ChessManager.h"


ChessBlock::ChessBlock()
{
	m_Id = 0;
	m_Color = CHESS_RED;
	m_Type = CHESS_NULL;
	m_Hp = 0;
	m_Attack = 0;
	m_MaxHp = 0;
	m_MaxAttack = 0;
	m_CoinLevel = 0;	
	m_MaxCoinLevel = 0;
}


ChessBlock::~ChessBlock()
{
	
}


bool ChessBlock::IsInBoundary(Point point)
{
	MoveBoundary boundary = ChessManager::getInstance().GetChessBlockBoundary(m_Color, (TYPE_CHESS)m_Type, point);

	return boundary.IsInBoundary(point);
}

void ChessBlock::MoveToPos(const Point& pos)
{
	setPos(pos);

	OnMoveEvent();
}

bool ChessBlock::IsCanAttack(std::shared_ptr<ChessBlock> pBlock)
{
	if (nullptr == pBlock)
		return false;

	Point pt = pBlock->getPos();
	if (IsCanMove(pt))
		return true;

	return false;
}

void ChessBlock::GetDamage(int damage)
{
	if (damage)
	{
		m_Hp = m_Hp - damage;

		OnDamageEvent();

		if (m_Hp <= 0)
		{
			m_Hp = 0;
			OnDamageDeathEvent();
		}	
	}
}





void ChessBlock::OnMoveEvent()
{

}
void ChessBlock::OnAttackEvent()
{

}
void ChessBlock::OnAttackDeathEvent()
{

}
void ChessBlock::OnDamageEvent()
{

}
void ChessBlock::OnDamageDeathEvent()
{

}


bool ChessJiang::IsCanMove(const Point& pos)
{
	if (!IsInBoundary(pos))
		return false;

	if (m_Pos.m_posX == pos.m_posX)
	{
		if (abs(pos.m_posY - m_Pos.m_posY) == 1)
			return true;
	}

	if (m_Pos.m_posY == pos.m_posY)
	{
		if (abs(pos.m_posX - m_Pos.m_posX) == 1)
			return true;
	}

	return false;
}

bool ChessShi::IsCanMove(const Point& pos)
{
	if (!IsInBoundary(pos))
		return false;

	Point from_point = m_Pos;

	if (abs(pos.m_posX - from_point.m_posX) != 1)
		return false;

	if (abs(pos.m_posY - from_point.m_posY) != 1)
		return false;

	return true;
}

bool ChessXiang::IsCanMove(const Point& pos)
{
	if (!IsInBoundary(pos))
		return false;

	Point from_point = m_Pos;

	if (abs(pos.m_posX - from_point.m_posX) != 2)
		return false;

	if (abs(pos.m_posY - from_point.m_posY) != 2)
		return false;

	int min_x = from_point.m_posX;
	if (pos.m_posX < min_x)
		min_x = pos.m_posX;

	int min_y = from_point.m_posY;
	if (pos.m_posY < min_y)
		min_y = pos.m_posY;

	Point mid_point = Point(min_x + 1, min_y + 1);
	if (m_Game&&m_Game->IsHaveBlock(mid_point))
		return false;

	return true;
}

bool ChessMa::IsCanMove(const Point& pos)
{
	if (!IsInBoundary(pos))
		return false;

	Point from_point = m_Pos;

	if (abs(pos.m_posX - from_point.m_posX) == 1 &&
		abs(pos.m_posY - from_point.m_posY) == 2)
	{
		int min_x = from_point.m_posX;
		if (pos.m_posX < min_x)
			min_x = pos.m_posX;

		int min_y = from_point.m_posY;
		if (pos.m_posY < min_y)
			min_y = pos.m_posY;

		Point mid = Point(min_x, min_y + 1);
		if (m_Game&&!m_Game->IsHaveBlock(mid))
			return true;
	}

	if (abs(pos.m_posX - from_point.m_posX) == 1 &&
		abs(pos.m_posY - from_point.m_posY) == 2)
	{
		int min_x = from_point.m_posX;
		if (pos.m_posX < min_x)
			min_x = pos.m_posX;

		int min_y = from_point.m_posY;
		if (pos.m_posY < min_y)
			min_y = pos.m_posY;

		Point mid = Point(min_x+1, min_y );
		if (m_Game&&!m_Game->IsHaveBlock(mid))
			return true;
	}

	return false;
}

bool ChessChe::IsCanMove(const Point& pos)
{
	if (!IsInBoundary(pos))
		return false;

	Point from_point = m_Pos;
	int min = 0;
	int max = 0;

	if (from_point.m_posX == pos.m_posX)
	{
		if (from_point.m_posY < pos.m_posY)
		{
			min = from_point.m_posY + 1;
			max = pos.m_posY + 1;
		}
		else
		{
			max = from_point.m_posY + 1;
			min = pos.m_posY + 1;	
		}

		for (int i = min; i < max; ++i)
		{
			Point pt = Point(pos.m_posX, i);

			if (m_Game&&m_Game->IsHaveBlock(pt))
			{
				return false;
			}
		}
		
		return true;
	}

	if (from_point.m_posY == pos.m_posY)
	{
		if (from_point.m_posX < pos.m_posY)
		{
			min = from_point.m_posX + 1;
			max = pos.m_posX + 1;
			
		}
		else
		{
			max = from_point.m_posX + 1;
			min = pos.m_posX + 1;
		}


		for (int i = min; i < max; ++i)
		{
			Point pt = Point(i, from_point.m_posY);

			if (m_Game&&m_Game->IsHaveBlock(pt))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

bool ChessPao::IsCanMove(const Point& pos)
{
	if (!IsInBoundary(pos))
		return false;

	Point from_point = m_Pos;
	int min = 0;
	int max = 0;

	if (from_point.m_posX == pos.m_posX)
	{
		if (from_point.m_posY < pos.m_posY)
		{
			min = from_point.m_posY + 1;
			max = pos.m_posY + 1;
		}
		else
		{
			max = from_point.m_posY + 1;
			min = pos.m_posY + 1;
		}

		for (int i = min; i < max; ++i)
		{
			Point pt = Point(pos.m_posX, i);

			if (m_Game&&m_Game->IsHaveBlock(pt))
			{
				return false;
			}
		}

		return true;
	}

	if (from_point.m_posY == pos.m_posY)
	{
		if (from_point.m_posX < pos.m_posY)
		{
			min = from_point.m_posX + 1;
			max = pos.m_posX + 1;

		}
		else
		{
			max = from_point.m_posX + 1;
			min = pos.m_posX + 1;
		}


		for (int i = min; i < max; ++i)
		{
			Point pt = Point(i, from_point.m_posY);

			if (m_Game&&m_Game->IsHaveBlock(pt))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

bool ChessPao::IsCanAttack(std::shared_ptr<ChessBlock> pBlock)
{
	if (pBlock == nullptr)
		return false;

	if (m_Game == nullptr)
		return false;

	Point from_point = m_Pos;
	Point to_point = pBlock->getPos();

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
		for (int y = min + 1; y < max, y <= POS_LIMIT_Y; ++y)
		{
			if (m_Game->IsHaveBlock(Point(x,y)))
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
		for (int x = min + 1; x < max, x <= POS_LIMIT_X; ++x)
		{
			if (m_Game->IsHaveBlock(Point(x, y)))
			{
				nCnt++;
			}
		}

		if (nCnt == 1)
			return true;
	}

	return false;
}

bool ChessBing::IsCanMove(const Point& pos)
{
	if (!IsInBoundary(pos))
		return false;

	Point from_point = m_Pos;

	if (IsRed())
	{	//还没过河

		if (from_point.m_posX == pos.m_posX)
		{
			if (pos.m_posY - from_point.m_posY == 1)
			{
				return true;
			}
		}
		//过河的卒子能平移
		if (from_point.m_posY > 4)
		{
			if (pos.m_posY == from_point.m_posY)
			{
				if (abs(pos.m_posX - from_point.m_posX) == 1)
				{
					return true;
				}
			}
		}
		
	}
	else
	{
		if (from_point.m_posX == pos.m_posX)
		{
			if (from_point.m_posY - pos.m_posY == 1)
			{
				return true;
			}
		}
		//过河的卒子能平移
		if (from_point.m_posY <= 4)
		{
			if (pos.m_posY == from_point.m_posY)
			{
				if (abs(pos.m_posX - from_point.m_posX) == 1)
				{
					return true;
				}
			}
		}
	}

	return false;
}
