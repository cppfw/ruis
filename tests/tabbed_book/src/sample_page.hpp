#pragma once

#include "../../../src/morda/widgets/group/book.hpp"
#include "../../../src/morda/widgets/group/row.hpp"

class sample_page :
        public morda::page,
        public morda::row
{
public:
    sample_page(const utki::shared_ref<morda::context>& c, const std::string& text);
    ~sample_page(){
        std::cout << "page destroyed" << std::endl;
    }
};
