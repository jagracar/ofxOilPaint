#include "ofxOilTrace.h"
#include "ofxOilConstants.h"
#include "ofxOilBrush.h"
#include "ofMain.h"

ofxOilTrace::ofxOilTrace(const ofVec2f& startingPosition, int nSteps, float speed) {
	// Fill the positions and alphas vectors
	float initAng = ofRandom(TWO_PI);
	float noiseSeed = ofRandom(1000);
	float alphaDecrement = min(255.0 / nSteps, 25.0);
	positions.emplace_back(startingPosition);
	alphas.emplace_back(255);

	for (int step = 1; step < nSteps; ++step) {
		float ang = initAng + TWO_PI * (ofNoise(noiseSeed + NOISE_FACTOR * step) - 0.5);
		positions.emplace_back(positions[step - 1].x + speed * cos(ang), positions[step - 1].y + speed * sin(ang));
		alphas.emplace_back(ofClamp(255 - alphaDecrement * step, 0, 255));
	}

	// Set the average color
	averageColor.set(0, 0);
}

ofxOilTrace::ofxOilTrace(const vector<ofVec2f>& _positions, const vector<unsigned char>& _alphas) {
	// Do some sanity checks
	if (_positions.size() == 0) {
		throw invalid_argument("The _positions vector should not be empty");
	} else if (_positions.size() != _alphas.size()) {
		throw invalid_argument("The _positions and _alphas vectors should have the same size");
	}

	positions = _positions;
	alphas = _alphas;
	averageColor.set(0, 0);
}

void ofxOilTrace::setBrushSize(float brushSize) {
	// Initialize the brush
	brush = ofxOilBrush(brushSize);
	brush.init(positions[0]);

	// Reset the average color
	averageColor.set(0, 0);

	// Reset the bristle containers
	bPositions.clear();
	bColors.clear();
	bImgColors.clear();
	bPaintedColors.clear();
}

void ofxOilTrace::setAverageColor(const ofColor& col) {
	averageColor.set(col);
	bColors.clear();
}

bool ofxOilTrace::hasValidTrajectory(const vector<bool>& similarColorPixels, const vector<bool>& visitedPixels,
		const ofImage& img) const {
	// Extract some useful information
	int width = img.getWidth();
	int height = img.getHeight();

	// Obtain some pixel statistics along the trajectory
	int insideCounter = 0;
	int similarColorCounter = 0;
	int visitedCounter = 0;
	float imgRedSum = 0;
	float imgRedSqSum = 0;
	float imgGreenSum = 0;
	float imgGreenSqSum = 0;
	float imgBlueSum = 0;
	float imgBlueSqSum = 0;

	for (const ofVec2f& pos : positions) {
		// Check that the position is inside the image
		int x = pos.x;
		int y = pos.y;

		if (x >= 0 && x < width && y >= 0 && y < height) {
			// Increase the counters
			++insideCounter;

			if (similarColorPixels[x + y * width]) {
				++similarColorCounter;
			}

			if (visitedPixels[x + y * width]) {
				++visitedCounter;
			}

			// Extract the pixel color properties
			const ofColor& imgCol = img.getColor(x, y);
			int imgRed = imgCol.r;
			int imgGreen = imgCol.g;
			int imgBlue = imgCol.b;
			imgRedSum += imgRed;
			imgRedSqSum += imgRed * imgRed;
			imgGreenSum += imgGreen;
			imgGreenSqSum += imgGreen * imgGreen;
			imgBlueSum += imgBlue;
			imgBlueSqSum += imgBlue * imgBlue;
		}
	}

	// Obtain the colors standard deviation along the trajectory
	float imgRedStDev = 0;
	float imgGreenStDev = 0;
	float imgBlueStDev = 0;

	if (insideCounter > 1) {
		imgRedStDev = sqrt((imgRedSqSum - imgRedSum * imgRedSum / insideCounter) / (insideCounter - 1));
		imgGreenStDev = sqrt((imgGreenSqSum - imgGreenSum * imgGreenSum / insideCounter) / (insideCounter - 1));
		imgBlueStDev = sqrt((imgBlueSqSum - imgBlueSum * imgBlueSum / insideCounter) / (insideCounter - 1));
	}

	// Check if it's a valid trajectory
	bool insideCanvas = insideCounter >= MIN_INSIDE_FRACTION_IN_TRAJECTORY * getNSteps();
	bool badPainted = similarColorCounter <= MAX_SIMILAR_COLOR_FRACTION_IN_TRAJECTORY * insideCounter;
	bool notVisited = visitedCounter <= MAX_VISITS_FRACTION_IN_TRAJECTORY * insideCounter;
	bool smallColorChange = imgRedStDev < MAX_COLOR_STDEV_IN_TRAJECTORY && imgGreenStDev < MAX_COLOR_STDEV_IN_TRAJECTORY
			&& imgBlueStDev < MAX_COLOR_STDEV_IN_TRAJECTORY;

	return insideCanvas && badPainted && notVisited && smallColorChange;
}

