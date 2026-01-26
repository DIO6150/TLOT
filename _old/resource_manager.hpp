#pragma once
#include <vector>
#include <memory>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <utility>
#include <iostream>

namespace Engine {
	struct Resource {
		uint32_t index;
		uint32_t version;
		std::string_view name;

		Resource () : Resource (UINT32_MAX, 0, "") {

		}

		Resource (uint32_t index, uint32_t version, const std::string & name):
			index {index},
			version {version},
			name {name}
		{
			
		}

		bool operator== (Resource const & o) const noexcept {
			return (index == o.index && version == o.version);
		}

		bool valid () const noexcept {
			return (index != UINT32_MAX);
		}

		static Resource invalid () noexcept {
			return { UINT32_MAX, 0, "" };
		}
	};

	template<typename T>
	class ResourceManager {
	public:
		ResourceManager () = default;
		~ResourceManager () {
		};

		ResourceManager (const ResourceManager &) = delete;
		ResourceManager & operator= (const ResourceManager &) = delete;

		ResourceManager (ResourceManager &&) noexcept = default;
		ResourceManager & operator= (ResourceManager &&) noexcept = default;

		template<typename... Args>
		Resource create (const std::string & name, Args &&... args) {
			uint32_t idx;
			
			if (!free_list.empty ()) {
				idx = free_list.back ();
				free_list.pop_back ();
				storage[idx] = std::make_unique<T> (std::forward<Args> (args)...);
				// version[idx] stays as is (was incremented on destroy)
				id.push_back (name);
			}

			else {
				idx = static_cast<uint32_t> (storage.size());
				storage.push_back (std::make_unique<T> (std::forward<Args> (args)...));
				version.push_back (0);
				id.push_back (name);
			}
			return (Resource { idx, version[idx], id.back () });
		}

		bool destroy (Resource h) {
			if (!isValid(h)) return (false);

			storage[h.index].reset ();           // free the resource
			id[h.index].clear ();
			++version[h.index];                 // bump version to invalidate old handles
			free_list.push_back (h.index);       // reclaim index

			return (true);
		}

		T* get (Resource h) noexcept {
			if (!isValid(h)) return (nullptr);
			return (storage[h.index].get());
		}

		T& getRef (Resource h) {
			T* p = get(h);
			if (!p) throw std::runtime_error("Invalid handle");
			return (*p);
		}

		/*
		// Optional: try_get pattern returning std::optional<std::reference_wrapper<T>>
		std::optional<std::reference_wrapper<T>> try_get (Resource h) noexcept {
			T* p = get(h);
			if (p) return std::optional<std::reference_wrapper<T>>(*p);
			return std::nullopt;
		}
		*/

		bool isValid (Resource h) const noexcept {
			if (!h.valid()) return (false);
			if (h.index >= storage.size ()) return (false);
			if (version[h.index] != h.version) return (false);
			if (!storage[h.index]) return (false);
			
			return (true);
		}

		size_t size () const noexcept {
			return (storage.size () - free_list.size ());
		}

		template<typename F>
		void forEach (F&& callback) {
			for (uint32_t i = 0; i < storage.size (); ++i) {
				if (storage[i]) {
					callback ( Resource {i, version[i]}, *storage[i] );
				}
			}
		}

	private:
		std::vector<std::unique_ptr<T>>		storage;
		std::vector<std::string>		id;
		std::vector<uint32_t>			version;	// per-index version counter
		std::vector<uint32_t>			free_list;	// stack of free indices
	};

}

namespace std {
	template<> struct less <Engine::Resource> {
		bool operator() (const Engine::Resource & a, const Engine::Resource & b) const {
			return (a.index < b.index && a.version < b.version);
		}
	};

	template<>
	struct hash<Engine::Resource> {
		std::size_t operator()(const Engine::Resource & h) const {
			return (std::size_t(h.index) << 1) ^ std::size_t(h.version);
		}
	};
}