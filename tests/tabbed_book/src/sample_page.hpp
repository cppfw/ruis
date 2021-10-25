#pragma once

#include "../../../src/morda/widgets/group/book.hpp"
#include "../../../src/morda/widgets/label/text.hpp"

class sample_page :
        public morda::page,
        public morda::text
{
public:
    sample_page(std::shared_ptr<morda::context> c, const std::string& text);
    ~sample_page(){
        std::cout << "page destroyed" << std::endl;
    }
};
