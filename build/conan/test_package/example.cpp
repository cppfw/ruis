#include <ruis/util/length.hpp>

#include <iostream>

int main(int argc, const char** argv){
	ruis::length len(13);

	std::cout << "hello ruis! len.is_undefined() = " << (len.is_undefined() ? "true" : "false") << std::endl;

	return 0;
}