#pragma once

#include <concepts>
#include <map>
#include <set>
#include <string>
#include <queue>
#include <vector>

#include <TetoEngine/Resource/ResourceView.hpp>
#include <TetoEngine/Common/ID.hpp>

namespace TetoEngine
{

	// todo put all resources in a continuous vector ?
	// idk what that would do really, we dont iterate the resource list
	template<class ResourceType>
	class ResourceArray
	{
	public:
		ResourceArray()
		{
			std::make_shared<ID64>(0);
		}

		ID64 create()
		{
			auto handle = nextHandle_++;
			
			resources_.emplace(std::piecewise_construct,
				std::forward_as_tuple(handle),
				std::forward_as_tuple(ResourceType {}, handle)
			);

			(*generation_.get())++;

			return handle;
		}

		bool destroy(ID64 handle)
		{
			if (!resources_.contains(handle))
			{
				return false;
			}

			resources_.erase(handle);

			(*generation_.get())++;
			return true;
		}

		ResourceView<ResourceType> get(ID64 handle)
		{
			if (!resources_.contains(handle))
			{
				return nullptr;
			}

			return resources_.at(handle).view(generation_);
		}

	private:
		template<class T>
		struct Resource
		{
			ResourceType resource_;
			ID64 handle_;

			ResourceView<T> view(std::shared_ptr<ID64> generation)
			{
				return ResourceView {&resource_, handle_, generation};
			}
		};

	private:
		std::map<ID64, Resource<ResourceType>> resources_;
		std::shared_ptr<ID64> generation_;

		static ID64 nextHandle_;
	};

	template<class T>
	ID64 ResourceArray<T>::nextHandle_ = 0;
}
