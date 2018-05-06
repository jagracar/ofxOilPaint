#pragma once

#include "ofMain.h"
#include "ofxOilTrace.h"

/**
 * @brief Class used to simulate an oil paint
 *
 * @author Javier Graciá Carpio
 */
class ofxOilSimulator {
public:

	/**
	 * @brief The smaller brush size allowed
	 */
	static float SMALLER_BRUSH_SIZE;

	/**
	 * @brief The brush size decrement ratio
	 */
	static float BRUSH_SIZE_DECREMENT;

	/**
	 * @brief The maximum number of invalid trajectories allowed before the brush size is reduced
	 */
	static unsigned int MAX_INVALID_TRAJECTORIES;

	/**
	 * @brief The maximum number of invalid trajectories allowed for the smaller brush size before the painting is stopped
	 */
	static unsigned int MAX_INVALID_TRAJECTORIES_FOR_SMALLER_SIZE;

	/**
	 * @brief The maximum number of invalid traces allowed before the brush size is reduced
	 */
	static unsigned int MAX_INVALID_TRACES;

	/**
	 * @brief The maximum number of invalid traces allowed for the smaller brush size before the painting is stopped
	 */
	static unsigned int MAX_INVALID_TRACES_FOR_SMALLER_SIZE;

	/**
	 * @brief The trace speed in pixels/step
	 */
	static float TRACE_SPEED;

	/**
	 * @brief The typical trace length, relative to the brush size
	 */
	static float RELATIVE_TRACE_LENGTH;

	/**
	 * @brief The minimum trace length allowed
	 */
	static float MIN_TRACE_LENGTH;

	/**
	 * @brief The canvas background color
	 */
	static ofColor BACKGROUND_COLOR;

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
	 */
	ofxOilSimulator(bool _useCanvasBuffer = true, bool _verbose = true);

	void setImagePixels(const ofPixels& mewImgPixels, bool clearCanvas);
	void setImage(const ofImage& mewImg, bool clearCanvas);
	void update(bool stepByStep);
	void drawCanvas(float x, float y) const;
	void drawImage(float x, float y) const;
	void drawVisitedPixels(float x, float y) const;
	void drawSimilarColorPixels(float x, float y) const;
	bool isFinished() const;

protected:
	void updatePixelArrays();
	void updateVisitedPixels();
	void getNewTrace();

	/**
	 * @brief Checks if the trace trajectory falls in a region that has been visited before.
	 *
	 * @return true if the trace trajectory falls in a region that has been visited before
	 */
	bool alreadyVisitedTrajectory() const;

	/**
	 * @brief Checks if the trace trajectory is valid.
	 *
	 * To be valid it should fall on a region that was not painted correctly before, it should fall most of the
	 * time inside the canvas, and the image color changes should be small.
	 *
	 * @return true if the trace has a valid trajectory
	 */
	bool validTrajectory() const;

	/**
	 * @brief Checks if drawing the trace will improve the overall painting
	 *
	 * Note that the calculateBristleColors method should have been run before.
	 *
	 * @return false if the region covered by the trace was already painted with similar colors, most of the trace is
	 *         outside the canvas, or drawing the trace will not improve considerably the painting
	 */
	bool improvesPainting();

	void paintTrace();
	void paintTraceStep();

	bool useCanvasBuffer;
	bool verbose;
	ofImage img;
	ofFbo canvas;
	ofFbo canvasBuffer;
	ofPixels paintedPixels;
	ofPixels similarColorPixels;
	ofPixels visitedPixels;
	vector<unsigned int> badPaintedPixels;
	unsigned int nBadPaintedPixels;
	float averageBrushSize;
	bool paintingIsFinised;
	bool obtainNewTrace;
	ofxOilTrace trace;
	unsigned int traceStep;
	unsigned int nTraces;
};
