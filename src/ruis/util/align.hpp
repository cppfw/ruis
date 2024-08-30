#pragma once

namespace ruis {

/**
 * @brief Alignment parameter values.
 */
enum class align {
	/**
	 * @brief Undefined alignment.
	 * In case alignemnt is undefined the default center alignment is assumed.
	 */
	undefined,

	/**
	 * @brief Left or top alignment.
	 */
	front,

	/**
	 * @brief Center lignment.
	 */
	center,

	/**
	 * @brief Right or bottom alignment.
	 */
	back
};

} // namespace ruis
