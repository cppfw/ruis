#pragma once

namespace ruis {

/**
 * @brief Widget measurement modes.
 */
enum class measure_mode {
	/**
	 * @brief The widget will be resized to the given exact size.
	 * The widget is expected to return the same size value as is.
	 */
	exactly,

	/**
	 * @brief The widget can be at most the given size.
	 * The widget is expected to return a size less than the given size limit.
	 */
	at_most
};

} // namespace ruis
