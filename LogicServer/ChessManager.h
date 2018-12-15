#pragma once

#include "SingleBase.h"
#include "ChessBlock.h"


struct MoveBoundary
{
	int		m_min_pos_x;
	int		m_max_pos_x;
	int		m_min_pos_y;
	int		m_max_pos_y;

	MoveBoundary(int min_x = 0, int max_x = 0, int min_y = 0, int max_y = 0)
	{
		m_min_pos_x = min_x;
		m_max_pos_x = max_x;

		m_min_pos_y = min_y;
		m_max_pos_y = max_y;
	}

	bool IsInBoundary(const Point& pt)
	{
		if (pt.m_posX<m_min_pos_x || pt.m_posX>m_max_pos_x)
			return false;

		if (pt.m_posY<m_min_pos_y || pt.m_posY>m_max_pos_y)
			return false;

		return true;
	}
};

class ChessManager : public SingleBase<ChessManager>
{

public:
		void Init();
		std::vector<Point>	GetPoints(int color, TYPE_CHESS type);
		MoveBoundary 		GetChessBlockBoundary(int color, TYPE_CHESS type,Point pt);

public:
	bool IsSamePos(const Point& pt1, const Point& pt2)
	{
		if (pt1.m_posX == pt2.m_posX && pt1.m_posY == pt2.m_posY)
		return true;

		return false;
	}

	void Swap(int& x, int& y)
	{
		int tmp = x;
		x = y;
		y = tmp;
	}

private:

		void InitChessPos();     //初始化位置
		void InitMoveBoundary(); //初始化移动范围

private:

	//各个棋子的出生点，用于开局和以后买棋子用
	std::map< TYPE_CHESS, std::vector<Point> >	m_mapRedPoints;
	std::map< TYPE_CHESS, std::vector<Point> >	m_mapBlackPoints;

	//各个棋子的活动范围
	std::map<TYPE_CHESS, MoveBoundary>		m_mapRedMoveBoundary;
	std::map<TYPE_CHESS, MoveBoundary>		m_mapBlackMoveBoundary;
};

