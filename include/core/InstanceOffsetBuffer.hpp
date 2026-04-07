#pragma once

#include <map>

#include <resources/Resource.hpp>

#include <cassert>

namespace TLOT
{
	class InstanceOffsetBuffer {
	public:
		size_t Insert (ID const instanceID) {
			auto offset = m_nextOffset++;
			
			m_instances.emplace (instanceID, offset);

			return offset;
		}

		void Remove (ID const instanceID) {
			m_instances.erase (instanceID);
		}

		size_t GetOffset (ID const instanceID) {
			assert (m_instances.find (instanceID) != m_instances.end ());
			return m_instances[instanceID];
		}

		auto begin () { return m_instances.begin (); }
		auto end   () { return m_instances.end   (); }

	private:

		std::map<ID, size_t> m_instances;

		size_t m_nextOffset = 0;
		
	};
}