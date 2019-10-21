#pragma once

#include <puu/dom.hpp>

#include "../ResourceManager.hpp"

#include "../render/Texture2D.hpp"


namespace morda{


/**
 * @brief Texture resource.
 * 
 * %Resource description:
 * 
 * @param file - name of the image file, can be raster image.
 * 
 * Example:
 * @code
 * tex_sample{
 *     file{texture_sample.png}
 * }
 * @endcode
 */
//TODO: make intrusive PoolStored
class ResTexture : public morda::Resource{
	friend class morda::ResourceManager;

	std::shared_ptr<Texture2D> tex_v;
public:
	/**
	 * @brief Create texture.
	 * @param texture - texture object to initialize this resource with.
	 */
	ResTexture(decltype(tex_v) texture) :
			tex_v(std::move(texture))
	{
		ASSERT(this->tex_v)
	}

	~ResTexture()noexcept{}

	/**
	 * @brief Get texture object held by this resource.
	 * @return Texture object.
	 */
	const Texture2D& tex()const noexcept{
		return *this->tex_v;
	}

private:
	static std::shared_ptr<ResTexture> load(const stob::Node& chain, const papki::File& fi);
};



}//~namespace
