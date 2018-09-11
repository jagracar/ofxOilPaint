#include "ofxOilSimulator.h"
#include "ofxOilTrace.h"
#include "ofMain.h"

float ofxOilSimulator::SMALLER_BRUSH_SIZE = 4;

float ofxOilSimulator::BRUSH_SIZE_DECREMENT = 1.3;

unsigned int ofxOilSimulator::MAX_INVALID_TRAJECTORIES = 5000;

unsigned int ofxOilSimulator::MAX_INVALID_TRAJECTORIES_FOR_SMALLER_SIZE = 10000;

unsigned int ofxOilSimulator::MAX_INVALID_TRACES = 250;

unsigned int ofxOilSimulator::MAX_INVALID_TRACES_FOR_SMALLER_SIZE = 350;

float ofxOilSimulator::TRACE_SPEED = 2;

float ofxOilSimulator::RELATIVE_TRACE_LENGTH = 2.3;

float ofxOilSimulator::MIN_TRACE_LENGTH = 16;

ofColor ofxOilSimulator::BACKGROUND_COLOR = ofColor(255);

array<int, 3> ofxOilSimulator::MAX_COLOR_DIFFERENCE = { 40, 40, 40 };

float ofxOilSimulator::MAX_VISITS_FRACTION_IN_TRAJECTORY = 0.35;

float ofxOilSimulator::MIN_INSIDE_FRACTION_IN_TRAJECTORY = 0.4;

float ofxOilSimulator::MAX_SIMILAR_COLOR_FRACTION_IN_TRAJECTORY = 0.6;

float ofxOilSimulator::MAX_COLOR_STDEV_IN_TRAJECTORY = 45;

float ofxOilSimulator::MIN_INSIDE_FRACTION = 0.7;

float ofxOilSimulator::MAX_SIMILAR_COLOR_FRACTION = 0.8; // 0.8 - 0.85 - 0.5

float ofxOilSimulator::MAX_PAINTED_FRACTION = 0.65;

float ofxOilSimulator::MIN_COLOR_IMPROVEMENT_FACTOR = 0.6;

float ofxOilSimulator::BIG_WELL_PAINTED_IMPROVEMENT_FRACTION = 0.3; // 0.3 - 0.35 - 0.4

float ofxOilSimulator::MIN_BAD_PAINTED_REDUCTION_FRACTION = 0.45; // 0.45 - 0.3 - 0.45

float ofxOilSimulator::MAX_WELL_PAINTED_DESTRUCTION_FRACTION = 0.4; // 0.4 - 0.55 - 0.4

ofxOilSimulator::ofxOilSimulator(bool _useCanvasBuffer, bool _verbose) :
		useCanvasBuffer(_useCanvasBuffer), verbose(_verbose) {
	nBadPaintedPixels = 0;
	averageBrushSize = SMALLER_BRUSH_SIZE;
	paintingIsFinised = true;
	obtainNewTrace = false;
	traceStep = 0;
	nTraces = 0;
}

void ofxOilSimulator::setImagePixels(const ofPixels& imagePixels, bool clearCanvas) {
	// Set the image pixels
	img = ofImage(imagePixels);
	int imgWidth = img.getWidth();
	int imgHeight = img.getHeight();

	// Initialize the canvas and pixel containers if necessary
	if (clearCanvas || imgWidth != canvas.getWidth() || imgHeight != canvas.getHeight()) {
		// Initialize the canvas where the image will be painted
		canvas.allocate(imgWidth, imgHeight, GL_RGB, 2);
		canvas.begin();
		ofClear(BACKGROUND_COLOR);
		canvas.end();

		// Initialize the canvas buffer if necessary
		if (useCanvasBuffer) {
			canvasBuffer.allocate(imgWidth, imgHeight, GL_RGB);
			canvasBuffer.begin();
			ofClear(BACKGROUND_COLOR);
			canvasBuffer.end();
		}

		// Initialize all the pixel arrays
		visitedPixels.allocate(imgWidth, imgHeight, OF_PIXELS_GRAY);
		similarColorPixels.allocate(imgWidth, imgHeight, OF_PIXELS_GRAY);
		badPaintedPixels = vector<unsigned int>(imgWidth * imgHeight);
		nBadPaintedPixels = 0;
	}

	// Initialize the rest of the simulator variables
	averageBrushSize = max(SMALLER_BRUSH_SIZE, max(imgWidth, imgHeight) / 6.0f);
	paintingIsFinised = false;
	obtainNewTrace = true;
	traceStep = 0;
	nTraces = 0;
}

