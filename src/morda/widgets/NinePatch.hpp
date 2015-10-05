/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once


#include "containers/FrameContainer.hpp"
#include "containers/TableContainer.hpp"

#include "../resources/ResNinePatch.hpp"
#include "labels/ImageLabel.hpp"

namespace morda{


class NinePatch :
	public virtual Widget,
	private TableContainer
{
	std::shared_ptr<ResNinePatch> image;//hold reference to resource to avoid data duplication
	
	std::shared_ptr<ImageLabel> lt, t, rt, l, m, r, lb, b, rb;
	
	std::shared_ptr<FrameContainer> content;
	
public:	
	NinePatch(const NinePatch&) = delete;
	NinePatch& operator=(const NinePatch&) = delete;
	
	NinePatch(const stob::Node* chain = nullptr);
	
	void SetNinePatch(const std::shared_ptr<ResNinePatch>& np);
	
	FrameContainer& Content(){
		return *this->content;
	}
private:

};

}
