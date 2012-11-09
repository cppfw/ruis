#include <ting/Array.hpp>
#include <ting/util.hpp>

#include "LinearLayout.hpp"

#include "../util/Gravity.hpp"



using namespace morda;



LinearLayout::LinearLayout(const stob::Node& description){
	if(const stob::Node* n = description.GetProperty("orientation")){
		if(*n == "horizontal"){
			this->isVertical = false;
		}else{
			this->isVertical = true;
		}
	}else{
		this->isVertical = true;
	}
}



namespace{

class Info{
public:
	float weight;
	Vec2f dim;
	float margin;
	Gravity gravity;
};

}//~namespace


//TODO: take container padding into account


//override
void LinearLayout::ArrangeWidgets(Container& cont)const{
	unsigned longIndex = this->isVertical ? 1 : 0;
	unsigned transIndex = this->isVertical ? 0 : 1;
	
	ting::Array<Info> info(cont.NumChildren());
	
	//Calculate rigid size, net weight and store weights and margins
	float rigid = 0;
	float netWeight = 0;
	
	{
		Info* i = info.Begin();
		for(const ting::Ref<Widget>* c = &cont.Children(); *c; c = &(*c)->Next(), ++i){
			ASSERT(info.Overlaps(i))

			const stob::Node* layout = Layout::GetLayoutProp((*c)->prop.operator->());

			if(!layout){
				i->dim = (*c)->GetMinDim();
				i->weight = 0;
				i->gravity = Gravity::Default();
			}else{
				if(const stob::Node* weight = layout->GetProperty("weight")){
					i->weight = weight->AsFloat();
					netWeight += i->weight;
				}else{
					i->weight = 0;
				}

				if(const stob::Node* dim = layout->Child("dim").second){
					i->dim = Layout::Dim::FromSTOB(*dim).ForWidget(*(*c));
				}else{
					i->dim = (*c)->GetMinDim();
				}
				
				i->gravity = Gravity::FromLayout(*layout);
			}
			
			if((*c)->Prev().IsValid()){//if not first child
				i->margin = std::max(
						(*c)->Prev()->Margins()[longIndex + 2],
						(*c)->Margins()[longIndex]
					);
			}else{
				i->margin = 0;
			}
			
			rigid += i->dim[longIndex] + i->margin;
		}
	}
	
	{
		float flexible = cont.Rect().d[longIndex] - rigid;
		ting::util::ClampBottom(flexible, 0.0f);
		ASSERT(flexible >= 0)
		
		float pos = 0;
		Info *i = info.Begin();
		for(const ting::Ref<Widget>* c = &cont.Children(); *c; c = &(*c)->Next(), ++i){
			Vec2f newSize(i->dim);
			
			if(netWeight > 0){
				newSize[longIndex] += (i->weight / netWeight) * flexible;
			}

			Vec2f newPos;
			newPos[longIndex] = ting::math::Round(pos + i->margin);
			
			//apply gravity
			switch(i->gravity[transIndex]){
				case Gravity::LEFT:
	//			case Gravity::BOTTOM:
					newPos[transIndex] = cont.Padding()[transIndex == 0 ? 0 : 3];
					break;
				case Gravity::RIGHT:
	//			case Gravity::TOP:
					newPos[transIndex] = cont.Rect().d[transIndex] - newSize[transIndex] - cont.Padding()[transIndex == 0 ? 2 : 1];
					break;
				default:
				case Gravity::CENTER:
					newPos[transIndex] = (
							cont.Rect().d[transIndex]
									- newSize[transIndex]
									- (transIndex == 0 ?
											(cont.Padding()[0] + cont.Padding()[2]) :
											(cont.Padding()[1] + cont.Padding()[3])
										)
						) / 2;
					break;
			}

			newPos[transIndex] = ting::math::Round(newPos[transIndex]);
			
			(*c)->MoveTo(newPos);

			pos += i->margin + newSize[longIndex];
			
			newSize[longIndex] = ting::math::Round(newSize[longIndex]);
			newSize[transIndex] = ting::math::Round(newSize[transIndex]);

			(*c)->Resize(newSize);
		}
	}
}



//override
morda::Vec2f LinearLayout::ComputeMinDim(const Container& cont)const throw(){
	unsigned longIndex = this->isVertical ? 1 : 0;
	unsigned transIndex = this->isVertical ? 0 : 1;
	
	morda::Vec2f minDim(0);
	
	for(const ting::Ref<const Widget>* c = &cont.Children(); *c; c = &(*c)->Next()){
		const morda::Vec2f& md = (*c)->GetMinDim();
		
		if(minDim[transIndex] < md[transIndex]){
			minDim[transIndex] = md[transIndex];
		}
		minDim[longIndex] += md[longIndex];
		
		//margin works for non-first children only
		if((*c)->Prev().IsValid()){//if not first child
			minDim[longIndex] += std::max(
					(*c)->Prev()->Margins()[longIndex + 2],
					(*c)->Margins()[longIndex]
				);
		}
	}
	return minDim;
}
