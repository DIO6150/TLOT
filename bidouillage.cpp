#include <iostream>
#include <typeindex>
#include <string>
#include <vector>
#include <stdint.h>

class IFoo {
	public:
	virtual void PrintContent () = 0;
};

template<typename T>
class Foo : public IFoo {
	T data;

	public:
	Foo (T arg) {
		data = arg;
	}

	void PrintContent () override {
		std::cout << data << "\n";
	}
};

class Bar {
	std::vector<IFoo *> m_vec;

	using lbtype = decltype ([](Bar & other) -> void {});

	std::vector<lbtype> m_vec2;
public:
	template<typename T>
	void Create (T arg) {
		//std::cout << arg << "\n";
		m_vec.push_back (new Foo<T> {arg});
	}

	template<typename T, typename ... C>
	void Create (T arg, C ... args) {
		//std::cout << arg << ", ";
		m_vec.push_back (new Foo<T> {arg});
		Create (args ...);
	}

	template<typename ... C>
	Bar (C ... args) {
		Create (args...);
	}

	void PrintContent () {
		for (auto & e : m_vec) {
			e->PrintContent ();
		}
	}
};


template <typename ... Ts>
std::vector<uint64_t> generateTypeHashes() {
    return { static_cast<uint64_t>(typeid(Ts).hash_code())... };
}

template<typename ... args>
inline uint64_t multi_type_hash () {
	return (static_cast<uint64_t> (typeid (args).hash_code ()) ^ ...);
}


int main (void) {
	Bar b{5};
	b.PrintContent ();

	auto v = generateTypeHashes<int, float, std::string> ();

	for (auto & e : v) {
		std::cout << e << "\n";
	}
	return (0);
}