/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include "../ResourceManager.hpp"

#include "ResTexture.hpp"
#include <kolme/Rectangle.hpp>
#include "../shaders/PosTexShader.hpp"

#include <array>

namespace morda{

class ResImage : public Resource{
	friend class ResourceManager;
	
protected:
	ResImage(){}
	
public:
	ResImage(const ResImage& orig) = delete;
	ResImage& operator=(const ResImage& orig) = delete;
	
	virtual bool isSvg()const noexcept = 0;

	virtual Vec2r dim(real dpi = 0)const noexcept = 0;
	
	virtual void render(const Matr4r& matrix, PosTexShader& s)const = 0;
private:
	static std::shared_ptr<ResImage> load(const stob::Node& chain, const papki::File& fi);
};



class ResRasterImage : public ResImage{
	friend class ResImage;
	
	std::shared_ptr<ResTexture> tex;
	
	std::array<Vec2r, 4> texCoords;
	
	Vec2r dim_var;
	
public:
	ResRasterImage(std::shared_ptr<ResTexture> tex, const Rectr& rect);
	
	ResRasterImage(const ResRasterImage& orig) = delete;
	ResRasterImage& operator=(const ResRasterImage& orig) = delete;
	
	Vec2r dim(real dpi) const noexcept override{
		return this->dim_var;
	}
	
	void render(const Matr4r& matrix, PosTexShader& s)const override;
	
	bool isSvg() const noexcept override{
		return false;
	}

private:
	static std::shared_ptr<ResRasterImage> load(const stob::Node& chain, const papki::File& fi);
};

}
