#pragma once

#include <stdint.h>
#include <utility>

namespace Engine::Core {
	class InstanceBase {
		public:
		InstanceBase ():
			mID {UINT64_MAX}
			{

		}
		uint64_t GetID () const {
			return mID;
		}
		
	protected:
		void SetID (uint64_t id) {
			mID = id;
		}

		template<class T>
		friend class InstanceFactory;

	private:
		uint64_t mID;
	};

	template<class Data>
	struct Instance : InstanceBase {

	};

	template<class Data>
	class InstanceFactory {
	public:
		using Instanced = Instance<Data>;

		// return not a copy of an Instanced<Data> but the thing itself, std::move ?
		template<class ... Args>
		static Instanced CreateInstance (Args && ... args) {
			Instanced _instance {args ...};
			_instance.SetID (mNextID++);

			return _instance;
		}

	private:
		static uint64_t mNextID; // trick from the SymbolList class in the PCSX2 repo

	};

	template<class Data>
	uint64_t InstanceFactory<Data>::mNextID = 0; // does this reinitialize mNext across translation units ? I don't think it does if it stays in a .cpp
}
