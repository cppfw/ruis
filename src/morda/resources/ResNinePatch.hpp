/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "../config.hpp"

#include "../ResourceManager.hpp"

#include "ResImage.hpp"

namespace morda{

class ResNinePatch : public Resource{
	friend class ResourceManager;
	
	const std::shared_ptr<ResImage> image;
	
	Sidesr borders;
	
public:
	ResNinePatch(const ResNinePatch&) = delete;
	ResNinePatch& operator=(const ResNinePatch&) = delete;
	
	ResNinePatch(const std::shared_ptr<ResImage> image, Sidesr borders) :
			image(std::move(image)),
			borders(borders)
	{}
	
	class ImageMatrix :	virtual public utki::Shared{
		const std::array<std::array<std::shared_ptr<const ResImage>, 3>, 3> images_v;
		
		std::weak_ptr<const ResNinePatch> parent;
	
		real mul;//for erasing from the cache
	public:
		decltype(images_v)& images()const noexcept{
			return this->images_v;
		}
		
		ImageMatrix(std::array<std::array<std::shared_ptr<const ResImage>, 3>, 3>&& l, std::shared_ptr<const ResNinePatch> parent, real mul);
		
		~ImageMatrix()noexcept;
	};
	
	std::shared_ptr<ImageMatrix> get(Sidesr borders)const;
	
	const decltype(borders)& getBorders()const noexcept{
		return this->borders;
	}
private:
	mutable std::map<real, std::weak_ptr<ImageMatrix>> cache;
	
	static std::shared_ptr<ResNinePatch> load(const stob::Node& chain, const papki::File& fi);
};

}
