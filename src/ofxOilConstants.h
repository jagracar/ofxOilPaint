#pragma once

/**
 * @brief The maximum bristle length
 */
const float MAX_BRISTLE_LENGTH = 15;

/**
 * @brief The maximum bristle thickness
 */
const float MAX_BRISTLE_THICKNESS = 5;

/**
 * @brief The number of positions to use to calculate the brush average position
 */
const int POSITIONS_FOR_AVERAGE = 4;

/**
 * @brief The noise range to add to the bristles vertical position on the brush
 */
const float BRISTLE_VERTICAL_NOISE = 8;

/**
 * @brief The maximum noise range to add in each update to the bristles horizontal position on the brush
 */
const float MAX_BRISTLE_HORIZONTAL_NOISE = 4;

/**
 * @brief Controls the bristles horizontal noise speed
 */
const float NOISE_SPEED_FACTOR = 0.04;

/**
 * @brief Sets how random the trace movement is
 */
const float NOISE_FACTOR = 0.007;

/**
 * @brief The maximum allowed fraction of pixels in the trace trajectory with colors similar to the original image
 */
const float MAX_SIMILAR_COLOR_FRACTION_IN_TRAJECTORY = 0.6;

/**
 * @brief The maximum allowed fraction of pixels in the trace trajectory that have been visited before
 */
const float MAX_VISITS_FRACTION_IN_TRAJECTORY = 0.35;

/**
 * @brief The minimum fraction of pixels in the trace trajectory that should fall inside the canvas
 */
const float MIN_INSIDE_FRACTION_IN_TRAJECTORY = 0.4;

/**
 * @brief The maximum allowed value of the colors standard deviation along the trace trajectory
 */
const float MAX_COLOR_STDEV_IN_TRAJECTORY = 45;

/**
 * @brief The maximum allowed fraction of pixels in the trace with colors similar to the original image
 */
const float MAX_SIMILAR_COLOR_FRACTION = 0.8; // 0.8 - 0.85 - 0.5

/**
 * @brief The minimum fraction of pixels in the trace that should fall inside the canvas
 */
const float MIN_INSIDE_FRACTION = 0.7;

/**
 * @brief The minimum fraction of trace that needs to be painted already to consider it painted
 */
const float MIN_PAINTED_FRACTION = 0.65;

/**
 * @brief The minimum color improvement factor of the already painted pixels required to paint the trace on the canvas
 */
const float MIN_COLOR_IMPROVEMENT_FACTOR = 0.6;

/**
 * @brief The minimum improvement fraction in the number of well painted pixels to consider to paint the trace even if
 * there is not a significant color improvement
 */
const float BIG_WELL_PAINTED_IMPROVEMENT_FRACTION = 0.3; // 0.3 - 0.35 - 0.4

/**
 * @brief The minimum reduction fraction in the number of bad painted pixels required to paint the trace on the canvas
 */
const float MIN_BAD_PAINTED_REDUCTION_FRACTION = 0.45; // 0.45 - 0.3 - 0.45

/**
 * @brief The maximum allowed fraction of pixels in the trace that were previously well painted and will be now bad painted
 */
const float MAX_WELL_PAINTED_DESTRUCTION_FRACTION = 0.4; // 0.4 - 0.55 - 0.4

/**
 * @brief The brightness relative change range between the bristles
 */
const float BRIGHTNESS_RELATIVE_CHANGE = 0.09;

/**
 * @brief The typical step when the color mixing starts
 */
const int TYPICAL_MIX_STARTING_STEP = 5;

/**
 * @brief The color mixing strength
 */
const float MIX_STRENGTH = 0.012;

/**
 * @brief The minimum alpha value to be considered for the trace average color calculation
 */
const unsigned char MIN_ALPHA = 20;
