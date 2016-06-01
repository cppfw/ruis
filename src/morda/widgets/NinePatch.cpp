#include <utki/util.hpp>

#include "NinePatch.hpp"
#include "../App.hpp"
#include "../util/util.hpp"
#include "ResizeProxy.hpp"


using namespace morda;


namespace{
const char* D_NinePatchLayout = R"qwertyuiop(
		//1st row
		TableRow{
			FrameContainer{
				ImageLabel{
					name{morda_lt}
				}
				ResizeProxy{
					layout{dimX{max}dimY{max}}
					name{morda_resizeproxy_lt}
				}
			}

			ImageLabel{
				name{morda_t}
			}

			FrameContainer{
				ImageLabel{
					name{morda_rt}
				}
				ResizeProxy{
					layout{dimX{max}dimY{max}}
					name{morda_resizeproxy_rt}
				}
			}
		}

		//2nd row
		TableRow{
			layout{
				weight{1}
			}
			ImageLabel{
				name{morda_l}
				layout{
					dimY{max}
				}
			}

			FrameContainer{
				name{morda_content}
				layout{
					weight{1}
					dimY{max}
				}
				ImageLabel{
					name{morda_m}
					layout{
						dimX{max} dimY{max}
					}
				}
			}

			ImageLabel{
				name{morda_r}
				layout{
					dimY{max}
				}
			}
		}

		//3rd row
		TableRow{
			FrameContainer{
				ImageLabel{
					name{morda_lb}
				}
				ResizeProxy{
					layout{dimX{max}dimY{max}}
					name{morda_resizeproxy_lb}
				}
			}

			ImageLabel{
				name{morda_b}
			}

			FrameContainer{
				ImageLabel{
					name{morda_rb}
				}
				ResizeProxy{
					layout{dimX{max}dimY{max}}
					name{morda_resizeproxy_rb}
				}
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
	
	this->content_var = this->findChildByNameAs<FrameContainer>("morda_content");
	
	if(const stob::Node* n = getProperty(chain, "image")){
		this->setNinePatch(morda::App::inst().resMan.load<ResNinePatch>(n->value()));
	}
	
	if(chain){
		this->content_var->add(*chain);
	}
	
	{
		auto onResized = [this](const Vec2r& newSize){
			this->updateImagesNeeded = true;
		};
		
		this->findChildByNameAs<ResizeProxy>("morda_resizeproxy_lt")->resized = onResized;
		this->findChildByNameAs<ResizeProxy>("morda_resizeproxy_rt")->resized = onResized;
		this->findChildByNameAs<ResizeProxy>("morda_resizeproxy_lb")->resized = onResized;
		this->findChildByNameAs<ResizeProxy>("morda_resizeproxy_rb")->resized = onResized;
	}
}

void NinePatch::render(const morda::Matr4r& matrix) const {
	if(this->updateImagesNeeded){
		this->updateImages();
	}
	this->TableContainer::render(matrix);
}


void NinePatch::setNinePatch(const std::shared_ptr<ResNinePatch>& np){
	this->image = np;
	
	this->updateImages();
	
	this->clearCache();
}

void NinePatch::updateImages()const{
	this->updateImagesNeeded = false;
	
	Sidesr borders;
	borders.left() = this->l->parent()->getLayoutParams(*this->l).dim.x;
	borders.right() = this->r->parent()->getLayoutParams(*this->r).dim.x;
	borders.top() = this->t->parent()->getLayoutParams(*this->t).dim.y;
	borders.bottom() = this->b->parent()->getLayoutParams(*this->b).dim.y;
	
	auto im = this->image->get(borders);
	
	this->lt->setImage(im[0][0]);
	this->t->setImage(im[0][1]);
	this->rt->setImage(im[0][2]);
	
	this->l->setImage(im[1][0]);
	this->m->setImage(im[1][1]);
	this->r->setImage(im[1][2]);
	
	this->lb->setImage(im[2][0]);
	this->b->setImage(im[2][1]);
	this->rb->setImage(im[2][2]);
}
