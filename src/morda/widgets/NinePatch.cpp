#include <utki/util.hpp>
#include <utki/types.hpp>

#include "../Morda.hpp"

#include "../util/util.hpp"

#include "core/proxy/ResizeProxy.hpp"

#include "NinePatch.hpp"


using namespace morda;


namespace{
const char* ninePatchLayout_c = R"qwertyuiop(
		//1st row
		TableRow{
			Image{
				name{morda_lt}
			}

			Image{
				layout{dx{0}dy{fill}}
				name{morda_t}
			}

			Image{
				layout{dx{0}dy{fill}}
				name{morda_rt}
			}
		}

		//2nd row
		TableRow{
			layout{
				weight{1}
			}
			
			Image{
				name{morda_l}
				layout{dx{0}dy{fill}}
			}

			Pile{
				name{morda_content}
				layout{
					weight{1}
					dy{max}
				}

				Image{
					name{morda_m}
					layout{dx{fill}dy{fill}}
				}
			}
			Image{
				name{morda_r}
				layout{dx{0}dy{fill}}
			}
		}

		//3rd row
		TableRow{
			Image{
				layout{dx{0}dy{fill}}
				name{morda_lb}
			}

			Image{
				layout{dx{0}dy{fill}}
				name{morda_b}
			}

			Image{
				name{morda_rb}
			}
		}
	)qwertyuiop";
}


NinePatch::NinePatch(const stob::Node* chain) :
		Widget(chain),
		BlendingWidget(chain),
		Table(stob::parse(ninePatchLayout_c).get())
{
	this->imageMatrix[0][0] = this->findChildByNameAs<Image>("morda_lt");
	this->imageMatrix[0][1] = this->findChildByNameAs<Image>("morda_t");
	this->imageMatrix[0][2] = this->findChildByNameAs<Image>("morda_rt");
	
	this->imageMatrix[1][0] = this->findChildByNameAs<Image>("morda_l");
	this->imageMatrix[1][1] = this->findChildByNameAs<Image>("morda_m");
	this->imageMatrix[1][2] = this->findChildByNameAs<Image>("morda_r");
	
	this->imageMatrix[2][0] = this->findChildByNameAs<Image>("morda_lb");
	this->imageMatrix[2][1] = this->findChildByNameAs<Image>("morda_b");
	this->imageMatrix[2][2] = this->findChildByNameAs<Image>("morda_rb");
	
	this->onBlendingChanged();
	
	this->content_var = this->findChildByNameAs<Pile>("morda_content");
	
	if(auto n = getProperty(chain, "left")){
		this->borders.left() = dimValueFromSTOB(*n);//'min' is by default, but not allowed to specify explicitly, as well as 'max' and 'fill'
	}else{
		this->borders.left() = LayoutParams::min_c;
	}
	
	if(auto n = getProperty(chain, "right")){
		this->borders.right() = dimValueFromSTOB(*n);
	}else{
		this->borders.right() = LayoutParams::min_c;
	}
	
	if(auto n = getProperty(chain, "top")){
		this->borders.top() = dimValueFromSTOB(*n);
	}else{
		this->borders.top() = LayoutParams::min_c;
	}
	
	if(auto n = getProperty(chain, "bottom")){
		this->borders.bottom() = dimValueFromSTOB(*n);
	}else{
		this->borders.bottom() = LayoutParams::min_c;
	}
	
	if(auto n = getProperty(chain, "centerVisible")){
		this->setCenterVisible(n->asBool());
	}
	
	//this should go after setting up border widgets
	if(const stob::Node* n = getProperty(chain, "image")){
		this->setNinePatch(morda::Morda::inst().resMan.load<ResNinePatch>(n->value()));
	}
	
	if(chain){
		this->content_var->add(*chain);
	}
}

void NinePatch::render(const morda::Matr4r& matrix) const {
	this->Table::render(matrix);
}


void NinePatch::setNinePatch(const std::shared_ptr<ResNinePatch>& np){
	this->image = np;
	this->scaledImage.reset();
	
	this->applyImages();
	
	this->clearCache();
}



void NinePatch::applyImages(){
	auto& minBorders = this->image->borders();
//	TRACE(<< "minBorders = " << minBorders << std::endl)
	
	{
		//non-const call to getLayoutParams requests relayout which is not necessarily needed, so try to avoid it if possible
		auto& layoutParams = utki::makePtrToConst(this->imageMatrix[0][0].get())->getLayoutParams();
		
		if(this->borders.left() == LayoutParams::min_c){
			if(layoutParams.dim.x != minBorders.left()){
				auto& lp = this->imageMatrix[0][0].get()->getLayoutParams();
				lp.dim.x = minBorders.left();
			}
		}else{
			if(layoutParams.dim.x != this->borders.left()){
				auto& lp = this->imageMatrix[0][0].get()->getLayoutParams();
				lp.dim.x = this->borders.left();
			}
		}
		
		if(this->borders.top() == LayoutParams::min_c){
			if(layoutParams.dim.y != minBorders.top()){
				auto& lp = this->imageMatrix[0][0].get()->getLayoutParams();
				lp.dim.y = minBorders.top();
			}
		}else{
			if(layoutParams.dim.y != this->borders.top()){
				auto& lp = this->imageMatrix[0][0].get()->getLayoutParams();
				lp.dim.y = this->borders.top();
			}
		}
//		TRACE(<< "layoutParams.dim = " << layoutParams.dim << std::endl)
	}
	{
		//non-const call to getLayoutParams requests relayout which is not necessarily needed, so try to avoid it if possible
		auto& layoutParams = utki::makePtrToConst(this->imageMatrix[2][2].get())->getLayoutParams();
		
		if(this->borders.right() == LayoutParams::min_c){
			if(layoutParams.dim.x != minBorders.right()){
				auto& lp = this->imageMatrix[2][2]->getLayoutParams();
				lp.dim.x = minBorders.right();
			}
		}else{
			if(layoutParams.dim.x != this->borders.right()){
				auto& lp = this->imageMatrix[2][2]->getLayoutParams();
				lp.dim.x = this->borders.right();
			}
		}
		
		if(this->borders.bottom() == LayoutParams::min_c){
			if(layoutParams.dim.y != minBorders.bottom()){
				auto& lp = this->imageMatrix[2][2]->getLayoutParams();
				lp.dim.y = minBorders.bottom();
			}
		}else{
			if(layoutParams.dim.y != this->borders.bottom()){
				auto& lp = this->imageMatrix[2][2]->getLayoutParams();
				lp.dim.y = this->borders.bottom();
			}
		}
//		TRACE(<< "lp.dim = " << lp.dim << std::endl)
	}
//	TRACE(<< "this->borders = " << this->borders << std::endl)
			
	this->scaledImage = this->image->get(this->borders);
	
	for(unsigned i = 0; i != 3; ++i){
		for(unsigned j = 0; j != 3; ++j){
			this->imageMatrix[i][j]->setImage(this->scaledImage->images()[i][j]);
		}
	}
}

void NinePatch::setCenterVisible(bool visible){
	ASSERT(this->imageMatrix[1][1])
	this->imageMatrix[1][1]->setVisible(visible);
}

void NinePatch::onBlendingChanged(){
	for(unsigned i = 0; i != 3; ++i){
		for(unsigned j = 0; j != 3; ++j){
			this->imageMatrix[i][j]->setBlendingParams(this->blendingParams());
		}
	}
}
