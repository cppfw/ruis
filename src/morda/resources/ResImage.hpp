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
	
	std::shared_ptr<ResTexture> tex;
	
	std::array<Vec2r, 4> texCoords;
	
	Vec2r dim_var;
	
public:
	ResImage(std::shared_ptr<ResTexture> tex, const Rectr& rect);
	
	ResImage(const ResImage& orig) = delete;
	ResImage& operator=(const ResImage& orig) = delete;
	
	const Vec2r& dim()const noexcept{
		return this->dim_var;
	}
	
	void render(const Matr4r& matrix, PosTexShader& s)const;
	
private:
	static std::shared_ptr<ResImage> load(const stob::Node& chain, const papki::File& fi);
};

}
