
#include "Point.h"


Point::Point()
{
	m_posX = 0;
	m_posY = 0;
}

Point::Point(int x, int y)
{
	m_posX = x;
	m_posY = y;
}


Point::~Point()
{
}

bool Point::operator== (const Point& pt)
{
	if (m_posX == pt.m_posX &&
		m_posY == pt.m_posY)
		return true;

	return false;
}
