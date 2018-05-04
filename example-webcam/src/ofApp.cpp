#include "ofApp.h"
#include "ofxOilPaint.h"

//--------------------------------------------------------------
void ofApp::setup() {
	// Start the webcam
	webcam.setDeviceID(0);
	webcam.setDesiredFrameRate(webcamFrameRate);
	webcam.setup(webcamWidth, webcamHeight);

	// Resize the application window
	if (comparisonMode) {
		ofSetWindowShape(2 * webcamWidth, webcamHeight);
	} else {
		ofSetWindowShape(webcamWidth, webcamHeight);
	}

	// Initialize the image
	img.allocate(webcamWidth, webcamHeight, OF_IMAGE_COLOR);
	imgWidth = img.getWidth();
	imgHeight = img.getHeight();

	// Initialize the canvas where we will paint the image
	canvas.allocate(imgWidth, imgHeight, GL_RGB, 2);
	canvas.begin();
	ofClear(backgroundColor);
	canvas.end();

	// Initialize all the pixel arrays
	paintedPixels.allocate(imgWidth, imgHeight, OF_PIXELS_RGB);
	similarColorPixels.allocate(imgWidth, imgHeight, OF_PIXELS_GRAY);
	visitedPixels.allocate(imgWidth, imgHeight, OF_PIXELS_GRAY);
	badPaintedPixels = vector<unsigned int>(imgWidth * imgHeight);
	nBadPaintedPixels = 0;
}

//--------------------------------------------------------------
void ofApp::update() {
	// Update the webcam
	webcam.update();

	// Fill the image pixels with the current webcam pixels
	img.setFromPixels(webcam.getPixels());

	// Create an oil paint for the current image
	createOilPaint();
}

//--------------------------------------------------------------
void ofApp::draw() {
	// Draw the canvas
	if (comparisonMode) {
		webcam.draw(0, 0);
		canvas.draw(imgWidth, 0);
	} else {
		canvas.draw(0, 0);
	}
}

//--------------------------------------------------------------
void ofApp::createOilPaint() {
	// Clean the canvas if necessary
	if (startWithCleanCanvas) {
		canvas.begin();
		ofClear(backgroundColor);
		canvas.end();
	}

	// Set the pixel arrays initial values
	visitedPixels.setColor(255);
	updatePixelArrays();

	// Loop until the painting is finished
	float averageBrushSize = max(smallerBrushSize, max(imgWidth, imgHeight) / 6.0f);
	bool continuePainting = true;

	while (continuePainting) {
		// Obtain a new valid trace
		ofxOilTrace trace;
		bool traceNotFound = true;
		unsigned int invalidTrajectoriesCounter = 0;
		unsigned int invalidTracesCounter = 0;

		while (traceNotFound) {
			// Check if we should stop painting
			if (averageBrushSize == smallerBrushSize
					&& (invalidTrajectoriesCounter > maxInvalidTrajectoriesForSmallerSize
							|| invalidTracesCounter > maxInvalidTracesForSmallerSize)) {
				ofLogNotice() << "Processing time = " << ofGetElapsedTimef() << " seconds";

				traceNotFound = false;
				continuePainting = false;
			} else {
				// Change the average brush size if there were too many invalid traces
				if (averageBrushSize > smallerBrushSize
						&& (invalidTrajectoriesCounter > maxInvalidTrajectories
								|| invalidTracesCounter > maxInvalidTraces)) {
					averageBrushSize = max(smallerBrushSize,
							min(averageBrushSize / brushSizeDecrement, averageBrushSize - 2));

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
						traceNotFound = false;
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

		// Paint the trace if we didn't finish the painting
		if (continuePainting) {
			// Paint the trace
			canvas.begin();
			trace.paint();
			canvas.end();

			// Update the pixel arrays
			trace.setVisitedPixels(visitedPixels);
			updatePixelArrays();
		}
	}
}

//--------------------------------------------------------------
void ofApp::updatePixelArrays() {
	// Update the painted pixels
	canvas.readToPixels(paintedPixels);

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
