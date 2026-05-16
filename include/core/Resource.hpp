#pragma once

#include <concepts>
#include <string>

#include <Core/ResourceHandle.hpp>

#include <Internal/Hash.hpp>

template<class T>
concept IsResource = std::default_initializable<T> && std::copyable<T>;

namespace TLOT
{
	template<IsResource ResourceType>
	class Resource
	{
	public:
		Resource (ResourceHandle handle) : m_handle {handle} {}

		ResourceHandle GetHandle () const
		{
			return m_handle;
		}

		
		ResourceType const & Get ()
		{
			return m_resource;
		}
		
		bool LoadFromDisk (std::string path);

		ID_64 Hash ()
		{
			ID_64 hash = hash_combine (m_handle, m_handle);
			return hash;
		}

		std::string GetPath ()
		{
			return m_path;
		}

	private:
		ResourceType m_resource;
		
		ResourceHandle m_handle = InvalidResource;
		std::string m_path;
	};
}