void ofxOilSimulator::setImage(const ofImage& image, bool clearCanvas) {
	setImagePixels(image.getPixels(), clearCanvas);
}

void ofxOilSimulator::update(bool stepByStep) {
	// Don't do anything if the painting is finished
	if (paintingIsFinised) {
		return;
	}

	// Check if a new trace should be obtained
	if (obtainNewTrace) {
		// Update the pixel arrays
		updatePixelArrays();

		// Get a new trace
		getNewTrace();
	}

	// Paint the current trace if the painting is not finished
	if (!paintingIsFinised) {
		if (stepByStep) {
			// Paint the current trace step
			paintTraceStep();

			// Check if we finished painting the trace
			if (traceStep == trace.getNSteps()) {
				obtainNewTrace = true;
			}
		} else {
			// Paint all the trace steps
			paintTrace();
			obtainNewTrace = true;
		}
	}
}

void ofxOilSimulator::updatePixelArrays() {
	// Update the visited pixels array
	updateVisitedPixels();

	// Update the painted pixels array
	if (useCanvasBuffer) {
		canvasBuffer.readToPixels(paintedPixels);
	} else {
		canvas.readToPixels(paintedPixels);
	}

	// Update the similar color pixels and the bad painted pixels arrays
	const ofPixels& imgPixels = img.getPixels();
	unsigned int imgNumChannels = imgPixels.getNumChannels();
	unsigned int canvasNumChannels = paintedPixels.getNumChannels();
	nBadPaintedPixels = 0;

	for (unsigned int pixel = 0, nPixels = img.getWidth() * img.getHeight(); pixel < nPixels; ++pixel) {
		unsigned int imgPix = pixel * imgNumChannels;
		unsigned int canvasPix = pixel * canvasNumChannels;

		// Check if the pixel is well painted
		if (paintedPixels[canvasPix] != BACKGROUND_COLOR.r && paintedPixels[canvasPix + 1] != BACKGROUND_COLOR.g
				&& paintedPixels[canvasPix + 2] != BACKGROUND_COLOR.b
				&& abs(imgPixels[imgPix] - paintedPixels[canvasPix]) < MAX_COLOR_DIFFERENCE[0]
				&& abs(imgPixels[imgPix + 1] - paintedPixels[canvasPix + 1]) < MAX_COLOR_DIFFERENCE[1]
				&& abs(imgPixels[imgPix + 2] - paintedPixels[canvasPix + 2]) < MAX_COLOR_DIFFERENCE[2]) {
			similarColorPixels[pixel] = 0;
		} else {
			similarColorPixels[pixel] = 255;
			badPaintedPixels[nBadPaintedPixels] = pixel;
			++nBadPaintedPixels;
		}
	}
}

void ofxOilSimulator::updateVisitedPixels() {
	// Check if we are at the beginning of a simulation
	if (nTraces == 0) {
		// Reset the visited pixels array
		visitedPixels.setColor(255);
	} else {
		// Update the visited pixels arrays with the trace bristle positions
		const vector<unsigned char>& alphas = trace.getTrajectoryAphas();
		const vector<vector<glm::vec2>>& bristlePositions = trace.getBristlePositions();
		int width = visitedPixels.getWidth();
		int height = visitedPixels.getHeight();

		for (unsigned int i = 0, nSteps = trace.getNSteps(); i < nSteps; ++i) {
			// Fill the visited pixels array if alpha is high enough
			if (alphas[i] >= ofxOilTrace::MIN_ALPHA) {
				for (const glm::vec2& pos : bristlePositions[i]) {
					int x = pos.x;
					int y = pos.y;

					if (x >= 0 && x < width && y >= 0 && y < height) {
						visitedPixels.setColor(x, y, 0);
					}
				}
			}
		}
	}
}

