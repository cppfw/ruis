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
	
	std::uint32_t update(){
		return this->updater.update();
	}
};

}
