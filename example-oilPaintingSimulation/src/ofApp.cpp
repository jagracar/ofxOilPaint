#include "ofApp.h"
#include "ofxOilPaint.h"

//--------------------------------------------------------------
void ofApp::setup() {
	// Do not clean the screen at the beginning of each frame
	//ofSetBackgroundAuto(false);

	// Set the desired frame rate
	ofSetFrameRate(2000);
	ofSetVerticalSync(false);

	// Load the picture that we want to paint and resize it by the specified amount
	img.load(pictureFile);
	imgWidth = round(img.getWidth() / sizeReductionFactor);
	imgHeight = round(img.getHeight() / sizeReductionFactor);
	img.resize(imgWidth, imgHeight);

	// Resize the application window
	if (comparisonMode) {
		ofSetWindowShape(2 * imgWidth, imgHeight);
	} else if (debugMode) {
		ofSetWindowShape(3 * imgWidth, imgHeight);
	} else {
		ofSetWindowShape(imgWidth, imgHeight);
	}

	// Initialize the canvas where we will paint the image
	canvas.allocate(imgWidth, imgHeight, GL_RGB, 2);
	canvas.begin();
	ofClear(backgroundColor);
	canvas.end();

	// Initialize the canvas buffer if necessary
	if (useCanvasBuffer) {
		canvasBuffer.allocate(imgWidth, imgHeight, GL_RGB);
		canvasBuffer.begin();
		ofClear(backgroundColor);
		canvasBuffer.end();
	}

	// Initialize all the pixel arrays
	paintedPixels.allocate(imgWidth, imgHeight, OF_PIXELS_RGB);
	similarColorPixels.allocate(imgWidth, imgHeight, OF_PIXELS_GRAY);
	visitedPixels.allocate(imgWidth, imgHeight, OF_PIXELS_GRAY);
	badPaintedPixels = vector<unsigned int>(imgWidth * imgHeight);

	// Set the pixel arrays initial values
	visitedPixels.setColor(ofColor(255));
	updatePixelArrays();

	// Initialize the rest of the sketch variables
	averageBrushSize = max(smallerBrushSize, max(imgWidth, imgHeight) / 6.0f);
	continuePainting = true;
	obtainNewTrace = true;
	traceStep = 0;
	nTraces = 0;
}

