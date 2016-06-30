/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "../util/Sides.hpp"

#include "../resources/ResNinePatch.hpp"

#include "core/container/Frame.hpp"
#include "core/container/Table.hpp"

#include "label/ImageLabel.hpp"

namespace morda{


class NinePatch :
	public virtual Widget,
	private Table
{
	std::shared_ptr<ResNinePatch> image;
	
	std::shared_ptr<ResNinePatch::ImageMatrix> scaledImage;
	
	Sidesr borders;
	
	std::shared_ptr<ImageLabel> lt, t, rt, l, m, r, lb, b, rb;
	
	std::shared_ptr<Frame> content_var;
	
public:	
	NinePatch(const NinePatch&) = delete;
	NinePatch& operator=(const NinePatch&) = delete;
	
	NinePatch(const stob::Node* chain = nullptr);
	
	void setNinePatch(const std::shared_ptr<ResNinePatch>& np);
	
	Frame& content(){
		return *this->content_var;
	}
	
	void render(const morda::Matr4r& matrix) const override;

	decltype(borders) getBorders()const noexcept{
		return this->borders;
	}
	
	void setCenterVisible(bool visible);
	
	void setBorders(Sidesr borders){
		this->borders = borders;
		this->applyImages();
	}
	
private:
	void applyImages();
};

}
