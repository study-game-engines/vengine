#include "CommandSystem.h"
#include "ICommand.h"
#include "Input.h"

CommandSystem commandSystem;

void CommandSystem::Tick()
{
	if (Input::GetAsyncKey(Keys::Ctrl))
	{
		if (Input::GetKeyUp(Keys::Z))
		{
			Undo();
		}
		else if (Input::GetKeyUp(Keys::R))
		{
			Redo();
		}
	}
}

void CommandSystem::Add(ICommand* command)
{
	if (!commands.empty())
	{
		for (int i = commands.size() - 1; i > commandIndex; i--)
		{
			commands.pop_back();
		}
	}

	command->Execute();

	commands.push_back(command);

	commandIndex = commands.size() - 1;
}

void CommandSystem::Undo()
{
	if (commandIndex > 0)
	{
		commandIndex--;
	}

	commands[commandIndex]->Undo();
}

void CommandSystem::Redo()
{
	if (commandIndex < commands.size() - 1)
	{
		commandIndex++;
		commands[commandIndex]->Execute();
	}
}

void CommandSystem::Reset()
{
	commands.clear();
	commandIndex = 0;
}

void CommandSystem::WindToCommand(uint32_t windCommandIndex)
{
	if (windCommandIndex < commandIndex)
	{
		for (int i = commandIndex; i > windCommandIndex; i--)
		{
			commands[i]->Undo();
		}

		commandIndex = windCommandIndex;
		commands[commandIndex]->Undo();
	}
	else if (windCommandIndex > commandIndex)
	{
		for (int i = commandIndex; i < windCommandIndex; i++)
		{
			commands[i]->Execute();
		}

		commandIndex = windCommandIndex;
		commands[commandIndex]->Execute();
	}
	else if (windCommandIndex == commandIndex)
	{
		commands[windCommandIndex]->Execute();
	}	
	else if (windCommandIndex == 0)
	{
		commands[windCommandIndex]->Undo();
	}
}
