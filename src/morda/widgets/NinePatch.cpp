#include "NinePatch.hpp"
#include "../App.hpp"


using namespace morda;


namespace{
const char* D_NinePatchLayout = R"qwertyuiop(
		{
			//left column
			LinearContainer{
				vertical{true}
				layout{
					dim{min min}
					fill{false true}
				}

				ImageLabel{
					name{morda_lt}
					layout{
						dim{min min}
					}
				}

				ImageLabel{
					name{morda_l}
					layout{
						dim{min 0}
						weight{1}
					}
				}
				ImageLabel{
					name{morda_lb}
					layout{
						dim{min min}
					}
				}
			}

			//middle column
			LinearContainer{
				vertical{true}
				layout{
					dim{min min}
					fill{false true}
					weight{1}
				}

				ImageLabel{
					name{morda_t}
					layout{
						dim{0 min}
						fill{true false}
					}
				}
				FrameContainer{
					name{morda_content_area}
					layout{
						dim{min min}
						weight{1}
						fill{true true}
					}
					ImageLabel{
						name{morda_m}
						layout{
							dim{0 0}
							fill{true true}
						}
					}
				}
				ImageLabel{
					name{morda_b}
					layout{
						dim{0 min}
						fill{true false}
					}
				}
			}

			//right column
			LinearContainer{
				vertical{true}
				layout{
					dim{min min}
					fill{false true}
				}

				ImageLabel{
					name{morda_rt}
					layout{
						dim{min min}
					}
				}

				ImageLabel{
					name{morda_r}
					layout{
						dim{min 0}
						weight{1}
					}
				}
				ImageLabel{
					name{morda_rb}
					layout{
						dim{min min}
					}
				}
			}
		}
	)qwertyuiop";
}


NinePatch::NinePatch(const stob::Node* desc) :
		Widget(desc),
		LinearContainer(stob::Parse(D_NinePatchLayout).get())
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
	
	this->contentArea = this->FindChildByNameAs<FrameContainer>("morda_content_area");
	
	if(!desc){
		return;
	}
	
	if(const stob::Node* n = desc->GetProperty("image")){
		this->SetNinePatch(morda::App::Inst().resMan.Load<ResNinePatch>(n->Value()));
	}
	
	this->contentArea->Add(*desc);
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
