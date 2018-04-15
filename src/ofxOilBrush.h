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
	ofxOilBrush(float size = 20);

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
	void paint(const vector<ofColor>& colors, int alpha) const;

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

	ofVec2f position;
	int nBristles;
	vector<ofxOilBristle> bristles;
	vector<ofVec2f> bOffsets;
	vector<ofVec2f> bPositions;
	vector<ofVec2f> positionsHistory;
	ofVec2f averagePosition;
	float noiseSeed;
	int updatesCounter;
	float bristleHorizontalNoise;
};
