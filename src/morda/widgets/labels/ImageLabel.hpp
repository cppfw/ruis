/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include "../Widget.hpp"
#include "../../resources/ResImage.hpp"


namespace morda{

class ImageLabel : public virtual Widget{
	ImageLabel(const ImageLabel&);
	ImageLabel& operator=(const ImageLabel&);
	
	std::shared_ptr<const morda::ResImage> img;
	
	mutable std::shared_ptr<const morda::ResImage::QuadTexture> scaledImage;
	
	bool keepAspectRatio;
	
	kolme::Vec2b repeat_v;
	mutable std::array<kolme::Vec2f, 4> texCoords;
public:
	ImageLabel(const stob::Node* chain = nullptr);
public:
	virtual ~ImageLabel()noexcept{}
	
	void render(const morda::Matr4r& matrix)const override;

	morda::Vec2r measure(const morda::Vec2r& quotum)const override;
	
	void setImage(const std::shared_ptr<const ResImage>& image);
	
	void onResize() override;
	
	const decltype(repeat_v)& repeat()const noexcept{
		return this->repeat_v;
	}
	
	void setRepeat(decltype(repeat_v) r){
		this->repeat_v = r;
		this->scaledImage.reset();
	}
private:

};

}
