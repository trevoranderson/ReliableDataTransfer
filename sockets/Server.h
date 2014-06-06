#pragma once
#include "RDT_Header.h"
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <iostream>
class Server
{
public:
	std::string getPage(int byteOffset)
	{
		// Read the next 
		std::ifstream strm;
		strm.open(path);
		strm.seekg(byteOffset);
		std::string buffer;
		buffer.resize(pageSize);
		strm.read(&buffer[0], pageSize);
		return buffer;
	}
	Server(std::string serverpath = "Server.h", int serverPageSize = 1024, int serverWindowSize = 3, int timeoutMS = 1000)
	{
		path = serverpath;
		offset = 0;
		pageSize = serverPageSize;

		startTime = std::clock();
		timeout = timeoutMS;
		windowSize = serverWindowSize;
		seqbase = 1;
		seqmax = seqbase + windowSize;
		nextseq = seqbase;
	}
	bool isTimedOut()
	{
		return (std::clock() - startTime) > timeout;
	}
	bool isCorruptPacket()
	{
		return false;
	}
	std::vector<RDT_Header> nextPackets(RDT_Header lastResponse)
	{
		if (!isCorruptPacket())
		{
			seqbase = lastResponse.ackNum + 1;
			if (seqbase == nextseq)
			{

			}
			else
			{

			}
		}
		else
		{

		}

		std::vector<RDT_Header> ret(1);
		// send one packet with next seqNum
		ret[0].seqNum = lastResponse.seqNum + 1;
		ret[0].ackNum = lastResponse.ackNum;
		ret[0].data = getPage(offset);
		ret[0].len = ret[0].data.length();
		ret[0].fin = 0;
		if (lastResponse.fin || ret[0].data[0] == 0)
		{
			ret[0].fin = 1;
		}
		return ret;
	}
	// File reading settings:
	std::string path;
	int offset;
	int pageSize;
	// go back N vars
	std::clock_t  startTime;
	int timeout;
	int windowSize;
	int seqbase;
	int seqmax;
	int nextseq;
};