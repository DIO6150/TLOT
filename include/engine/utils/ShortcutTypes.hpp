#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

using String = std::string;

namespace Engine::Core {
	class ObjectID;
}

template<class T> using UPtr      = std::unique_ptr<T>;
template<class T> using Vector    = std::vector<T>;
template<class T> using Map       = std::unordered_map<std::string, T>;
template<class T> using ObjectMap = std::unordered_map<Engine::Core::ObjectID, T>;