/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "../ResourceManager.hpp"
#include "ResImage.hpp"

namespace morda{

class ResNinePatch : public Resource{
	friend class ResourceManager;
public:
	const std::shared_ptr<ResImage> lt, t, rt, l, m, r, lb, b, rb;
	
	ResNinePatch(const ResNinePatch&) = delete;
	ResNinePatch& operator=(const ResNinePatch&) = delete;
	
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
private:
	static std::shared_ptr<ResNinePatch> load(const stob::Node& chain, const papki::File& fi);
};

}
