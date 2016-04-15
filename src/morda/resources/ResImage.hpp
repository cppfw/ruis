/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include <array>

#include <kolme/Rectangle.hpp>

#include "../ResourceManager.hpp"
#include "../shaders/PosTexShader.hpp"
#include "ResTexture.hpp"



namespace morda{

class ResImage : public Resource{
	friend class ResourceManager;
	
protected:
	ResImage(){}
	
public:
	ResImage(const ResImage& orig) = delete;
	ResImage& operator=(const ResImage& orig) = delete;
	
	class Image : public utki::Shared{
	protected:
		Texture2D tex;
	public:
		Image(Texture2D&& tex) :
				tex(std::move(tex))
		{}

		void render(const Matr4r& matrix, PosTexShader& s)const;
	};
	
	virtual bool isScalable()const noexcept = 0;

	virtual Vec2r dim(real dpi = 96)const noexcept = 0;
	
	/**
	 * @brief Get raster image of given dimensions.
	 * @param forDim - dimensions request for raster image.
	 *        If any of the dimensions is 0 then it will be adjusted to preserve aspect ratio.
	 */
	virtual std::shared_ptr<Image> get(Vec2r forDim = 0)const{
		return nullptr;
	}
	
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
	
	bool isScalable() const noexcept override{
		return false;
	}
	
private:
	static std::shared_ptr<ResRasterImage> load(const stob::Node& chain, const papki::File& fi);
};


}
