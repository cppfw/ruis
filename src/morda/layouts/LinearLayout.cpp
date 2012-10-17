#include "LinearLayout.hpp"

#include <ting/Array.hpp>



using namespace morda;



LinearLayout::LinearLayout(const stob::Node& description){
	if(const stob::Node* n = description.GetProperty("orientation")){
		if(n->Value() == "horizontal"){
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
	float minSize;
};

}//~namespace



//override
void LinearLayout::ArrangeWidgets(Container& cont)const{
	unsigned longIndex = this->isVertical ? 1 : 0;
	unsigned transIndex = this->isVertical ? 0 : 1;
	
	const morda::Vec2f& minDim = cont.GetMinDim();
	
	//if size of the container is less than minimal required size
	if(cont.Rect().d[longIndex] <= minDim[longIndex]){
		float factor = cont.Rect().d[longIndex] / minDim[longIndex];
		
		float pos = 0;
		for(const ting::Ref<Widget>* c = &cont.Children(); *c; c = &(*c)->Next()){
//			const stob::Node* layout = Layout::GetLayoutProp(**c);
			//TODO: layout props
			
			morda::Vec2f newPos;
			newPos[longIndex] = ting::math::Round(pos);
			newPos[transIndex] = 0;

			(*c)->MoveTo(newPos);
			
			morda::Vec2f newSize = (*c)->GetMinDim();
			newSize[longIndex] *= factor;
			
			pos += newSize[longIndex];
			
			newSize[longIndex] = ting::math::Round(newSize[longIndex]);
			
			(*c)->Resize(newSize);
		}
		
		return;
	}
	
	//calculate net weight
	float netWeight = 0;
	float zeroWeightsLength = 0;
	
	ting::Array<Info> weights(cont.NumChildren());
	
	{
		Info *i = weights.Begin();
		for(const ting::Ref<Widget>* c = &cont.Children(); *c; c = &(*c)->Next(), ++i){
			const stob::Node* layout = Layout::GetLayoutProp((*c)->prop.operator->());
			ASSERT(weights.Overlaps(i))
			if(!layout){
				(*i).weight = 0;
			}else if(const stob::Node* weight = layout->GetProperty("weight")){
				(*i).weight = weight->AsFloat();
			}else{
				(*i).weight = 0;
			}
			
			(*i).minSize = (*c)->GetMinDim()[longIndex];
			if((*i).weight == 0){
				zeroWeightsLength += (*i).minSize;
			}else{
				netWeight += (*i).weight;
			}
		}
	}
	
	float weightedLength = cont.Rect().d[longIndex] - zeroWeightsLength;
	
	//find children whose minSize is bigger than resulting weighted size and set their weight to 0
	if(netWeight != 0){
		float lengthPerUnit = weightedLength / netWeight;

		for(bool doAgain = true; doAgain;){
			doAgain = false;

			for(Info *i = weights.Begin(); i != weights.End(); ++i){
				if((*i).weight == 0){
					continue;
				}
				
				if((*i).weight * lengthPerUnit <= (*i).minSize){
					weightedLength -= (*i).minSize;
					netWeight -= (*i).weight;
					lengthPerUnit = weightedLength / netWeight;
					(*i).weight = 0;
					doAgain = true;
				}
			}
		}
	}
	
	float pos = 0;
	Info *i = weights.Begin();
	for(const ting::Ref<Widget>* c = &cont.Children(); *c; c = &(*c)->Next(), ++i){
//			const stob::Node* layout = Layout::GetLayoutProp(**c);
		//TODO: layout props

		morda::Vec2f newPos;
		newPos[longIndex] = ting::math::Round(pos);
		newPos[transIndex] = 0;

		(*c)->MoveTo(newPos);

		morda::Vec2f newSize;
		if(i->weight > 0){
			newSize[longIndex] = (i->weight / netWeight) * weightedLength;
		}else{
			newSize[longIndex] = (*c)->GetMinDim()[longIndex];
		}
		
		newSize[transIndex] = (*c)->GetMinDim()[transIndex];

		pos += newSize[longIndex];

		newSize[longIndex] = ting::math::Round(newSize[longIndex]);

		(*c)->Resize(newSize);
	}
}



//override
morda::Vec2f LinearLayout::ComputeMinDim(const Container& cont)const throw(){
	morda::Vec2f minDim(0);
	
	unsigned longIndex, transIndex;
	if(this->isVertical){
		longIndex = 1;
		transIndex = 0;
	}else{
		longIndex = 0;
		transIndex = 1;
	}
	
	for(const ting::Ref<const Widget>* c = &cont.Children(); *c; c = &(*c)->Next()){
		const morda::Vec2f& md = (*c)->GetMinDim();
		if(minDim[transIndex] < md[transIndex]){
			minDim[transIndex] = md[transIndex];
		}
		minDim[longIndex] += md[longIndex];
	}
	return minDim;
}
