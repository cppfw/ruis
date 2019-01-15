#include "Window.hpp"

#include "../../Morda.hpp"

#include "../../util/util.hpp"



using namespace morda;


namespace{

const char* windowDesc_c = R"qwertyuiop(
		Column{
			layout{
				dx{max} dy{max}
			}

			Row{
				layout{dx{fill}}

				MouseProxy{
					id{morda_lt_proxy}
				}

				MouseProxy{
					id{morda_t_proxy}
					layout{
						dx{0} dy{fill} weight{1}
					}
				}

				MouseProxy{
					id{morda_rt_proxy}
				}
			}

			Row{
				layout{
					weight{1}
					dx{max}
				}

				MouseProxy{
					id{morda_l_proxy}
					layout{dy{fill}}
				}

				//middle
				Column{
					layout{
						weight{1}
						dy{max}
					}

					//caption
					Pile{
						layout{
							dx{max}
						}

						clip{true}

						MouseProxy{
							id{morda_caption_proxy}
							layout{
								dx{max} dy{max}
							}
						}

						Color{
							id{morda_window_title_bg}
							layout{
								dx{max} dy{max}
							}
						}

						Row{
							layout{
								dx{max} dy{max}
							}

							Margins{
								left{3dp}
								top{2dp}
								bottom{2dp}

								layout{
									dx{0}
									weight{1}
								}

								Text{
									id{morda_title}
								}
							}
						}
					}

					Pile{
						id{morda_content}
						clip{true}
						layout{
							dx{fill} dy{0}
							weight{1}
						}
					}
				}

				MouseProxy{
					id{morda_r_proxy}
					layout{dy{fill}}
				}
			}

			Row{
				layout{
					dx{fill}
				}
				MouseProxy{
					id{morda_lb_proxy}
				}

				MouseProxy{
					id{morda_b_proxy}
					layout{
						dy{fill}
						dx{0}
						weight{1}
					}
				}

				MouseProxy{
					id{morda_rb_proxy}
				}
			}
		}
	)qwertyuiop";

}


void morda::Window::setBackground(std::shared_ptr<Widget> w) {
	ASSERT(this->children().size() == 1 || this->children().size() == 2)
	if(this->children().size() == 2){
		this->remove(this->children().begin());
	}

	ASSERT(this->children().size() == 1)

	if(w){
		this->add(std::move(w), this->children().begin());
	}
}



morda::Window::Window(const stob::Node* chain) :
		Widget(chain),
		Pile(stob::parse(windowDesc_c).get())
{
	this->setupWidgets();

	if(auto n = getProperty(chain, "title")){
		this->setTitle(n->value());
	}

	{
		auto ch = getProperty(chain, "look");

		if(auto n = getProperty(ch, "titleColorTopmost")){
			this->titleBgColorTopmost = n->asUint32();
		}else{
			this->titleBgColorTopmost = 0xffff0000;
		}

		if(auto n = getProperty(ch, "titleColorNonTopmost")){
			this->titleBgColorNonTopmost = n->asUint32();
		}else{
			this->titleBgColorNonTopmost = 0xff808080;
		}

		if(auto n = getProperty(ch, "background")){
			this->setBackground(morda::Morda::inst().inflater.inflate(*n));
		}

		{
			Sidesr borders;

			if(auto n = getProperty(ch, "left")){
				borders.left() = dimValueFromSTOB(*n);
			}else{
				borders.left() = 0;
			}

			if(auto n = getProperty(ch, "top")){
				borders.top() = dimValueFromSTOB(*n);
			}else{
				borders.top() = 0;
			}

			if(auto n = getProperty(ch, "right")){
				borders.right() = dimValueFromSTOB(*n);
			}else{
				borders.right() = 0;
			}

			if(auto n = getProperty(ch, "bottom")){
				borders.bottom() = dimValueFromSTOB(*n);
			}else{
				borders.bottom() = 0;
			}

			this->setBorders(borders);
		}
	}

	//this should go after initializing borders
	this->emptyMinDim = this->measure(Vec2r(-1));

	if(chain){
		this->contentArea->add(*chain);
	}
}

