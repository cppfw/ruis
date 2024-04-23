include_subdirs

tex_sample{
	file {texture.jpg}
	min_filter{linear}
	mag_filter{linear}
	mipmap{nearest}
}

tex_lattice{
	file {lattice.png}
}

img_button_unpressed{
	file{empty_cell_test.svg}
}

img_button_pressed{
	file{shot_test.svg}
}

img_lattice{
	file{lattice.png}
}

img_arrow{
	file{mouse_arrow.png}
}

crs_arrow{
	image{img_arrow}
	hotspot{2 3}
}

tex_images{
	file{sample.png}
}

img_sample{
	tex{tex_sample}
//	rect{0 0 120 90}
}

img_camera{
	file{camera.svg}
}

grd_sample{
	vertical{false}

	stop{ pos{0.3} color{0x0} }
	stop{ pos{0.7} color{0xff00ffff} }
	stop{ pos{0.8} color{0xffffff00} }
}
