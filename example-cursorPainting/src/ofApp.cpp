#include "ofApp.h"
#include "ofxOilPaint.h"

//--------------------------------------------------------------
void ofApp::setup() {
	// Set the background color
	backgroundColor = ofColor(255);

	// Initialize the canvas where we are going to paint
	canvas.allocate(ofGetWidth(), ofGetHeight(), GL_RGB, 3);
	canvas.begin();
	ofClear(backgroundColor);
	canvas.end();

	// Initialize the application variables
	alphaValue = 0;
	nextPathLength = 0;
}

//--------------------------------------------------------------
void ofApp::update() {
	// Get the canvas painted pixels
	canvas.readToPixels(paintedPixels);
	int width = paintedPixels.getWidth();
	int height = paintedPixels.getHeight();

	// Get the cursor path total length
	float totalPathLength = cursorPath.getLengthAtIndex(cursorPath.size() - 1);

	// Paint the brush on the canvas step by step
	canvas.begin();

	while (nextPathLength < totalPathLength) {
		// Update the brush position
		const ofVec2f& newPosition = cursorPath.getPointAtLength(nextPathLength);
		brush.updatePosition(newPosition, true);

		// Get the bristle positions
		const vector<ofVec2f>& bristlePositions = brush.getBristlesPositions();

		// Mix the bristle colors with the color under the bristles positions
		for (unsigned int i = 0; i < bristlePositions.size(); ++i) {
			// Check that the bristle is inside the canvas
			const ofVec2f& pos = bristlePositions[i];
			int x = pos.x;
			int y = pos.y;

			if (x >= 0 && x < width && y >= 0 && y < height) {
				// Get the painted color under the bristle
				const ofColor& color = paintedPixels.getColor(x, y);

				if (color != backgroundColor) {
					// Mix the current bristle color with the painted color
					bristleColors[i].lerp(color, 0.005 * color.getSaturation() / 255);
				}
			}
		}

		// Decrease the alpha value in each step
		alphaValue -= 0.01 * nextPathLength;

		// Paint the brush on the canvas
		if (alphaValue > 0) {
			brush.paint(bristleColors, alphaValue);
		}

		// Move to the next path length value
		++nextPathLength;
	}

	canvas.end();
}

//--------------------------------------------------------------
void ofApp::draw() {
	// Paint the canvas on the application window
	canvas.draw(0, 0);
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
	// Check that we moved considerably
	ofVec2f mousePos = ofVec2f(x, y);

	if (cursorPath.getPointAtLength(nextPathLength).squareDistance(mousePos) > 5) {
		cursorPath.curveTo(mousePos);
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	// Create a new brush
	brush = ofxOilBrush(ofVec2f(x, y), ofRandom(50, 70));

	// Calculate the brush bristles colors
	bristleColors.clear();
	float hueValue = ofRandom(0, 255);

	for (unsigned int i = 0, nBristles = brush.getNBristles(); i < nBristles; ++i) {
		bristleColors.push_back(ofColor::fromHsb(hueValue, 200, 200 + ofRandom(-5, 5)));
	}

	// Set the initial alpha value
	alphaValue = 200;

	// Start a new cursor path at the mouse position
	cursorPath.clear();
	cursorPath.curveTo(x, y);
	cursorPath.curveTo(x, y);

	// Reset the next path length variable
	nextPathLength = 0;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	// Finish the cursor path
	cursorPath.curveTo(x, y);
	cursorPath.curveTo(x, y);
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
