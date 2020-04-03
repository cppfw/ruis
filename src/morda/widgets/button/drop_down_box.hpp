#pragma once

#include "../widget.hpp"
#include "../group/pile.hpp"

namespace morda{

/**
 * @brief Drop down box base class.
 * This is a base class for drop down box widgets.
 * Drop down box widget allows selection of an item from list of items.
 */
class drop_down_box : virtual public widget{
	pile& selection_container;
public:
	/**
	 * @brief Item provider class.
	 * User should subclass this class to provide items to a drop down box.
	 */
	class provider : virtual public utki::shared{
		friend class drop_down_box;

		drop_down_box* dd = nullptr;
	protected:
		provider(){}
	public:
		/**
		 * @brief Get drop down box widget to which this provider is set.
		 * @return pointer to a drop down box widget.
		 * @return nullptr if this provider is not set to any drop down box.
		 */
		drop_down_box* get_drop_down_box(){
			return this->dd;
		}

		/**
		 * @brief Get number of items.
		 * Override this method to propvide total number of items to choose from.
		 * @return Number of items in the choice list.
		 */
		virtual size_t count()const noexcept = 0;

		/**
		 * @brief Get widget for item.
		 * Override this method to provide a widget for requested item.
		 * @param index - index of the item to provide widget for.
		 * @return Widget for requested item.
		 */
		virtual std::shared_ptr<widget> get_widget(size_t index) = 0;

		/**
		 * @brief Recycle item widget.
		 * Override this method to recycle item widgets to use them for another items for optimization.
		 * @param index - index of item to recycle widget for.
		 * @param w - item widget to recycle.
		 */
		virtual void recycle(size_t index, std::shared_ptr<widget> w){}

		/**
		 * @brief Notify about change of items model.
		 */
		void notify_data_set_changed();
	};

private:
	std::shared_ptr<provider> item_provider;

	std::size_t selected_index = 0;
public:
	void set_provider(std::shared_ptr<provider> item_provider = nullptr);

	provider* get_provider(){
		return this->item_provider.get();
	}
protected:
	drop_down_box(std::shared_ptr<morda::context> c, const puu::forest& desc, pile& selection_container);

	drop_down_box(const drop_down_box&) = delete;
	drop_down_box& operator=(const drop_down_box&) = delete;

public:
	/**
	 * @brief Set currently selected item.
	 * @param i - index of the item to set as currently selected.
	 */
	void set_selection(size_t i);

	/**
	 * @brief Get index of the selected item.
	 * @return Index of the selected item.
	 */
	std::size_t get_selection()const noexcept{
		return this->selected_index;
	}

	std::function<void(drop_down_box& dds)> selection_handler;

private:
	void handle_data_set_changed();
};

}