void ofxOilSimulator::getNewTrace() {
	// Loop until a new trace is found or the painting is finished
	unsigned int invalidTrajectoriesCounter = 0;
	unsigned int invalidTracesCounter = 0;
	int imgWidth = img.getWidth();

	while (true) {
		// Check if we should stop the painting simulation
		if (averageBrushSize == SMALLER_BRUSH_SIZE
				&& (invalidTrajectoriesCounter > MAX_INVALID_TRAJECTORIES_FOR_SMALLER_SIZE
						|| invalidTracesCounter > MAX_INVALID_TRACES_FOR_SMALLER_SIZE)) {
			// Print some debug information if necessary
			if (verbose) {
				ofLogNotice() << "Total number of painted traces: " << nTraces;
				ofLogNotice() << "Processing time = " << ofGetElapsedTimef() << " seconds";
			}

			// Stop the painting
			paintingIsFinised = true;
			break;
		} else {
			// Change the average brush size if there were too many invalid traces
			if (averageBrushSize > SMALLER_BRUSH_SIZE
					&& (invalidTrajectoriesCounter > MAX_INVALID_TRAJECTORIES
							|| invalidTracesCounter > MAX_INVALID_TRACES)) {
				// Decrease the brush size
				averageBrushSize = max(SMALLER_BRUSH_SIZE,
						min(averageBrushSize / BRUSH_SIZE_DECREMENT, averageBrushSize - 2));

				// Print some debug information if necessary
				if (verbose) {
					ofLogNotice() << "Frame = " << ofGetFrameNum() << ", traces = " << nTraces
							<< ", new average brush size = " << averageBrushSize << "";
				}

				// Reset some the counters
				invalidTrajectoriesCounter = 0;
				invalidTracesCounter = 0;

				// Reset the visited pixels array
				visitedPixels.setColor(255);
			}

			// Create new traces until one of them has a valid trajectory or we exceed a number of tries
			bool isValidTrajectory = false;
			float brushSize = max(SMALLER_BRUSH_SIZE, averageBrushSize * ofRandom(0.95, 1.05));
			int nSteps = max(MIN_TRACE_LENGTH, RELATIVE_TRACE_LENGTH * brushSize * ofRandom(0.9, 1.1)) / TRACE_SPEED;

			while (!isValidTrajectory && invalidTrajectoriesCounter % 500 != 499) {
				// Create the trace starting from a bad painted pixel
				unsigned int pixel = badPaintedPixels[floor(ofRandom(nBadPaintedPixels))];
				glm::vec2 startingPosition = glm::vec2(pixel % imgWidth, pixel / imgWidth);
				trace = ofxOilTrace(startingPosition, nSteps, TRACE_SPEED);

				// Check if the trace has a valid trajectory
				isValidTrajectory = !alreadyVisitedTrajectory() && validTrajectory();

				// Increase the counter
				++invalidTrajectoriesCounter;
			}

			// Check if we have a valid trajectory
			if (isValidTrajectory) {
				// Reset the invalid trajectories counter
				invalidTrajectoriesCounter = 0;

				// Set the trace brush size
				trace.setBrushSize(brushSize);

				// Calculate the trace average color and the bristle colors along the trajectory
				trace.calculateAverageColor(img);
				trace.calculateBristleColors(paintedPixels, BACKGROUND_COLOR);

				// Check if painting the trace will improve the painting
				if (traceImprovesPainting()) {
					// Test passed, the trace is good enough to be painted
					obtainNewTrace = false;
					traceStep = 0;
					++nTraces;
					break;
				} else {
					// The trace is not good enough, try again in the next loop step
					++invalidTracesCounter;
				}
			} else {
				// The trace is not good enough, try again in the next loop step
				++invalidTracesCounter;
			}
		}
	}
}

