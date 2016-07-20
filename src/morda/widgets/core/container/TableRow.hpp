#pragma once

#include "LinearArea.hpp"
#include "Table.hpp"

namespace morda{

/**
 * @brief Row of a table container.
 * This is a container which should be used as a row of a Table container.
 * From GUI script it can be instantiated as "TableRow".
 */
class TableRow : public HorizontalArea{
	friend class Table;
public:
	TableRow(const stob::Node* chain = nullptr);
	
	TableRow(const TableRow&) = delete;
	TableRow& operator=(const TableRow&) = delete;
	
protected:
	class LayoutParams : public HorizontalArea::LayoutParams{
		friend class TableRow;
		friend class Table;
		
		mutable HorizontalArea::LayoutParams processedParams;
	public:
		LayoutParams(const stob::Node* chain = nullptr);
	};

	const Widget::LayoutParams& getLayoutParamsDuringLayout(const Widget& w)const override;

	
private:
	std::unique_ptr<Widget::LayoutParams> createLayoutParams(const stob::Node* chain)const override{
		return utki::makeUnique<LayoutParams>(chain);
	}
	
	const LayoutParams& getTableRowLayoutParams(const Widget& w)const;
};

}
