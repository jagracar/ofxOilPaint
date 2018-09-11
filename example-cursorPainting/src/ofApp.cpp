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
	// Get the canvas pixels
	canvas.readToPixels(canvasPixels);
	int width = canvasPixels.getWidth();
	int height = canvasPixels.getHeight();

	// Get the cursor current path length
	float currentPathLength = cursorPath.getPerimeter();

	// Paint the brush on the canvas, starting from the last painted point
	canvas.begin();

	while (nextPathLength < currentPathLength) {
		// Update the brush position
		const glm::vec2& pathPoint = cursorPath.getPointAtLength(nextPathLength);
		brush.updatePosition(pathPoint, true);

		// Get the bristle positions
		const vector<glm::vec2>& bristlePositions = brush.getBristlesPositions();

		// Mix the current bristle colors with the color under the bristles positions
		for (unsigned int i = 0; i < bristlePositions.size(); ++i) {
			// Check that the bristle is inside the canvas
			const glm::vec2& pos = bristlePositions[i];
			int x = pos.x;
			int y = pos.y;

			if (x >= 0 && x < width && y >= 0 && y < height) {
				// Get the color under the bristle
				const ofColor& color = canvasPixels.getColor(x, y);

				// Check if we are over a pixel that has been painted already
				if (color != backgroundColor) {
					// Mix the current bristle color with the painted color
					currentBristleColors[i].lerp(color, 0.001);

					// Add some of the initial color
					currentBristleColors[i].lerp(initialBristleColors[i], 0.001);
				} else {
					// Add some of the initial color
					currentBristleColors[i].lerp(initialBristleColors[i], 0.05);
				}
			}
		}

		// Decrease the alpha value in each step
		alphaValue -= 1;

		// Paint the brush on the canvas
		if (alphaValue > 0) {
			brush.paint(currentBristleColors, alphaValue);
		}

		// Move to the next path length value
		nextPathLength += 1;
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
	// Check that we moved enough
	glm::vec2 mousePos = glm::vec2(x, y);

	if (glm::distance(mousePos, lastAddedPoint) > 2.5) {
		// Add the point to the path
		cursorPath.curveTo(mousePos.x, mousePos.y);

		// Save the last added point position
		lastAddedPoint = mousePos;
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	// Create a new brush
	glm::vec2 mousePos = glm::vec2(x, y);
	brush = ofxOilBrush(mousePos, ofRandom(50, 70));

	// Calculate the brush bristles colors
	initialBristleColors.clear();
	float hueValue = ofRandom(255);

	for (unsigned int i = 0, nBristles = brush.getNBristles(); i < nBristles; ++i) {
		initialBristleColors.push_back(ofColor::fromHsb(hueValue, 200, 180 + ofRandom(-10, 10)));
	}

	currentBristleColors = initialBristleColors;

	// Set the initial alpha value
	alphaValue = 255;

	// Start a new cursor path at the mouse position
	cursorPath.clear();
	cursorPath.curveTo(mousePos.x, mousePos.y);
	lastAddedPoint = mousePos;

	// Reset the next path length variable
	nextPathLength = 0;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	// Finish the cursor path
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
