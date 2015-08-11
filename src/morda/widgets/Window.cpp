#include "Window.hpp"

#include "../App.hpp"

#include "../util/util.hpp"


using namespace morda;


namespace{

const char* DWindowDesc = R"qwertyuiop(
		//1st row
		TableRow{
			FrameContainer{
				ImageLabel{
					image{morda_img_window_lt}
				}
				MouseProxy{
					name{morda_lt_proxy}
					layout{
						dimX{max} dimY{max}
					}
				}
			}

			FrameContainer{
				layout{
					dimX{0}
				}
				ImageLabel{
					image{morda_img_window_t}
					layout{
						dimX{max}
					}
				}
				MouseProxy{
					name{morda_t_proxy}
					layout{
						dimX{max} dimY{max}
					}
				}
			}

			FrameContainer{
				ImageLabel{
					image{morda_img_window_rt}
				}
				MouseProxy{
					name{morda_rt_proxy}
					layout{
						dimX{max} dimY{max}
					}
				}
			}
		}//~TableRow

		//2nd row
		TableRow{
			layout{
				weight{1}
			}
			FrameContainer{
				layout{
					dimY{max}
				}
				ImageLabel{
					image{morda_img_window_l}
					layout{
						dimY{max}
					}
				}
				MouseProxy{
					name{morda_l_proxy}
					layout{
						dimX{max} dimY{max}
					}
				}
			}


			//middle
			VerticalContainer{
				layout{
					weight{1}
					dimY{max}
				}

				//caption
				FrameContainer{
					layout{
						dimX{max}
					}

					clip{true}

					MouseProxy{
						name{morda_caption_proxy}
						layout{
							dimX{max} dimY{max}
						}
					}

					ColorLabel{
						name{morda_window_title_bg}
						layout{
							dimX{max} dimY{max}
						}
					}

					HorizontalContainer{
						layout{
							dimX{max} dimY{max}
						}

						Label{
							name{morda_title}
							layout{
								dimX{0}
								weight{1}
							}
						}
					}
				}

				BlackHoleContainer{
					layout{
						dimX{max} dimY{0}
						weight{1}
					}
					FrameContainer{
						name{morda_content}
						clip{true}
						layout{
							dimX{max} dimY{max}
						}
					}
				}
			}

			FrameContainer{
				layout{
					dimY{max}
				}

				ImageLabel{
					image{morda_img_window_r}
					layout{
						dimY{max}
					}
				}
				MouseProxy{
					name{morda_r_proxy}
					layout{
						dimX{max} dimY{max}
					}
				}
			}
		}

		//3rd row
		TableRow{
			FrameContainer{
				ImageLabel{
					image{morda_img_window_lb}
				}
				MouseProxy{
					name{morda_lb_proxy}
					layout{
						dimX{max} dimY{max}
					}
				}
			}

			FrameContainer{
				layout{
					dimX{0}
				}
				ImageLabel{
					image{morda_img_window_b}
					layout{
						dimX{max}
					}
				}
				MouseProxy{
					name{morda_b_proxy}
					layout{
						dimX{max} dimY{max}
					}
				}
			}

			FrameContainer{
				ImageLabel{
					image{morda_img_window_rb}
				}
				MouseProxy{
					name{morda_rb_proxy}
					layout{
						dimX{max} dimY{max}
					}
				}
			}
		}//~TableRow
	)qwertyuiop";

}



morda::Window::Window(const stob::Node* chain) :
		Widget(chain),
		TableContainer(stob::Parse(DWindowDesc).get()),
		emptyMinDim(this->TableContainer::onMeasure(Vec2r(-1)))
{
	this->SetupWidgets();
	
	if(auto n = GetProperty(chain, "title")){
		this->SetTitle(n->Value());
	}
	
	this->titleBgColorTopmost = 0xffff0000;
	this->titleBgColorNonTopmost = 0xff808080;
	
	if(auto a = GetProperty(chain, "appearance")){
		if(auto n = GetProperty(a, "titleColorTopmost")){
			this->titleBgColorTopmost = n->AsUint32();
		}
		
		if(auto n = GetProperty(a, "titleColorNonTopmost")){
			this->titleBgColorNonTopmost = n->AsUint32();
		}
	}
	
	if(chain){
		this->contentArea->Add(*chain);
	}
}

