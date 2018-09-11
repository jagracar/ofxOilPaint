#include "ofxOilTrace.h"
#include "ofxOilBrush.h"
#include "ofMain.h"

float ofxOilTrace::NOISE_FACTOR = 0.007;

unsigned char ofxOilTrace::MIN_ALPHA = 20;

float ofxOilTrace::BRIGHTNESS_RELATIVE_CHANGE = 0.09;

unsigned int ofxOilTrace::TYPICAL_MIX_STARTING_STEP = 5;

float ofxOilTrace::MIX_STRENGTH = 0.012;

ofxOilTrace::ofxOilTrace(const glm::vec2& startingPosition, unsigned int nSteps, float speed) {
	// Check that the input makes sense
	if (nSteps == 0) {
		throw invalid_argument("The trace should have at least one step.");
	}

	// Fill the positions and alphas containers
	float initAng = ofRandom(TWO_PI);
	float noiseSeed = ofRandom(1000);
	float alphaDecrement = min(255.0 / nSteps, 25.0);
	positions.push_back(startingPosition);
	alphas.push_back(255);

	for (unsigned int i = 1; i < nSteps; ++i) {
		float ang = initAng + TWO_PI * (ofNoise(noiseSeed + NOISE_FACTOR * i) - 0.5);
		positions.emplace_back(positions[i - 1].x + speed * cos(ang), positions[i - 1].y + speed * sin(ang));
		alphas.push_back(255 - alphaDecrement * i);
	}

	// Set the average color as totally transparent
	averageColor.set(0, 0);
}

ofxOilTrace::ofxOilTrace(const vector<glm::vec2>& _positions, const vector<unsigned char>& _alphas) {
	// Check that the input makes sense
	if (_positions.size() == 0) {
		throw invalid_argument("The trace should have at least one step.");
	} else if (_positions.size() != _alphas.size()) {
		throw invalid_argument("The _positions and _alphas vectors should have the same size.");
	}

	positions = _positions;
	alphas = _alphas;
	averageColor.set(0, 0);
}

void ofxOilTrace::setBrushSize(float brushSize) {
	// Initialize the brush
	brush = ofxOilBrush(positions[0], brushSize);

	// Reset the average color
	averageColor.set(0, 0);

	// Reset the bristle containers
	bPositions.clear();
	bImgColors.clear();
	bPaintedColors.clear();
	bColors.clear();
}

void ofxOilTrace::calculateBristlePositions() {
	// Reset the container
	bPositions.clear();

	for (const glm::vec2& pos : positions) {
		// Move the brush
		brush.updatePosition(pos, false);

		// Save the bristles positions
		bPositions.push_back(brush.getBristlesPositions());
	}

	// Reset the brush to the initial position
	brush.resetPosition(positions[0]);
}

void ofxOilTrace::calculateBristleImageColors(const ofImage& img) {
	// Extract some useful information
	int width = img.getWidth();
	int height = img.getHeight();

	// Calculate the bristle positions if necessary
	if (bPositions.size() == 0) {
		calculateBristlePositions();
	}

	// Calculate the image colors at the bristles positions
	bImgColors.clear();

	for (const vector<glm::vec2>& bp : bPositions) {
		bImgColors.emplace_back();
		vector<ofColor>& bic = bImgColors.back();

		for (const glm::vec2& pos : bp) {
			// Check that the bristle is inside the image
			int x = pos.x;
			int y = pos.y;

			if (x >= 0 && x < width && y >= 0 && y < height) {
				bic.push_back(img.getColor(x, y));
			} else {
				bic.emplace_back(0, 0);
			}
		}
	}
}

void ofxOilTrace::calculateBristlePaintedColors(const ofPixels& paintedPixels, const ofColor& backgroundColor) {
	// Extract some useful information
	int width = paintedPixels.getWidth();
	int height = paintedPixels.getHeight();

	// Calculate the bristle positions if necessary
	if (bPositions.size() == 0) {
		calculateBristlePositions();
	}

	// Calculate the painted colors at the bristles positions
	bPaintedColors.clear();

	for (const vector<glm::vec2>& bp : bPositions) {
		bPaintedColors.emplace_back();
		vector<ofColor>& bpc = bPaintedColors.back();

		for (const glm::vec2& pos : bp) {
			// Check that the bristle is inside the canvas
			int x = pos.x;
			int y = pos.y;

			if (x >= 0 && x < width && y >= 0 && y < height) {
				const ofColor& color = paintedPixels.getColor(x, y);

				if (color != backgroundColor && color.a != 0) {
					bpc.push_back(color);
				} else {
					bpc.emplace_back(0, 0);
				}
			} else {
				bpc.emplace_back(0, 0);
			}
		}
	}
}

