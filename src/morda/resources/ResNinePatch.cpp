#include "ResNinePatch.hpp"
#include "../App.hpp"
#include "../util/util.hpp"


using namespace morda;



std::shared_ptr<ResNinePatch> ResNinePatch::load(const stob::Node& chain, const papki::File& fi){
	auto tex = morda::App::inst().resMan.load<ResTexture>(chain.side("tex").up().value());
	
	auto rect = makeRectrFromSTOB(&chain.side("rect").up());
	
	auto borders = makeRectrFromSTOB(&chain.side("borders").up());
	
	return utki::makeShared<ResNinePatch>(
			utki::makeShared<ResAtlasRasterImage>(tex, Rectr(rect.p, borders.p)), //lt
			utki::makeShared<ResAtlasRasterImage>(tex, Rectr(rect.p.x + borders.p.x, rect.p.y, rect.d.x - borders.p.x - borders.d.x, borders.p.y)), //t
			utki::makeShared<ResAtlasRasterImage>(tex, Rectr(rect.right() - borders.d.x, rect.p.y, borders.d.x, borders.p.y)), //rt
			utki::makeShared<ResAtlasRasterImage>(tex, Rectr(rect.p.x, rect.p.y + borders.p.y, borders.p.x, rect.d.y - borders.p.y - borders.d.y)), //l
			utki::makeShared<ResAtlasRasterImage>(tex, Rectr(rect.p.x + borders.p.x, rect.p.y + borders.p.y, rect.d.x - borders.p.x - borders.d.x, rect.d.y - borders.p.y - borders.d.y)), //m
			utki::makeShared<ResAtlasRasterImage>(tex, Rectr(rect.right() - borders.d.x, rect.p.y + borders.p.y, borders.d.x, rect.d.y - borders.p.y - borders.d.y)), //r
			utki::makeShared<ResAtlasRasterImage>(tex, Rectr(rect.p.x, rect.top() - borders.d.y, borders.p.x, borders.d.y)), //lb
			utki::makeShared<ResAtlasRasterImage>(tex, Rectr(rect.p.x + borders.p.x, rect.top() - borders.d.y, rect.d.x - borders.p.x - borders.d.x, borders.d.y)), //b
			utki::makeShared<ResAtlasRasterImage>(tex, Rectr(rect.right() - borders.d.x, rect.top() - borders.d.y, borders.d.x, borders.d.y)) //br
		);
}
