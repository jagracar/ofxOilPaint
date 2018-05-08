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

	// The path to the video that we want to paint
	// https://archive.org/details/0536_Westinghouse_Travelers_Choice_66_18_51_40_25
	string videoFile = "westinghouse.mov";
	// The size reduction factor between the input video and the final painting
	float sizeReductionFactor = 1.0;
	// Use a separate canvas buffer for color mixing (a bit slower)
	bool useCanvasBuffer = false;
	// Paint each picture with a clean canvas
	bool startWithCleanCanvas = false;
	// Compare the oil paint simulation with the video picture
	bool comparisonMode = true;

	// Application variables
	ofVideoPlayer video;
	ofImage img;
	int imgWidth;
	int imgHeight;
	ofxOilSimulator simulator;
};
