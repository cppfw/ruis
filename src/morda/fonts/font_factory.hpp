#pragma once

#include "font.hpp"

namespace feis{

class font_factory{
public:
    virtual std::unique_ptr<font> create(size_t size) = 0;
};

}
