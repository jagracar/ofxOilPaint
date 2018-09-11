#include "ofxOilBrush.h"
#include "ofxOilBristle.h"
#include "ofMain.h"

float ofxOilBrush::MAX_BRISTLE_LENGTH = 15;

float ofxOilBrush::MAX_BRISTLE_THICKNESS = 5;

float ofxOilBrush::MAX_BRISTLE_HORIZONTAL_NOISE = 4;

float ofxOilBrush::BRISTLE_VERTICAL_NOISE = 8;

float ofxOilBrush::NOISE_SPEED_FACTOR = 0.04;

unsigned int ofxOilBrush::POSITIONS_FOR_AVERAGE = 4;

ofxOilBrush::ofxOilBrush(const glm::vec2& _position, float _size) :
		position(_position), size(_size) {
	// Calculate some of the bristles properties
	bristlesLength = min(size, MAX_BRISTLE_LENGTH);
	bristlesThickness = min(0.8f * bristlesLength, MAX_BRISTLE_THICKNESS);
	bristlesHorizontalNoise = min(0.3f * size, MAX_BRISTLE_HORIZONTAL_NOISE);
	bristlesHorizontalNoiseSeed = ofRandom(1000);

	// Initialize the bristles offsets and positions containers with default values
	unsigned int nBristles = floor(size * ofRandom(1.6, 1.9));
	bOffsets = vector<glm::vec2>(nBristles);
	bPositions = vector<glm::vec2>(nBristles);

	// Randomize the bristle offset positions
	for (glm::vec2& offset : bOffsets) {
		offset.x = size * ofRandom(-0.5, 0.5);
		offset.y = BRISTLE_VERTICAL_NOISE * ofRandom(-0.5, 0.5);
	}

	// Initialize the variables used to calculate the brush average position
	averagePosition = position;
	positionsHistory.push_back(position);
	updatesCounter = 0;
}

void ofxOilBrush::resetPosition(const glm::vec2& newPosition) {
	// Reset the brush position
	position = newPosition;

	// Reset the variables used to calculate the brush average position
	averagePosition = position;
	positionsHistory.clear();
	positionsHistory.push_back(position);
	updatesCounter = 0;
}

void ofxOilBrush::updatePosition(const glm::vec2& newPosition, bool updateBristlesElements) {
	// Update the brush position
	position = newPosition;

	// Increment the updates counter
	updatesCounter++;

	// Add the new position to the positions history
	if (positionsHistory.size() < POSITIONS_FOR_AVERAGE) {
		positionsHistory.push_back(position);
	} else {
		positionsHistory[updatesCounter % POSITIONS_FOR_AVERAGE] = position;
	}

	// Update the average position
	glm::vec2 prevAveragePosition = averagePosition;
	averagePosition.x = 0;
	averagePosition.y = 0;
	int counter = 0;

	for (const glm::vec2& pos : positionsHistory) {
		averagePosition += pos;
		++counter;
	}

	averagePosition /= counter;

	// Update the bristles containers only if the average position is stable or is close to be stable
	if (positionsHistory.size() >= POSITIONS_FOR_AVERAGE - 1) {
		// Calculate the direction angle
		float directionAngle = HALF_PI
				+ atan2(averagePosition.y - prevAveragePosition.y, averagePosition.x - prevAveragePosition.x);

		// Update the bristles positions
		unsigned int nBristles = getNBristles();
		float cosAng = cos(directionAngle);
		float sinAng = sin(directionAngle);
		float noisePos = bristlesHorizontalNoiseSeed + NOISE_SPEED_FACTOR * updatesCounter;

		for (unsigned int i = 0; i < nBristles; ++i) {
			// Add some horizontal noise to the offset to make it look more realistic
			const glm::vec2& offset = bOffsets[i];
			float x = offset.x + bristlesHorizontalNoise * (ofNoise(noisePos + 0.1 * i) - 0.5);
			float y = offset.y;

			// Rotate the offset and add it to the brush central position
			bPositions[i].x = position.x + (x * cosAng - y * sinAng);
			bPositions[i].y = position.y + (x * sinAng + y * cosAng);
		}

		// Update the bristles elements to their new positions if necessary
		if (updateBristlesElements) {
			// Check if the bristles container should be initialized
			if (bristles.size() == 0) {
				bristles = vector<ofxOilBristle>(nBristles, ofxOilBristle(glm::vec2(), bristlesLength));
			}

			if (positionsHistory.size() == POSITIONS_FOR_AVERAGE - 1) {
				for (unsigned int i = 0; i < nBristles; ++i) {
					bristles[i].setElementsPositions(bPositions[i]);
				}
			} else {
				for (unsigned int i = 0; i < nBristles; ++i) {
					bristles[i].updatePosition(bPositions[i]);
				}
			}
		}
	}
}

void ofxOilBrush::paint(const ofColor& color) const {
	if (positionsHistory.size() == POSITIONS_FOR_AVERAGE) {
		ofPushStyle();

		for (const ofxOilBristle& bristle : bristles) {
			bristle.paint(color, bristlesThickness);
		}

		ofPopStyle();
	}
}

void ofxOilBrush::paint(const vector<ofColor>& colors, unsigned char alpha) const {
	// Check that the input makes sense
	if (colors.size() != getNBristles()) {
		throw invalid_argument("There should be one color for each bristle in the brush.");
	}

	if (positionsHistory.size() == POSITIONS_FOR_AVERAGE) {
		ofPushStyle();

		for (unsigned int i = 0, nBristles = getNBristles(); i < nBristles; ++i) {
			bristles[i].paint(ofColor(colors[i], alpha), bristlesThickness);
		}

		ofPopStyle();
	}
}

unsigned int ofxOilBrush::getNBristles() const {
	return bOffsets.size();
}

const vector<glm::vec2> ofxOilBrush::getBristlesPositions() const {
	return positionsHistory.size() == POSITIONS_FOR_AVERAGE ? bPositions : vector<glm::vec2>();
}
