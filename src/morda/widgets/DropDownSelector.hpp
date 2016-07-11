#pragma once

#include "core/Widget.hpp"
#include "core/container/LinearArea.hpp"
#include "core/container/Frame.hpp"

namespace morda{

class DropDownSelector :
		virtual public Widget,
		private HorizontalArea
{
	std::shared_ptr<Frame> selectionContainer;
public:
	class ItemsProvider : virtual public utki::Shared{
		friend class DropDownSelector;
		
		DropDownSelector* dd = nullptr;
	protected:
		ItemsProvider(){}
	public:
		DropDownSelector* dropDownSelector(){
			return this->dd;
		}
		
		virtual size_t count()const noexcept = 0;
		
		virtual std::shared_ptr<Widget> getWidget(size_t index) = 0;
		
		virtual void recycle(size_t index, std::shared_ptr<Widget> w){}
		
		void notifyDataSetChanged();
	};
	
private:
	std::shared_ptr<ItemsProvider> provider;
	
	std::size_t selectedItem_v = 0;
public:
	void setItemsProvider(std::shared_ptr<ItemsProvider> provider = nullptr);
	
public:
	
	DropDownSelector(const stob::Node* chain = nullptr);
	
	DropDownSelector(const DropDownSelector&) = delete;
	DropDownSelector& operator=(const DropDownSelector&) = delete;
	
	void setSelection(size_t i);
	
	std::size_t selectedItem()const noexcept{
		return this->selectedItem_v;
	}
	
private:
	void handleDataSetChanged();
	
	std::shared_ptr<Widget> wrapItem(std::shared_ptr<Widget>&& w, size_t index);
};

}
