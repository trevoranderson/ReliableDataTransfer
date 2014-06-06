#pragma once
#include "RDT_Header.h"
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <iostream>
#include <limits>
class Server
{
public:
	Server(std::string serverpath = "Server.h", int serverPageSize = 1024, int serverWindowSize = 3, int timeoutMS = 1000, double corruptProb = 0.0)
	{
		corruptionProbability = corruptProb;

		path = serverpath;
		offset = 0;
		pageSize = serverPageSize;

		startTime = std::clock();
		timeout = timeoutMS;
		windowSize = serverWindowSize;
		seqbase = 0;
		nextseq = seqbase;
	}
	bool isTimedOut()
	{
		return (std::clock() - startTime) > timeout;
	}

	std::vector<RDT_Header> nextPackets(RDT_Header lastResponse)
	{
		std::vector<RDT_Header> ret;
		if (!isCorruptPacket())
		{
			seqbase = lastResponse.ackNum + 1;
			if (seqbase == nextseq)
			{
				startTime = clock();
			}
			else
			{
				
			}
			if (nextseq < seqbase + windowSize)
			{
				for (; nextseq < seqbase + windowSize; nextseq++)
				{
					RDT_Header toPush = makeHeader(nextseq, getPage(nextseq*pageSize));
					toPush.fin = (lastResponse.fin) ? 1 : toPush.fin;
					ret.push_back(toPush);
				}
			}
		}
		else
		{

		}
		return ret;
	}
	std::vector<RDT_Header> timeOutPackets()
	{
		std::vector<RDT_Header> ret;
		startTime = std::clock();
		for (int i = seqbase; i < nextseq; i++)
		{
			ret.push_back(makeHeader(i, getPage(i*pageSize)));
		}
		return ret;
	}
private:
	RDT_Header makeHeader(int nseq, std::string data)
	{
		RDT_Header ret;
		ret.seqNum = nseq;
		ret.ackNum = 99;
		ret.data = data;
		ret.len = ret.data.length();
		ret.fin = 0;
		if (ret.data[0] == 0)
		{
			ret.fin = 1;
		}
		return ret;
	}
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
	bool isCorruptPacket()
	{
		bool ret = (((double)rand() / (RAND_MAX)) < corruptionProbability);
		return ret;
	}
	double corruptionProbability;
	// File reading settings:
	std::string path;
	int offset;
	int pageSize;
	// go back N vars
	std::clock_t  startTime;
	int timeout;
	int windowSize;
	int seqbase;
	int nextseq;
};