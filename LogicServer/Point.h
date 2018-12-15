#pragma once
class Point
{
public:
	Point();
	Point(int x, int y);
	~Point();

	bool operator == (const Point& pt);

public:
	int		m_posX;
	int		m_posY;
};

