#pragma once

#include "../widget.hpp"
#include "NinePatchPushButton.hpp"

namespace morda{

/**
 * @brief Dropdown selector.
 * This widget allows selection of an item from a list of items.
 * From GUI script it can be instantiated as "DropDownSelector".
 * If any child widgets are specified then those will be used as items to select from.
 */
class DropDownSelector :
		virtual public widget,
		private NinePatchPushButton
{
	Pile& selectionContainer;
public:
	/**
	 * @brief Item provider class.
	 * User should subclass this class to provide items to a dropdown selector.
	 */
	class ItemsProvider : virtual public utki::shared{
		friend class DropDownSelector;

		DropDownSelector* dd = nullptr;
	protected:
		ItemsProvider(){}
	public:
		/**
		 * @brief Get Dropdown selector widget to which this provider is set.
		 * @return pointer to a DropdownSelector widget.
		 * @return nullptr if this provider is not set to any DropdownSelector.
		 */
		DropDownSelector* dropDownSelector(){
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
		virtual std::shared_ptr<widget> getWidget(size_t index) = 0;

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
		void notifyDataSetChanged();
	};

private:
	std::shared_ptr<ItemsProvider> provider;

	std::size_t selectedItem_v = 0;

	int hoveredIndex = -1;
public:
	void setItemsProvider(std::shared_ptr<ItemsProvider> provider = nullptr);

public:
	DropDownSelector(std::shared_ptr<morda::context> c, const puu::forest& desc);

	DropDownSelector(const DropDownSelector&) = delete;
	DropDownSelector& operator=(const DropDownSelector&) = delete;

	/**
	 * @brief Set currently selected item.
	 * @param i - index of the item to set as currently selected.
	 */
	void setSelection(size_t i);

	/**
	 * @brief Get index of the selected item.
	 * @return Index of the selected item.
	 */
	std::size_t selectedItem()const noexcept{
		return this->selectedItem_v;
	}

	std::function<void(DropDownSelector& dds)> selectionChanged;

private:
	void handleDataSetChanged();

	std::shared_ptr<widget> wrapItem(std::shared_ptr<widget>&& w, size_t index);

	void showDropdownMenu();

	void mouseButtonUpHandler(bool isFirstOne);

	bool on_mouse_button(bool isDown, const morda::Vec2r& pos, mouse_button button, unsigned pointerID)override;
};

}
