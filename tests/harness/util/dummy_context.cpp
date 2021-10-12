#include "dummy_context.hpp"

#include "fake_renderer.hpp"

std::shared_ptr<morda::context> make_dummy_context(){
    return std::make_shared<morda::context>(
            std::make_shared<fake_renderer>(),
            std::make_shared<morda::updater>(),
            [](std::function<void()>&&){},
            [](morda::mouse_cursor){},
            morda::real(0),
            morda::real(0)
        );
}
