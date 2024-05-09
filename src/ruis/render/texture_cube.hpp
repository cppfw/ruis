#pragma once

namespace ruis::render {

class texture_cube
{
protected:
	texture_cube() = default;

public:
	texture_cube(const texture_cube&) = delete;
	texture_cube& operator=(const texture_cube&) = delete;

	texture_cube(texture_cube&&) = delete;
	texture_cube& operator=(texture_cube&&) = delete;

	virtual ~texture_cube() = default;
};

} // namespace ruis::render
