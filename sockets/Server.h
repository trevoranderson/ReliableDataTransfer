#pragma once
#include "RDT_Header.h"
#include <string>
#include <vector>
#include <fstream>
class Server
{
public:
	struct funfile 
	{ 
		funfile(){}
		funfile(std::string ppath, int oofset = 0, int ppageSize = 1024)
		{
			path = ppath;
			offset = oofset;
			pageSize = ppageSize;
		}
		std::string nextPage()
		{
			// Read the next 
			std::ifstream strm;
			strm.open(path);
			strm.seekg(offset);
			std::string buffer;
			buffer.resize(pageSize);
			strm.read(&buffer[0], pageSize);
			offset += pageSize;
			return buffer;
		}
		std::string path; 
		int offset; 
		int pageSize;
	};
	Server(std::string path= "Server.h", int pageSize = 1024)
	{
		toRead.path = path;
		toRead.offset = 0;
		toRead.pageSize = pageSize;
	}
	std::vector<RDT_Header> nextPackets(RDT_Header lastResponse)
	{
		std::vector<RDT_Header> ret(1);
		// send one packet with next seqNum
		ret[0].seqNum = lastResponse.seqNum + 1;
		ret[0].data = toRead.nextPage();
		ret[0].fin = 0;
		if (lastResponse.fin || ret[0].data[0] == 0)
		{
			ret[0].fin = 1;
		}

		return ret;
	}
	funfile toRead;
};