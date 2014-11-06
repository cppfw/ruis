#include "ResNinePatch.hpp"
#include "../App.hpp"
#include "../util/util.hpp"


using namespace morda;



std::shared_ptr<ResNinePatch> ResNinePatch::Load(const stob::Node& chain, const ting::fs::File& fi){
	auto tex = morda::App::Inst().resMan.Load<ResTexture>(chain.side("tex").up().Value());
	
	auto rect = Rect2rFromSTOB(&chain.side("rect").up());
	
	auto borders = Rect2rFromSTOB(&chain.side("borders").up());
	
	return ting::New<ResNinePatch>(
			ting::New<ResImage>(tex, Rect2r(rect.p, borders.p)), //lt
			ting::New<ResImage>(tex, Rect2r(rect.p.x + borders.p.x, rect.p.y, rect.d.x - borders.p.x - borders.d.x, borders.p.y)), //t
			ting::New<ResImage>(tex, Rect2r(rect.Right() - borders.d.x, rect.p.y, borders.d.x, borders.p.y)), //rt
			ting::New<ResImage>(tex, Rect2r(rect.p.x, rect.p.y + borders.p.y, borders.p.x, rect.d.y - borders.p.y - borders.d.y)), //l
			ting::New<ResImage>(tex, Rect2r(rect.p.x + borders.p.x, rect.p.y + borders.p.y, rect.d.x - borders.p.x - borders.d.x, rect.d.y - borders.p.y - borders.d.y)), //m
			ting::New<ResImage>(tex, Rect2r(rect.Right() - borders.d.x, rect.p.y + borders.p.y, borders.d.x, rect.d.y - borders.p.y - borders.d.y)), //r
			ting::New<ResImage>(tex, Rect2r(rect.p.x, rect.Top() - borders.d.y, borders.p.x, borders.d.y)), //lb
			ting::New<ResImage>(tex, Rect2r(rect.p.x + borders.p.x, rect.Top() - borders.d.y, rect.d.x - borders.p.x - borders.d.x, borders.d.y)), //b
			ting::New<ResImage>(tex, Rect2r(rect.Right() - borders.d.x, rect.Top() - borders.d.y, borders.d.x, borders.d.y)) //br
		);
}
