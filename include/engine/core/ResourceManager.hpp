#pragma once

#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include <stdint.h>

namespace Engine::Core {
	template<class T>
	class ResourceManager;

	class HandleID {
	public:
		HandleID () : index {UINT32_MAX}, version {0} {}
		
		static HandleID invalid () { return (HandleID {}); }
		void invalidate () { index = UINT32_MAX; }
		bool isValid () const { return (index != UINT32_MAX); }

		bool operator== (const HandleID & o) const noexcept {
			return (index == o.index && version == o.version);
		}

		friend std::ostream& operator<< (std::ostream &out, const HandleID & data) {
			out << "(id=" << data.index << ", ver=" << data.version << ")";
			return out;
		}
		
	private:
		HandleID (size_t index, uint32_t version) : index {index}, version {version} {}

		size_t index;
		uint32_t version;

		template<class T>
		friend class ResourceManager;

		template<class T>
		friend class CompoundElementArray;

		friend struct std::less<HandleID>;
		friend struct std::hash<HandleID>;
	};

}

namespace std {
	template<> struct less <Engine::Core::HandleID> {
		bool operator() (const Engine::Core::HandleID & a, const Engine::Core::HandleID & b) const {
			if (a.index == b.index) {
				return (a.version < b.version);
			}
			return (a.index < b.index);
		}
	};

	template<>
	struct hash<Engine::Core::HandleID> {
		std::size_t operator()(const Engine::Core::HandleID & h) const {
			// TODO: sus math
			return (std::size_t(h.index) << 1) ^ std::size_t(h.version);
		}
	};
}

namespace Engine::Core {
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
		HandleID add (Args && ... args) {
			size_t idx;
			size_t index;

			if (objects.size () > max_objects) {
				return (HandleID::invalid ());
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

			return (HandleID {nodes[index].idx, nodes[index].version});
		}

		void remove (HandleID id) {
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

		T & operator[] (HandleID id) {
			if (!isValid (id)) {
				throw std::runtime_error ("Invalid HandleID.");
			}

			size_t idx = nodes[id.index].idx;
			return (objects[idx]);
		}

		bool isValid (HandleID id) const {
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
		// Edit: erm really ?
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
		HandleID create (Args &&... args) {
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
			return (HandleID { idx, version[idx] });
		}

		bool destroy (HandleID h) {
			if (!isValid(h)) return (false);

			storage[h.index].reset ();           // free the resource
			++version[h.index];                 // bump version to invalidate old handles
			free_list.push_back (h.index);       // reclaim index

			return (true);
		}

		T* get (HandleID h) const noexcept {
			if (!isValid (h)) return (nullptr);
			return (storage[h.index].get());
		}

		T& getRef (HandleID h) const {
			T* p = get(h);
			if (!p) {
				if (version.size () > h.index) {	
					printf ("Wrong HandleID with idx %llu and ver %u with idx ver %u\n", h.index, h.version, version[h.index]);
				}
				else {	
					printf ("Wrong HandleID with idx %llu and ver %u with idx ver too high\n", h.index, h.version);
				}
				throw std::runtime_error("Invalid handle");
			}
			return (*p);
		}

		bool isValid (HandleID h) const noexcept {
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
					callback ( HandleID {i, version[i]}, *storage[i] );
				}
			}
		}

	private:
		std::vector<std::unique_ptr<T>>		storage;
		std::vector<uint32_t>			version;	// per-index version counter
		std::vector<uint32_t>			free_list;	// stack of free indices
	};

	template <class T>
	class CompoundElementArray {
	private:
		struct CompoundElementNode {
			CompoundElementNode (size_t begin, size_t end, uint32_t version):
				mBegin   {begin},
				mEnd     {end},
				mVersion {version}
				{

			}
			
			CompoundElementNode ():
				CompoundElementNode (0, 0, UINT32_MAX)
				{

			}

			size_t mBegin;
			size_t mEnd;

			uint32_t mVersion;
		};

	public:
		bool IsValid (const HandleID & handle) const {
			return (
				handle.version != UINT32_MAX &&
				handle.index < mPositions.size () &&
				mPositions[handle.index].mVersion == handle.version
			);
		}

