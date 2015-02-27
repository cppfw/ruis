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
		TableContainer(stob::Parse(D_NinePatchLayout).get())
{
	this->lt = this->FindChildByNameAs<ImageLabel>("morda_lt");
	this->t = this->FindChildByNameAs<ImageLabel>("morda_t");
	this->rt = this->FindChildByNameAs<ImageLabel>("morda_rt");
	
	this->l = this->FindChildByNameAs<ImageLabel>("morda_l");
	this->m = this->FindChildByNameAs<ImageLabel>("morda_m");
	this->r = this->FindChildByNameAs<ImageLabel>("morda_r");
	
	this->lb = this->FindChildByNameAs<ImageLabel>("morda_lb");
	this->b = this->FindChildByNameAs<ImageLabel>("morda_b");
	this->rb = this->FindChildByNameAs<ImageLabel>("morda_rb");
	
	this->content = this->FindChildByNameAs<FrameContainer>("morda_content");
	
	if(const stob::Node* n = GetProperty(chain, "image")){
		this->SetNinePatch(morda::App::Inst().resMan.Load<ResNinePatch>(n->Value()));
	}
	
	if(chain){
		this->content->Add(*chain);
	}
}


void NinePatch::SetNinePatch(const std::shared_ptr<ResNinePatch>& np){
	this->image = np;
	
	this->lt->SetImage(np->lt);
	this->t->SetImage(np->t);
	this->rt->SetImage(np->rt);
	
	this->l->SetImage(np->l);
	this->m->SetImage(np->m);
	this->r->SetImage(np->r);
	
	this->lb->SetImage(np->lb);
	this->b->SetImage(np->b);
	this->rb->SetImage(np->rb);
}