void morda::Window::SetupWidgets(){
	this->contentArea = this->FindChildByNameAs<FrameContainer>("morda_content");
	ASSERT(this->contentArea)
	
	this->title = this->FindChildByNameAs<Label>("morda_title");
	ASSERT(this->title)
	
	this->titleBg = this->FindChildByNameAs<ColorLabel>("morda_window_title_bg");
	ASSERT(this->titleBg);
	
	std::function<decltype(MouseProxy::onMouseButton)(bool&)> getButtonFunc = [this](bool& flag){
		return decltype(MouseProxy::onMouseButton)([this, &flag](Widget& widget, bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId){
			if(button != Widget::EMouseButton::LEFT){
				return false;
			}

			if(isDown){
				flag = true;
				this->capturePoint = pos;
				return true;
			}
			flag = false;
			return false;
		});
	};
	
	{
		auto caption = this->FindChildByNameAs<MouseProxy>("morda_caption_proxy");
		ASSERT(caption)
	
		caption->onMouseButton = getButtonFunc(this->captionCaptured);

		caption->onMouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->captionCaptured){
				this->MoveBy(pos - this->capturePoint);
				return true;
			}
			return false;
		};
	}
	
	{
		auto w = this->FindChildByNameAs<MouseProxy>("morda_lt_proxy");
		ASSERT(w)
		w->onMouseButton = getButtonFunc(this->leftTopResizeCaptured);
		w->onMouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->leftTopResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				ting::util::ClampTop(d.x, this->rect().d.x - this->emptyMinDim.x);
				ting::util::ClampBottom(d.y, -(this->rect().d.y - this->emptyMinDim.y));
				this->MoveBy(morda::Vec2r(d.x, 0));
				this->ResizeBy(morda::Vec2r(-d.x, d.y));
			}
			return false;
		};
	}
	
	{	
		auto w = this->FindChildByNameAs<MouseProxy>("morda_lb_proxy");
		ASSERT(w)
		w->onMouseButton = getButtonFunc(this->leftBottomResizeCaptured);
		w->onMouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->leftBottomResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				ting::util::ClampTop(d.x, this->rect().d.x - this->emptyMinDim.x);
				ting::util::ClampTop(d.y, this->rect().d.y - this->emptyMinDim.y);
				this->MoveBy(d);
				this->ResizeBy(morda::Vec2r(-d.x, -d.y));
			}
			return false;
		};
	}

	{
		auto w = this->FindChildByNameAs<MouseProxy>("morda_rt_proxy");
		ASSERT(w)
		w->onMouseButton = getButtonFunc(this->rightTopResizeCaptured);
		w->onMouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->rightTopResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				ting::util::ClampBottom(d.x, -(this->rect().d.x - this->emptyMinDim.x));
				ting::util::ClampBottom(d.y, -(this->rect().d.y - this->emptyMinDim.y));
				this->ResizeBy(d);
			}
			return false;
		};
	}
	
	{
		auto w = this->FindChildByNameAs<MouseProxy>("morda_rb_proxy");
		ASSERT(w)
		w->onMouseButton = getButtonFunc(this->rightBottomResizeCaptured);
		w->onMouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->rightBottomResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				ting::util::ClampBottom(d.x, -(this->rect().d.x - this->emptyMinDim.x));
				ting::util::ClampTop(d.y, this->rect().d.y - this->emptyMinDim.y);
				this->MoveBy(morda::Vec2r(0, d.y));
				this->ResizeBy(morda::Vec2r(d.x, -d.y));
			}
			return false;
		};
	}
	
	{
		auto w = this->FindChildByNameAs<MouseProxy>("morda_l_proxy");
		ASSERT(w)
		w->onMouseButton = getButtonFunc(this->leftResizeCaptured);
		w->onMouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->leftResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				ting::util::ClampTop(d.x, this->rect().d.x - this->emptyMinDim.x);
				this->MoveBy(morda::Vec2r(d.x, 0));
				this->ResizeBy(morda::Vec2r(-d.x, 0));
			}
			return false;
		};
	}
	
	{
		auto w = this->FindChildByNameAs<MouseProxy>("morda_r_proxy");
		ASSERT(w)
		w->onMouseButton = getButtonFunc(this->rightResizeCaptured);
		w->onMouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->rightResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				ting::util::ClampBottom(d.x, -(this->rect().d.x - this->emptyMinDim.x));
				this->ResizeBy(morda::Vec2r(d.x, 0));
			}
			return false;
		};
	}
	
	{
		auto w = this->FindChildByNameAs<MouseProxy>("morda_t_proxy");
		ASSERT(w)
		w->onMouseButton = getButtonFunc(this->topResizeCaptured);
		w->onMouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->topResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				ting::util::ClampBottom(d.y, -(this->rect().d.y - this->emptyMinDim.y));
				this->ResizeBy(morda::Vec2r(0, d.y));
			}
			return false;
		};
	}
	
	{
		auto w = this->FindChildByNameAs<MouseProxy>("morda_b_proxy");
		ASSERT(w)
		w->onMouseButton = getButtonFunc(this->bottomResizeCaptured);
		w->onMouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->bottomResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				ting::util::ClampTop(d.y, this->rect().d.y - this->emptyMinDim.y);
				this->MoveBy(morda::Vec2r(0, d.y));
				this->ResizeBy(morda::Vec2r(0, -d.y));
			}
			return false;
		};
	}
}



void morda::Window::SetTitle(const std::string& str){
	this->title->SetText(ting::utf8::ToUTF32(str));
}



bool morda::Window::OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId){
	if(isDown){
		morda::App::Inst().PostToUIThread_ts(
				[this](){
					this->makeTopmost();
				}
			);

		if(!this->IsTopmost()){
			this->Focus();
		}
	}
	
	this->Container::OnMouseButton(isDown, pos, button, pointerId);
	
	return true;
}



bool morda::Window::OnMouseMove(const morda::Vec2r& pos, unsigned pointerId){
	this->Container::OnMouseMove(pos, pointerId);
	return true;
}

void morda::Window::OnTopmostChanged(){
	this->titleBg->setColor(this->IsTopmost() ? this->titleBgColorTopmost : this->titleBgColorNonTopmost);
}
