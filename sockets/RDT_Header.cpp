#include "RDT_Header.h"
using namespace std;
string RDT_Header::toString()
{
	string ret;
	ret += seqNum;
	ret += ackNum;
	ret += len;
	ret += fin;
	ret += data;
	return ret;
}

RDT_Header::RDT_Header(string datagram)
{
	int size = sizeof(unsigned char);
	memcpy(&seqNum, &datagram[0], size);
	memcpy(&ackNum, &datagram[size* 1], size);
	memcpy(&len, &datagram[size* 2], size);
	memcpy(&fin, &datagram[size* 3], size);
	data = datagram.substr(size* 4, datagram.size() - size* 4);
}