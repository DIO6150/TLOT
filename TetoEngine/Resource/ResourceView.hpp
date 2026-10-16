#pragma once

#include <memory>
#include <string_view>

#include <TetoEngine/Common/ID.hpp>


namespace TetoEngine
{
	/*
		Should always check if ResourceView is valid before doing anyting with it
	*/
	template<class T>
	class ResourceView
	{
	public:
		ResourceView() {}
		ResourceView(std::nullptr_t) {}

		ResourceView(T * resource, ID64 handle, std::shared_ptr<ID64> generator)
			: resource_{resource}
			, handle_{handle}
			, generation_ {*generator.get()}
			, currentGeneration_ {generator}
		{}

		operator bool() const
		{
			if (!resource_)
			{
				return false;
			}

			return generation_ == *currentGeneration_.get();
		}

		T * get()
		{
			return resource_;
		}

		ID64 handle()
		{
			return handle_;
		}

		T * operator->()
		{
			return resource_;
		}

	private:
		T * resource_ = nullptr;
		ID64 handle_;

		ID64 generation_;
		std::shared_ptr<ID64> currentGeneration_;
	};
}