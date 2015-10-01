#include "ResNinePatch.hpp"
#include "../App.hpp"
#include "../util/util.hpp"


using namespace morda;



std::shared_ptr<ResNinePatch> ResNinePatch::Load(const stob::Node& chain, const papki::File& fi){
	auto tex = morda::App::inst().resMan.Load<ResTexture>(chain.side("tex").up().value());
	
	auto rect = Rect2rFromSTOB(&chain.side("rect").up());
	
	auto borders = Rect2rFromSTOB(&chain.side("borders").up());
	
	return utki::makeShared<ResNinePatch>(
			utki::makeShared<ResImage>(tex, Rectr(rect.p, borders.p)), //lt
			utki::makeShared<ResImage>(tex, Rectr(rect.p.x + borders.p.x, rect.p.y, rect.d.x - borders.p.x - borders.d.x, borders.p.y)), //t
			utki::makeShared<ResImage>(tex, Rectr(rect.right() - borders.d.x, rect.p.y, borders.d.x, borders.p.y)), //rt
			utki::makeShared<ResImage>(tex, Rectr(rect.p.x, rect.p.y + borders.p.y, borders.p.x, rect.d.y - borders.p.y - borders.d.y)), //l
			utki::makeShared<ResImage>(tex, Rectr(rect.p.x + borders.p.x, rect.p.y + borders.p.y, rect.d.x - borders.p.x - borders.d.x, rect.d.y - borders.p.y - borders.d.y)), //m
			utki::makeShared<ResImage>(tex, Rectr(rect.right() - borders.d.x, rect.p.y + borders.p.y, borders.d.x, rect.d.y - borders.p.y - borders.d.y)), //r
			utki::makeShared<ResImage>(tex, Rectr(rect.p.x, rect.top() - borders.d.y, borders.p.x, borders.d.y)), //lb
			utki::makeShared<ResImage>(tex, Rectr(rect.p.x + borders.p.x, rect.top() - borders.d.y, rect.d.x - borders.p.x - borders.d.x, borders.d.y)), //b
			utki::makeShared<ResImage>(tex, Rectr(rect.right() - borders.d.x, rect.top() - borders.d.y, borders.d.x, borders.d.y)) //br
		);
}
