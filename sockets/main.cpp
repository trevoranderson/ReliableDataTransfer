#include <thread>
#include "sockets.h"
#include <iostream>
#include "RDT_Header.h"
using namespace std;
int main()
{
	RDT_Header test;
	test.seqNum = 0;
	test.ackNum = 1;
	test.data = "I had a dream";
	test.len = test.data.length();
	test.fin = 0;

	string header = test.toString();

	RDT_Header test2(header);

	thread a(server);
	thread b(client);
	a.join();
	b.join();
}
