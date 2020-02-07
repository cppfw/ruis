namespace mordavokne{

const decltype(application::windowPimpl)& getWindowPimpl(application& app){
	return app.windowPimpl;
}

void render(application& app){
	app.render();
}

void updateWindowRect(application& app, const morda::Rectr& rect){
	app.updateWindowRect(rect);
}

void handleMouseMove(application& app, const r4::vec2f& pos, unsigned id){
	app.handleMouseMove(pos, id);
}

void handleMouseButton(application& app, bool isDown, const r4::vec2f& pos, morda::MouseButton_e button, unsigned id){
	app.handleMouseButton(isDown, pos, button, id);
}

void handleMouseHover(application& app, bool isHovered, unsigned pointerID){
	app.handleMouseHover(isHovered, pointerID);
}

void handleCharacterInput(application& app, const morda::context::UnicodeProvider& unicodeResolver, morda::key key){
	app.handleCharacterInput(unicodeResolver, key);
}

void handleKeyEvent(application& app, bool isDown, morda::key keyCode){
	app.handleKeyEvent(isDown, keyCode);
}

}
