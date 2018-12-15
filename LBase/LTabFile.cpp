#include "LTabFile.h"
#include <string.h>
#include "LTool.h"
#include "LLog.h"

LTabFile::LTabFile():m_index(0)
{
}
LTabFile::~LTabFile()
{
}

bool LTabFile::Load(const char*  FileName)
{
	clear();
	char line_content[MAX_TXT_FILE];
	std::vector<Lstring> des;
	std::ifstream ifs(FileName);
	int i = 0;
	while(ifs)
	{
		des.clear();
		memset(line_content, 0, MAX_TXT_FILE);
		ifs.getline(line_content,MAX_TXT_FILE);
		printf("getline=%s\n",line_content );

		std::string s;
#ifdef _LINUX
		s = line_content;
#else
		s = string_To_UTF8(line_content);
#endif

		printf("utf8-getline=%s\n",s.c_str() );

		L_ParseString(s, des, "\t");
		if (des.size() > 0)
		{
			fileItems.push_back(des) ;
			LLOG_DEBUG("LTabFile::Load item number = %d", des.size());
		}
	}
	return true;
}

void LTabFile::clear()
{
	m_index = 1;		//去掉最上面一行注释
	fileItems.clear();
}

std::vector< std::string >& LTabFile::getTapFileLine()
{
	if (m_index >= fileItems.size())
	{
		static std::vector< std::string > nullItem;
		return nullItem;
	}
	return fileItems[m_index ++];
}
