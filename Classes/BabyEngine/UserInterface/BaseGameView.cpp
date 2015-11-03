#include "BaseGameView.h"

//////////////////////////////////////////////////////////////////////////
//Definition of BaseGameViewImpl;
//////////////////////////////////////////////////////////////////////////
struct BaseGameView::BaseGameViewImpl
{
public:
	BaseGameViewImpl();
	~BaseGameViewImpl();

	GameViewID m_ID{ INVALID_GAME_VIEW_ID };
	std::weak_ptr<BaseGameLogic> m_Logic;

private:
	static GameViewID getAvaliableID();
	static void updateAvaliableID();
	static GameViewID s_AvaliableID;
};

GameViewID BaseGameView::BaseGameViewImpl::s_AvaliableID{ INVALID_GAME_VIEW_ID + 1 };

BaseGameView::BaseGameViewImpl::BaseGameViewImpl()
{
	m_ID = BaseGameViewImpl::getAvaliableID();
	BaseGameViewImpl::updateAvaliableID();
}

BaseGameView::BaseGameViewImpl::~BaseGameViewImpl()
{
}

GameViewID BaseGameView::BaseGameViewImpl::getAvaliableID()
{
	return s_AvaliableID;
}

void BaseGameView::BaseGameViewImpl::updateAvaliableID()
{
	++s_AvaliableID;
}

//////////////////////////////////////////////////////////////////////////
//Implementation of BaseGameView.
//////////////////////////////////////////////////////////////////////////
BaseGameView::BaseGameView() : pimpl{ std::make_unique<BaseGameViewImpl>() }
{
}

BaseGameView::~BaseGameView()
{
}

GameViewID BaseGameView::getID() const
{
	return pimpl->m_ID;
}

bool BaseGameView::isAttachedToLogic() const
{
	return !pimpl->m_Logic.expired();
}

std::shared_ptr<BaseGameLogic> BaseGameView::getLogic() const
{
	if (!isAttachedToLogic())
		return nullptr;

	return pimpl->m_Logic.lock();
}

void BaseGameView::setLogic(std::weak_ptr<BaseGameLogic> gameLogic)
{
	pimpl->m_Logic = gameLogic;
}
