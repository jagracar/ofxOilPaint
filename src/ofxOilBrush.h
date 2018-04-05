#pragma once

#include "ofMain.h"
#include "ofxOilBristle.h"

/**
 * @brief Brush class
 *
 * @author Javier Graciá Carpio
 */
class ofxOilBrush {
public:

	/**
	 * @brief Constructor
	 */
	ofxOilBrush(float size);

protected:
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
