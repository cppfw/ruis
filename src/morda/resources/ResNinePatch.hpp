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
	const std::shared_ptr<ResImage> lt, t, rt, l, m, r, lb, b, rb;//TODO: make private
	
	ResNinePatch(const ResNinePatch&) = delete;
	ResNinePatch& operator=(const ResNinePatch&) = delete;
	
	ResNinePatch(const std::shared_ptr<ResImage> image, Sidesr borders) :
			image(std::move(image)),
			borders(borders)
	{}
	
//	ResNinePatch(const std::array<std::shared_ptr<ResImage>, 9>& images) :
//			lt(images[0]),
//			t(images[1]),
//			rt(images[2]),
//			l(images[3]),
//			m(images[4]),
//			r(images[5]),
//			lb(images[6]),
//			b(images[7]),
//			rb(images[8])
//	{}
	
	ResNinePatch(
			const std::shared_ptr<ResImage>& lt,
			const std::shared_ptr<ResImage>& t,
			const std::shared_ptr<ResImage>& rt,
			const std::shared_ptr<ResImage>& l,
			const std::shared_ptr<ResImage>& m,
			const std::shared_ptr<ResImage>& r,
			const std::shared_ptr<ResImage>& lb,
			const std::shared_ptr<ResImage>& b,
			const std::shared_ptr<ResImage>& rb
		) :
			lt(lt),
			t(t),
			rt(rt),
			l(l),
			m(m),
			r(r),
			lb(lb),
			b(b),
			rb(rb)
	{}
	
	std::array<std::shared_ptr<ResImage>, 9> get(std::array<real, 4> borders)const;
private:
	static std::shared_ptr<ResNinePatch> load(const stob::Node& chain, const papki::File& fi);
};

}
