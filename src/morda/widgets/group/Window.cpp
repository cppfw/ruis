#include "Window.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"

//TODO: remove
#include "../../gui.hpp"


using namespace morda;


namespace{

const auto windowDesc_c = puu::read(R"qwertyuiop(
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
	)qwertyuiop");

}


void morda::Window::setBackground(std::shared_ptr<Widget> w) {
	ASSERT(this->children().size() == 1 || this->children().size() == 2)
	if(this->children().size() == 2){
		this->remove(this->children().begin());
	}

	ASSERT(this->children().size() == 1)

	if(w){
		this->insert(std::move(w), this->children().begin());
	}
}



morda::Window::Window(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		Pile(nullptr, windowDesc_c)
{
	this->setupWidgets();

	Sidesr borders(0);

	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "title"){
			this->setTitle(get_property_value(p).to_string());
		}else if(p.value == "look"){
			for(const auto& pp : p.children){
				if(!is_property(pp)){
					continue;
				}

				if(pp.value == "titleColorTopmost"){
					this->titleBgColorTopmost = get_property_value(pp).to_uint32();
				}else if(pp.value == "titleColorNonTopmost"){
					this->titleBgColorNonTopmost = get_property_value(pp).to_uint32();
				}else if(pp.value == "background"){
					this->setBackground(morda::inst().context->inflater.inflate(pp.children));
				}else if(pp.value == "left"){
					borders.left() = parse_dimension_value(get_property_value(pp));
				}else if(pp.value == "top"){
					borders.top() = parse_dimension_value(get_property_value(pp));
				}else if(pp.value == "right"){
					borders.right() = parse_dimension_value(get_property_value(pp));
				}else if(pp.value == "bottom"){
					borders.bottom() = parse_dimension_value(get_property_value(pp));
				}
			}
		}
	}
	this->setBorders(borders);

	// this should go after initializing borders
	this->emptyMinDim = this->measure(Vec2r(-1));

	this->contentArea->inflate_push_back(desc);
}

void morda::Window::setupWidgets(){
	this->contentArea = this->try_get_widget_as<Pile>("morda_content");
	ASSERT(this->contentArea)

	this->title = this->try_get_widget_as<Text>("morda_title");
	ASSERT(this->title)

	this->titleBg = this->try_get_widget_as<Color>("morda_window_title_bg");
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
		auto caption = this->try_get_widget_as<MouseProxy>("morda_caption_proxy");
		ASSERT(caption)

		caption->mouseButton = getButtonFunc(this->captionCaptured);

		caption->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->captionCaptured){
				this->move_by(pos - this->capturePoint);
				return true;
			}
			return false;
		};
	}

	{
		auto w = this->try_get_widget_as<MouseProxy>("morda_lt_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->leftTopResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->leftTopResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampTop(d.x, this->rect().d.x - this->emptyMinDim.x);
				utki::clampTop(d.y, this->rect().d.y - this->emptyMinDim.y);
				this->move_by(d);
				this->resize_by(-d);
			}
			return false;
		};
		this->ltBorder = w;
	}

	{
		auto w = this->try_get_widget_as<MouseProxy>("morda_lb_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->leftBottomResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->leftBottomResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampTop(d.x, this->rect().d.x - this->emptyMinDim.x);
				utki::clampBottom(d.y, -(this->rect().d.y - this->emptyMinDim.y));
				this->move_by(morda::Vec2r(d.x, 0));
				this->resize_by(morda::Vec2r(-d.x, d.y));
			}
			return false;
		};
		this->lbBorder = w;
	}

	{
		auto w = this->try_get_widget_as<MouseProxy>("morda_rt_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->rightTopResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->rightTopResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampBottom(d.x, -(this->rect().d.x - this->emptyMinDim.x));
				utki::clampTop(d.y, this->rect().d.y - this->emptyMinDim.y);
				this->move_by(morda::Vec2r(0, d.y));
				this->resize_by(morda::Vec2r(d.x, -d.y));
			}
			return false;
		};
		this->rtBorder = w;
	}

	{
		auto w = this->try_get_widget_as<MouseProxy>("morda_rb_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->rightBottomResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->rightBottomResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampBottom(d.x, -(this->rect().d.x - this->emptyMinDim.x));
				utki::clampBottom(d.y, -(this->rect().d.y - this->emptyMinDim.y));
				this->resize_by(d);
			}
			return false;
		};
		this->rbBorder = w;
	}

	{
		auto w = this->try_get_widget_as<MouseProxy>("morda_l_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->leftResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->leftResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampTop(d.x, this->rect().d.x - this->emptyMinDim.x);
				this->move_by(morda::Vec2r(d.x, 0));
				this->resize_by(morda::Vec2r(-d.x, 0));
			}
			return false;
		};
		this->lBorder = w;
	}

	{
		auto w = this->try_get_widget_as<MouseProxy>("morda_r_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->rightResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->rightResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampBottom(d.x, -(this->rect().d.x - this->emptyMinDim.x));
				this->resize_by(morda::Vec2r(d.x, 0));
			}
			return false;
		};
		this->rBorder = w;
	}

	{
		auto w = this->try_get_widget_as<MouseProxy>("morda_t_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->topResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->topResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampTop(d.y, this->rect().d.y - this->emptyMinDim.y);
				this->move_by(morda::Vec2r(0, d.y));
				this->resize_by(morda::Vec2r(0, -d.y));
			}
			return false;
		};
		this->tBorder = w;
	}

	{
		auto w = this->try_get_widget_as<MouseProxy>("morda_b_proxy");
		ASSERT(w)
		w->mouseButton = getButtonFunc(this->bottomResizeCaptured);
		w->mouseMove = [this](Widget& widget, const morda::Vec2r& pos, unsigned pointerId){
			if(this->bottomResizeCaptured){
				morda::Vec2r d = pos - this->capturePoint;
				utki::clampBottom(d.y, -(this->rect().d.y - this->emptyMinDim.y));
				this->resize_by(morda::Vec2r(0, d.y));
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
	this->lBorder->get_layout_params().dim.x = borders.left();
	this->tBorder->get_layout_params().dim.y = borders.top();
	this->rBorder->get_layout_params().dim.x = borders.right();
	this->bBorder->get_layout_params().dim.y = borders.bottom();

	this->lbBorder->get_layout_params().dim.x = borders.left();
	this->lbBorder->get_layout_params().dim.y = borders.bottom();

	this->rbBorder->get_layout_params().dim.x = borders.right();
	this->rbBorder->get_layout_params().dim.y = borders.bottom();

	this->ltBorder->get_layout_params().dim.x = borders.left();
	this->ltBorder->get_layout_params().dim.y = borders.top();

	this->rtBorder->get_layout_params().dim.x = borders.right();
	this->rtBorder->get_layout_params().dim.y = borders.top();
}


bool morda::Window::on_mouse_button(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId){
	if(isDown && !this->isTopmost()){
		morda::gui::inst().postToUiThread(
				[this](){
					this->makeTopmost();
					this->focus();
				}
			);
	}

	this->Container::on_mouse_button(isDown, pos, button, pointerId);

	return true;
}



bool morda::Window::on_mouse_move(const morda::Vec2r& pos, unsigned pointerId){
	this->Container::on_mouse_move(pos, pointerId);
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

	p->change_child_z_position(p->find(this), p->children().end());

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


void Window::lay_out() {
	this->updateTopmost();
	this->Pile::lay_out();
}
