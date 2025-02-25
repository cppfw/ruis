#pragma once

#include <ruis/widget/group/book.hpp>

class sample_page :
        public ruis::page,
        public ruis::container
{
public:
    sample_page(
        utki::shared_ref<ruis::context> context, //
        std::string_view text
    );

    sample_page(const sample_page&) = delete;
    sample_page& operator=(const sample_page&) = delete;

    sample_page(sample_page&&) = delete;
    sample_page& operator=(sample_page&&) = delete;

    ~sample_page()override{
        std::cout << "page destroyed" << std::endl;
    }
};
