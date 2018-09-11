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
	ofPixels canvasPixels;
	ofxOilBrush brush;
	vector<ofColor> initialBristleColors;
	vector<ofColor> currentBristleColors;
	float alphaValue;
	ofPolyline cursorPath;
	glm::vec2 lastAddedPoint;
	float nextPathLength;
};
