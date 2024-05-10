#pragma once

#include "../render/texture_cube.hpp"
#include "../resource_loader.hpp"

namespace ruis::res {

/**
 * @brief Cube map texture resource.
 *
 * %resource description:
 *
 * @param file_px - name of the image file for positive-x cube face, can be raster image.
 * @param file_nx - name of the image file for negative-x cube face, can be raster image.
 * @param file_py - name of the image file for positive-y cube face, can be raster image.
 * @param file_ny - name of the image file for negative-y cube face, can be raster image.
 * @param file_pz - name of the image file for positive-z cube face, can be raster image.
 * @param file_nz - name of the image file for negative-z cube face, can be raster image.
 *
 * TODO:
 * @param min_filter - texture minification filter, possible values are [nearest, linear]. Default is 'linear'.
 * @param mag_filter - texture magnification filter, possible values are [nearest, linear]. Default is 'linear'.
 * @param mipmap - texture mipmapping, possible values are [none, nearest, linear]. Default is 'nearest'.
 *
 * Example:
 * @code
 * cube_sample{
 *     file_px{cube_sample_px.png}
 *     file_nx{cube_sample_nx.png}
 *     file_py{cube_sample_py.png}
 *     file_ny{cube_sample_ny.png}
 *     file_pz{cube_sample_pz.png}
 *     file_nz{cube_sample_nz.png}
 *
 *     // TODO:
 *     min_filter{linear}
 *     mag_filter{linear}
 *     mipmap{nearest}
 * }
 * @endcode
 */
class texture_cube : public ruis::resource
{
	friend class ruis::resource_loader;

	const utki::shared_ref<const render::texture_cube> tex_cube;

public:
};

} // namespace ruis::res
