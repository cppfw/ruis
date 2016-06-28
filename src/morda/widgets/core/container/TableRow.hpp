/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "LinearContainer.hpp"
#include "TableContainer.hpp"

namespace morda{

class TableRow : public Horizontal{
	friend class TableContainer;
public:
	TableRow(const stob::Node* chain = nullptr);
	
	TableRow(const TableRow&) = delete;
	TableRow& operator=(const TableRow&) = delete;
	
protected:
	class LayoutParams : public Horizontal::LayoutParams{
		friend class TableRow;
		friend class TableContainer;
		
		mutable Horizontal::LayoutParams processedParams;
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
