namespace mordavokne{

const decltype(application::window_pimpl)& get_window_pimpl(application& app){
	return app.window_pimpl;
}

void render(application& app){
	app.render();
}

void update_window_rect(application& app, const morda::rectangle& rect){
	app.update_window_rect(rect);
}

void handle_mouse_move(application& app, const r4::vector2<float>& pos, unsigned id){
	app.handle_mouse_move(pos, id);
}

void handle_mouse_button(application& app, bool isDown, const r4::vector2<float>& pos, morda::mouse_button button, unsigned id){
	app.handle_mouse_button(isDown, pos, button, id);
}

void handleMouseHover(application& app, bool isHovered, unsigned pointerID){
	app.handleMouseHover(isHovered, pointerID);
}

void handle_character_input(application& app, const morda::gui::unicode_provider& unicode_resolver, morda::key key_code){
	app.handle_character_input(unicode_resolver, key_code);
}

void handle_key_event(application& app, bool isDown, morda::key keyCode){
	app.handle_key_event(isDown, keyCode);
}

}
