#include <ruis/util/length.hpp>

#include <iostream>

int main(int argc, const char** argv){
    auto l = ruis::length::make_pp(13);

    std::cout << "length = " << l << std::endl;

    return 0;
}
