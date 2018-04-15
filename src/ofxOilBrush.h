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
	 * @brief Constructor
	 *
	 * @param size the brush size
	 */
	ofxOilBrush(float size = 5);

	/**
	 * @brief Moves the brush to a new position and resets some internal counters
	 *
	 * @param newPosition the new position
	 */
	void init(const ofVec2f& newPosition);

	/**
	 * @brief Updates the brush properties
	 *
	 * @param newPosition the new position
	 * @param updateBristleElements true if the bristles element positions should be updated
	 */
	void update(const ofVec2f& newPosition, bool updateBristleElements);

	/**
	 * @brief Paints the brush using the provided color
	 *
	 * @param color the brush color
	 */
	void paint(const ofColor& color) const;

	/**
	 * @brief Paints the brush using the provided bristle colors
	 *
	 * @param colors the bristle colors
	 * @param alpha the colors alpha value
	 */
	void paint(const vector<ofColor>& colors, unsigned char alpha) const;

	/**
	 * @brief Returns the total number of bristles in the brush
	 *
	 * @return the total number of bristles in the brush
	 */
	int getNBristles() const;

	/**
	 * @brief Returns the current bristles positions
	 *
	 * @return a vector with the current bristles positions
	 */
	vector<ofVec2f> getBristlesPositions() const;

protected:

	/**
	 * @brief Updates the bristle positions vectors
	 *
	 * @param directionAngle the brush movement direction angle
	 */
	void updateBristlePositions(float directionAngle);

	/**
	 * @brief The brush central position
	 */
	ofVec2f position;

	/**
	 * @brief The total number of bristles in the brush
	 */
	int nBristles;

	/**
	 * @brief The brush's bristles
	 */
	vector<ofxOilBristle> bristles;

	/**
	 * @brief The bristles offset positions relative to the brush center
	 */
	vector<ofVec2f> bOffsets;

	/**
	 * @brief The bristles positions
	 */
	vector<ofVec2f> bPositions;

	/**
	 * @brief An array containing the bush central positions from the last updates
	 */
	vector<ofVec2f> positionsHistory;

	/**
	 * @brief The average bush central position, considering the last updates
	 */
	ofVec2f averagePosition;

	/**
	 * @brief The horizontal noise seed
	 */
	float noiseSeed;

	/**
	 * @brief Counts the number of times that the brush has been updated
	 */
	int updatesCounter;

	/**
	 * @brief The bristles horizontal noise factor
	 */
	float bristleHorizontalNoise;
};