bool ofxOilTrace::hasValidTrajectory(const vector<int>& maxColorDiff, const ofImage& img, const ofPixels& paintedPixels,
		const ofColor& backgroundColor) const {
	// Extract some useful information
	int width = img.getWidth();
	int height = img.getHeight();

	// Obtain some pixel statistics along the trajectory
	int insideCounter = 0;
	int similarColorCounter = 0;
	float imgRedSum = 0;
	float imgRedSqSum = 0;
	float imgGreenSum = 0;
	float imgGreenSqSum = 0;
	float imgBlueSum = 0;
	float imgBlueSqSum = 0;

	for (const ofVec2f& pos : positions) {
		// Check that the position is inside the image
		int x = pos.x;
		int y = pos.y;

		if (x >= 0 && x < width && y >= 0 && y < height) {
			const ofColor& imgColor = img.getColor(x, y);
			const ofColor& paintedColor = paintedPixels.getColor(x, y);

			// Increase the counters
			++insideCounter;

			if (paintedColor != backgroundColor && abs(imgColor.r - paintedColor.r) < maxColorDiff[0]
					&& abs(imgColor.g - paintedColor.g) < maxColorDiff[1]
					&& abs(imgColor.b - paintedColor.b) < maxColorDiff[2]) {
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
		}
	}

	// Obtain the colors standard deviation along the trajectory
	float imgRedStDev = 0;
	float imgGreenStDev = 0;
	float imgBlueStDev = 0;

	if (insideCounter > 1) {
		imgRedStDev = sqrt((imgRedSqSum - imgRedSum * imgRedSum / insideCounter) / (insideCounter - 1));
		imgGreenStDev = sqrt((imgGreenSqSum - imgGreenSum * imgGreenSum / insideCounter) / (insideCounter - 1));
		imgBlueStDev = sqrt((imgBlueSqSum - imgBlueSum * imgBlueSum / insideCounter) / (insideCounter - 1));
	}

	// Check if it's a valid trajectory
	bool insideCanvas = insideCounter >= MIN_INSIDE_FRACTION_IN_TRAJECTORY * getNSteps();
	bool badPainted = similarColorCounter <= MAX_SIMILAR_COLOR_FRACTION_IN_TRAJECTORY * insideCounter;
	bool smallColorChange = imgRedStDev < MAX_COLOR_STDEV_IN_TRAJECTORY && imgGreenStDev < MAX_COLOR_STDEV_IN_TRAJECTORY
			&& imgBlueStDev < MAX_COLOR_STDEV_IN_TRAJECTORY;

	return insideCanvas && badPainted && smallColorChange;
}

void ofxOilTrace::calculateBristlePositions() {
	// Reset the container
	bPositions.clear();

	for (const ofVec2f& pos : positions) {
		// Move the brush
		brush.update(pos, false);

		// Save the bristles positions
		bPositions.emplace_back(brush.getBristlesPositions());
	}

	// Reset the brush to the initial position
	brush.init(positions[0]);
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
		bImgColors.emplace_back(vector<ofColor>());
		vector<ofColor>& bic = bImgColors.back();

		for (const ofVec2f& pos : bp) {
			// Check that the bristle is inside the image
			int x = pos.x;
			int y = pos.y;

			if (x >= 0 && x < width && y >= 0 && y < height) {
				bic.emplace_back(img.getColor(x, y));
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
		bPaintedColors.emplace_back(vector<ofColor>());
		vector<ofColor>& bpc = bPaintedColors.back();

		for (const ofVec2f& pos : bp) {
			// Check that the bristle is inside the canvas
			int x = pos.x;
			int y = pos.y;

			if (x >= 0 && x < width && y >= 0 && y < height) {
				const ofColor& col = paintedPixels.getColor(x, y);

				if (col != backgroundColor && col.a != 0) {
					bpc.emplace_back(col);
				} else {
					bpc.emplace_back(0, 0);
				}
			} else {
				bpc.emplace_back(0, 0);
			}
		}
	}
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

	for (int step = 0, nSteps = getNSteps(); step < nSteps; ++step) {
		// Check that the alpha value is high enough for the average color calculation
		if (alphas[step] >= MIN_ALPHA) {
			for (const ofColor& col : bImgColors[step]) {
				if (col.a != 0) {
					redSum += col.r;
					greenSum += col.g;
					blueSum += col.b;
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

bool ofxOilTrace::improvesPainting(const vector<int>& maxColorDiff, const vector<bool>& similarColorPixels,
		const ofImage& img, const ofPixels& paintedPixels, const ofColor& backgroundColor) {
	// Extract some useful information
	int nSteps = getNSteps();
	int nBristles = brush.getNBristles();
	int width = img.getWidth();
	int height = img.getHeight();

	// Calculate the bristle positions if necessary
	if (bPositions.size() == 0) {
		calculateBristlePositions();
	}

	// Obtain some trace statistics
	int insideCounter = 0;
	int outsideCounter = 0;
	int similarColorCounter = 0;
	vector<vector<bool>> similarColor;

	for (int step = 0; step < nSteps; ++step) {
		similarColor.emplace_back(vector<bool>());

		// Check that the alpha value is high enough
		if (alphas[step] >= MIN_ALPHA) {
			vector<bool>& sc = similarColor.back();

			for (const ofVec2f& pos : bPositions[step]) {
				// Check that the bristle is inside the image
				int x = pos.x;
				int y = pos.y;

				if (x >= 0 && x < width && y >= 0 && y < height) {
					++insideCounter;

					if (similarColorPixels[x + y * width]) {
						sc.emplace_back(true);
						++similarColorCounter;
					} else {
						sc.emplace_back(false);
					}
				} else {
					sc.emplace_back(false);
					++outsideCounter;
				}
			}
		}
	}

	// Check if the trace region was painted before with similar colors or falls outside the image
	bool wellPainted = similarColorCounter >= MAX_SIMILAR_COLOR_FRACTION * insideCounter;
	bool outsideCanvas = insideCounter < MIN_INSIDE_FRACTION * (insideCounter + outsideCounter);

	if (wellPainted || outsideCanvas) {
		// The trace will not improve the painting
		return false;
	}

	// Calculate the bristle image colors if necessary
	if (bImgColors.size() == 0) {
		calculateImgColors(img);
	}

	// Calculate the bristle painted colors if necessary
	if (bPaintedColors.size() == 0) {
		calculatePaintedColors(paintedPixels, backgroundColor);
	}

	// Calculate the average color if necessary
	if (averageColor.a == 0) {
		calculateAverageColor(img);
	}

	// Check that drawing the trace will improve the accuracy of the painting
	int wellPaintedCounter = 0;
	int destroyedWellPaintedCounter = 0;
	int alreadyPaintedCounter = 0;
	int colorImprovement = 0;

	for (int step = 0; step < nSteps; ++step) {
		// Check if the alpha value is high enough
		if (alphas[step] >= MIN_ALPHA) {
			const vector<ofColor>& bic = bImgColors[step];
			const vector<ofColor>& bpc = bPaintedColors[step];
			const vector<bool>& sc = similarColor[step];

			if (bic.size() > 0) {
				for (int bristle = 0; bristle < nBristles; ++bristle) {
					// Check that the bristle is inside the image
					const ofColor& imgColor = bic[bristle];
					const ofColor& paintedColor = bpc[bristle];

					if (imgColor.a != 0) {
						// Count the number of well painted pixels, and how many are not well painted anymore
						int redDiff = abs(imgColor.r - averageColor.r);
						int greenDiff = abs(imgColor.g - averageColor.g);
						int blueDiff = abs(imgColor.b - averageColor.b);

						if ((redDiff < maxColorDiff[0]) && (greenDiff < maxColorDiff[1])
								&& (blueDiff < maxColorDiff[2])) {
							++wellPaintedCounter;
						} else if (sc[bristle]) {
							++destroyedWellPaintedCounter;
						}

						// Count previously painted pixels and calculate their color improvement
						if (paintedColor.a != 0) {
							++alreadyPaintedCounter;

							// Calculate the color improvement
							colorImprovement += abs(imgColor.r - paintedColor.r) - redDiff
									+ abs(imgColor.g - paintedColor.g) - greenDiff + abs(imgColor.b - paintedColor.b)
									- blueDiff;
						}
					}
				}
			}
		}
	}

	int wellPaintedImprovement = wellPaintedCounter - similarColorCounter;
	int previousBadPainted = insideCounter - similarColorCounter;
	float averageMaxColorDiff = (maxColorDiff[0] + maxColorDiff[1] + maxColorDiff[2]) / 3.0;

	bool alreadyPainted = alreadyPaintedCounter >= MIN_PAINTED_FRACTION * insideCounter;
	bool colorImproves = colorImprovement >= MIN_COLOR_IMPROVEMENT_FACTOR * averageMaxColorDiff * alreadyPaintedCounter;
	bool bigWellPaintedImprovement = wellPaintedImprovement >= BIG_WELL_PAINTED_IMPROVEMENT_FRACTION * insideCounter;
	bool reducedBadPainted = wellPaintedImprovement >= MIN_BAD_PAINTED_REDUCTION_FRACTION * previousBadPainted;
	bool lowWellPaintedDestruction = destroyedWellPaintedCounter
			<= MAX_WELL_PAINTED_DESTRUCTION_FRACTION * wellPaintedImprovement;
	bool improves = (colorImproves || bigWellPaintedImprovement) && reducedBadPainted && lowWellPaintedDestruction;

	if (alreadyPainted && !improves) {
		// The trace will not improve the painting
		return false;
	}

	// The trace will improve the painting
	return true;
}

bool ofxOilTrace::improvesPainting(const vector<int>& maxColorDiff, const ofImage& img, const ofPixels& paintedPixels,
		const ofColor& backgroundColor) {
	// Extract some useful information
	int nSteps = getNSteps();
	int nBristles = brush.getNBristles();

	// Calculate the bristle image colors if necessary
	if (bImgColors.size() == 0) {
		calculateImgColors(img);
	}

	// Calculate the bristle painted colors if necessary
	if (bPaintedColors.size() == 0) {
		calculatePaintedColors(paintedPixels, backgroundColor);
	}

	// Obtain some trace statistics
	int insideCounter = 0;
	int outsideCounter = 0;
	int similarColorCounter = 0;
	vector<vector<bool>> similarColor;

	for (int step = 0; step < nSteps; ++step) {
		similarColor.emplace_back(vector<bool>());

		// Check that the alpha value is high enough
		if (alphas[step] >= MIN_ALPHA) {
			vector<bool>& sc = similarColor.back();
			const vector<ofColor>& bic = bImgColors[step];
			const vector<ofColor>& bpc = bPaintedColors[step];

			if (bic.size() > 0) {
				for (int bristle = 0; bristle < nBristles; ++bristle) {
					const ofColor& imgColor = bic[bristle];
					const ofColor& paintedColor = bpc[bristle];

					// Check that the bristle is inside the image
					if (imgColor.a != 0) {
						++insideCounter;

						bool wellPainted = false;

						if (paintedColor != backgroundColor) {
							wellPainted = abs(imgColor.r - paintedColor.r) < maxColorDiff[0]
									&& abs(imgColor.g - paintedColor.g) < maxColorDiff[1]
									&& abs(imgColor.b - paintedColor.b) < maxColorDiff[2];
						}

						if (wellPainted) {
							sc.emplace_back(true);
							++similarColorCounter;
						} else {
							sc.emplace_back(false);
						}
					} else {
						sc.emplace_back(false);
						++outsideCounter;
					}
				}
			}
		}
	}

	// Check if the trace region was painted before with similar colors or falls outside the image
	bool wellPainted = similarColorCounter >= MAX_SIMILAR_COLOR_FRACTION * insideCounter;
	bool outsideCanvas = insideCounter < MIN_INSIDE_FRACTION * (insideCounter + outsideCounter);

	if (wellPainted || outsideCanvas) {
		// The trace will not improve the painting
		return false;
	}

	// Calculate the average color if necessary
	if (averageColor.a == 0) {
		calculateAverageColor(img);
	}

	// Check that drawing the trace will improve the accuracy of the painting
	int wellPaintedCounter = 0;
	int destroyedWellPaintedCounter = 0;
	int alreadyPaintedCounter = 0;
	int colorImprovement = 0;

	for (int step = 0; step < nSteps; ++step) {
		// Check if the alpha value is high enough
		if (alphas[step] >= MIN_ALPHA) {
			const vector<ofColor>& bic = bImgColors[step];
			const vector<ofColor>& bpc = bPaintedColors[step];
			const vector<bool>& sc = similarColor[step];

			if (bic.size() > 0) {
				for (int bristle = 0; bristle < nBristles; ++bristle) {
					// Check that the bristle is inside the image
					const ofColor& imgColor = bic[bristle];
					const ofColor& paintedColor = bpc[bristle];

					if (imgColor.a != 0) {
						// Count the number of well painted pixels, and how many are not well painted anymore
						int redDiff = abs(imgColor.r - averageColor.r);
						int greenDiff = abs(imgColor.g - averageColor.g);
						int blueDiff = abs(imgColor.b - averageColor.b);

						if ((redDiff < maxColorDiff[0]) && (greenDiff < maxColorDiff[1])
								&& (blueDiff < maxColorDiff[2])) {
							++wellPaintedCounter;
						} else if (sc[bristle]) {
							++destroyedWellPaintedCounter;
						}

						// Count previously painted pixels and calculate their color improvement
						if (paintedColor.a != 0) {
							++alreadyPaintedCounter;

							// Calculate the color improvement
							colorImprovement += abs(imgColor.r - paintedColor.r) - redDiff
									+ abs(imgColor.g - paintedColor.g) - greenDiff + abs(imgColor.b - paintedColor.b)
									- blueDiff;
						}
					}
				}
			}
		}
	}

	int wellPaintedImprovement = wellPaintedCounter - similarColorCounter;
	int previousBadPainted = insideCounter - similarColorCounter;
	float averageMaxColorDiff = (maxColorDiff[0] + maxColorDiff[1] + maxColorDiff[2]) / 3.0;

	bool alreadyPainted = alreadyPaintedCounter >= MIN_PAINTED_FRACTION * insideCounter;
	bool colorImproves = colorImprovement >= MIN_COLOR_IMPROVEMENT_FACTOR * averageMaxColorDiff * alreadyPaintedCounter;
	bool bigWellPaintedImprovement = wellPaintedImprovement >= BIG_WELL_PAINTED_IMPROVEMENT_FRACTION * insideCounter;
	bool reducedBadPainted = wellPaintedImprovement >= MIN_BAD_PAINTED_REDUCTION_FRACTION * previousBadPainted;
	bool lowWellPaintedDestruction = destroyedWellPaintedCounter
			<= MAX_WELL_PAINTED_DESTRUCTION_FRACTION * wellPaintedImprovement;
	bool improves = (colorImproves || bigWellPaintedImprovement) && reducedBadPainted && lowWellPaintedDestruction;

	if (alreadyPainted && !improves) {
		// The trace will not improve the painting
		return false;
	}

	// The trace will improve the painting
	return true;
}

void ofxOilTrace::calculateBristleColors(const ofPixels& paintedPixels, const ofColor& backgroundColor) {
	// Extract some useful information
	int nSteps = getNSteps();
	int nBristles = brush.getNBristles();

	// Calculate the bristle painted colors if necessary
	if (bPaintedColors.size() == 0) {
		calculatePaintedColors(paintedPixels, backgroundColor);
	}

	// Calculate the bristle starting colors
	vector<ofColor> startingColors;
	float averageHue = averageColor.getHue();
	float averageSaturation = averageColor.getSaturation();
	float averageBrightness = averageColor.getBrightness();
	float noiseSeed = ofRandom(1000);

	for (int bristle = 0; bristle < nBristles; ++bristle) {
// Add some brightness changes to make it more realistic
		float deltaBrightness = BRIGHTNESS_RELATIVE_CHANGE * averageBrightness
				* (ofNoise(noiseSeed + 0.4 * bristle) - 0.5);
		startingColors.emplace_back(
				ofColor::fromHsb(averageHue, averageSaturation, ofClamp(averageBrightness + deltaBrightness, 0, 255)));
	}

	// Use the bristle starting colors until the step where the mixing starts
	int mixStartingStep = ofClamp(TYPICAL_MIX_STARTING_STEP, 1, nSteps);
	bColors = vector<vector<ofColor>>(mixStartingStep, startingColors);

	// Mix the previous step colors with the already painted colors
	for (int step = mixStartingStep; step < nSteps; ++step) {
// Copy the previous step colors
		bColors.emplace_back(bColors.back());

// Check if the alpha value is high enough for mixing
		if (alphas[step] >= MIN_ALPHA) {
			// Calculate the bristle colors for this step
			vector<ofColor>& bc = bColors.back();
			const vector<ofColor>& bpc = bPaintedColors[step];

			if (bpc.size() > 0) {
				for (int bristle = 0; bristle < nBristles; ++bristle) {
					const ofColor& paintedCol = bpc[bristle];

					if (paintedCol.a != 0) {
						bc[bristle].lerp(paintedCol, MIX_STRENGTH);
					}
				}
			}
		}
	}
}

void ofxOilTrace::paint() {
	// Check that it makes sense to paint the trace
	if (bColors.size() > 0) {
		for (int step = 0, nSteps = getNSteps(); step < nSteps; ++step) {
			// Move the brush
			brush.update(positions[step], true);

			// Paint the brush
			brush.paint(bColors[step], alphas[step]);
		}

// Reset the brush to the initial position
		brush.init(positions[0]);
	}
}

void ofxOilTrace::paint(vector<bool>& visitedPixels, int width, int height) {
	// Check that it makes sense to paint the trace
	if (bColors.size() > 0) {
		for (int step = 0, nSteps = getNSteps(); step < nSteps; ++step) {
			// Move the brush
			brush.update(positions[step], true);

			// Paint the brush
			brush.paint(bColors[step], alphas[step]);

			// Fill the visited pixels array if alpha is high enough
			if (alphas[step] >= MIN_ALPHA) {
				for (const ofVec2f& pos : bPositions[step]) {
					int x = pos.x;
					int y = pos.y;

					if (x >= 0 && x < width && y >= 0 && y < height) {
						visitedPixels[x + y * width] = true;
					}
				}
			}
		}

// Reset the brush to the initial position
		brush.init(positions[0]);
	}
}

void ofxOilTrace::paintStep(int step) {
	// Check that it makes sense to paint the given step
	int nSteps = getNSteps();

	if (step < nSteps && bColors.size() > 0) {
// Move the brush
		brush.update(positions[step], true);

// Paint the brush
		brush.paint(bColors[step], alphas[step]);

// Check if we are at the last step
		if (step == nSteps - 1) {
			// Reset the brush to the initial position
			brush.init(positions[0]);
		}
	}
}

int ofxOilTrace::getNSteps() const {
	return positions.size();
}

const vector<ofVec2f>& ofxOilTrace::getPositions() const {
	return positions;
}

const vector<vector<ofVec2f>>& ofxOilTrace::getBristlePositions() const {
	return bPositions;
}
