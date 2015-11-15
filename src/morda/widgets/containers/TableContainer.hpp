/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <functional>

#include "LinearContainer.hpp"
#include "TableRow.hpp"

namespace morda{


/**
 * @brief Table container.
 * It finds the maximum weight and maximum width in the column to use it in row lay outing.
 */
class TableContainer : public  VerticalContainer{
public:
	TableContainer(const stob::Node* chain = nullptr);
	
	TableContainer(const TableContainer&) = delete;
	TableContainer& operator=(const TableContainer&) = delete;
	
	void layOut()override;
	
	morda::Vec2r measure(const morda::Vec2r& quotum)const override;
	
protected:
	Widget::LayoutParams& getLayoutParams_internal(Widget& w)override;
	
private:
	void UpdateRowsLayoutParam(const morda::Vec2r& constraint)const;
};



}
