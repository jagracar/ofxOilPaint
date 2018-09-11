#include "ofxOilBristle.h"
#include "ofMain.h"

ofxOilBristle::ofxOilBristle(const glm::vec2& position, float length) {
	// Check that the input makes sense
	if (length <= 0) {
		throw invalid_argument("There bristle length should be higher than zero.");
	}

	// Fill the positions and lengths containers
	unsigned int nElements = round(sqrt(2 * length));
	positions = vector<glm::vec2>(nElements + 1, position);

	for (unsigned int i = 0; i < nElements; ++i) {
		lengths.push_back(nElements - i);
	}
}

void ofxOilBristle::updatePosition(const glm::vec2& newPosition) {
	// Set the first element head position
	positions[0] = newPosition;

	// Set the elements tail positions
	for (unsigned int i = 0, nElements = getNElements(); i < nElements; ++i) {
		const glm::vec2& headPos = positions[i];
		glm::vec2& tailPos = positions[i + 1];
		float length = lengths[i];
		float ang = atan2(headPos.y - tailPos.y, headPos.x - tailPos.x);
		tailPos.x = headPos.x - length * cos(ang);
		tailPos.y = headPos.y - length * sin(ang);
	}
}

void ofxOilBristle::setElementsPositions(const glm::vec2& newPosition) {
	for (glm::vec2& pos : positions) {
		pos = newPosition;
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
