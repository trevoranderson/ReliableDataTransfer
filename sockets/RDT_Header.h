#pragma once
#include <string>
#include <time.h>
class RDT_Header
{
public:
	std::string toString();
	RDT_Header()
	{}
	RDT_Header(std::string datagram);
	unsigned char seqNum;
	unsigned char ackNum;
	unsigned char len;
	unsigned char fin;
	std::string data;
};