#include <mutex>
#include <unordered_map>

#include "SingletonContainer.h"
#include "cocos2d.h"

//////////////////////////////////////////////////////////////////////////
//Definition of SingletonContainer::SingletonContainerImpl
//////////////////////////////////////////////////////////////////////////

struct SingletonContainer::SingletonContainerImpl
{
public:
	SingletonContainerImpl();
	~SingletonContainerImpl();

	std::unordered_map<std::type_index, std::shared_ptr<void>> m_Objects;
};

SingletonContainer::SingletonContainerImpl::SingletonContainerImpl()
{

}

SingletonContainer::SingletonContainerImpl::~SingletonContainerImpl()
{

}


//////////////////////////////////////////////////////////////////////////
//Implementation of SingletonContainer
//////////////////////////////////////////////////////////////////////////

std::unique_ptr<SingletonContainer> SingletonContainer::s_Instance;

SingletonContainer::SingletonContainer() :pimpl(new SingletonContainerImpl())
{

}

SingletonContainer::~SingletonContainer()
{
	//Must manually release here, otherwise the m_instance may point to a deleted instance of SingletonContainer,
	//making method calls (get() for example) result in undefined behavior. 
	s_Instance.release();
}

const std::unique_ptr<SingletonContainer> & SingletonContainer::getInstance()
{
	static std::once_flag s_InitFlag;

	if (!s_Instance)
		std::call_once(s_InitFlag, []{s_Instance.reset(new SingletonContainer); });

	return s_Instance;
}

std::shared_ptr<void> SingletonContainer::getHelper(const std::type_index & typeIndex) const
{
	auto objectIter = pimpl->m_Objects.find(typeIndex);
	return objectIter == pimpl->m_Objects.end() ?
		nullptr :
		objectIter->second;
}

std::shared_ptr<void> SingletonContainer::setHelper(std::type_index && typeIndex, std::shared_ptr<void> && obj)
{
	//if an object of the same type doesn't exist, simply emplace the new object and return it
	if (pimpl->m_Objects.find(typeIndex) == pimpl->m_Objects.end())
		return pimpl->m_Objects.emplace(std::move(typeIndex), std::move(obj)).first->second;

	//else, log and replace the object with the new one
	cocos2d::log("SingletonContainer::set : An object of %s already exists and is being replaced.", typeIndex.name());
	return pimpl->m_Objects[std::move(typeIndex)] = std::move(obj);
}
