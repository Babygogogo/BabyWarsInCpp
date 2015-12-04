#ifndef __GAME_COMMAND__
#define __GAME_COMMAND__

#include <string>
#include <functional>

class GameCommand
{
public:
	using Callback = std::function<void()>;

	GameCommand(std::string && name, Callback && callback) : m_Name{ std::move(name) }, m_Callback{ std::move(callback) } {}

	const std::string & getName() const
	{
		return m_Name;
	}
	const std::function<void()> & getCallback() const
	{
		return m_Callback;
	}

private:
	std::string m_Name;
	Callback m_Callback;
};

#endif // !__GAME_COMAND__
