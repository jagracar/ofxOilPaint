#pragma once

#include "ofMain.h"
#include "ofxOilBrush.h"

/**
 * @brief Trace class
 *
 * @author Javier Graciá Carpio
 */
class ofxOilTrace {
public:

	/**
	 * @brief Constructor
	 */
	ofxOilTrace(const ofVec2f& position=ofVec2f(), int nSteps=20, float speed=5);

	//bool hasValidTrajectory(const vector<bool>& similarColor, const vector<bool>& visitedPixels, const ofImage& originalImg);
	void setBrushSize(float brushSize);
	void paintOnScreen(const ofColor& col);
	int getNSteps();

protected:
	int nSteps;
	vector<ofVec2f> positions;
	vector<vector<ofColor>> colors;
	vector<int> alphas;
	ofxOilBrush brush;
	int nBristles;
};
