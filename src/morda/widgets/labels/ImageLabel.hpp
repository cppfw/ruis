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
	
	std::shared_ptr<morda::ResImage> img;
	
	mutable std::shared_ptr<const morda::ResImage::Texture> scaledImage;
	
	bool keepAspectRatio;
	
	kolme::Vec2b repeat;
	mutable std::array<kolme::Vec2f, 4> texCoords;
public:
	ImageLabel(const stob::Node* chain = nullptr);
public:
	virtual ~ImageLabel()noexcept{}
	
	void render(const morda::Matr4r& matrix)const override;

	morda::Vec2r measure(const morda::Vec2r& quotum)const override;
	
	void setImage(const std::shared_ptr<ResImage>& image);
	
	void onResize() override;
private:

};

}
