#include "DimContainer.hpp"
#include "../../util/util.hpp"

using namespace morda;


DimContainer::DimContainer(const stob::Node* chain) :
		Widget(chain),
		Container(chain)
{
}



void DimContainer::OnResize() {
	for(auto i = this->Children().begin(); i != this->Children().end(); ++i){
		auto& lp = this->GetLayoutParamsAs<LayoutParams>(**i);
		
		Vec2r d = lp.DimForWidget(**i);
		for(unsigned i = 0; i != 2; ++i){
			if(!lp.fill[i]){
				continue;
			}
			d[i] = this->Rect().d[i];
		}
		
		(*i)->Resize(d);
	}
}

morda::Vec2r DimContainer::onMeasure(const morda::Vec2r& quotum) const {
	morda::Vec2r minDim(0);

	for(auto i = this->Children().begin(); i != this->Children().end(); ++i){
		auto& lp = this->GetLayoutParamsAs<LayoutParams>(**i);

		morda::Vec2r d = lp.DimForWidget(**i) + (*i)->Rect().p;
		
		ting::util::ClampBottom(minDim.x, d.x);
		ting::util::ClampBottom(minDim.y, d.y);
	}
	
	return minDim;
}



std::unique_ptr<Widget::LayoutParams> DimContainer::CreateLayoutParams(const stob::Node* chain) const {
	return LayoutParams::New(chain);
}







Vec2r DimContainer::LayoutParams::DimForWidget(const Widget& w)const NOEXCEPT{
	Vec2r ret;

	for(unsigned i = 0; i != 2; ++i){
		const real& v = this->dim[i];

		if(v < 0){
			ret[i] = w.measure(Vec2r(-1))[i];
		}else{
			ret[i] = v;
		}
	}

	return ret;
}

