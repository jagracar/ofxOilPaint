#pragma once

#include "ofMain.h"

/**
 * @brief Class that simulates the movement of a bristle
 *
 * @author Javier Graciá Carpio
 */
class ofxOilBristle {
public:

	/**
	 * @brief Constructor
	 *
	 * @param position the bristle position
	 * @param length the bristle total length
	 */
	ofxOilBristle(const ofVec2f& position = ofVec2f(), float length = 10);

	/**
	 * @brief Updates the bristle position
	 *
	 * @param newPosition the new bristle position
	 */
	void updatePosition(const ofVec2f& newPosition);

	/**
	 * @brief Sets the bristle elements positions
	 *
	 * @param newPosition the new bristle elements position
	 */
	void setElementsPositions(const ofVec2f& newPosition);

	/**
	 * @brief Sets the bristle elements lengths
	 *
	 * @param newLengths the new bristle elements lengths
	 */
	void setElementsLengths(const vector<float>& newLengths);

	/**
	 * @brief Paints the bristle
	 *
	 * @param color the color to use
	 * @param thickness the thickness of the first bristle element
	 */
	void paint(const ofColor& color, float thickness) const;

	/**
	 * @brief Returns the number of bristle elements
	 *
	 * @return the number of bristle elements
	 */
	unsigned int getNElements() const;

protected:

	/**
	 * @brief The bristle elements positions
	 */
	vector<ofVec2f> positions;

	/**
	 * @brief The bristle elements lengths
	 */
	vector<float> lengths;
};
