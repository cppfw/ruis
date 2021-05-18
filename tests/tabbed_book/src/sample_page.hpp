#pragma once

#include "../../../src/morda/morda/widgets/group/book.hpp"
#include "../../../src/morda/morda/widgets/label/text.hpp"

class sample_page :
        public morda::page,
        public morda::text
{
public:
    sample_page(std::shared_ptr<morda::context> c, const treeml::forest& desc);
};
