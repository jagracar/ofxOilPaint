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

	// Initialize the oil painting simulator
	simulator = ofxOilSimulator(false, false);
}

//--------------------------------------------------------------
void ofApp::update() {
	// Update the webcam
	webcam.update();

	// Obtain an oil paint of the current webcam image
	simulator.setImagePixels(webcam.getPixels(), startWithCleanCanvas);

	while (!simulator.isFinished()) {
		simulator.update(false);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	// Draw the result on the screen
	simulator.drawCanvas(0, 0);

	if (comparisonMode) {
		simulator.drawImage(webcamWidth, 0);
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