void ofxOilTrace::setAverageColor(const ofColor& color) {
	averageColor.set(color);

	// Reset the bristle colors since they are not valid anymore
	bColors.clear();
}

void ofxOilTrace::calculateAverageColor(const ofImage& img) {
	// Calculate the bristle image colors if necessary
	if (bImgColors.size() == 0) {
		calculateBristleImageColors(img);
	}

	// Calculate the trace average color
	float redSum = 0;
	float greenSum = 0;
	float blueSum = 0;
	int counter = 0;

	for (unsigned int i = 0, nSteps = getNSteps(); i < nSteps; ++i) {
		// Check that the alpha value is high enough for the average color calculation
		if (alphas[i] >= MIN_ALPHA) {
			for (const ofColor& color : bImgColors[i]) {
				if (color.a != 0) {
					redSum += color.r;
					greenSum += color.g;
					blueSum += color.b;
					++counter;
				}
			}
		}
	}

	if (counter > 0) {
		averageColor.set(redSum / counter, greenSum / counter, blueSum / counter, 255);
	} else {
		averageColor.set(0, 0);
	}
}

void ofxOilTrace::calculateBristleColors(const ofPixels& paintedPixels, const ofColor& backgroundColor) {
	// Get some useful information
	unsigned int nSteps = getNSteps();
	unsigned int nBristles = getNBristles();

	// Calculate the bristle painted colors if necessary
	if (bPaintedColors.size() == 0) {
		calculateBristlePaintedColors(paintedPixels, backgroundColor);
	}

	// Calculate the starting colors for each bristle
	vector<ofColor> startingColors = vector<ofColor>(nBristles);
	float noiseSeed = ofRandom(1000);
	float averageHue, averageSaturation, averageBrightness;
	averageColor.getHsb(averageHue, averageSaturation, averageBrightness);

	for (unsigned int bristle = 0; bristle < nBristles; ++bristle) {
		// Add some brightness changes to make it more realistic
		float deltaBrightness = BRIGHTNESS_RELATIVE_CHANGE * averageBrightness
				* (ofNoise(noiseSeed + 0.4 * bristle) - 0.5);
		startingColors[bristle].setHsb(averageHue, averageSaturation, averageBrightness + deltaBrightness);
	}

	// Use the bristle starting colors until the step where the mixing starts
	unsigned int mixStartingStep = ofClamp(TYPICAL_MIX_STARTING_STEP, 1, nSteps);
	bColors = vector<vector<ofColor>>(mixStartingStep, startingColors);

	// Mix the previous step colors with the already painted colors
	vector<float> redPrevious;
	vector<float> greenPrevious;
	vector<float> bluePrevious;

	for (const ofColor& color : startingColors) {
		redPrevious.push_back(color.r);
		greenPrevious.push_back(color.g);
		bluePrevious.push_back(color.b);
	}

	float f = 1 - MIX_STRENGTH;

	for (unsigned int i = mixStartingStep; i < nSteps; ++i) {
		// Copy the previous step colors
		bColors.push_back(bColors.back());

		// Check that the alpha value is high enough for mixing
		if (alphas[i] >= MIN_ALPHA) {
			// Calculate the bristle colors for this step
			vector<ofColor>& bc = bColors.back();
			const vector<ofColor>& bpc = bPaintedColors[i];

			if (bpc.size() > 0) {
				for (unsigned int bristle = 0; bristle < nBristles; ++bristle) {
					const ofColor& paintedColor = bpc[bristle];

					if (paintedColor.a != 0) {
						float redMix = f * redPrevious[bristle] + MIX_STRENGTH * paintedColor.r;
						float greenMix = f * greenPrevious[bristle] + MIX_STRENGTH * paintedColor.g;
						float blueMix = f * bluePrevious[bristle] + MIX_STRENGTH * paintedColor.b;
						redPrevious[bristle] = redMix;
						greenPrevious[bristle] = greenMix;
						bluePrevious[bristle] = blueMix;
						bc[bristle].set(redMix, greenMix, blueMix);
					}
				}
			}
		}
	}
}

