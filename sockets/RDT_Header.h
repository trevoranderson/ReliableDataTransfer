#pragma once
#include <string>
#include <time.h>
#include <iostream>
typedef char T;

class RDT_Header
{
public:
	std::string TtoString(T t)
	{
		std::string ret;
		ret.resize(sizeof(T));
		memcpy(&ret[0], &t, sizeof(T));
		return ret;
	}
	void printReadable()
	{
		std::cout << "seq: " << (int)seqNum << ". ack: " << (int)ackNum << ". len: " << (int)len << ". fin: " << (int)fin << std::endl;
		std::cout << "Data: " << data << std::endl;
	}
	std::string RDT_Header::toString()
	{
		std::string ret;
		ret += TtoString(seqNum);
		ret += TtoString(ackNum);
		ret += TtoString(len);
		ret += TtoString(fin);
		ret += data;
		return ret;
	}
	RDT_Header()
	{}
	RDT_Header::RDT_Header(std::string datagram)
	{
		if (datagram.size() < sizeof(T)* 4)
		{
			return; // No header
		}
		int size = sizeof(T);
		memcpy(&seqNum, &datagram[0], size);
		memcpy(&ackNum, &datagram[size * 1], size);
		memcpy(&len, &datagram[size * 2], size);
		memcpy(&fin, &datagram[size * 3], size);
		data = datagram.substr(size * 4, datagram.size() - size * 4);
	}
	T seqNum;
	T ackNum;
	T len;
	T fin;
	std::string data;
};