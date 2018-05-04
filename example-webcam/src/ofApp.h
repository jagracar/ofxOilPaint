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

	// Creates an oil paint from the current image
	void createOilPaint();

	// Updates the pixels arrays used in the oil paint simulation
	void updatePixelArrays();

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
	// The smaller brush size allowed
	float smallerBrushSize = 4;
	// The brush size decrement ratio
	float brushSizeDecrement = 1.3;
	// The maximum number of invalid trajectories allowed before the brush size is reduced
	unsigned int maxInvalidTrajectories = 5000;
	// The maximum number of invalid trajectories allowed for the smaller brush size before the painting is stopped
	unsigned int maxInvalidTrajectoriesForSmallerSize = 10000;
	// The maximum number of invalid traces allowed before the brush size is reduced
	unsigned int maxInvalidTraces = 250;
	// The maximum number of invalid traces allowed for the smaller brush size before the painting is stopped
	unsigned int maxInvalidTracesForSmallerSize = 350;
	// The trace speed in pixels/step
	float traceSpeed = 2;
	// The typical trace length, relative to the brush size
	float relativeTraceLength = 2.3;
	// The minimum trace length allowed
	float minTraceLength = 16;
	// The canvas background color
	ofColor backgroundColor = ofColor(255);

	// Application variables
    ofVideoGrabber webcam;
	ofImage img;
	int imgWidth;
	int imgHeight;
	ofFbo canvas;
	ofPixels paintedPixels;
	ofPixels similarColorPixels;
	ofPixels visitedPixels;
	vector<unsigned int> badPaintedPixels;
	unsigned int nBadPaintedPixels;
};
