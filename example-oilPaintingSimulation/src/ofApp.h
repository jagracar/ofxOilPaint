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

	// Updates the pixels arrays used in the simulation
	void updatePixelArrays();

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
	// Save a screenshot when the painting is finished
	bool saveFinalScreenshot = false;
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
	ofImage img;
	int imgWidth;
	int imgHeight;
	ofFbo canvas;
	ofFbo canvasBuffer;
	ofPixels paintedPixels;
	ofPixels similarColorPixels;
	ofPixels visitedPixels;
	vector<unsigned int> badPaintedPixels;
	unsigned int nBadPaintedPixels;
	float averageBrushSize;
	bool continuePainting;
	bool obtainNewTrace;
	ofxOilTrace trace;
	unsigned int traceStep;
	unsigned int nTraces;
};
