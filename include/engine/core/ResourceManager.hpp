#pragma once

#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include <stdint.h>

namespace Engine::Core {
	template<class T>
	class ResourceManager;

	class ObjectID {
	public:
		ObjectID () : index {UINT32_MAX}, version {0} {}
		
		static ObjectID invalid () { return (ObjectID {}); }
		void invalidate () { index = UINT32_MAX; }
		bool isValid () const { return (index != UINT32_MAX); }

		bool operator== (const ObjectID & o) const noexcept {
			return (index == o.index && version == o.version);
		}
		
	private:
		ObjectID (size_t index, uint32_t version) : index {index}, version {version} {}

		size_t index;
		uint32_t version;

		template<class T>
		friend class ResourceManager;

		friend struct std::less<ObjectID>;
		friend struct std::hash<ObjectID>;
	};

	template<class T>
	class PackedResourceManager {
	private:
		struct ArrayNode {
			size_t			idx;
			uint32_t 		version;
		};

	public:
		PackedResourceManager () : max_objects {10000000} {
			objects	.reserve (max_objects);
			nodes	.reserve (max_objects);
		}

		template<class ... Args>
		ObjectID add (Args && ... args) {
			size_t idx;
			size_t index;

			if (objects.size () > max_objects) {
				return (ObjectID::invalid ());
			}

			idx = objects.size ();
			index = idx;
			
			if (free_list.empty ()) {
				idx_to_indexes.emplace_back (index);

				nodes	.emplace_back (idx, 0);
				objects	.emplace_back (std::forward<Args> (args)...);
			}

			else {
				index = free_list.back ();

				idx_to_indexes[idx]	= index;
				nodes[index].idx	= idx;

				free_list.pop_back ();
				objects.emplace_back (std::forward<Args> (args)...);
			}

			return (ObjectID {nodes[index].idx, nodes[index].version});
		}

		void remove (ObjectID id) {
			size_t idx;

			if (!isValid (id)) return;
			
			idx = nodes[id.index].idx;

			if (1) {
				nodes[idx_to_indexes[objects.size () - 1]].idx = idx;
				idx_to_indexes[idx] = objects.size () - 1;

				std::swap (objects.at (idx), objects.back ());
			}

			objects.pop_back ();
		
			nodes[id.index].idx = UINT32_MAX;
			++nodes[id.index].version;

			free_list.push_back (id.index);
		}

		T & operator[] (ObjectID id) {
			if (!isValid (id)) {
				throw std::runtime_error ("Invalid ObjectID.");
			}

			size_t idx = nodes[id.index].idx;
			return (objects[idx]);
		}

		bool isValid (ObjectID id) const {
			return (id.isValid () && id.index < nodes.size () && id.version == nodes[id.index].version);
		}

		template<typename C>
		void forEach (C func) {
			for (size_t i = 0; i < nodes.size (); ++i) {
				if (nodes[i].idx == UINT32_MAX) {
					continue;
				}

				func (objects[nodes[i].idx], nodes[i].key);
			}
		}

		size_t size () {
			return (objects.size ());
		}

		T * data () {
			return (objects.data ());
		}

	private:
		std::vector<T>		objects;

		const size_t		max_objects;

		std::vector<ArrayNode>	nodes;		// hold references to an object in object vector, can have holes
							// each object have its version at 0 and is incremented when the id is reused, thus invalidating previous references

		// TODO: fix this shit, way too much overhead
		std::vector<size_t>	idx_to_indexes; // keep track of each object index in a node

		std::vector<size_t>	free_list;	// hold all the "holes" in the nodes list
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
		ObjectID create (Args &&... args) {
			uint32_t idx;
			
			if (!free_list.empty ()) {
				idx = free_list.back ();
				free_list.pop_back ();
				storage[idx] = std::make_unique<T> (std::forward<Args> (args)...);
			}

			else {
				idx = static_cast<uint32_t> (storage.size());
				storage.push_back (std::make_unique<T> (std::forward<Args> (args)...));
				version.push_back (0);
			}
			return (ObjectID { idx, version[idx] });
		}

		bool destroy (ObjectID h) {
			if (!isValid(h)) return (false);

			storage[h.index].reset ();           // free the resource
			++version[h.index];                 // bump version to invalidate old handles
			free_list.push_back (h.index);       // reclaim index

			return (true);
		}

		T* get (ObjectID h) const noexcept {
			if (!isValid (h)) return (nullptr);
			return (storage[h.index].get());
		}

		T& getRef (ObjectID h) const {
			T* p = get(h);
			if (!p) {
				if (version.size () > h.index) {	
					printf ("Wrong ObjectID with idx %llu and ver %u with idx ver %u\n", h.index, h.version, version[h.index]);
				}
				else {	
					printf ("Wrong ObjectID with idx %llu and ver %u with idx ver too high\n", h.index, h.version);
				}
				throw std::runtime_error("Invalid handle");
			}
			return (*p);
		}

		bool isValid (ObjectID h) const noexcept {
			if (!h.isValid()) return (false);
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
					callback ( ObjectID {i, version[i]}, *storage[i] );
				}
			}
		}

	private:
		std::vector<std::unique_ptr<T>>		storage;
		std::vector<uint32_t>			version;	// per-index version counter
		std::vector<uint32_t>			free_list;	// stack of free indices
	};
}


namespace std {
	template<> struct less <Engine::Core::ObjectID> {
		bool operator() (const Engine::Core::ObjectID & a, const Engine::Core::ObjectID & b) const {
			return (a.index < b.index && a.version < b.version);
		}
	};

	template<>
	struct hash<Engine::Core::ObjectID> {
		std::size_t operator()(const Engine::Core::ObjectID & h) const {
			// TODO: sus math
			return (std::size_t(h.index) << 1) ^ std::size_t(h.version);
		}
	};
}