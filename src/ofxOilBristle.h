#pragma once

#include "ofMain.h"

/**
 * @brief Bristle class
 *
 * @author Javier Graciá Carpio
 */
class ofxOilBristle {
public:

	/**
	 * @brief Constructor
	 */
	ofxOilBristle(int nElements, float thickness);

	void setPosition(const ofVec2f& newPosition);
	void updatePosition(const ofVec2f& newPosition);
	void paintOnScreen(const ofColor& col);

protected:
	vector<ofVec2f> positions;
	vector<float> lengths;
	vector<float> thicknesses;
};
