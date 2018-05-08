#include "ofApp.h"
#include "ofxOilPaint.h"

//--------------------------------------------------------------
void ofApp::setup() {
	// Load and start the video
	video.load(videoFile);
	video.play();
	video.setPaused(true);

	// Select the image size
	imgWidth = video.getWidth() / sizeReductionFactor;
	imgHeight = video.getHeight() / sizeReductionFactor;

	// Resize the application window
	if (comparisonMode) {
		ofSetWindowShape(2 * imgWidth, imgHeight);
	} else {
		ofSetWindowShape(imgWidth, imgHeight);
	}

	// Initialize the oil painting simulator
	simulator = ofxOilSimulator(false, false);

	// Change some of the simulator default parameters
	ofxOilSimulator::MAX_COLOR_DIFFERENCE = {60, 60, 60};
}

//--------------------------------------------------------------
void ofApp::update() {
	// Move the video to the next frame
	video.setFrame(ofGetFrameNum() % video.getTotalNumFrames());
	video.update();

	// Save an image of the current frame
	img.setFromPixels(video.getPixels());
	img.resize(imgWidth, imgHeight);

	// Obtain an oil paint of the current image
	simulator.setImage(img, startWithCleanCanvas);

	while (!simulator.isFinished()) {
		simulator.update(false);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	// Draw the result on the screen
	simulator.drawCanvas(0, 0);

	if (comparisonMode) {
		simulator.drawImage(imgWidth, 0);
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
