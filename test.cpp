
#include "tests/rendering.hpp"

#include <filesystem>

int main (__attribute__ ((unused)) int argc, __attribute__ ((unused)) char ** argv) {

	printf ("Info: Current Working Directory : %ls\n", std::filesystem::current_path ().c_str ());
	TestRendering::test ();
}