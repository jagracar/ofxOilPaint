#include "ofxOilBrush.h"
#include "ofMain.h"
#include "ofxOilConstants.h"

ofxOilBrush::ofxOilBrush(float size) {
	position = ofVec2f();
	nBristles = floor(size * ofRandom(1.6, 1.9));
	positionsHistory = vector<ofVec2f>(POSITIONS_FOR_AVERAGE);
	averagePosition = position;
	noiseSeed = ofRandom(1000);
	updatesCounter = 0;
	bristleHorizontalNoise = MIN(0.3 * size, MAX_BRISTLE_HORIZONTAL_NOISE);

	// Populate the bristles arrays
	float bristleLength = MIN(size, MAX_BRISTLE_LENGTH);
	int nElements = round(sqrt(2 * bristleLength));
	float bristleThickness = MIN(0.8 * bristleLength, MAX_BRISTLE_THICKNESS);

	for (int bristle = 0; bristle < nBristles; ++bristle) {
		bristles.emplace_back(nElements, bristleThickness);
		bOffsets.emplace_back(size * ofRandom(-0.5, 0.5), BRISTLE_VERTICAL_NOISE * ofRandom(-0.5, 0.5));
		bPositions.emplace_back(0, 0);
	}
}
