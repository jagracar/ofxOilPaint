#include "ofxOilBrush.h"
#include "ofxOilBristle.h"
#include "ofMain.h"

float ofxOilBrush::MAX_BRISTLE_LENGTH = 15;

float ofxOilBrush::MAX_BRISTLE_THICKNESS = 5;

float ofxOilBrush::MAX_BRISTLE_HORIZONTAL_NOISE = 4;

float ofxOilBrush::BRISTLE_VERTICAL_NOISE = 8;

float ofxOilBrush::NOISE_SPEED_FACTOR = 0.04;

unsigned int ofxOilBrush::POSITIONS_FOR_AVERAGE = 4;

ofxOilBrush::ofxOilBrush(const ofVec2f& _position, float _size) :
		position(_position), size(_size) {
	// Calculate some of the bristles properties
	bristlesLength = min(size, MAX_BRISTLE_LENGTH);
	bristlesThickness = min(0.8f * bristlesLength, MAX_BRISTLE_THICKNESS);
	bristlesHorizontalNoise = min(0.3f * size, MAX_BRISTLE_HORIZONTAL_NOISE);
	bristlesHorizontalNoiseSeed = ofRandom(1000);

	// Initialize the bristles offsets and positions containers with default values
	unsigned int nBristles = floor(size * ofRandom(1.6, 1.9));
	bOffsets = vector<ofVec2f>(nBristles);
	bPositions = vector<ofVec2f>(nBristles);

	// Randomize the bristle offset positions
	for (ofVec2f& offset : bOffsets) {
		offset.set(size * ofRandom(-0.5, 0.5), BRISTLE_VERTICAL_NOISE * ofRandom(-0.5, 0.5));
	}

	// Initialize the variables used to calculate the brush average position
	averagePosition.set(position);
	positionsHistory.push_back(position);
	updatesCounter = 0;
}

void ofxOilBrush::resetPosition(const ofVec2f& newPosition) {
	// Reset the brush position
	position.set(newPosition);

	// Reset the variables used to calculate the brush average position
	averagePosition.set(position);
	positionsHistory.clear();
	positionsHistory.push_back(position);
	updatesCounter = 0;
}

void ofxOilBrush::updatePosition(const ofVec2f& newPosition, bool updateBristlesElements) {
	// Update the brush position
	position.set(newPosition);

	// Increment the updates counter
	updatesCounter++;

	// Add the new position to the positions history
	if (positionsHistory.size() < POSITIONS_FOR_AVERAGE) {
		positionsHistory.push_back(position);
	} else {
		positionsHistory[updatesCounter % POSITIONS_FOR_AVERAGE].set(position);
	}

	// Update the average position
	ofVec2f prevAveragePosition = averagePosition;
	averagePosition.set(0, 0);
	int counter = 0;

	for (const ofVec2f& pos : positionsHistory) {
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
			const ofVec2f& offset = bOffsets[i];
			float x = offset.x + bristlesHorizontalNoise * (ofNoise(noisePos + 0.1 * i) - 0.5);
			float y = offset.y;

			// Rotate the offset and add it to the brush central position
			bPositions[i].set(position.x + (x * cosAng - y * sinAng), position.y + (x * sinAng + y * cosAng));
		}

		// Update the bristles elements to their new positions if necessary
		if (updateBristlesElements) {
			// Check if the bristles container should be initialized
			if (bristles.size() == 0) {
				bristles = vector<ofxOilBristle>(nBristles, ofxOilBristle(ofVec2f(), bristlesLength));
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
		for (const ofxOilBristle& bristle : bristles) {
			bristle.paint(color, bristlesThickness);
		}
	}
}

void ofxOilBrush::paint(const vector<ofColor>& colors, unsigned char alpha) const {
	// Check that the input makes sense
	if (colors.size() != getNBristles()) {
		throw invalid_argument("There should be one color for each bristle in the brush.");
	}

	if (positionsHistory.size() == POSITIONS_FOR_AVERAGE) {
		for (unsigned int i = 0, nBristles = getNBristles(); i < nBristles; ++i) {
			bristles[i].paint(ofColor(colors[i], alpha), bristlesThickness);
		}
	}
}

unsigned int ofxOilBrush::getNBristles() const {
	return bOffsets.size();
}

const vector<ofVec2f> ofxOilBrush::getBristlesPositions() const {
	return positionsHistory.size() == POSITIONS_FOR_AVERAGE ? bPositions : vector<ofVec2f>();
}
