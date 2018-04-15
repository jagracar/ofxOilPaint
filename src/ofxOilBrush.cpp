#include "ofxOilBrush.h"
#include "ofxOilConstants.h"
#include "ofxOilBristle.h"
#include "ofMain.h"

ofxOilBrush::ofxOilBrush(float size) {
	position = ofVec2f();
	nBristles = floor(size * ofRandom(1.6, 1.9));
	averagePosition = position;
	noiseSeed = ofRandom(1000);
	updatesCounter = 0;
	bristleHorizontalNoise = min(0.3f * size, MAX_BRISTLE_HORIZONTAL_NOISE);

	// Populate the bristles arrays
	float bristleLength = min(size, MAX_BRISTLE_LENGTH);
	int nElements = round(sqrt(2 * bristleLength));
	float bristleThickness = min(0.8f * bristleLength, MAX_BRISTLE_THICKNESS);

	for (int bristle = 0; bristle < nBristles; ++bristle) {
		bristles.emplace_back(nElements, bristleThickness);
		bOffsets.emplace_back(size * ofRandom(-0.5, 0.5), BRISTLE_VERTICAL_NOISE * ofRandom(-0.5, 0.5));
		bPositions.emplace_back(0, 0);
	}
}

void ofxOilBrush::init(const ofVec2f& newPosition) {
	position.set(newPosition);
	positionsHistory.clear();
	averagePosition.set(position);
	updatesCounter = 0;
}

void ofxOilBrush::update(const ofVec2f& newPosition, bool updateBristleElements) {
	// Update the position
	position.set(newPosition);

	// Add the new position to the positions history
	int historySize = positionsHistory.size();

	if (historySize < POSITIONS_FOR_AVERAGE) {
		positionsHistory.emplace_back(newPosition.x, newPosition.y);
		++historySize;
	} else {
		positionsHistory[updatesCounter % POSITIONS_FOR_AVERAGE].set(newPosition);
	}

	// Calculate the new average position
	ofVec2f newAveragePosition = ofVec2f();

	for (const ofVec2f& pos : positionsHistory) {
		newAveragePosition += pos;
	}

	newAveragePosition /= historySize;

	// Calculate the direction angle
	float directionAngle = HALF_PI
			+ atan2(newAveragePosition.y - averagePosition.y, newAveragePosition.x - averagePosition.x);

	// Update the average position
	averagePosition.set(newAveragePosition);

	// Update the bristles positions array
	updateBristlePositions(directionAngle);

	// Update the bristles elements to their new positions
	if (updateBristleElements) {
		if (historySize == POSITIONS_FOR_AVERAGE) {
			for (int bristle = 0; bristle < nBristles; ++bristle) {
				bristles[bristle].updatePosition(bPositions[bristle]);
			}
		} else if (historySize == POSITIONS_FOR_AVERAGE - 1) {
			for (int bristle = 0; bristle < nBristles; ++bristle) {
				bristles[bristle].setPosition(bPositions[bristle]);
			}
		}
	}

	// Increment the update counter
	updatesCounter++;
}

void ofxOilBrush::updateBristlePositions(float directionAngle) {
	if (positionsHistory.size() >= POSITIONS_FOR_AVERAGE - 1) {
		// This saves some calculations
		float cosAng = cos(directionAngle);
		float sinAng = sin(directionAngle);
		float noisePos = noiseSeed + NOISE_SPEED_FACTOR * updatesCounter;

		for (int bristle = 0; bristle < nBristles; ++bristle) {
			// Add some horizontal noise to make it look more realistic
			const ofVec2f& offset = bOffsets[bristle];
			float x = offset.x + bristleHorizontalNoise * (ofNoise(noisePos + 0.1 * bristle) - 0.5);
			float y = offset.y;

			// Rotate the offset vector and add it to the position
			bPositions[bristle].set(position.x + (x * cosAng - y * sinAng), position.y + (x * sinAng + y * cosAng));
		}
	}
}

void ofxOilBrush::paint(const ofColor& color) const {
	if (positionsHistory.size() == POSITIONS_FOR_AVERAGE) {
		for (const ofxOilBristle& bristle : bristles) {
			bristle.paint(color);
		}
	}
}

void ofxOilBrush::paint(const vector<ofColor>& colors, unsigned char alpha) const {
	if (positionsHistory.size() == POSITIONS_FOR_AVERAGE) {
		for (int bristle = 0; bristle < nBristles; ++bristle) {
			bristles[bristle].paint(ofColor(colors[bristle], alpha));
		}
	}
}

int ofxOilBrush::getNBristles() const {
	return nBristles;
}

vector<ofVec2f> ofxOilBrush::getBristlesPositions() const {
	return positionsHistory.size() == POSITIONS_FOR_AVERAGE ? bPositions : vector<ofVec2f>();
}