bool ofxOilSimulator::alreadyVisitedTrajectory() const {
	// Extract some useful information
	const vector<glm::vec2>& positions = trace.getTrajectoryPositions();
	const vector<unsigned char>& alphas = trace.getTrajectoryAphas();
	int width = visitedPixels.getWidth();
	int height = visitedPixels.getHeight();

	// Check if the trace trajectory has been visited before
	int insideCounter = 0;
	int visitedCounter = 0;

	for (unsigned int i = ofxOilBrush::POSITIONS_FOR_AVERAGE, nSteps = trace.getNSteps(); i < nSteps; ++i) {
		// Check that the alpha value is high enough
		if (alphas[i] >= ofxOilTrace::MIN_ALPHA) {
			// Check that the position is inside the image
			const glm::vec2& pos = positions[i];
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

bool ofxOilSimulator::validTrajectory() const {
	// Extract some useful information
	const vector<glm::vec2>& positions = trace.getTrajectoryPositions();
	const vector<unsigned char>& alphas = trace.getTrajectoryAphas();
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

	for (unsigned int i = ofxOilBrush::POSITIONS_FOR_AVERAGE, nSteps = trace.getNSteps(); i < nSteps; ++i) {
		// Check that the alpha value is high enough
		if (alphas[i] >= ofxOilTrace::MIN_ALPHA) {
			// Check that the position is inside the image
			const glm::vec2& pos = positions[i];
			int x = pos.x;
			int y = pos.y;

			if (x >= 0 && x < width && y >= 0 && y < height) {
				++insideCounter;

				// Get the image color and the painted color at the trajectory position
				const ofColor& imgColor = img.getColor(x, y);
				const ofColor& paintedColor = paintedPixels.getColor(x, y);

				// Check if the two colors are similar
				if (paintedColor != BACKGROUND_COLOR && abs(imgColor.r - paintedColor.r) < MAX_COLOR_DIFFERENCE[0]
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

bool ofxOilSimulator::traceImprovesPainting() const {
	// Extract some useful information
	const vector<unsigned char>& alphas = trace.getTrajectoryAphas();
	const vector<vector<ofColor>>& bristleImgColors = trace.getBristleImageColors();
	const vector<vector<ofColor>>& bristlePaintedColors = trace.getBristlePaintedColors();
	const vector<vector<ofColor>>& bristleColors = trace.getBristleColors();

	// Obtain some trace statistics
	int insideCounter = 0;
	int outsideCounter = 0;
	int paintedCounter = 0;
	int similarColorCounter = 0;
	int wellPaintedCounter = 0;
	int destroyedSimilarColorCounter = 0;
	int colorImprovement = 0;

	for (unsigned int i = 0, nSteps = trace.getNSteps(); i < nSteps; ++i) {
		// Check that the alpha value is high enough
		if (alphas[i] >= ofxOilTrace::MIN_ALPHA) {
			// Get the bristles image colors and painted colors for this step
			const vector<ofColor>& bic = bristleImgColors[i];
			const vector<ofColor>& bpc = bristlePaintedColors[i];
			const vector<ofColor>& bc = bristleColors[i];

			// Make sure that the containers are not empty
			if (bic.size() > 0) {
				for (unsigned int bristle = 0, nBristles = trace.getNBristles(); bristle < nBristles; ++bristle) {
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

void ofxOilSimulator::paintTrace() {
	// Pain the trace in the canvas and the canvas buffer if necessary
	canvas.begin();
	useCanvasBuffer ? trace.paint(canvasBuffer) : trace.paint();
	canvas.end();
}

void ofxOilSimulator::paintTraceStep() {
	// Pain the trace step in the canvas and the canvas buffer if necessary
	canvas.begin();
	useCanvasBuffer ? trace.paintStep(traceStep, canvasBuffer) : trace.paintStep(traceStep);
	canvas.end();

	// Increment the trace step
	++traceStep;
}

void ofxOilSimulator::drawCanvas(float x, float y) const {
	canvas.draw(x, y);
}

void ofxOilSimulator::drawImage(float x, float y) const {
	img.draw(x, y);
}

void ofxOilSimulator::drawVisitedPixels(float x, float y) const {
	ofImage visitedPixelsImg;
	visitedPixelsImg.setFromPixels(visitedPixels);
	visitedPixelsImg.draw(x, y);
}

void ofxOilSimulator::drawSimilarColorPixels(float x, float y) const {
	ofImage similarColorPixelsImg;
	similarColorPixelsImg.setFromPixels(similarColorPixels);
	similarColorPixelsImg.draw(x, y);
}

bool ofxOilSimulator::isFinished() const {
	return paintingIsFinised;
}
