#pragma once

#include <utki/signal.hpp>

#include "../widget.hpp"

namespace ruis {

class list_providable;

/**
 * @brief Item widgets provider class.
 * User should subclass this class to provide item widgets to a list_providable.
 */
class list_provider : virtual public utki::shared
{
	friend class list_providable;

	utki::signal<> model_change_signal;

protected:
	list_provider(utki::shared_ref<ruis::context> context);

public:
	const utki::shared_ref<ruis::context> context;

	/**
	 * @brief Get number of items.
	 * Override this method to propvide total number of items to choose from.
	 * @return Number of items in the choice list.
	 */
	virtual size_t count() const noexcept = 0;

	/**
	 * @brief Get widget for item.
	 * Override this method to provide a widget for requested item.
	 * @param index - index of the item to provide widget for.
	 * @return Widget for requested item.
	 */
	virtual utki::shared_ref<widget> get_widget(size_t index) = 0;

	/**
	 * @brief Recycle item widget.
	 * Override this method to recycle item widgets to use them for another items for optimization.
	 * @param index - index of item to recycle widget for.
	 * @param w - item widget to recycle.
	 */
	virtual void recycle(
		size_t index, //
		std::shared_ptr<widget> w
	)
	{}

	/**
	 * @brief Reload callback.
	 * Called from owner selection_box's on_reload().
	 */
	virtual void on_reload() {}

	/**
	 * @brief Notify about change of items model.
	 * The user is supposed to invoke this function when items model change.
	 */
	void notify_model_change();
};

class list_providable
{
	friend class list_provider;

public:
	struct parameters {
		std::shared_ptr<list_provider> provider;
	};

	decltype(list_provider::model_change_signal)::connection provider_signal_connection;

private:
	parameters params;

public:
	virtual ~list_providable() = default;

	void set_provider(std::shared_ptr<list_provider> provider);

	list_provider* get_provider()
	{
		return this->params.provider.get();
	}

	virtual void handle_model_change() {}
};

} // namespace ruis
