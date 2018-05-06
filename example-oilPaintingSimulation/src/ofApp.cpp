#include "ofApp.h"
#include "ofxOilPaint.h"

//--------------------------------------------------------------
void ofApp::setup() {
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

	// Initialize the oil painting simulator
	simulator = ofxOilSimulator(useCanvasBuffer, true);
	simulator.setImage(img, true);
}

//--------------------------------------------------------------
void ofApp::update() {
	// Update the simulator if the painting is not finished
	if (!simulator.isFinished()) {
		simulator.update(paintStepByStep);
	}

	// Update the window title
	if (simulator.isFinished()) {
		ofSetWindowTitle("Oil painting simulation ( finished )");
	} else {
		ofSetWindowTitle("Oil painting simulation ( frame rate: " + ofToString(round(ofGetFrameRate())) + " )");
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	// Draw the result on the screen
	simulator.drawCanvas(0, 0);

	if (comparisonMode) {
		simulator.drawImage(imgWidth, 0);
	} else if (debugMode) {
		simulator.drawVisitedPixels(imgWidth, 0);
		simulator.drawSimilarColorPixels(2 * imgWidth, 0);
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
