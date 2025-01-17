#pragma once

#include <string>

class ICommand
{
public:
	virtual void Execute() = 0;

protected:
	std::string name;
};
