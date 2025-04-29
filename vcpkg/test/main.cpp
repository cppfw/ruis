#include <ruis/util/color.hpp>

int main(int argc, const char** argv){
    auto c = ruis::color::make_from(tml::read("0x12345678"));

    std::cout << "color = " << std::hex << "0x" << c.to_uint32_t() << std::endl;

    return 0;
}
