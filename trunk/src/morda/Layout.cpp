#include "Layout.hpp"



using namespace morda;



namespace{

const char* D_Min = "min";
const char* D_Max = "max";
const char* D_Dim = "dim";

inline bool NodeHoldsFractionValue(const stob::Node& node)throw(){
	size_t len = node.ValueLength();
	return len != 0 && node.Value()[len - 1] == '%';
}

}//~namespace


//static
Layout::Dim Layout::Dim::FromSTOB(const stob::Node& node)throw(){
	Dim ret;
	
	const stob::Node* n = node.Child();
	
	for(unsigned i = 0; i != 2; ++i){
		if(!n){
			ret[i].unit = MIN;
			continue;
		}
		
		if(*n == D_Min){
			ret[i].unit = MIN;
		}else if(*n == D_Max){
			ret[i].unit = MAX;
		}else if(NodeHoldsFractionValue(*n)){
			ret[i].unit = FRACTION;
			ret[i].value = n->AsFloat() / 100;
		}else{
			ret[i].unit = PIXEL;
			ret[i].value = n->AsFloat();
		}
		
		n = n->Next();
	}
	
	return ret;
}



//static
Layout::Dim Layout::Dim::FromLayout(const stob::Node& layout)throw(){
	const stob::Node* dim = layout.Child(D_Dim).second;
	if(!dim){
		return Dim::Default();
	}
	return Dim::FromSTOB(*dim);
}



//static
Layout::Dim Layout::Dim::FromPropLayout(const stob::Node& prop)throw(){
	const stob::Node* layout = prop.Child(D_Layout()).second;
	if(!layout){
		return Dim::Default();
	}
	return Dim::FromLayout(*layout);
}



Vec2f Layout::Dim::ForWidget(const Widget& w)const throw(){
	Vec2f ret;
	
	for(unsigned i = 0; i != 2; ++i){
		const Value& v = this->operator[](i);
		
		switch(v.unit){
			case PIXEL:
				ret[i] = v.value;
				break;
			case FRACTION:
				if(ting::Ref<const Container> p = w.Parent()){
					ret[i] = v.value * p->Rect().d[i];
				}else{
					ret[i] = 0;
				}
				break;
			case MAX:
				if(ting::Ref<const Container> p = w.Parent()){
					ret[i] = p->Rect().d[i];
				}else{
					ret[i] = 0;
				}
				break;
			default:
			case MIN:
				ret[i] = w.GetMinDim()[i];
				break;
		}
	}
	
	return ret;
}
