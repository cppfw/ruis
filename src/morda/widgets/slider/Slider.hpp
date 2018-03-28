#pragma once

#include <functional>

#include "../Widget.hpp"

#include "../group/Pile.hpp"

#include "../base/FractionWidget.hpp"
#include "../base/OrientedWidget.hpp"

//disable stupid warnings
#if M_COMPILER == M_COMPILER_MSVC
#	pragma warning(disable : 4250)
#endif


namespace morda{


class AreaSlider :
		public FractionWidget,
		protected OrientedWidget
{
	float curAreaSizeFraction = 0; //Current area size factor from 0 to 1
protected:
	AreaSlider(const stob::Node* chain, bool vertical) :
			Widget(chain),
			FractionWidget(chain),
			OrientedWidget(chain, vertical)
	{}
	
	virtual void onAreaSizeChanged();
	
public:
	std::function<void(AreaSlider& s)> areaSizeChanged;
	
	void setAreaSizeFraction(real fraction);
	
	real areaSizeFraction()const noexcept{
		return this->curAreaSizeFraction;
	}
};


class HandleSlider :
		public AreaSlider,
		private Pile //users do not need to know that it is a container
{
	HandleSlider(const HandleSlider&) = delete;
	HandleSlider& operator=(const HandleSlider&) = delete;
	
	Widget& handle;
	
	bool isGrabbed = false;
	float clickPoint;
	
protected:
	HandleSlider(const stob::Node* chain, bool vertical);

	void onFractionChange() override;

	void onAreaSizeChanged() override;
	
public:

	virtual ~HandleSlider()noexcept{}
	
private:
	void layOut() override;
};



class VerticalSlider : public HandleSlider{
public:
	VerticalSlider(const stob::Node* chain = nullptr) : 
			Widget(chain),
			HandleSlider(chain, true)
	{}
	
	VerticalSlider(const VerticalSlider&) = delete;
	VerticalSlider& operator=(const VerticalSlider&) = delete;
};


class HorizontalSlider : public HandleSlider{
public:
	HorizontalSlider(const stob::Node* chain = nullptr) : 
			Widget(chain),
			HandleSlider(chain, false)
	{}
	
	HorizontalSlider(const HorizontalSlider&) = delete;
	HorizontalSlider& operator=(const HorizontalSlider&) = delete;
};

}
