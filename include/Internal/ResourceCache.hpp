#pragma once

#include <optional>
#include <cassert>
#include <map>

#include <Core/Resource.hpp>
#include <Core/ResourceHandle.hpp>
#include <Core/Logger.hpp>

namespace TLOT
{
	template<IsResource ResourceType>
	class ResourceView
	{
	public:
		ResourceView() {}
		ResourceView(ResourceType const & resource): m_hasValue{true}, m_value{&resource} {}

		ResourceType const & Get() { return *m_value; }
		ResourceType const * operator->() { return m_value; }
		operator bool() { return m_hasValue; }

		bool HasValue() { return m_hasValue; }

	private:
		bool m_hasValue = false;
		ResourceType const * m_value;
	};

	template<IsResource ResourceType>
	class ResourceCache
	{
	public:
		ResourceHandle Load (std::string path)
		{
			ResourceHandle handle = GenerateHandle ();

			m_index.emplace (handle, m_resources.size ());
			Resource<ResourceType> & _resource = m_resources.emplace_back (handle);
			_resource.LoadFromDisk (path);

			return _resource.GetHandle ();
		}

		template<class ... Args>
		ResourceHandle Create(Args && ... args)
		{
			ResourceHandle handle = GenerateHandle();

			m_index.emplace(handle, m_resources.size());
			Resource<ResourceType> & _resource = m_resources.emplace_back(handle);
			_resource.template Create<Args...>(std::forward<Args>(args)...);

			return _resource.GetHandle();
		}

		ResourceView<ResourceType> Get (ResourceHandle handle)
		{
			if (handle == InvalidResource)
				return {};

			size_t index = m_index.at (handle);
			return m_resources[index].Get ();
		}

		auto begin () { return m_resources.begin (); }
		auto end   () { return m_resources.end (); }

	private:
		std::vector<Resource<ResourceType>> m_resources;
		std::map<ResourceHandle, size_t> m_index;
	};
}
