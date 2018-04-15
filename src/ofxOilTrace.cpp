#include "ofxOilTrace.h"
#include "ofMain.h"
#include "ofxOilConstants.h"
#include "ofxOilBrush.h"

ofxOilTrace::ofxOilTrace(const ofVec2f& position, int nSteps, float speed) {
	// Fill the trace vectors
	float initAng = ofRandom(TWO_PI);
	float noiseSeed = ofRandom(1000);
	float alphaDecrement = min(255.0f / nSteps, 25.0f);
	positions.emplace_back(position);
	colors.emplace_back(vector<ofColor>());
	alphas.emplace_back(255);

	for (int step = 1; step < nSteps; ++step) {
		float ang = initAng + TWO_PI * (ofNoise(noiseSeed + NOISE_FACTOR * step) - 0.5);
		positions.emplace_back(positions[step - 1].x + speed * cos(ang), positions[step - 1].y + speed * sin(ang));
		colors.emplace_back(vector<ofColor>());
		alphas.emplace_back(ofClamp(255 - alphaDecrement * step, 0, 255));
	}
}

void ofxOilTrace::setBrushSize(float brushSize) {
	brush = ofxOilBrush(brushSize);
	brush.init(positions[0]);
}

ofColor ofxOilTrace::calculateAverageColor(const ofImage& img) {
	// Extract some useful information
	int width = img.getWidth();
	int height = img.getHeight();
	int nSteps = positions.size();

	// Calculate the trace average color
	int redAverage = 0;
	int greenAverage = 0;
	int blueAverage = 0;
	int counter = 0;

	for (int step = 0; step < nSteps; ++step) {
		// Move the brush and get the bristles positions
		brush.update(positions[step], false);
		const vector<ofVec2f>& bristlesPositions = brush.getBristlesPositions();

		// Check that the alpha value is high enough for the average color calculation
		if (alphas[step] >= MIN_ALPHA) {
			for (const ofVec2f& pos : bristlesPositions) {
				// Check that the bristle is inside the image
				int x = pos.x;
				int y = pos.y;

				if (x >= 0 && x < width && y >= 0 && y < height) {
					// Get the image color
					ofColor imgColor = img.getColor(x, y);

					// Add the image color to the average
					redAverage += imgColor.r;
					greenAverage += imgColor.g;
					blueAverage += imgColor.b;

					// Increment the counter
					counter++;
				}
			}
		}
	}

	if (counter > 0) {
		redAverage /= counter;
		greenAverage /= counter;
		blueAverage /= counter;
	}

	// Reset the brush to the initial position
	brush.init(positions[0]);

	return ofColor(redAverage, greenAverage, blueAverage);
}

void ofxOilTrace::calculateColors(const ofColor& col) {
	int nBristles = brush.getNBristles();
	float colHue = col.getHue();
	float colSaturation = col.getSaturation();
	float colBrightness = col.getBrightness();
	float noiseSeed = ofRandom(1000);

	for (vector<ofColor>& bColors : colors) {
		// Clean the bristle colors
		bColors.clear();

		// Calculate the colors
		for (int bristle = 0; bristle < nBristles; ++bristle) {
			// Add some brightness changes to make it more realistic
			float deltaBrightness = BRIGHTNESS_RELATIVE_CHANGE * colBrightness
					* (ofNoise(noiseSeed + 0.4 * bristle) - 0.5);
			bColors.emplace_back(
					ofColor::fromHsb(colHue, colSaturation, ofClamp(colBrightness + deltaBrightness, 0, 255), 255));
		}
	}
}

void ofxOilTrace::paint() {
	// Check that it makes sense to paint the trace
	int nSteps = positions.size();

	if (nSteps > 0 && colors[0].size() > 0) {
		for (int step = 0; step < nSteps; ++step) {
			// Move the brush
			brush.update(positions[step], true);

			// Paint the brush
			brush.paint(colors[step], alphas[step]);
		}

		// Reset the brush to the initial position
		brush.init(positions[0]);
	}
}

void ofxOilTrace::paintStep(int step) {
	// Check that it makes sense to paint the given step
	int nSteps = positions.size();

	if (step < nSteps && colors[step].size() > 0) {
		// Move the brush
		brush.update(positions[step], true);

		// Paint the brush
		brush.paint(colors[step], alphas[step]);

		// Check if we are at the last step
		if (step == nSteps - 1) {
			// Reset the brush to the initial position
			brush.init(positions[0]);
		}
	}
}

int ofxOilTrace::getNSteps() {
	return positions.size();
}
