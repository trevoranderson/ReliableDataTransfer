#include <thread>
#include "sockets.h"
using namespace std;
int main()
{
	thread a(server);
	thread b(client);
	a.join();
	b.join();
}
