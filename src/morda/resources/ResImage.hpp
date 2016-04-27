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
	
	class Texture : virtual public utki::Shared{
	protected:
		Texture2D tex_v;
	public:
		Texture(Texture2D&& texture) :
				tex_v(std::move(texture))
		{}
		
		const Texture2D& tex()const noexcept{
			return this->tex_v;
		}

		void render(const Matr4r& matrix, PosTexShader& s, const std::array<kolme::Vec2f, 4>& texCoords = PosTexShader::quadFanTexCoords)const;
	};
	
	virtual bool isScalable()const noexcept = 0; //TODO: remove

	virtual Vec2r dim(real dpi = 96)const noexcept = 0;
	
	/**
	 * @brief Get raster image of given dimensions.
	 * @param forDim - dimensions request for raster image.
	 *        If any of the dimensions is 0 then it will be adjusted to preserve aspect ratio.
	 */
	virtual std::shared_ptr<const Texture> get(Vec2r forDim = 0)const = 0;
	
	virtual void render_old(const Matr4r& matrix, PosTexShader& s)const{}//TODO: remove
private:
	static std::shared_ptr<ResImage> load(const stob::Node& chain, const papki::File& fi);
	
public:
	static std::shared_ptr<ResImage> load(const papki::File& fi);
};



class ResAtlasRasterImage : public ResImage{
	friend class ResImage;
	
	std::shared_ptr<ResTexture> tex;
	
	std::array<Vec2r, 4> texCoords;
	
	Vec2r dim_v;
	
public:
	ResAtlasRasterImage(std::shared_ptr<ResTexture> tex, const Rectr& rect);
	
	ResAtlasRasterImage(const ResAtlasRasterImage& orig) = delete;
	ResAtlasRasterImage& operator=(const ResAtlasRasterImage& orig) = delete;
	
	Vec2r dim(real dpi) const noexcept override{
		return this->dim_v;
	}
	
	void render_old(const Matr4r& matrix, PosTexShader& s)const override;
	
	bool isScalable() const noexcept override{
		return false;
	}
	
	virtual std::shared_ptr<const Texture> get(Vec2r forDim = 0)const override{
		return nullptr;
	}
	
private:
	static std::shared_ptr<ResAtlasRasterImage> load(const stob::Node& chain, const papki::File& fi);
};


}
