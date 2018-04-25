#include "ofxOilBristle.h"
#include "ofMain.h"

ofxOilBristle::ofxOilBristle(const ofVec2f& position, float length) {
	// Check that the input makes sense
	if (length <= 0) {
		throw invalid_argument("There bristle length should be higher than zero.");
	}

	// Fill the positions and lengths containers
	unsigned int nElements = round(sqrt(2 * length));
	positions = vector<ofVec2f>(nElements + 1, position);

	for (unsigned int i = 0; i < nElements; ++i) {
		lengths.push_back(nElements - i);
	}
}

void ofxOilBristle::updatePosition(const ofVec2f& newPosition) {
	// Set the first element head position
	positions[0].set(newPosition);

	// Set the elements tail positions
	for (unsigned int i = 0, nElements = getNElements(); i < nElements; ++i) {
		const ofVec2f& headPos = positions[i];
		ofVec2f& tailPos = positions[i + 1];
		float length = lengths[i];
		float ang = atan2(headPos.y - tailPos.y, headPos.x - tailPos.x);
		tailPos.set(headPos.x - length * cos(ang), headPos.y - length * sin(ang));
	}
}

void ofxOilBristle::setElementsPositions(const ofVec2f& newPosition) {
	for (ofVec2f& pos : positions) {
		pos.set(newPosition);
	}
}

void ofxOilBristle::setElementsLengths(const vector<float>& newLengths) {
	// Check that the input makes sense
	if (newLengths.size() != getNElements()) {
		throw invalid_argument("The newLengths vector should contain as many values as elements in the bristle");
	}

	lengths = newLengths;
}

void ofxOilBristle::paint(const ofColor& color, float thickness) const {
	// Set the stroke color
	ofSetColor(color);

	// Paint the bristle elements
	unsigned int nElements = getNElements();
	float deltaThickness = thickness / nElements;

	for (unsigned int i = 0; i < nElements; ++i) {
		ofSetLineWidth(thickness - i * deltaThickness);
		ofDrawLine(positions[i].x, positions[i].y, 0, positions[i + 1].x, positions[i + 1].y, 0);
	}
}

unsigned int ofxOilBristle::getNElements() const {
	return lengths.size();
}
