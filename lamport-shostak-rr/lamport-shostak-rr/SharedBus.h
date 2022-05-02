#pragma once

#include <iostream>

class SharedBus
{
public:
	void send(std::string, int ID);
	std::string receive();

};

