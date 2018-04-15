#pragma once

#include "ofMain.h"
#include "ofxOilBrush.h"

/**
 * @brief Class that simulates the movement of a brush on the canvas
 *
 * @author Javier Graciá Carpio
 */
class ofxOilTrace {
public:

	/**
	 * @brief Constructor
	 *
	 * @param position the trace initial position
	 * @param nSteps the total number of steps in the trace trajectory
	 * @param speed the trace moving speed
	 */
	ofxOilTrace(const ofVec2f& position = ofVec2f(), int nSteps = 20, float speed = 2);

	/**
	 * @brief Defines the brush size that should be used to paint the trace
	 *
	 * @param brushSize the brush size
	 */
	void setBrushSize(float brushSize);

	/**
	 * @brief Returns the image average color along the trace
	 *
	 * @param img the image
	 * @return the image average color
	 */
	ofColor calculateAverageColor(const ofImage& img);

	/**
	 * @brief Calculates the trace bristle colors
	 *
	 * @param col the trace initial color
	 */
	void calculateColors(const ofColor& col);

	/**
	 * @brief Paints the trace
	 */
	void paint();

	/**
	 * @brief Paints the trace for a given trajectory step
	 *
	 * @param step the trajectory step
	 */
	void paintStep(int step);

	/**
	 * @brief Returns the number of steps in the trace trajectory
	 *
	 * @return the total number of steps in the trace trajectory
	 */
	int getNSteps();

protected:

	/**
	 * @brief The trace trajectory positions
	 */
	vector<ofVec2f> positions;

	/**
	 * @brief The trace bristle colors along the trajectory
	 */
	vector<vector<ofColor>> colors;

	/**
	 * @brief The colors alpha values along the trajectory
	 */
	vector<unsigned char> alphas;

	/**
	 * @brief The trace brush
	 */
	ofxOilBrush brush;
};
