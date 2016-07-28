#include <morda/App.hpp>
#include <morda/AppFactory.hpp>

#include <morda/widgets/label/TextLabel.hpp>
#include <morda/widgets/button/Button.hpp>


class Application : public morda::App{
public:
	Application() :
			morda::App(morda::App::WindowParams(kolme::Vec2ui(800, 600)))
	{
		this->initStandardWidgets();
		
		//Inflate widgets hierarchy from GUI description script
		auto c = morda::App::inst().inflater.inflate(
				*this->createResourceFileInterface("res/main.gui.stob")
			);
		
		//set the widgets hierarchy to the application
		this->setRootWidget(c);
		
		auto textLabel = c->findChildByNameAs<morda::TextLabel>("info_text");
		ASSERT(textLabel)
		
		auto button = c->findChildByNameAs<morda::PushButton>("hw_button");
		
		auto textLabelWeak = utki::makeWeak(textLabel);//make a weak pointer to the TextLabel widget.
		
		bool even = true;
		
		//connect some action on button click
		button->clicked = [textLabelWeak, even](morda::PushButton&) mutable {
			if(auto tl = textLabelWeak.lock()){
				even = !even;
				if(even){
					tl->setText("even");
				}else{
					tl->setText("odd");
				}
			}
		};
	}
};



std::unique_ptr<morda::App> morda::createApp(int argc, const char** argv, const utki::Buf<std::uint8_t> savedState){
	return utki::makeUnique<Application>();
}

