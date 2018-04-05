#pragma once

/**
 * @brief The maximum bristle length
 */
const float MAX_BRISTLE_LENGTH = 15.0;

/**
 * @brief The maximum bristle thickness
 */
const float MAX_BRISTLE_THICKNESS = 5.0;

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
