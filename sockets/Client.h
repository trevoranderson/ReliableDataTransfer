#pragma once
#include "RDT_Header.h"
#include <string>
#include <vector>
#include <fstream>
class Client
{
public:
	Client(double corruptProb = 0.0)
	{
		expectedSeq = 0;
		corruptionProbability = corruptProb;
	}
	bool isCorruptPacket()
	{
		bool ret = (((double)rand() / (RAND_MAX)) < corruptionProbability);
		return ret;
	}
	std::vector<RDT_Header> nextPacket(RDT_Header lastResponse)
	{
		std::vector<RDT_Header> ret;
		if (!isCorruptPacket() && lastResponse.seqNum == expectedSeq)
		{
			CurrentChars += lastResponse.data;
			RDT_Header toPush;
			toPush.ackNum = lastResponse.seqNum;
			toPush.data = "";
			toPush.fin = lastResponse.fin;
			toPush.len = 0;
			toPush.seqNum = 99;
			ret.push_back(toPush);
			expectedSeq++;
		}
		return ret;
	}
	int expectedSeq;
	std::string CurrentChars;
	double corruptionProbability;
};
