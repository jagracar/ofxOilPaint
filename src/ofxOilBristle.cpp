#include "ofxOilBristle.h"
#include "ofMain.h"

ofxOilBristle::ofxOilBristle(int nElements, float thickness) {
	int nPositions = nElements + 1;
	float thicknessDecrement = thickness / nElements;

	for (int i = 0; i < nPositions; ++i) {
		positions.emplace_back(0, 0, 0);
		lengths.emplace_back(nPositions - i);
		thicknesses.emplace_back(thickness - (i - 1) * thicknessDecrement);
	}
}

void ofxOilBristle::setPosition(const ofVec3f& newPosition) {
	fill(positions.begin(), positions.end(), newPosition);
}

void ofxOilBristle::updatePosition(const ofVec3f& newPosition) {
	int nPositions = positions.size();
	positions[0].set(newPosition);

	for (int i = 1; i < nPositions; ++i) {
		ofVec3f& pos = positions[i];
		ofVec3f& previousPos = positions[i-1];
		float length = lengths[i];
		float ang = atan2(previousPos.y - pos.y, previousPos.x - pos.x);
		pos.set(previousPos.x - length * cos(ang), previousPos.y - length * sin(ang));
	}
}

void ofxOilBristle::paintOnScreen(const ofColor& col) {
	// Set the stroke color
	ofSetColor(col);

	// Paint the bristle elements
	int nPositions = positions.size();

	for (int i = 1; i < nPositions; ++i) {
		ofSetLineWidth(thicknesses[i]);
		ofDrawLine(positions[i - 1], positions[i]);
	}
}
