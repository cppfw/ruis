/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "LinearContainer.hpp"
#include "TableContainer.hpp"

namespace morda{

class TableRow : public HorizontalContainer{
	friend class TableContainer;
public:
	TableRow(const stob::Node* chain = nullptr);
	
	TableRow(const TableRow&) = delete;
	TableRow& operator=(const TableRow&) = delete;
	
protected:
	class LayoutParams : public HorizontalContainer::LayoutParams{
		friend class TableRow;
		friend class TableContainer;
		
		HorizontalContainer::LayoutParams processedParams;
	public:
		LayoutParams(const stob::Node* chain = nullptr);
	};
	
	Widget::LayoutParams& getLayoutParams_internal(Widget& w) override;

private:
	std::unique_ptr<Widget::LayoutParams> createLayoutParams(const stob::Node* chain)const override{
		return utki::makeUnique<LayoutParams>(chain);
	}
	
	LayoutParams& getTableRowLayoutParams(Widget& w);
};

}
