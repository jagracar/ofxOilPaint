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

	ofColor backgroundColor;
	ofFbo canvas;
	ofxOilBrush brush;
	vector<ofColor> bristleColors;
	float alphaValue;
	ofPolyline cursorPath;
	float nextPathLength;
	ofPixels paintedPixels;
};
