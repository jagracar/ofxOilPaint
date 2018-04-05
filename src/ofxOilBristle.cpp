#include "ofxOilBristle.h"
#include "ofMain.h"

ofxOilBristle::ofxOilBristle(int nElements, float thickness) {
	int nPositions = nElements + 1;
	float thicknessDecrement = thickness / nElements;

	for (int i = 0; i < nPositions; ++i) {
		positions.emplace_back(0, 0);
		lengths.emplace_back(nPositions - i);
		thicknesses.emplace_back(thickness - (i - 1) * thicknessDecrement);
	}
}

void ofxOilBristle::setPosition(const ofVec2f& newPosition) {
	for (ofVec2f& pos : positions) {
		pos.set(newPosition);
	}
}

void ofxOilBristle::updatePosition(const ofVec2f& newPosition) {
	positions[0].set(newPosition);

	for (vector<ofVec2f>::size_type i = 1; i < positions.size(); ++i) {
		ofVec2f& previousPos = positions[i - 1];
		ofVec2f& pos = positions[i];
		float length = lengths[i];
		float ang = atan2(previousPos.y - pos.y, previousPos.x - pos.x);
		pos.set(previousPos.x - length * cos(ang), previousPos.y - length * sin(ang));
	}
}

void ofxOilBristle::paintOnScreen(const ofColor& col) {
	// Set the stroke color
	ofSetColor(col);

	// Paint the bristle elements
	for (vector<ofVec2f>::size_type i = 1; i < positions.size(); ++i) {
		ofSetLineWidth(thicknesses[i]);
		ofDrawLine(positions[i - 1].x, positions[i - 1].y, 0, positions[i].x, positions[i].y, 0);
	}
}
