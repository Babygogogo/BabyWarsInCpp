#ifndef __STRING_TO_VECTOR__
#define __STRING_TO_VECTOR__

#include <sstream>
#include <vector>

template <typename T>
std::vector<T> toVector(const char * rawString)
{
	auto sstream = std::stringstream(rawString);
	auto elementT = T();
	auto vectorT = std::vector<T>();

	while (sstream >> elementT)
		vectorT.emplace_back(std::move(elementT));

	return vectorT;
}

#endif // !__STRING_TO_VECTOR__
