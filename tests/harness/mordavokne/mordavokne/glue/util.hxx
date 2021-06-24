#pragma once

#include "../application.hpp"

namespace mordavokne{

struct version_duplet{
    int major;
    int minor;
};

version_duplet get_opengl_version_duplet(window_params::graphics_api api);

}
