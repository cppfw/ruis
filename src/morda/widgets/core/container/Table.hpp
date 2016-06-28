/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <functional>

#include "LinearArea.hpp"
#include "TableRow.hpp"

namespace morda{


/**
 * @brief Table container.
 * It finds the maximum weight and maximum width in the column to use it in row lay outing.
 */
class Table : public VerticalArea{
public:
	Table(const stob::Node* chain = nullptr);
	
	Table(const Table&) = delete;
	Table& operator=(const Table&) = delete;
	
	void layOut()override;
	
	morda::Vec2r measure(const morda::Vec2r& quotum)const override;
	
protected:
	const Widget::LayoutParams& getLayoutParamsDuringLayout(const Widget& w)const override;

	
private:
	void updateRowsLayoutParam(const morda::Vec2r& constraint)const;
};



}
