/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <stob/dom.hpp>

#include "../render/Texture2D.hpp"


#include "../ResourceManager.hpp"


namespace morda{



//TODO: make intrusive PoolStored
class ResTexture : public morda::Resource{
	friend class morda::ResourceManager;

	Texture2D tex_var;

public:
	ResTexture(const Image& image) :
			tex_var(image)
	{}

	~ResTexture()noexcept{}

	const Texture2D& tex()const noexcept{
		return this->tex_var;
	}

private:
	static std::shared_ptr<ResTexture> load(const stob::Node& chain, const papki::File& fi);
};



}//~namespace
