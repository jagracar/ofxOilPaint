#pragma once

#include "ofMain.h"
#include "ofxOilBristle.h"

/**
 * @brief Class that simulates a brush composed of several bristles
 *
 * @author Javier Graciá Carpio
 */
class ofxOilBrush {
public:

	/**
	 * @brief The maximum bristle length
	 */
	static float MAX_BRISTLE_LENGTH;

	/**
	 * @brief The maximum bristle thickness
	 */
	static float MAX_BRISTLE_THICKNESS;

	/**
	 * @brief The maximum noise range to add in each update to the bristles horizontal position on the brush
	 */
	static float MAX_BRISTLE_HORIZONTAL_NOISE;

	/**
	 * @brief The noise range to add to the bristles vertical position on the brush
	 */
	static float BRISTLE_VERTICAL_NOISE;

	/**
	 * @brief Controls the bristles horizontal noise speed
	 */
	static float NOISE_SPEED_FACTOR;

	/**
	 * @brief The number of positions to use to calculate the brush average position
	 */
	static unsigned int POSITIONS_FOR_AVERAGE;

	/**
	 * @brief Constructor
	 *
	 * @param _position the brush central position
	 * @param _size the brush size
	 */
	ofxOilBrush(const glm::vec2& _position = glm::vec2(), float _size = 5);

	/**
	 * @brief Moves the brush to a new position and resets some internal variables
	 *
	 * @param newPosition the new brush central position
	 */
	void resetPosition(const glm::vec2& newPosition);

	/**
	 * @brief Updates the brush to a new central position
	 *
	 * @param newPosition the new brush central position
	 * @param updateBristlesElements true if the bristles elements positions should be updated
	 */
	void updatePosition(const glm::vec2& newPosition, bool updateBristlesElements = true);

	/**
	 * @brief Paints the brush using the provided color
	 *
	 * @param color the brush color
	 */
	void paint(const ofColor& color) const;

	/**
	 * @brief Paints the brush using the provided bristles colors
	 *
	 * @param colors the bristles colors
	 * @param alpha the colors alpha value
	 */
	void paint(const vector<ofColor>& colors, unsigned char alpha) const;

	/**
	 * @brief Returns the total number of bristles in the brush
	 *
	 * @return the total number of bristles in the brush
	 */
	unsigned int getNBristles() const;

	/**
	 * @brief Returns the current bristles positions
	 *
	 * @return a vector with the current bristles positions
	 */
	const vector<glm::vec2> getBristlesPositions() const;

protected:

	/**
	 * @brief The brush central position
	 */
	glm::vec2 position;

	/**
	 * @brief The brush size
	 */
	float size;

	/**
	 * @brief The bristles length
	 */
	float bristlesLength;

	/**
	 * @brief The bristles thickness
	 */
	float bristlesThickness;

	/**
	 * @brief The bristles horizontal noise factor
	 */
	float bristlesHorizontalNoise;

	/**
	 * @brief The bristles horizontal noise seed
	 */
	float bristlesHorizontalNoiseSeed;

	/**
	 * @brief The bristles offset positions relative to the brush center
	 */
	vector<glm::vec2> bOffsets;

	/**
	 * @brief The bristles positions
	 */
	vector<glm::vec2> bPositions;

	/**
	 * @brief The brush bristles
	 */
	vector<ofxOilBristle> bristles;

	/**
	 * @brief The average bush central position, considering the last position updates
	 */
	glm::vec2 averagePosition;

	/**
	 * @brief An array containing the bush central positions from the last position updates
	 */
	vector<glm::vec2> positionsHistory;

	/**
	 * @brief Counts the number of times that the brush central position has been updated
	 */
	int updatesCounter;
};
