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



//override
void LinearLayout::ArrangeWidgets(Container& cont)const{
	unsigned longIndex;
	unsigned transIndex;
	if(this->isVertical){
		longIndex = 1;
		transIndex = 0;
	}else{
		longIndex = 0;
		transIndex = 1;
	}
	
	const morda::Vec2f& minDim = cont.GetMinDim();
	
	//if size of the container is less than minimal required size
	if(cont.Rect().d[longIndex] <= minDim[longIndex]){
		float factor = minDim[longIndex] / cont.Rect().d[longIndex];
		
		float pos = 0;
		for(const ting::Ref<Widget>* c = &cont.Children(); *c; c = &(*c)->Next()){
//			const stob::Node* layout = Layout::GetLayoutProp(**c);
			//TODO: layout props
			
			morda::Vec2f newPos;
			newPos[longIndex] = ting::math::Round(pos);
			newPos[transIndex] = 0;

			(*c)->Move(newPos);
			
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
	
	//weight - minSize pair
	typedef std::pair<float, float> T_Pair;
	ting::Array<T_Pair> weights(cont.NumChildren());
	
	{
		T_Pair *i = weights.Begin();
		for(const ting::Ref<Widget>* c = &cont.Children(); *c; c = &(*c)->Next(), ++i){
			const stob::Node* layout = Layout::GetLayoutProp(**c);
			ASSERT(weights.Overlaps(i))
			if(!layout){
				(*i).first = 0;
			}else if(const stob::Node* weight = layout->GetProperty("weight")){
				(*i).first = weight->AsFloat();
			}else{
				(*i).first = 0;
			}
			
			(*i).second = (*c)->GetMinDim()[longIndex];
			if((*i).first == 0){
				zeroWeightsLength += (*i).second;
			}else{
				netWeight += (*i).first;
			}
		}
	}
	
	float weightedLength = cont.Rect().d[longIndex] - zeroWeightsLength;
	
	//find children whose minSize is bigger than resulting weighted size and set their weight to 0
	if(netWeight != 0){
		float lengthPerUnit = weightedLength / netWeight;

		for(bool doAgain = true; doAgain;){
			doAgain = false;

			for(T_Pair *i = weights.Begin(); i != weights.End(); ++i){
				if((*i).first == 0){
					continue;
				}
				
				if((*i).first * lengthPerUnit <= (*i).second){
					weightedLength -= (*i).second;
					netWeight -= (*i).first;
					lengthPerUnit = weightedLength / netWeight;
					(*i).first = 0;
					doAgain = true;
				}
			}
		}
	}
	
	float pos = 0;
	T_Pair *i = weights.Begin();
	for(const ting::Ref<Widget>* c = &cont.Children(); *c; c = &(*c)->Next(), ++i){
//			const stob::Node* layout = Layout::GetLayoutProp(**c);
		//TODO: layout props

		morda::Vec2f newPos;
		newPos[longIndex] = ting::math::Round(pos);
		newPos[transIndex] = 0;

		(*c)->Move(newPos);

		morda::Vec2f newSize;
		if(i->first > 0){
			newSize[longIndex] = (i->first / netWeight) * weightedLength;
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
		minDim[longIndex] =+ md[longIndex];
	}
	return minDim;
}
