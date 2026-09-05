#pragma once

#include "../widget.hpp"

namespace ruis {

/**
 * @brief Base class for widgets that decorate other widgets.
 * In the class hieararchy, the decorated_widget must be initialized
 * after the decorated widget's ancestor container.
 */
template <typename widget_type>
class decorated_widget : virtual public widget{
    widget_type& decorated;

protected:
    decorated_widget(
        utki::shared_ref<ruis::context> context, //
        widget_type& decorated
    ) :
        widget(std::move(context), {}, {}),
        decorated(decorated)
    {
        utki::assert(this->decorated.has_ancestor(*this));
    }

public:
    widget_type& get_decorated() noexcept
    {
        return this->decorated;
    }

    const widget_type& get_decorated() const noexcept
    {
        return this->decorated;
    }
};
}
