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
		TableContainer(stob::parse(DWindowDesc).get()),
		emptyMinDim(this->TableContainer::measure(Vec2r(-1)))
{
	this->setupWidgets();
	
	if(auto n = getProperty(chain, "title")){
		this->setTitle(n->value());
	}
	
	this->titleBgColorTopmost = 0xffff0000;
	this->titleBgColorNonTopmost = 0xff808080;
	
	if(auto a = getProperty(chain, "appearance")){
		if(auto n = getProperty(a, "titleColorTopmost")){
			this->titleBgColorTopmost = n->asUint32();
		}
		
		if(auto n = getProperty(a, "titleColorNonTopmost")){
			this->titleBgColorNonTopmost = n->asUint32();
		}
	}
	
	if(chain){
		this->contentArea->add(*chain);
	}
}

void morda::Window::setupWidgets(){
	this->contentArea = this->findChildByNameAs<FrameContainer>("morda_content");
	ASSERT(this->contentArea)
	
	this->title = this->findChildByNameAs<Label>("morda_title");
	ASSERT(this->title)
	
	this->titleBg = this->findChildByNameAs<ColorLabel>("morda_window_title_bg");
	ASSERT(this->titleBg);
	
	std::function<decltype(MouseProxy::mouseButton)(bool&)> getButtonFunc = [this](bool& flag){
		return decltype(MouseProxy::mouseButton)([this, &flag](Widget& widget, bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId){
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
		auto caption = this->findChildByNameAs<MouseProxy>("morda_caption_proxy");
		ASSERT(caption)
	
		caption->mouseButton = getButtonFunc(this->captionCaptured);

		caption->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->captionCaptured){
				this->moveBy(pos - this->capturePoint);
				return true;
			}
			return false;
		};
	}
	
	{
		auto w = this->findChildByNameAs<MouseProxy>("morda_lt_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->leftTopResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->leftTopResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampTop(d.x, this->rect().d.x - this->emptyMinDim.x);
				utki::clampBottom(d.y, -(this->rect().d.y - this->emptyMinDim.y));
				this->moveBy(morda::Vec2r(d.x, 0));
				this->resizeBy(morda::Vec2r(-d.x, d.y));
			}
			return false;
		};
	}
	
	{	
		auto w = this->findChildByNameAs<MouseProxy>("morda_lb_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->leftBottomResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->leftBottomResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampTop(d.x, this->rect().d.x - this->emptyMinDim.x);
				utki::clampTop(d.y, this->rect().d.y - this->emptyMinDim.y);
				this->moveBy(d);
				this->resizeBy(morda::Vec2r(-d.x, -d.y));
			}
			return false;
		};
	}

	{
		auto w = this->findChildByNameAs<MouseProxy>("morda_rt_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->rightTopResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->rightTopResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampBottom(d.x, -(this->rect().d.x - this->emptyMinDim.x));
				utki::clampBottom(d.y, -(this->rect().d.y - this->emptyMinDim.y));
				this->resizeBy(d);
			}
			return false;
		};
	}
	
	{
		auto w = this->findChildByNameAs<MouseProxy>("morda_rb_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->rightBottomResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->rightBottomResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampBottom(d.x, -(this->rect().d.x - this->emptyMinDim.x));
				utki::clampTop(d.y, this->rect().d.y - this->emptyMinDim.y);
				this->moveBy(morda::Vec2r(0, d.y));
				this->resizeBy(morda::Vec2r(d.x, -d.y));
			}
			return false;
		};
	}
	
	{
		auto w = this->findChildByNameAs<MouseProxy>("morda_l_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->leftResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->leftResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampTop(d.x, this->rect().d.x - this->emptyMinDim.x);
				this->moveBy(morda::Vec2r(d.x, 0));
				this->resizeBy(morda::Vec2r(-d.x, 0));
			}
			return false;
		};
	}
	
	{
		auto w = this->findChildByNameAs<MouseProxy>("morda_r_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->rightResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->rightResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampBottom(d.x, -(this->rect().d.x - this->emptyMinDim.x));
				this->resizeBy(morda::Vec2r(d.x, 0));
			}
			return false;
		};
	}
	
	{
		auto w = this->findChildByNameAs<MouseProxy>("morda_t_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->topResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->topResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampBottom(d.y, -(this->rect().d.y - this->emptyMinDim.y));
				this->resizeBy(morda::Vec2r(0, d.y));
			}
			return false;
		};
	}
	
	{
		auto w = this->findChildByNameAs<MouseProxy>("morda_b_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->bottomResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->bottomResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampTop(d.y, this->rect().d.y - this->emptyMinDim.y);
				this->moveBy(morda::Vec2r(0, d.y));
				this->resizeBy(morda::Vec2r(0, -d.y));
			}
			return false;
		};
	}
}



void morda::Window::setTitle(const std::string& str){
	this->title->setText(unikod::toUtf32(str));
}



bool morda::Window::onMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId){
	if(isDown){
		morda::App::inst().PostToUIThread_ts(
				[this](){
					this->makeTopmost();
				}
			);

		if(!this->isTopmost()){
			this->focus();
		}
	}
	
	this->Container::onMouseButton(isDown, pos, button, pointerId);
	
	return true;
}



bool morda::Window::onMouseMove(const morda::Vec2r& pos, unsigned pointerId){
	this->Container::onMouseMove(pos, pointerId);
	return true;
}

void morda::Window::onTopmostChanged(){
	this->titleBg->setColor(this->isTopmost() ? this->titleBgColorTopmost : this->titleBgColorNonTopmost);
}
