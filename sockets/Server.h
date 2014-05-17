#pragma once
#include "RDT_Header.h"
#include <string>
#include <vector>
#include <fstream>
class Server
{
	struct funfile 
	{ 
		funfile(){}
		funfile(std::string ppath, int oofset = 0, int ppageSize = 1024)
		{
			path = ppath;
			offset = oofset;
			pageSize = ppageSize;
		}
		std::string nextPage(funfile f)
		{
			// Read the next 
			std::ifstream strm;
			strm.open(f.path);
			strm.seekg(f.offset);
			std::string buffer;
			buffer.resize(pageSize);
			strm.read(&buffer[0], pageSize);
			return buffer;
		}
		std::string path; 
		int offset; 
		int pageSize;
	};
	Server(std::string path, int pageSize = 1024)
	{
		toRead.path = path;
		toRead.offset = 0;
		toRead.pageSize = pageSize;
	}
	std::vector<RDT_Header> nextPackets(RDT_Header lastResponse);
	funfile toRead;
};