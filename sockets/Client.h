#pragma once
#include "RDT_Header.h"
#include <string>
#include <vector>
#include <fstream>
class Client
{
public:
	Client()
	{

	}
	std::vector<RDT_Header> nextPackets(RDT_Header lastResponse)
	{
		std::vector<RDT_Header> ret(1);
		// send one packet with next seqNum
		ret[0].seqNum = lastResponse.seqNum + 1;
		ret[0].fin = 0;
		if (lastResponse.fin)
		{
			ret[0].fin = 1;
		}
		return ret;
	}

};
