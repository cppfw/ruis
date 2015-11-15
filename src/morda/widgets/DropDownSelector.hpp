#pragma once

#include "Widget.hpp"
#include "containers/LinearContainer.hpp"

namespace morda{

class DropDownSelector :
		virtual public Widget,
		private HorizontalContainer
{
public:
	class ItemsProvider : virtual public utki::Shared{
		friend class DropDownSelector;
		
		DropDownSelector* dd = nullptr;
	protected:
		ItemsProvider(){}
	public:
		
		virtual size_t count()const noexcept = 0;
		
		virtual std::shared_ptr<Widget> getWidget(size_t index) = 0;
		
		virtual void recycle(size_t index, std::shared_ptr<Widget> w){}
		
		void notifyDataSetChanged();
	};
	
private:
	std::shared_ptr<ItemsProvider> provider;
	
	std::size_t selectedItem = 0;
public:
	void setItemsProvider(std::shared_ptr<ItemsProvider> provider = nullptr);
	
public:
	
	DropDownSelector(const stob::Node* chain = nullptr);
	
	DropDownSelector(const DropDownSelector&) = delete;
	DropDownSelector& operator=(const DropDownSelector&) = delete;
	
	
private:
	void handleDataSetChanged();
};

}
