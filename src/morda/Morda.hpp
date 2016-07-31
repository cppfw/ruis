#pragma once

#include <utki/Singleton.hpp>

#include "shaders/ColorPosShader.hpp"
#include "shaders/ColorPosTexShader.hpp"
#include "shaders/ClrPosShader.hpp"
#include "shaders/PosTexShader.hpp"
#include "shaders/SimpleGrayscalePosTexShader.hpp"
#include "shaders/SimpleBlurPosTexShader.hpp"

#include "Updateable.hpp"

#include "Inflater.hpp"
#include "ResourceManager.hpp"

namespace morda{

class Morda : public utki::IntrusiveSingleton<Morda>{
	friend T_Singleton;
	static T_Instance instance;
	
	friend class Updateable;
	
public:

	Morda(){}

	Morda(const Morda&) = delete;
	Morda& operator=(const Morda&) = delete;
	
	virtual ~Morda()noexcept{}

	
	/**
	 * @brief Collection of standard shaders.
	 */
	struct DefaultShaders{
		ColorPosShader colorPosShader;
		ColorPosTexShader colorPosTexShader;
		ClrPosShader clrPosShader;
		PosTexShader posTexShader;
		SimpleGrayscalePosTexShader simpleGrayscalePosTexShader;
		SimpleBlurPosTexShader simpleBlurPosTexShader;
	} shaders;

private:
	Updateable::Updater updater;
public:
	
	/**
	 * @brief Instantiation of the resource manager.
	 */
	ResourceManager resMan;

	/**
	 * @brief Instantiation of the GUI inflater.
	 */
	Inflater inflater;
	
	
	/**
	 * @brief Initialize standard widgets library.
	 * In addition to core widgets it is possible to use standard widgets.
	 * This function loads necessarey resource packs and initializes standard
	 * widgets to be used by application.
	 */
	void initStandardWidgets();
	
	
	/**
	 * @brief Update GUI.
	 * Call this function from main loop of the program.
	 * @return number of milliseconds to sleep before next call.
	 */
	std::uint32_t update(){
		return this->updater.update();
	}
	
	/**
	 * @brief Execute code on UI thread.
	 * This function should be thread-safe.
	 * This function should post a message/event to programs main loop event queue.
	 * When the event is handled it should execute the specified function.
	 * @param f - function to execute on UI thread.
	 */
	virtual void postToUiThread_ts(std::function<void()>&& f) = 0;
};

}
