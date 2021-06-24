#include "util.hxx"

using namespace mordavokne;

version_duplet mordavokne::get_opengl_version_duplet(window_params::graphics_api api){
    using ga = window_params::graphics_api;
    switch(api){
        case ga::gl_2_0:
            return {2, 0};
        case ga::gl_2_1:
            return {2, 1};
        case ga::gl_3_0:
            return {3, 0};
        case ga::gl_3_1:
            return {3, 1};
        case ga::gl_3_2:
            return {3, 2};
        case ga::gl_3_3:
            return {3, 3};
        case ga::gl_4_0:
            return {4, 0};
        case ga::gl_4_1:
            return {4, 1};
        case ga::gl_4_2:
            return {4, 2};
        case ga::gl_4_3:
            return {4, 3};
        case ga::gl_4_4:
            return {4, 4};
        case ga::gl_4_5:
            return {4, 5};
        case ga::gl_4_6:
            return {4, 6};
        default:
            break;
    }

    std::stringstream ss;
    ss << "non-opengl api requested from opengl-based implementation of mordavokne: window_params::graphics_api = ";
    ss << unsigned(api);

    throw std::logic_error(ss.str());
}
