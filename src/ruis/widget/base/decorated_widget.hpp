#pragma once

#include "../widget.hpp"

namespace ruis {

/**
 * @brief Base class for widgets that decorate other widgets.
 *
 * The decorated_widget class template implements the Decorator pattern for widgets.
 * It wraps another widget (the "decorated" widget) and can modify or extend its behavior.
 *
 * In the class hierarchy, the decorated_widget must be initialized after the decorated
 * widget's ancestor container to ensure proper parent-child relationships.
 *
 * @tparam widget_type The type of the widget being decorated. Must be a widget type.
 */
template <typename widget_type>
class decorated_widget : virtual public widget {
    widget_type& decorated;

protected:
    /**
     * @brief Construct a decorated_widget.
     *
     * @param context Shared reference to the ruis context.
     * @param decorated Reference to the widget being decorated.
     *                 This widget must have this decorated_widget as an ancestor.
     */
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
    /**
     * @brief Get a mutable reference to the decorated widget.
     *
     * @return Reference to the decorated widget.
     */
    widget_type& get_decorated() noexcept
    {
        return this->decorated;
    }

    /**
     * @brief Get a const reference to the decorated widget.
     *
     * @return Const reference to the decorated widget.
     */
    const widget_type& get_decorated() const noexcept
    {
        return this->decorated;
    }
};
}