void morda::Window::setupWidgets(){
	this->contentArea = this->findByIdAs<Pile>("morda_content");
	ASSERT(this->contentArea)

	this->title = this->findByIdAs<Text>("morda_title");
	ASSERT(this->title)

	this->titleBg = this->findByIdAs<Color>("morda_window_title_bg");
	ASSERT(this->titleBg);

	std::function<decltype(MouseProxy::mouseButton)(bool&)> getButtonFunc = [this](bool& flag){
		return decltype(MouseProxy::mouseButton)([this, &flag](Widget& widget, bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId){
			if(button != MouseButton_e::LEFT){
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
		auto caption = this->findByIdAs<MouseProxy>("morda_caption_proxy");
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
		auto w = this->findByIdAs<MouseProxy>("morda_lt_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->leftTopResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->leftTopResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampTop(d.x, this->rect().d.x - this->emptyMinDim.x);
				utki::clampTop(d.y, this->rect().d.y - this->emptyMinDim.y);
				this->moveBy(d);
				this->resizeBy(-d);
			}
			return false;
		};
		this->ltBorder = w;
	}

	{
		auto w = this->findByIdAs<MouseProxy>("morda_lb_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->leftBottomResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->leftBottomResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampTop(d.x, this->rect().d.x - this->emptyMinDim.x);
				utki::clampBottom(d.y, -(this->rect().d.y - this->emptyMinDim.y));
				this->moveBy(morda::Vec2r(d.x, 0));
				this->resizeBy(morda::Vec2r(-d.x, d.y));
			}
			return false;
		};
		this->lbBorder = w;
	}

	{
		auto w = this->findByIdAs<MouseProxy>("morda_rt_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->rightTopResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->rightTopResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampBottom(d.x, -(this->rect().d.x - this->emptyMinDim.x));
				utki::clampTop(d.y, this->rect().d.y - this->emptyMinDim.y);
				this->moveBy(morda::Vec2r(0, d.y));
				this->resizeBy(morda::Vec2r(d.x, -d.y));
			}
			return false;
		};
		this->rtBorder = w;
	}

	{
		auto w = this->findByIdAs<MouseProxy>("morda_rb_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->rightBottomResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->rightBottomResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampBottom(d.x, -(this->rect().d.x - this->emptyMinDim.x));
				utki::clampBottom(d.y, -(this->rect().d.y - this->emptyMinDim.y));
				this->resizeBy(d);
			}
			return false;
		};
		this->rbBorder = w;
	}

	{
		auto w = this->findByIdAs<MouseProxy>("morda_l_proxy");
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
		this->lBorder = w;
	}

	{
		auto w = this->findByIdAs<MouseProxy>("morda_r_proxy");
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
		this->rBorder = w;
	}

	{
		auto w = this->findByIdAs<MouseProxy>("morda_t_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->topResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->topResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampTop(d.y, this->rect().d.y - this->emptyMinDim.y);
				this->moveBy(morda::Vec2r(0, d.y));
				this->resizeBy(morda::Vec2r(0, -d.y));
			}
			return false;
		};
		this->tBorder = w;
	}

	{
		auto w = this->findByIdAs<MouseProxy>("morda_b_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->bottomResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->bottomResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampBottom(d.y, -(this->rect().d.y - this->emptyMinDim.y));
				this->resizeBy(morda::Vec2r(0, d.y));
			}
			return false;
		};
		this->bBorder = w;
	}
}



void morda::Window::setTitle(const std::string& str){
	this->title->setText(unikod::toUtf32(str));
}

void morda::Window::setBorders(Sidesr borders) {
	this->lBorder->getLayoutParams().dim.x = borders.left();
	this->tBorder->getLayoutParams().dim.y = borders.top();
	this->rBorder->getLayoutParams().dim.x = borders.right();
	this->bBorder->getLayoutParams().dim.y = borders.bottom();

	this->lbBorder->getLayoutParams().dim.x = borders.left();
	this->lbBorder->getLayoutParams().dim.y = borders.bottom();

	this->rbBorder->getLayoutParams().dim.x = borders.right();
	this->rbBorder->getLayoutParams().dim.y = borders.bottom();

	this->ltBorder->getLayoutParams().dim.x = borders.left();
	this->ltBorder->getLayoutParams().dim.y = borders.top();

	this->rtBorder->getLayoutParams().dim.x = borders.right();
	this->rtBorder->getLayoutParams().dim.y = borders.top();
}


bool morda::Window::onMouseButton(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId){
	if(isDown && !this->isTopmost()){
		morda::Morda::inst().postToUiThread(
				[this](){
					this->makeTopmost();
					this->focus();
				}
			);
	}

	this->Container::onMouseButton(isDown, pos, button, pointerId);

	return true;
}



bool morda::Window::onMouseMove(const morda::Vec2r& pos, unsigned pointerId){
	this->Container::onMouseMove(pos, pointerId);
	return true;
}


void Window::makeTopmost(){
	if(!this->parent()){
		return;
	}

	ASSERT(this->parent()->children().size() != 0)

	if(this->parent()->children().size() == 1){
		return;
	}

	auto prevTopmost = this->parent()->children().rbegin()->get();
	ASSERT(prevTopmost)
	if(prevTopmost == this){
		return;//already topmost
	}

	Container* p = this->parent();

	p->changeChildZPosition(*this, p->children().end());

	this->updateTopmost();

	if(auto pt = dynamic_cast<Window*>(prevTopmost)){
		pt->updateTopmost();
	}
}


bool Window::isTopmost()const noexcept{
	if(!this->parent()){
		return false;
	}

	ASSERT(this->parent()->children().size() != 0)

	return this->parent()->children().back().get() == this;
}

void Window::updateTopmost() {
	this->titleBg->setColor(
			this->isTopmost() ? this->titleBgColorTopmost : this->titleBgColorNonTopmost
		);
}


void Window::layOut() {
	this->updateTopmost();
	this->Pile::layOut();
}
