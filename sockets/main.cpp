#include <thread>
#include "sockets.h"
#include <iostream>
#include "RDT_Header.h"
#include "Client.h"
#include "Server.h"
using namespace std;

void NetworkTest()
{
	Client C(0.1);

	// Client initiates to Server
	RDT_Header clientFirst;
	clientFirst.ackNum = -1;
	clientFirst.data = "testfile.txt";
	clientFirst.fin = 0;
	clientFirst.len = clientFirst.data.length();
	clientFirst.seqNum = 99;

	// Create Server object from client request
	Server S("frognerd.html", 10, 2, 100, 0.1);


	vector<RDT_Header> clientResponses = { clientFirst };
	vector<RDT_Header> serverResponses = S.nextPackets(clientFirst);

	while (1)
	{
		if ((clientResponses.size() && clientResponses[0].fin) && serverResponses[serverResponses.size() - 1].fin)
		{
			break;
		}
		// Respond to server requests and ack
		clientResponses.resize(0);
		for (auto &i : serverResponses)
		{
			auto cc = C.nextPacket(i);
			clientResponses.insert(clientResponses.begin(), cc.begin(), cc.end());
		}


		if ((clientResponses.size() && clientResponses[0].fin) && serverResponses[serverResponses.size() - 1].fin)
		{
			break;
		}
		// Respond to all client requests and generate requests to the client
		serverResponses.resize(0);
		if (!S.isTimedOut())
		{
			for (auto &i : clientResponses)
			{
				auto ss = S.nextPackets(i);
				serverResponses.insert(serverResponses.begin(), ss.begin(), ss.end());
			}
		}
		else
		{
			serverResponses = S.timeOutPackets();
		}
		cout << C.CurrentChars << endl;
	}
	cout << C.CurrentChars << endl;
}

int main()
{
	srand(time(NULL));
	NetworkTest();
	return 0;
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
