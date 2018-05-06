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

	// The webcam frame width to use
	int webcamWidth = 320;
	// The webcam frame height to use
	int webcamHeight = 240;
	// The webcam frame rate to use
	int webcamFrameRate = 30;
	// Paint each picture with a clean canvas
	bool startWithCleanCanvas = false;
	// Compare the oil paint simulation with the webcam picture
	bool comparisonMode = true;

	// Application variables
	ofVideoGrabber webcam;
	ofxOilSimulator simulator;
};
