#include "ResCursor.hpp"

#include "../Morda.hpp"

#include "../util/util.hpp"


using namespace morda;

ResCursor::ResCursor(ResImage& image, const Vec2r& hotspot) :
		image_v(std::dynamic_pointer_cast<ResImage>(image.shared_from_this())),
		hotspot_v(hotspot)
{}


std::shared_ptr<ResCursor> ResCursor::load(const stob::Node& chain, const papki::File& fi) {
	std::shared_ptr<ResImage> image;
	if(auto n = getProperty(&chain, "image")){
		image = morda::Morda::inst().resMan.load<ResImage>(n->value());
	}else{
		throw ResourceManager::Exc("ResCursor::load(): resource description does not contain 'image' property");
	}
	
	Vec2r hotspot;
	if(auto n = getProperty(&chain, "hotspot")){
		hotspot = makeVec2rFromSTOB(n);
		hotspot.y = image->dim().y - hotspot.y;
	}else{
		throw ResourceManager::Exc("ResCursor::load(): resource description does not contain 'hotspot' property");
	}
	
	return utki::makeShared<ResCursor>(*image, hotspot);
}
