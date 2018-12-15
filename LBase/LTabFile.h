#ifndef _L_TABFILE_H
#define _L_TABFILE_H
#endif
#include "LBase.h"

#define MAX_TXT_FILE	(2048)
class LTabFile
{
private:
	int			m_index;	
	std::vector< std::vector< std::string > > fileItems;
public:
	LTabFile(void);
	~LTabFile(void);
	bool		Load(const char* FileName);
	void		clear();
	std::vector< std::string >& getTapFileLine();
};