void ofxOilTrace::paint() {
	// Check that the bristle colors have been calculated before running this method
	if (bColors.size() == 0) {
		throw logic_error("Please, run calculateBristleColors method before paint.");
	}

	for (unsigned int i = 0, nSteps = getNSteps(); i < nSteps; ++i) {
		// Move the brush
		brush.updatePosition(positions[i], true);

		// Paint the brush
		brush.paint(bColors[i], alphas[i]);
	}

	// Reset the brush to the initial position
	brush.resetPosition(positions[0]);
}

void ofxOilTrace::paint(ofFbo& canvasBuffer) {
	// Check that the bristle colors have been calculated before running this method
	if (bColors.size() == 0) {
		throw logic_error("Please, run calculateBristleColors method before paint.");
	}

	for (unsigned int i = 0, nSteps = getNSteps(); i < nSteps; ++i) {
		// Move the brush
		brush.updatePosition(positions[i], true);

		// Paint the brush
		brush.paint(bColors[i], alphas[i]);

		// Paint the trace on the canvas only if alpha is high enough
		if (alphas[i] >= MIN_ALPHA) {
			canvasBuffer.begin();
			brush.paint(bColors[i], 255);
			canvasBuffer.end();
		}
	}

	// Reset the brush to the initial position
	brush.resetPosition(positions[0]);
}

void ofxOilTrace::paintStep(unsigned int step) {
	// Check that the bristle colors have been calculated before running this method
	if (bColors.size() == 0) {
		throw logic_error("Please, run calculateBristleColors method before paint.");
	}

	// Check that it makes sense to paint the given step
	if (step < getNSteps()) {
		// Move the brush
		brush.updatePosition(positions[step], true);

		// Paint the brush
		brush.paint(bColors[step], alphas[step]);

		// Reset the brush to the initial position if we are at the last trajectory step
		if (step == getNSteps() - 1) {
			brush.resetPosition(positions[0]);
		}
	}
}

void ofxOilTrace::paintStep(unsigned int step, ofFbo& canvasBuffer) {
	// Check that the bristle colors have been calculated before running this method
	if (bColors.size() == 0) {
		throw logic_error("Please, run calculateBristleColors method before paint.");
	}

	// Check that it makes sense to paint the given step
	if (step < getNSteps()) {
		// Move the brush
		brush.updatePosition(positions[step], true);

		// Paint the brush
		brush.paint(bColors[step], alphas[step]);

		// Paint the trace on the canvas only if alpha is high enough
		if (alphas[step] >= MIN_ALPHA) {
			canvasBuffer.begin();
			brush.paint(bColors[step], 255);
			canvasBuffer.end();
		}

		// Reset the brush to the initial position if we are at the last trajectory step
		if (step == getNSteps() - 1) {
			brush.resetPosition(positions[0]);
		}
	}
}

unsigned int ofxOilTrace::getNSteps() const {
	return positions.size();
}

const vector<glm::vec2>& ofxOilTrace::getTrajectoryPositions() const {
	return positions;
}

const vector<unsigned char>& ofxOilTrace::getTrajectoryAphas() const {
	return alphas;
}

const ofColor& ofxOilTrace::getAverageColor() const {
	return averageColor;
}

unsigned int ofxOilTrace::getNBristles() const {
	return brush.getNBristles();
}

const vector<vector<glm::vec2>>& ofxOilTrace::getBristlePositions() const {
	return bPositions;
}

const vector<vector<ofColor>>& ofxOilTrace::getBristleImageColors() const {
	return bImgColors;
}

const vector<vector<ofColor>>& ofxOilTrace::getBristlePaintedColors() const {
	return bPaintedColors;
}

const vector<vector<ofColor>>& ofxOilTrace::getBristleColors() const {
	return bColors;
}
