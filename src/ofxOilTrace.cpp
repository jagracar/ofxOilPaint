#include "ofxOilTrace.h"
#include "ofxOilBrush.h"
#include "ofMain.h"

float ofxOilTrace::NOISE_FACTOR = 0.007;

unsigned char ofxOilTrace::MIN_ALPHA = 20;

float ofxOilTrace::BRIGHTNESS_RELATIVE_CHANGE = 0.09;

unsigned int ofxOilTrace::TYPICAL_MIX_STARTING_STEP = 5;

float ofxOilTrace::MIX_STRENGTH = 0.012;

array<int, 3> ofxOilTrace::MAX_COLOR_DIFFERENCE = { 40, 40, 40 };

float ofxOilTrace::MAX_VISITS_FRACTION_IN_TRAJECTORY = 0.35;

float ofxOilTrace::MIN_INSIDE_FRACTION_IN_TRAJECTORY = 0.4;

float ofxOilTrace::MAX_SIMILAR_COLOR_FRACTION_IN_TRAJECTORY = 0.6;

float ofxOilTrace::MAX_COLOR_STDEV_IN_TRAJECTORY = 45;

float ofxOilTrace::MIN_INSIDE_FRACTION = 0.7;

float ofxOilTrace::MAX_SIMILAR_COLOR_FRACTION = 0.8; // 0.8 - 0.85 - 0.5

float ofxOilTrace::MAX_PAINTED_FRACTION = 0.65;

float ofxOilTrace::MIN_COLOR_IMPROVEMENT_FACTOR = 0.6;

float ofxOilTrace::BIG_WELL_PAINTED_IMPROVEMENT_FRACTION = 0.3; // 0.3 - 0.35 - 0.4

float ofxOilTrace::MIN_BAD_PAINTED_REDUCTION_FRACTION = 0.45; // 0.45 - 0.3 - 0.45

float ofxOilTrace::MAX_WELL_PAINTED_DESTRUCTION_FRACTION = 0.4; // 0.4 - 0.55 - 0.4

ofxOilTrace::ofxOilTrace(const ofVec2f& startingPosition, unsigned int nSteps, float speed) {
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

	// Set the average color
	averageColor.set(0, 0);
}

