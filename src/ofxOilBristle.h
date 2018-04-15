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
	 * @param nElements the total number of bristle elements
	 * @param thickness the thickness of the first bristle element
	 */
	ofxOilBristle(int nElements = 5, float thickness = 5);

	/**
	 * @brief Moves all the bristle elements to a new position
	 *
	 * @param newPosition the new bristle elements position
	 */
	void setPosition(const ofVec2f& newPosition);

	/**
	 * @brief Updates the bristle position
	 *
	 * @param newPosition the new bristle position
	 */
	void updatePosition(const ofVec2f& newPosition);

	/**
	 * @brief Paints the bristle
	 *
	 * @param col the color to use
	 */
	void paint(const ofColor& col) const;

protected:

	/**
	 * @brief The bristle elements positions
	 */
	vector<ofVec2f> positions;

	/**
	 * @brief The bristle elements lengths
	 */
	vector<float> lengths;

	/**
	 * @brief The bristle elements thicknesses
	 */
	vector<float> thicknesses;
};
