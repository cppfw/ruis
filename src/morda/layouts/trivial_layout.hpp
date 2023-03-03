#pragma once

#include "../layout.hpp"

namespace morda{

class trivial_layout : public layout{
public:
    static const utki::shared_ref<trivial_layout> instance;
};

}
