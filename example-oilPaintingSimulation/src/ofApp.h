#pragma once

#include "ofMain.h"
#include "ofxOilPaint.h"

class ofApp: public ofBaseApp {
public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	// The path to the picture that we want to paint
	string pictureFile = "picture.jpg";
	// The size reduction factor between the input picture and the final painting
	float sizeReductionFactor = 1.0;
	// Use a separate canvas buffer for color mixing (a bit slower)
	bool useCanvasBuffer = true;
	// Compare the oil paint simulation with the input picture
	bool comparisonMode = false;
	// Show additional debug images
	bool debugMode = true;
	// Paint the traces step by step, or in one go
	bool paintStepByStep = true;

	// Application variables
	ofImage img;
	int imgWidth;
	int imgHeight;
	ofxOilSimulator simulator;
};
