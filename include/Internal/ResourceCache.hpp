#pragma once

#include <cassert>
#include <map>

#include <Core/Resource.hpp>
#include <Core/ResourceHandle.hpp>
#include <Core/Logger.hpp>

namespace TLOT
{
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

		ResourceType const & Get (ResourceHandle handle)
		{
			if (handle == InvalidResource)
				exit (-1);

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
