#include "NinePatch.hpp"
#include "../App.hpp"
#include "../util/util.hpp"


using namespace morda;


namespace{
const char* D_NinePatchLayout = R"qwertyuiop(
		//1st row
		TableRow{
			ImageLabel{
				name{morda_lt}
			}

			ImageLabel{
				name{morda_t}
			}

			ImageLabel{
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
			ImageLabel{
				name{morda_lb}
			}

			ImageLabel{
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
	
	this->content_var = this->findChildByNameAs<FrameContainer>("morda_content");
	
	if(const stob::Node* n = getProperty(chain, "image")){
		this->setNinePatch(morda::App::inst().resMan.load<ResNinePatch>(n->value()));
	}
	
	if(chain){
		this->content_var->add(*chain);
	}
}


void NinePatch::setNinePatch(const std::shared_ptr<ResNinePatch>& np){
	this->image = np;
	
	this->lt->setImage(np->lt);
	this->t->setImage(np->t);
	this->rt->setImage(np->rt);
	
	this->l->setImage(np->l);
	this->m->setImage(np->m);
	this->r->setImage(np->r);
	
	this->lb->setImage(np->lb);
	this->b->setImage(np->b);
	this->rb->setImage(np->rb);
	
	this->clearCache();
}
