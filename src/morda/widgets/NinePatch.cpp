#include <utki/util.hpp>
#include <utki/types.hpp>

#include "NinePatch.hpp"
#include "../App.hpp"
#include "../util/util.hpp"
#include "core/proxy/ResizeProxy.hpp"


using namespace morda;


namespace{
const char* ninePatchLayout_c = R"qwertyuiop(
		//1st row
		TableRow{
			ImageLabel{
				name{morda_lt}
			}

			ImageLabel{
				layout{dx{0}dy{fill}}
				name{morda_t}
			}

			ImageLabel{
				layout{dx{0}dy{fill}}
				name{morda_rt}
			}
		}

		//2nd row
		TableRow{
			layout{
				weight{1}
			}
			
			ImageLabel{
				name{morda_l}
				layout{dx{0}dy{fill}}
			}

			Frame{
				name{morda_content}
				layout{
					weight{1}
					dy{max}
				}

				ImageLabel{
					name{morda_m}
					layout{dx{fill}dy{fill}}
				}
			}
			ImageLabel{
				name{morda_r}
				layout{dx{0}dy{fill}}
			}
		}

		//3rd row
		TableRow{
			ImageLabel{
				layout{dx{0}dy{fill}}
				name{morda_lb}
			}

			ImageLabel{
				layout{dx{0}dy{fill}}
				name{morda_b}
			}

			ImageLabel{
				name{morda_rb}
			}
		}
	)qwertyuiop";
}


NinePatch::NinePatch(const stob::Node* chain) :
		Widget(chain),
		BlendWidget(chain),
		Table(stob::parse(ninePatchLayout_c).get())
{
	this->imageLabelMatrix[0][0] = this->findChildByNameAs<ImageLabel>("morda_lt");
	this->imageLabelMatrix[0][1] = this->findChildByNameAs<ImageLabel>("morda_t");
	this->imageLabelMatrix[0][2] = this->findChildByNameAs<ImageLabel>("morda_rt");
	
	this->imageLabelMatrix[1][0] = this->findChildByNameAs<ImageLabel>("morda_l");
	this->imageLabelMatrix[1][1] = this->findChildByNameAs<ImageLabel>("morda_m");
	this->imageLabelMatrix[1][2] = this->findChildByNameAs<ImageLabel>("morda_r");
	
	this->imageLabelMatrix[2][0] = this->findChildByNameAs<ImageLabel>("morda_lb");
	this->imageLabelMatrix[2][1] = this->findChildByNameAs<ImageLabel>("morda_b");
	this->imageLabelMatrix[2][2] = this->findChildByNameAs<ImageLabel>("morda_rb");
	
	this->onBlendChanged();
	
	this->content_var = this->findChildByNameAs<Frame>("morda_content");
	
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
		this->setNinePatch(morda::App::inst().resMan.load<ResNinePatch>(n->value()));
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
		auto& layoutParams = utki::makePtrToConst(this->imageLabelMatrix[0][0].get())->getLayoutParams();
		
		if(this->borders.left() == LayoutParams::min_c){
			if(layoutParams.dim.x != minBorders.left()){
				auto& lp = this->imageLabelMatrix[0][0].get()->getLayoutParams();
				lp.dim.x = minBorders.left();
			}
		}else{
			if(layoutParams.dim.x != this->borders.left()){
				auto& lp = this->imageLabelMatrix[0][0].get()->getLayoutParams();
				lp.dim.x = this->borders.left();
			}
		}
		
		if(this->borders.top() == LayoutParams::min_c){
			if(layoutParams.dim.y != minBorders.top()){
				auto& lp = this->imageLabelMatrix[0][0].get()->getLayoutParams();
				lp.dim.y = minBorders.top();
			}
		}else{
			if(layoutParams.dim.y != this->borders.top()){
				auto& lp = this->imageLabelMatrix[0][0].get()->getLayoutParams();
				lp.dim.y = this->borders.top();
			}
		}
//		TRACE(<< "layoutParams.dim = " << layoutParams.dim << std::endl)
	}
	{
		//non-const call to getLayoutParams requests relayout which is not necessarily needed, so try to avoid it if possible
		auto& layoutParams = utki::makePtrToConst(this->imageLabelMatrix[2][2].get())->getLayoutParams();
		
		if(this->borders.right() == LayoutParams::min_c){
			if(layoutParams.dim.x != minBorders.right()){
				auto& lp = this->imageLabelMatrix[2][2]->getLayoutParams();
				lp.dim.x = minBorders.right();
			}
		}else{
			if(layoutParams.dim.x != this->borders.right()){
				auto& lp = this->imageLabelMatrix[2][2]->getLayoutParams();
				lp.dim.x = this->borders.right();
			}
		}
		
		if(this->borders.bottom() == LayoutParams::min_c){
			if(layoutParams.dim.y != minBorders.bottom()){
				auto& lp = this->imageLabelMatrix[2][2]->getLayoutParams();
				lp.dim.y = minBorders.bottom();
			}
		}else{
			if(layoutParams.dim.y != this->borders.bottom()){
				auto& lp = this->imageLabelMatrix[2][2]->getLayoutParams();
				lp.dim.y = this->borders.bottom();
			}
		}
//		TRACE(<< "lp.dim = " << lp.dim << std::endl)
	}
//	TRACE(<< "this->borders = " << this->borders << std::endl)
			
	this->scaledImage = this->image->get(this->borders);
	
	for(unsigned i = 0; i != 3; ++i){
		for(unsigned j = 0; j != 3; ++j){
			this->imageLabelMatrix[i][j]->setImage(this->scaledImage->images()[i][j]);
		}
	}
}

void NinePatch::setCenterVisible(bool visible){
	ASSERT(this->imageLabelMatrix[1][1])
	this->imageLabelMatrix[1][1]->setVisible(visible);
}

void NinePatch::onBlendChanged(){
	for(unsigned i = 0; i != 3; ++i){
		for(unsigned j = 0; j != 3; ++j){
			this->imageLabelMatrix[i][j]->setBlend(this->blend());
		}
	}
}
