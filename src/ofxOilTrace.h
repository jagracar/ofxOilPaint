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
	 * @param startingPosition the trace starting position
	 * @param nSteps the total number of steps in the trace trajectory
	 * @param speed the trace moving speed (pixels/step)
	 */
	ofxOilTrace(const ofVec2f& startingPosition = ofVec2f(), int nSteps = 20, float speed = 2);

	/**
	 * @brief Constructor
	 *
	 * @param _positions the trace trajectory positions
	 * @param _alphas the trace alpha value at each trajectory step
	 */
	ofxOilTrace(const vector<ofVec2f>& _positions, const vector<unsigned char>& _alphas);

	/**
	 * @brief Sets the trace brush size
	 *
	 * @param brushSize the brush size
	 */
	void setBrushSize(float brushSize);

	/**
	 * @brief Sets the trace average color
	 *
	 * @param color the trace average color
	 */
	void setAverageColor(const ofColor& col);

	/**
	 * @brief Checks if the trace trajectory is valid.
	 *
	 * To be valid it should fall on a region that was not painted correctly before, the fraction of visited pixels in
	 * the trace trajectory should be small, it should not fall most of the time outside the canvas, and the color
	 * changes should not be too high.
	 *
	 * @param similarColorPixels the similar color pixels array
	 * @param visitedPixels the visited pixels array
	 * @param img the painted image
	 * @return true if the trace has a valid trajectory
	 */
	bool hasValidTrajectory(const vector<bool>& similarColorPixels, const vector<bool>& visitedPixels,
			const ofImage& img) const;

	bool hasValidTrajectory(const vector<int>& maxColorDiff, const ofImage& img, const ofPixels& paintedPixels,
			const ofColor& backgroundColor) const;

	/**
	 * @brief Calculates the trace average color along the image
	 *
	 * @param img the painted image
	 */
	void calculateAverageColor(const ofImage& img);

	/**
	 * @brief Calculates the trace bristle colors
	 *
	 * @param paintedPixels the painted pixels
	 * @param backgroundColor the background color
	 */
	void calculateBristleColors(const ofPixels& paintedPixels, const ofColor& backgroundColor);

	/**
	 * @brief Calculates the trace colors
	 *
	 * @param maxColorDiff the maximum color difference between the painted image and the already painted color
	 * @param similarColor the similar color array
	 * @param img the painted image
	 * @param paintedPixels the painted pixels
	 * @param bgColor the canvas background color
	 * @return false if the region covered by the trace was already painted with similar colors, most of the trace is
	 *         outside the canvas, or drawing the trace will not improve considerably the painting
	 */
	bool improvesPainting(const vector<int>& maxColorDiff, const vector<bool>& similarColor, const ofImage& img,
			const ofPixels& paintedPixels, const ofColor& backgroundColor);

	bool improvesPainting(const vector<int>& maxColorDiff, const ofImage& img, const ofPixels& paintedPixels,
			const ofColor& backgroundColor);

	/**
	 * @brief Paints the trace
	 */
	void paint();
	void paint(vector<bool>& visitedPixels, int width, int height);

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
	int getNSteps() const;

	const vector<ofVec2f>& getPositions() const;
	const vector<vector<ofVec2f>>& getBristlePositions() const;

protected:

	/**
	 * @brief Calculates the bristles positions along the trace trajectory
	 */
	void calculateBristlePositions();

	/**
	 * @brief Calculates the image colors at the bristles positions
	 *
	 * @param img the painted image
	 */
	void calculateImgColors(const ofImage& img);

	/**
	 * @brief Calculates the painted colors at the bristles positions
	 *
	 * @param paintedPixels the painted pixels
	 * @param backgroundColor the background color
	 */
	void calculatePaintedColors(const ofPixels& paintedPixels, const ofColor& backgroundColor);

	/**
	 * @brief The trace trajectory positions
	 */
	vector<ofVec2f> positions;

	/**
	 * @brief The colors alpha values along the trajectory
	 */
	vector<unsigned char> alphas;

	/**
	 * @brief The trace brush
	 */
	ofxOilBrush brush;

	/**
	 * @brief The trace bristle positions along the trajectory
	 */
	vector<vector<ofVec2f>> bPositions;

	/**
	 * @brief The trace bristle colors along the trajectory
	 */
	ofColor averageColor;
	vector<vector<ofColor>> bColors;
	vector<vector<ofColor>> bImgColors;
	vector<vector<ofColor>> bPaintedColors;
};
