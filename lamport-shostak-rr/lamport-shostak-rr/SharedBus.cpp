#include "SharedBus.h"

/* Stubs */
void SharedBus::send(std::string str, int ID)
{
	std::cout << str << std::endl;
	return;
}

std::string SharedBus::receive()
{
	return "23,1";
}