ofxOilTrace::ofxOilTrace(const vector<ofVec2f>& _positions, const vector<unsigned char>& _alphas) {
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

bool ofxOilTrace::alreadyVisitedTrajectory(const ofPixels& visitedPixels) const {
	// Extract some useful information
	int width = visitedPixels.getWidth();
	int height = visitedPixels.getHeight();

	// Obtain some pixel statistics along the trajectory
	int insideCounter = 0;
	int visitedCounter = 0;

	for (unsigned int i = ofxOilBrush::POSITIONS_FOR_AVERAGE, nSteps = getNSteps(); i < nSteps; ++i) {
		// Check that the alpha value is high enough
		if (alphas[i] >= MIN_ALPHA) {
			// Check that the position is inside the image
			const ofVec2f& pos = positions[i];
			int x = pos.x;
			int y = pos.y;

			if (x >= 0 && x < width && y >= 0 && y < height) {
				++insideCounter;

				if (visitedPixels.getColor(x, y) == 0) {
					++visitedCounter;
				}
			}
		}
	}

	return visitedCounter > MAX_VISITS_FRACTION_IN_TRAJECTORY * insideCounter;
}

bool ofxOilTrace::hasValidTrajectory(const ofImage& img, const ofPixels& paintedPixels,
		const ofColor& backgroundColor) const {
	// Extract some useful information
	int width = img.getWidth();
	int height = img.getHeight();

	// Obtain some pixel statistics along the trajectory
	int insideCounter = 0;
	int outsideCounter = 0;
	int similarColorCounter = 0;
	float imgRedSum = 0;
	float imgRedSqSum = 0;
	float imgGreenSum = 0;
	float imgGreenSqSum = 0;
	float imgBlueSum = 0;
	float imgBlueSqSum = 0;

	for (unsigned int i = ofxOilBrush::POSITIONS_FOR_AVERAGE, nSteps = getNSteps(); i < nSteps; ++i) {
		// Check that the alpha value is high enough
		if (alphas[i] >= MIN_ALPHA) {
			// Check that the position is inside the image
			const ofVec2f& pos = positions[i];
			int x = pos.x;
			int y = pos.y;

			if (x >= 0 && x < width && y >= 0 && y < height) {
				++insideCounter;

				// Get the image color and the painted color at the trajectory position
				const ofColor& imgColor = img.getColor(x, y);
				const ofColor& paintedColor = paintedPixels.getColor(x, y);

				// Check if the two colors are similar
				if (paintedColor != backgroundColor && abs(imgColor.r - paintedColor.r) < MAX_COLOR_DIFFERENCE[0]
						&& abs(imgColor.g - paintedColor.g) < MAX_COLOR_DIFFERENCE[1]
						&& abs(imgColor.b - paintedColor.b) < MAX_COLOR_DIFFERENCE[2]) {
					++similarColorCounter;
				}

				// Extract the pixel color properties
				int imgRed = imgColor.r;
				int imgGreen = imgColor.g;
				int imgBlue = imgColor.b;
				imgRedSum += imgRed;
				imgRedSqSum += imgRed * imgRed;
				imgGreenSum += imgGreen;
				imgGreenSqSum += imgGreen * imgGreen;
				imgBlueSum += imgBlue;
				imgBlueSqSum += imgBlue * imgBlue;
			} else {
				++outsideCounter;
			}
		}
	}

	// Obtain the image colors standard deviation along the trajectory
	float imgRedStDevSq = 0;
	float imgGreenStDevSq = 0;
	float imgBlueStDevSq = 0;

	if (insideCounter > 1) {
		imgRedStDevSq = (imgRedSqSum - imgRedSum * imgRedSum / insideCounter) / (insideCounter - 1);
		imgGreenStDevSq = (imgGreenSqSum - imgGreenSum * imgGreenSum / insideCounter) / (insideCounter - 1);
		imgBlueStDevSq = (imgBlueSqSum - imgBlueSum * imgBlueSum / insideCounter) / (insideCounter - 1);
	}

	// Check if we have a valid trajectory
	bool insideCanvas = insideCounter >= MIN_INSIDE_FRACTION_IN_TRAJECTORY * (insideCounter + outsideCounter);
	bool badPainted = similarColorCounter <= MAX_SIMILAR_COLOR_FRACTION_IN_TRAJECTORY * insideCounter;
	float maxSqDevSq = pow(MAX_COLOR_STDEV_IN_TRAJECTORY, 2);
	bool smallColorChange = imgRedStDevSq < maxSqDevSq && imgGreenStDevSq < maxSqDevSq && imgBlueStDevSq < maxSqDevSq;

	return insideCanvas && badPainted && smallColorChange;
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

	for (const ofVec2f& pos : positions) {
		// Move the brush
		brush.updatePosition(pos, false);

		// Save the bristles positions
		bPositions.push_back(brush.getBristlesPositions());
	}

	// Reset the brush to the initial position
	brush.resetPosition(positions[0]);
}

void ofxOilTrace::calculateImgColors(const ofImage& img) {
	// Extract some useful information
	int width = img.getWidth();
	int height = img.getHeight();

	// Calculate the bristle positions if necessary
	if (bPositions.size() == 0) {
		calculateBristlePositions();
	}

	// Calculate the image colors at the bristles positions
	bImgColors.clear();

	for (const vector<ofVec2f>& bp : bPositions) {
		bImgColors.emplace_back();
		vector<ofColor>& bic = bImgColors.back();

		for (const ofVec2f& pos : bp) {
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

void ofxOilTrace::calculatePaintedColors(const ofPixels& paintedPixels, const ofColor& backgroundColor) {
	// Extract some useful information
	int width = paintedPixels.getWidth();
	int height = paintedPixels.getHeight();

	// Calculate the bristle positions if necessary
	if (bPositions.size() == 0) {
		calculateBristlePositions();
	}

	// Calculate the painted colors at the bristles positions
	bPaintedColors.clear();

	for (const vector<ofVec2f>& bp : bPositions) {
		bPaintedColors.emplace_back();
		vector<ofColor>& bpc = bPaintedColors.back();

		for (const ofVec2f& pos : bp) {
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
		calculateImgColors(img);
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
	// Extract some useful information
	unsigned int nSteps = getNSteps();
	unsigned int nBristles = brush.getNBristles();

	// Calculate the bristle painted colors if necessary
	if (bPaintedColors.size() == 0) {
		calculatePaintedColors(paintedPixels, backgroundColor);
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

bool ofxOilTrace::improvesPainting(const ofImage& img) {
	// Check that the bristle colors have been calculated before running this method
	if (bColors.size() == 0) {
		throw logic_error("Please, run calculateBristleColors method before improvesPainting.");
	}

	// Calculate the bristle image colors if necessary
	if (bImgColors.size() == 0) {
		calculateImgColors(img);
	}

	// Obtain some trace statistics
	int insideCounter = 0;
	int outsideCounter = 0;
	int paintedCounter = 0;
	int similarColorCounter = 0;
	int wellPaintedCounter = 0;
	int destroyedSimilarColorCounter = 0;
	int colorImprovement = 0;

	for (unsigned int i = 0, nSteps = getNSteps(); i < nSteps; ++i) {
		// Check that the alpha value is high enough
		if (alphas[i] >= MIN_ALPHA) {
			// Get the bristles image colors and painted colors for this step
			const vector<ofColor>& bic = bImgColors[i];
			const vector<ofColor>& bpc = bPaintedColors[i];
			const vector<ofColor>& bc = bColors[i];

			// Make sure that the containers are not empty
			if (bic.size() > 0) {
				for (unsigned int bristle = 0, nBristles = brush.getNBristles(); bristle < nBristles; ++bristle) {
					// Get the image color and the painted color at the bristle position
					const ofColor& imgColor = bic[bristle];
					const ofColor& paintedColor = bpc[bristle];
					const ofColor& bristleColor = bc[bristle];

					// Check that the bristle is inside the image
					if (imgColor.a != 0) {
						++insideCounter;

						// Count the number of painted pixels
						bool paintedPixel = paintedColor.a != 0;

						if (paintedPixel) {
							++paintedCounter;
						}

						// Count the number of painted pixels whose color is similar to the image color
						int redPaintedDiff = abs(imgColor.r - paintedColor.r);
						int greenPaintedDiff = abs(imgColor.g - paintedColor.g);
						int bluePaintedDiff = abs(imgColor.b - paintedColor.b);
						bool similarColorPixel = paintedPixel && redPaintedDiff < MAX_COLOR_DIFFERENCE[0]
								&& greenPaintedDiff < MAX_COLOR_DIFFERENCE[1]
								&& bluePaintedDiff < MAX_COLOR_DIFFERENCE[2];

						if (similarColorPixel) {
							++similarColorCounter;
						}

						// Count the number of pixels that will be well painted
						int redAverageDiff = abs(imgColor.r - bristleColor.r);
						int greenAverageDiff = abs(imgColor.g - bristleColor.g);
						int blueAverageDiff = abs(imgColor.b - bristleColor.b);
						bool wellPaintedPixel = redAverageDiff < MAX_COLOR_DIFFERENCE[0]
								&& greenAverageDiff < MAX_COLOR_DIFFERENCE[1]
								&& blueAverageDiff < MAX_COLOR_DIFFERENCE[2];

						if (wellPaintedPixel) {
							++wellPaintedCounter;
						}

						// Count the number of pixels that will not be well painted anymore
						if (similarColorPixel && !wellPaintedPixel) {
							++destroyedSimilarColorCounter;
						}

						// Calculate the color improvement
						if (paintedPixel) {
							colorImprovement += redPaintedDiff - redAverageDiff + greenPaintedDiff - greenAverageDiff
									+ bluePaintedDiff - blueAverageDiff;
						}
					} else {
						++outsideCounter;
					}
				}
			}
		}
	}

	int wellPaintedImprovement = wellPaintedCounter - similarColorCounter;
	int previouslyBadPainted = insideCounter - similarColorCounter;
	float averageMaxColorDiff = (MAX_COLOR_DIFFERENCE[0] + MAX_COLOR_DIFFERENCE[1] + MAX_COLOR_DIFFERENCE[2]) / 3.0;

	bool outsideCanvas = insideCounter < MIN_INSIDE_FRACTION * (insideCounter + outsideCounter);
	bool alreadyWellPainted = similarColorCounter > MAX_SIMILAR_COLOR_FRACTION * insideCounter;
	bool alreadyPainted = paintedCounter >= MAX_PAINTED_FRACTION * insideCounter;
	bool colorImproves = colorImprovement >= MIN_COLOR_IMPROVEMENT_FACTOR * averageMaxColorDiff * paintedCounter;
	bool bigWellPaintedImprovement = wellPaintedImprovement >= BIG_WELL_PAINTED_IMPROVEMENT_FRACTION * insideCounter;
	bool reducedBadPainted = wellPaintedImprovement >= MIN_BAD_PAINTED_REDUCTION_FRACTION * previouslyBadPainted;
	bool lowWellPaintedDestruction = destroyedSimilarColorCounter
			<= MAX_WELL_PAINTED_DESTRUCTION_FRACTION * wellPaintedImprovement;
	bool improves = (colorImproves || bigWellPaintedImprovement) && reducedBadPainted && lowWellPaintedDestruction;

	// Check if the trace will improve the painting
	return (outsideCanvas || alreadyWellPainted || (alreadyPainted && !improves)) ? false : true;
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

void ofxOilTrace::setVisitedPixels(ofPixels& visitedPixels) {
	// Extract some useful information
	int width = visitedPixels.getWidth();
	int height = visitedPixels.getHeight();

	// Calculate the bristle positions if necessary
	if (bPositions.size() == 0) {
		calculateBristlePositions();
	}

	for (unsigned int i = 0, nSteps = getNSteps(); i < nSteps; ++i) {
		// Fill the visited pixels array if alpha is high enough
		if (alphas[i] >= MIN_ALPHA) {
			for (const ofVec2f& pos : bPositions[i]) {
				int x = pos.x;
				int y = pos.y;

				if (x >= 0 && x < width && y >= 0 && y < height) {
					visitedPixels.setColor(x, y, ofColor(0));
				}
			}
		}
	}
}

void ofxOilTrace::setVisitedPixels(unsigned int step, ofPixels& visitedPixels) {
	// Extract some useful information
	int width = visitedPixels.getWidth();
	int height = visitedPixels.getHeight();

	// Calculate the bristle positions if necessary
	if (bPositions.size() == 0) {
		calculateBristlePositions();
	}

	// Fill the visited pixels array if alpha is high enough
	if (alphas[step] >= MIN_ALPHA) {
		for (const ofVec2f& pos : bPositions[step]) {
			int x = pos.x;
			int y = pos.y;

			if (x >= 0 && x < width && y >= 0 && y < height) {
				visitedPixels.setColor(x, y, ofColor(0));
			}
		}
	}
}

unsigned int ofxOilTrace::getNSteps() const {
	return positions.size();
}

const vector<ofVec2f>& ofxOilTrace::getTrajectoryPositions() const {
	return positions;
}

const vector<unsigned char>& ofxOilTrace::getTrajectoryAphas() const {
	return alphas;
}

const vector<vector<ofVec2f>>& ofxOilTrace::getBristlePositions() const {
	return bPositions;
}

const ofColor& ofxOilTrace::getAverageColor() const {
	return averageColor;
}