//--------------------------------------------------------------
void ofApp::update() {
	if (obtainNewTrace) {
		// Obtain a new valid trace
		bool traceNotFound = true;
		unsigned int invalidTrajectoriesCounter = 0;
		unsigned int invalidTracesCounter = 0;

		while (traceNotFound) {
			// Check if we should stop painting
			if (averageBrushSize == smallerBrushSize
					&& (invalidTrajectoriesCounter > maxInvalidTrajectoriesForSmallerSize
							|| invalidTracesCounter > maxInvalidTracesForSmallerSize)) {
				ofLogNotice() << "Total number of painted traces: " << nTraces;
				ofLogNotice() << "Processing time = " << ofGetElapsedTimef() << " seconds";

				// Save a final screenshot if necessary
				if (saveFinalScreenshot) {
					ofSaveScreen("screenshot-" + ofToString(ofGetFrameNum()) + ".png");
				}

				// Stop the simulation
				traceNotFound = false;
				continuePainting = false;
			} else {
				// Change the average brush size if there were too many invalid traces
				if (averageBrushSize > smallerBrushSize
						&& (invalidTrajectoriesCounter > maxInvalidTrajectories
								|| invalidTracesCounter > maxInvalidTraces)) {
					averageBrushSize = ofClamp(averageBrushSize / brushSizeDecrement, smallerBrushSize,
							averageBrushSize - 2);

					ofLogNotice() << "Frame = " << ofGetFrameNum() << ", traces = " << nTraces
							<< ", new average brush size = " << averageBrushSize << "";

					// Reset some the counters
					invalidTrajectoriesCounter = 0;
					invalidTracesCounter = 0;

					// Reset the visited pixels array
					visitedPixels.setColor(255);
				}

				// Create new traces until one of them has a valid trajectory or we exceed a number of tries
				bool validTrajectory = false;
				float brushSize = max(smallerBrushSize, averageBrushSize * ofRandom(0.95, 1.05));
				int nSteps = max(minTraceLength, relativeTraceLength * brushSize * ofRandom(0.9, 1.1)) / traceSpeed;

				while (!validTrajectory && invalidTrajectoriesCounter % 500 != 499) {
					// Create the trace starting from a bad painted pixel
					unsigned int pixel = badPaintedPixels[floor(ofRandom(nBadPaintedPixels))];
					ofVec2f startingPosition = ofVec2f(pixel % imgWidth, pixel / imgWidth);
					trace = ofxOilTrace(startingPosition, nSteps, traceSpeed);

					// Check if the trace has a valid trajectory
					validTrajectory = !trace.alreadyVisitedTrajectory(visitedPixels)
							&& trace.hasValidTrajectory(img, paintedPixels, backgroundColor);

					// Increase the counter
					++invalidTrajectoriesCounter;
				}

				// Check if we have a valid trajectory
				if (validTrajectory) {
					// Reset the invalid trajectories counter
					invalidTrajectoriesCounter = 0;

					// Set the trace brush size
					trace.setBrushSize(brushSize);

					// Calculate the trace bristle colors along the trajectory
					trace.calculateAverageColor(img);
					trace.calculateBristleColors(paintedPixels, backgroundColor);

					// Check if painting the trace will improve the painting
					if (trace.improvesPainting(img)) {
						// Test passed, the trace is good enough to be painted
						obtainNewTrace = false;
						traceNotFound = false;
						traceStep = 0;
						++nTraces;
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

		// Update the window title
		if (continuePainting) {
			ofSetWindowTitle("Oil painting simulation ( frame rate: " + ofToString(round(ofGetFrameRate())) + " )");
		} else {
			ofSetWindowTitle("Oil painting simulation ( finished )");
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	if (continuePainting) {
		// Paint the trace step by step or in one go
		if (paintStepByStep) {
			if (useCanvasBuffer) {
				canvas.begin();
				trace.paintStep(traceStep, canvasBuffer);
				canvas.end();
			} else {
				canvas.begin();
				trace.paintStep(traceStep);
				canvas.end();
			}

			// Increment the trace step
			++traceStep;

			// Check if we finished painting the trace
			if (traceStep == trace.getNSteps()) {
				obtainNewTrace = true;
			}
		} else {
			if (useCanvasBuffer) {
				canvas.begin();
				trace.paint(canvasBuffer);
				canvas.end();
			} else {
				canvas.begin();
				trace.paint();
				canvas.end();
			}

			obtainNewTrace = true;
		}

		// Update the pixel arrays if we finished painting the trace
		if (obtainNewTrace) {
			trace.setVisitedPixels(visitedPixels);
			updatePixelArrays();
		}
	}

	// Draw the result on the screen
	canvas.draw(0, 0);

	if (comparisonMode) {
		img.draw(imgWidth, 0);
	} else if (debugMode) {
		ofImage visitedPixelsImg;
		visitedPixelsImg.setFromPixels(visitedPixels);
		visitedPixelsImg.draw(imgWidth, 0);

		ofImage similarColorImg;
		similarColorImg.setFromPixels(similarColorPixels);
		similarColorImg.draw(2 * imgWidth, 0);
	}
}

//--------------------------------------------------------------
void ofApp::updatePixelArrays() {
	// Update the painted pixels
	if (useCanvasBuffer) {
		canvasBuffer.readToPixels(paintedPixels);
	} else {
		canvas.readToPixels(paintedPixels);
	}

	// Update the similar color pixels and the bad painted pixels
	const ofPixels& imgPixels = img.getPixels();
	unsigned int imgNumChannels = imgPixels.getNumChannels();
	unsigned int canvasNumChannels = paintedPixels.getNumChannels();
	nBadPaintedPixels = 0;

	for (unsigned int pixel = 0, nPixels = imgWidth * imgHeight; pixel < nPixels; ++pixel) {
		unsigned int imgPix = pixel * imgNumChannels;
		unsigned int canvasPix = pixel * canvasNumChannels;

		// Check if the pixel is well painted
		if (paintedPixels[canvasPix] != backgroundColor.r && paintedPixels[canvasPix + 1] != backgroundColor.g
				&& paintedPixels[canvasPix + 2] != backgroundColor.b
				&& abs(imgPixels[imgPix] - paintedPixels[canvasPix]) < ofxOilTrace::MAX_COLOR_DIFFERENCE[0]
				&& abs(imgPixels[imgPix + 1] - paintedPixels[canvasPix + 1]) < ofxOilTrace::MAX_COLOR_DIFFERENCE[1]
				&& abs(imgPixels[imgPix + 2] - paintedPixels[canvasPix + 2]) < ofxOilTrace::MAX_COLOR_DIFFERENCE[2]) {
			similarColorPixels[pixel] = 0;
		} else {
			similarColorPixels[pixel] = 255;
			badPaintedPixels[nBadPaintedPixels] = pixel;
			++nBadPaintedPixels;
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
