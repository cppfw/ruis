#include <utki/util.hpp>

#include "NinePatch.hpp"
#include "../App.hpp"
#include "../util/util.hpp"
#include "core/proxy/ResizeProxy.hpp"


using namespace morda;


namespace{
const char* D_NinePatchLayout = R"qwertyuiop(
		//1st row
		TableRow{
			ImageLabel{
				name{morda_lt}
			}

			ImageLabel{
				layout{dimX{0}dimY{fill}}
				name{morda_t}
			}

			ImageLabel{
				layout{dimX{0}dimY{fill}}
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
				layout{dimX{0}dimY{fill}}
			}

			Frame{
				name{morda_content}
				layout{
					weight{1}
					dimY{max}
				}

				ImageLabel{
					name{morda_m}
					layout{dimX{fill}dimY{fill}}
				}
			}
			ImageLabel{
				name{morda_r}
				layout{dimX{0}dimY{fill}}
			}
		}

		//3rd row
		TableRow{
			ImageLabel{
				layout{dimX{0}dimY{fill}}
				name{morda_lb}
			}

			ImageLabel{
				layout{dimX{0}dimY{fill}}
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
		TableContainer(stob::parse(D_NinePatchLayout).get())
{
	this->lt = this->findChildByNameAs<ImageLabel>("morda_lt");
	this->t = this->findChildByNameAs<ImageLabel>("morda_t");
	this->rt = this->findChildByNameAs<ImageLabel>("morda_rt");
	
	this->l = this->findChildByNameAs<ImageLabel>("morda_l");
	this->m = this->findChildByNameAs<ImageLabel>("morda_m");
	this->r = this->findChildByNameAs<ImageLabel>("morda_r");
	
	this->lb = this->findChildByNameAs<ImageLabel>("morda_lb");
	this->b = this->findChildByNameAs<ImageLabel>("morda_b");
	this->rb = this->findChildByNameAs<ImageLabel>("morda_rb");
	
	this->content_var = this->findChildByNameAs<Frame>("morda_content");
	
	if(auto n = getProperty(chain, "borderLeft")){
		this->borders.left() = dimValueFromSTOB(*n);
	}else{
		this->borders.left() = LayoutParams::D_Min;
	}
	
	if(auto n = getProperty(chain, "borderRight")){
		this->borders.right() = dimValueFromSTOB(*n);
	}else{
		this->borders.right() = LayoutParams::D_Min;
	}
	
	if(auto n = getProperty(chain, "borderTop")){
		this->borders.top() = dimValueFromSTOB(*n);
	}else{
		this->borders.top() = LayoutParams::D_Min;
	}
	
	if(auto n = getProperty(chain, "borderBottom")){
		this->borders.bottom() = dimValueFromSTOB(*n);
	}else{
		this->borders.bottom() = LayoutParams::D_Min;
	}
	
	//this should go after setting borders
	if(const stob::Node* n = getProperty(chain, "image")){
		this->setNinePatch(morda::App::inst().resMan.load<ResNinePatch>(n->value()));
	}
	
	if(chain){
		this->content_var->add(*chain);
	}
}

void NinePatch::render(const morda::Matr4r& matrix) const {
	this->TableContainer::render(matrix);
}


void NinePatch::setNinePatch(const std::shared_ptr<ResNinePatch>& np){
	this->image = np;
	this->scaledImage.reset();
	
	this->applyImages();
	
	this->clearCache();
}

void NinePatch::applyImages(){
	auto& minBorders = this->image->getBorders();
//	TRACE(<< "minBorders = " << minBorders << std::endl)
	
	{
		//TODO: getLayoutParams requests relayout which is not necessarily needed
		auto& lp = this->lt->parent()->getLayoutParams(*this->lt);
		
		lp.dim.x = this->borders.left();
		if(lp.dim.x == LayoutParams::D_Min){
			lp.dim.x = minBorders.left();
		}
		
		lp.dim.y = this->borders.top();
		if(lp.dim.y == LayoutParams::D_Min){
			lp.dim.y = minBorders.top();
		}
//		TRACE(<< "lp.dim = " << lp.dim << std::endl)
	}
	{
		auto& lp = this->rb->parent()->getLayoutParams(*this->rb);
		
		lp.dim.x = this->borders.right();
		if(lp.dim.x == LayoutParams::D_Min){
			lp.dim.x = minBorders.right();
		}
		
		lp.dim.y = this->borders.bottom();
		if(lp.dim.y == LayoutParams::D_Min){
			lp.dim.y = minBorders.bottom();
		}
//		TRACE(<< "lp.dim = " << lp.dim << std::endl)
	}
//	TRACE(<< "this->borders = " << this->borders << std::endl)
			
	this->scaledImage = this->image->get(this->borders);
	
	this->lt->setImage(this->scaledImage->images()[0][0]);
	this->t->setImage(this->scaledImage->images()[0][1]);
	this->rt->setImage(this->scaledImage->images()[0][2]);
	
	this->l->setImage(this->scaledImage->images()[1][0]);
	this->m->setImage(this->scaledImage->images()[1][1]);
	this->r->setImage(this->scaledImage->images()[1][2]);
	
	this->lb->setImage(this->scaledImage->images()[2][0]);
	this->b->setImage(this->scaledImage->images()[2][1]);
	this->rb->setImage(this->scaledImage->images()[2][2]);
}
