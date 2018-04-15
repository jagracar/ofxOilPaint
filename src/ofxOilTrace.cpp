#include "ofxOilTrace.h"
#include "ofMain.h"
#include "ofxOilConstants.h"
#include "ofxOilBrush.h"

ofxOilTrace::ofxOilTrace(const ofVec2f& position, int _nSteps, float speed) {
	nSteps = _nSteps;
	positions = vector<ofVec2f>();
	colors = vector<vector<ofColor>>();
	alphas = vector<int>();
	brush = ofxOilBrush();
	nBristles = brush.getNBristles();

	// Fill the positions array
	float initAng = ofRandom(TWO_PI);
	float noiseSeed = ofRandom(1000);
	positions.push_back(position);

	for (int step = 1; step < nSteps; ++step) {
		float ang = initAng + TWO_PI * (ofNoise(noiseSeed + NOISE_FACTOR * step) - 0.5);
		positions.emplace_back(positions[step - 1].x + speed * cos(ang), positions[step - 1].y + speed * sin(ang));
	}
}

void ofxOilTrace::setBrushSize(float brushSize) {
	brush = ofxOilBrush(brushSize);
	brush.init(positions[0]);
	nBristles = brush.getNBristles();
}

void ofxOilTrace::paintOnScreen(const ofColor& col) {
	int counter = 0;

	for (const ofVec2f& pos : positions) {
		brush.update(pos, true);
		ofColor alphaCol = ofColor(col, 255 - counter);
		brush.paint(alphaCol);
		counter += 5;
	}

	// Reset the brush to the initial position
	brush.init(positions[0]);
}

int ofxOilTrace::getNSteps() {
	return nSteps;
}