		HandleID Push (const std::vector<T> & elements) {
			size_t _elementsCount = elements.size ();

			if (mCapacity - mSize <= _elementsCount) {
				return HandleID::invalid ();
			}

			size_t _begin = mElements.size ();
			size_t _end   = _begin + _elementsCount; //TODO-maybe: -1 or 0 ? cant think rn

			size_t _idx;
			uint32_t _version;

			if (!mFreeList.empty ()) {
				_idx = mFreeList.back ();
				mFreeList.pop_back ();

				CompoundElementNode & _node = mPositions.at (_idx);
				_node.mBegin = _begin;
				_node.mEnd   = _end;

				_version = _node.mVersion;
			}
			else {
				_idx = mPositions.size ();
				mPositions.emplace_back (_begin, _end, 0);
				
				_version = 0;
			}
			
			mElements.insert (mElements.end (), elements.begin (), elements.end ()); // inserts a copy of an object T in the vector so T must be copy constructible

			return HandleID {_idx, _version};
		}

		void Remove (const HandleID & handle) {
			if (!IsValid (handle)) {
				throw (std::runtime_error ("void Remove: Invalid handle ID"));
			}

			size_t _begin = mPositions[handle.index].mBegin;
			size_t _end = mPositions[handle.index].mEnd;
			size_t _freedSpace = _end - _begin;

			// not very optimal, but probably the easiest way to do it, since its not an operation done often (and honesty, I can't think of any use of this, but here it is anyway)
			// this container entire purpose is to hold vertices and indices in a packed way, there is not many use case of deleting geometry data anyway
			mElements.erase (mElements.begin () + _begin, mElements.end () + _end);
			for (size_t _idx = handle.index + 1; _idx < mPositions.size (); ++_idx) {
				mPositions[_idx].mBegin -= _freedSpace;
				mPositions[_idx].mEnd   -= _freedSpace;
			}

			++mPositions[handle.index];
			mFreeList.push_back (handle.index);
		}

		std::vector<T> operator[] (const HandleID & handle) const {
			if (!IsValid (handle)) {
				throw (std::runtime_error ("T & operator[]: Invalid handle ID"));
			}

			size_t _begin = mPositions[handle.index].mBegin;
			size_t _end = mPositions[handle.index].mEnd;

			std::vector<T> result;
			result.insert (result.begin (), mElements.begin () + _begin, mElements.begin () + _end);

			return (result);
		}

		T * GetData (const HandleID & handle) const {
			if (!IsValid (handle)) {
				throw (std::runtime_error ("T * GetData: Invalid handle ID"));
			}

			size_t _idx = mPositions[handle.index].mBegin;

			return (&mElements.at (_idx));
		}

		size_t GetSize (const HandleID & handle) {
			if (!IsValid (handle)) {
				throw (std::runtime_error ("T * GetData: Invalid handle ID"));
			}

			size_t _size = mPositions[handle.index].mEnd - mPositions[handle.index].mBegin;

			return (_size);
		}

		template<typename F>
		void ForEach (const F && callback) {
			for (auto & element: mElements) {
				callback (element);
			}
		}

		size_t GetRemainingSpace () const {
			return (mCapacity - mSize);
		}

		CompoundElementArray (size_t capacity):
			mCapacity {capacity},
			mSize     {0}
			{
			static_assert (std::is_copy_constructible_v<T>); // we need to, in order to be able to extends mElements

			mElements.reserve (capacity);
		}

		// totally arbitrary value
		CompoundElementArray ():
			CompoundElementArray {1000000} {

		}

		CompoundElementArray (CompoundElementArray && other) = default;

		CompoundElementArray (CompoundElementArray & other) = delete;
		CompoundElementArray (const CompoundElementArray & other) = delete;
		~CompoundElementArray () = default;

	private:
		size_t mCapacity;
		size_t mSize;

		std::vector<T> mElements;
		std::vector<CompoundElementNode> mPositions;

		std::vector<size_t> mFreeList;
	};

	/*
	template <class T>
	class PackedCompoundArray {
	public:
		PackedCompoundArray () {
			objects	.reserve (max_objects);
			nodes	.reserve (max_objects);
		}
		
		template<class ... Args>
		HandleID add (Args && ... args) {
			
		}
	
		void remove (HandleID id) {
				
		}

		T & operator[] (HandleID id) {
			
		}

		bool isValid (HandleID id) const {
			return (id.isValid () && id.index < nodes.size () && id.version == nodes[id.index].version);
		}

	private:
		// since we may hold a heavy number of elements, keeping continuity across the whole array may be difficult, thats why we are keeping buckets of predifined size
		// that can hold only a certain number of elements
		// the goal is to never split a compound, so in the case of a compound being greater in size than a CompountelementBucket we may:
		// 1) create a fresh new one or 2) create a new one and increase its default size if its not enought
		std::vector<CompoundElementBucket<T>> buckets;
		std::unordered_map<HandleID, size_t> positions; // HandleID -> position in the vector
		
	};
	*/
}
