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
	 * @brief Sets how random the trace movement is
	 */
	static float NOISE_FACTOR;

	/**
	 * @brief The minimum alpha value to be considered for the trace average color calculation
	 */
	static unsigned char MIN_ALPHA;

	/**
	 * @brief The brightness relative change range between the bristles
	 */
	static float BRIGHTNESS_RELATIVE_CHANGE;

	/**
	 * @brief The typical step when the color mixing starts
	 */
	static unsigned int TYPICAL_MIX_STARTING_STEP;

	/**
	 * @brief The color mixing strength
	 */
	static float MIX_STRENGTH;

	/**
	 * @brief The maximum color difference between the painted image and the already painted color to consider it well
	 * painted
	 */
	static array<int, 3> MAX_COLOR_DIFFERENCE;

	/**
	 * @brief The maximum allowed fraction of pixels in the trace trajectory that have been visited before
	 */
	static float MAX_VISITS_FRACTION_IN_TRAJECTORY;

	/**
	 * @brief The minimum fraction of pixels in the trace trajectory that should fall inside the canvas
	 */
	static float MIN_INSIDE_FRACTION_IN_TRAJECTORY;

	/**
	 * @brief The maximum allowed fraction of pixels in the trace trajectory with colors similar to the painted image
	 */
	static float MAX_SIMILAR_COLOR_FRACTION_IN_TRAJECTORY;

	/**
	 * @brief The maximum allowed value of the colors standard deviation along the trace trajectory
	 */
	static float MAX_COLOR_STDEV_IN_TRAJECTORY;

	/**
	 * @brief The minimum fraction of pixels in the trace that should fall inside the canvas
	 */
	static float MIN_INSIDE_FRACTION;

	/**
	 * @brief The maximum fraction of pixels in the trace with colors similar to the painted image
	 */
	static float MAX_SIMILAR_COLOR_FRACTION;

	/**
	 * @brief The maximum fraction of pixels in the trace that has been painted already
	 */
	static float MAX_PAINTED_FRACTION;

	/**
	 * @brief The minimum color improvement factor of the already painted pixels required to paint the trace on the canvas
	 */
	static float MIN_COLOR_IMPROVEMENT_FACTOR;

	/**
	 * @brief The minimum improvement fraction in the number of well painted pixels to consider to paint the trace even if
	 * there is not a significant color improvement
	 */
	static float BIG_WELL_PAINTED_IMPROVEMENT_FRACTION;

	/**
	 * @brief The minimum reduction fraction in the number of bad painted pixels required to paint the trace on the canvas
	 */
	static float MIN_BAD_PAINTED_REDUCTION_FRACTION;

	/**
	 * @brief The maximum allowed fraction of pixels in the trace that were previously well painted and will be now bad painted
	 */
	static float MAX_WELL_PAINTED_DESTRUCTION_FRACTION;

	/**
	 * @brief Constructor
	 *
	 * @param startingPosition the trace starting position
	 * @param nSteps the total number of steps in the trace trajectory
	 * @param speed the trace moving speed (pixels/step)
	 */
	ofxOilTrace(const ofVec2f& startingPosition = ofVec2f(), unsigned int nSteps = 20, float speed = 2);

	/**
	 * @brief Constructor
	 *
	 * @param _positions the trace trajectory positions
	 * @param _alphas the trace alpha values at each trajectory step
	 */
	ofxOilTrace(const vector<ofVec2f>& _positions, const vector<unsigned char>& _alphas);

	/**
	 * @brief Checks if the trace trajectory falls in a region that has been visited before.
	 *
	 * @param visitedPixels the pixels object that indicates if a pixel has been visited or not
	 */
	bool alreadyVisitedTrajectory(const ofPixels& visitedPixels) const;

	/**
	 * @brief Checks if the trace trajectory is valid.
	 *
	 * To be valid it should fall on a region that was not painted correctly before, it should fall most of the
	 * time inside the canvas, and the image color changes should be small.
	 *
	 * @param img the painted image
	 * @param paintedPixels the painted pixels
	 * @param backgroundColor the background color
	 * @return true if the trace has a valid trajectory
	 */
	bool hasValidTrajectory(const ofImage& img, const ofPixels& paintedPixels, const ofColor& backgroundColor) const;

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
	void setAverageColor(const ofColor& color);

	/**
	 * @brief Calculates the trace average color along the painted image
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
	 * @brief Checks if drawing the trace will improve the overall painting
	 *
	 * Note that the calculateBristleColors method should have been run before.
	 *
	 * @param img the painted image
	 * @return false if the region covered by the trace was already painted with similar colors, most of the trace is
	 *         outside the canvas, or drawing the trace will not improve considerably the painting
	 */
	bool improvesPainting(const ofImage& img);

	/**
	 * @brief Paints the trace
	 *
	 * Note that the calculateBristleColors method should have been run before.
	 */
	void paint();

	/**
	 * @brief Paints the trace
	 *
	 * Note that the calculateBristleColors method should have been run before.
	 *
	 * @param canvasBuffer the canvas buffer where the trace should also be painted when the color exceeds a minimum
	 * alpha value
	 */
	void paint(ofFbo& canvasBuffer);

	/**
	 * @brief Paints a given step in the trace trajectory
	 *
	 * Note that the calculateBristleColors method should have been run before.
	 *
	 * @param step the trace trajectory step to paint
	 */
	void paintStep(unsigned int step);

	/**
	 * @brief Paints a given step in the trace trajectory
	 *
	 * Note that the calculateBristleColors method should have been run before.
	 *
	 * @param step the trace trajectory step to paint
	 * @param canvasBuffer the canvas buffer where the trace should also be painted when the color exceeds a minimum
	 * alpha value
	 */
	void paintStep(unsigned int step, ofFbo& canvasBuffer);

	/**
	 * @brief Paints the trace
	 *
	 * Note that the calculateBristleColors method should have been run before.
	 *
	 * @param visitedPixels the pixels object that indicates if a pixel has been visited or not
	 */
	void setVisitedPixels(ofPixels& visitedPixels);

	/**
	 * @brief Paints a given step in the trace trajectory
	 *
	 * Note that the calculateBristleColors method should have been run before.
	 *
	 * @param step the trace trajectory step to paint
	 * @param visitedPixels the pixels object that indicates if a pixel has been visited or not
	 */
	void setVisitedPixels(unsigned int step, ofPixels& visitedPixels);

	/**
	 * @brief Returns the number of steps in the trace trajectory
	 *
	 * @return the total number of steps in the trace trajectory
	 */
	unsigned int getNSteps() const;

	/**
	 * @brief Returns the trace trajectory positions
	 *
	 * @return the trace trajectory positions
	 */
	const vector<ofVec2f>& getTrajectoryPositions() const;

	/**
	 * @brief Returns the trace alpha values along the trace trajectory
	 *
	 * @return the trace alpha values along the trace trajectory
	 */
	const vector<unsigned char>& getTrajectoryAphas() const;

	/**
	 * @brief Returns the brush bristle positions along the trace trajectory
	 *
	 * @return the brush bristle positions along the trace trajectory
	 */
	const vector<vector<ofVec2f>>& getBristlePositions() const;

	/**
	 * @brief Returns the trace average color
	 *
	 * @return the trace average color
	 */
	const ofColor& getAverageColor() const;

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
	 * @brief The trace average color
	 */
	ofColor averageColor;

	/**
	 * @brief The trace brush
	 */
	ofxOilBrush brush;

	/**
	 * @brief The trace bristle positions along the trajectory
	 */
	vector<vector<ofVec2f>> bPositions;

	/**
	 * @brief The trace bristle image colors along the trajectory
	 */
	vector<vector<ofColor>> bImgColors;

	/**
	 * @brief The trace bristle painted colors along the trajectory
	 */
	vector<vector<ofColor>> bPaintedColors;

	/**
	 * @brief The trace bristle colors along the trajectory
	 */
	vector<vector<ofColor>> bColors;
};
