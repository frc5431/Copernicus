/*------------------------------------------------------------------------------||
|                                                                                |
| Copyright (C) 2017 by Titan Robotics                                           |
| License Date: 01/23/2017                                                       |
| Modifiers: none                                                                |
|                                                                                |
| Permission is hereby granted, free of charge, to any person obtaining a copy   |
| of this software and associated documentation files (the "Software"), to deal  |
| in the Software without restriction, including without limitation the rights   |
| to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      |
| copies of the Software, and to permit persons to whom the Software is          |
| furnished to do so, subject to the following conditions:                       |
|                                                                                |
| The above copyright notice and this permission notice shall be included in all |
| copies or substantial portions of the Software.                                |
|                                                                                |
| THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     |
| IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       |
| FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    |
| AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         |
| LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  |
| OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  |
| SOFTWARE.                                                                      |
|                                                                                |
||------------------------------------------------------------------------------*/

/**
 *
 * @file processing.hpp
 * @date 01/23/2017
 * @brief Header to control the processing of Perception
 *
 * @details This header file contains all the definitions for the vision
 * processing of Perception and its components. The reason why the functions
 * are just in a namespace is to shorten the compile time by just the symbols
 * so we don't have to recompile main code every single time.
 */

//OPENCV INCLUDES
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/video.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/calib3d.hpp>

//BOOST INCLUDES
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

//STANDARD INCLUDES
#include <functional>
#include <memory>
#include <vector>

#define _USE_MATH_DEFINES 1

#include <cmath>

//NTCORE INCLUDES
#include <networktables/NetworkTable.h>

//PERCEPTION INCLUDES
//OLDSRC
//#include "kinect.hpp"
#include "log.hpp"

#ifndef INCLUDE_PROCESSING_HPP_
#define INCLUDE_PROCESSING_HPP_

#define MUTEX_S_LOCK(X) boost::mutex::scoped_lock(X)

//Settings definitions
#define PROCESSING_LOG_TAG			"PROCER" //Logger processing namespace
#define PROCESSING_LOOP_TIME		2000 //Milliseconds to delay between each setting loop
#define PROCESSING_LOOP_TAG			"proc_time" //Network table processing tag

//PreProcessing definitions
#define PROCESSING_MIN_THRESH_H		49 //Minimum Hue value
#define PROCESSING_MIN_THRESH_S		100 //Minimum Saturation value
#define PROCESSING_MIN_THRESH_V		46 //Minimum Value value

#define PROCESSING_MAX_THRESH_H		82 //Maximum Hue value
#define PROCESSING_MAX_THRESH_S		255 //Maximum Saturation value
#define PROCESSING_MAX_THRESH_V		201 //Maximum Value value

#define PROCESSING_MEDIAN_BLUR		7 //The median blur level (must be an odd number)
#define PROCESSING_DILATION_SIZE	1 //The dilation size for target detection

//Extra details for the boiler
#define PROCESSING_BOILER_HEIGHT	80.25f //The boiler tape centered height in inches	
#define PROCESSING_BOILER_OFFSET	10.2f //The distance (in inches) that the base of the boiler is from the tape
//Extra details for camera
#define PROCESSING_CAMERA_WIDTH		800.0f //Camera dimension width in pixels
#define PROCESSING_CAMERA_HEIGHT	600.0f //Camera dimension height in pixels
#define PROCESSING_CAMERA_FPS		15.0f //Camera stream fps
#define PROCESSING_CAMERA_FOV 		71.63f //Camera field of view in degrees
#define PROCESSING_CAMERA_FOCAL		2.8f //The camera focal length in millimeters
#define PROCESSING_CAMERA_VFOV		51.35f //The camera vertical field of view
#define PROCESSING_CAMERA_ANGLE		50.9f //The current angle of the camera (Back is positive and forward is negative angle)
#define PROCESSING_CAMERA_R_HEIGHT	23.75f //The current camera height from the ground to the center of the lense
#define PROCESSING_CAMERA_DEG_PIXEL				(PROCESSING_CAMERA_FOV / PROCESSING_CAMERA_WIDTH) //Camera degrees per pixel (IT'S FOV DIVIDED BY PIXEL WIDTH)
#define PROCESSING_CAMERA_VDEG_PIXEL			(PROCESSING_CAMERA_VFOV / PROCESSING_CAMERA_HEIGHT) //Camera degreed per pixel vertically
#define PROCESSING_CAMERA_HALF_SCREEN			((PROCESSING_CAMERA_WIDTH / 2.0f) - 0.5f) //Camera half screen angle calculation
/*#define PROCESSING_CAMERA_UNDISTORT				{270.8238366, 0.0, 157.45643041, 0.0, \
272.6589298370, 129.04290129, 0.0, 0.0, 1.0}
#define PROCESSING_CAMERA_D_UNDISTORT			{0.0, 0.0, 0.0, 0.0}
#define PROCESSING_CAMERA_GET_OPTIMAL(X, Y)		cv::getOptimalNewCameraMatrix(X, Y, \
cv::Size(PROCESSING_CAMERA_WIDTH, PROCESSING_CAMERA_HEIGHT), 0)*/
#define PROCESSING_CAMERA_HALF_VERT				(PROCESSING_CAMERA_VFOV / 2) //Vertical camera split height
#define PROCESSING_CAMERA_HEIGHT_DIFF			(PROCESSING_BOILER_HEIGHT - PROCESSING_CAMERA_R_HEIGHT) //The camera delta difference from the boiler height and camera ground height
#define PROCESSING_CAMERA_REAL_ANGLE			(180 - (PROCESSING_CAMERA_ANGLE + (90 - PROCESSING_CAMERA_HALF_VERT) + PROCESSING_CAMERA_VFOV)) // 15//;	(PROCESSING_CAMERA_ANGLE - PROCESSING_CAMERA_HALF_VERT) //The camera displacement angle in real life
#define PROCESSING_GET_DISTANCE(X)				(PROCESSING_CAMERA_HEIGHT_DIFF / (tan((M_PI * (X + PROCESSING_CAMERA_REAL_ANGLE)) / 180))) - PROCESSING_BOILER_OFFSET //Get the distance of the robot from the tower
#define PROCESSING_GET_DEGREES(X)				(X - PROCESSING_CAMERA_HALF_SCREEN) * PROCESSING_CAMERA_DEG_PIXEL //Get current angle horizontally
#define PROCESSING_GET_VDEGREES(X)				(X * PROCESSING_CAMERA_VDEG_PIXEL) //Get current angle vertically

//Processing definitions to check if the contour could be a possible target
#define PROCESSING_MIN_AREA			300 //Minimum pixels squared of target
#define PROCESSING_MAX_AREA			3000 //Maximum pixels squared of target

//We don't want blobs so lets put a minimum and maximum blob count
#define PROCESSING_MIN_SIDES		2 //Minimum side count for contour
#define PROCESSING_MAX_SIDES		9 //Maximum side count for contour

//Fix obscurely long contours (ratio calculator should handle it on its own)
#define PROCESSING_MIN_PERIM		25 //Minimum perimeter length of contour in pixels
#define PROCESSING_MAX_PERIM		2000 //Maximum perimeter length of contour in pixels

//Remove obscure contours that will ruin our ratio calculation
#define PROCESSING_MAX_HEIGHT		500 //Maximum height of contour in pixels
#define PROCESSING_WIDTH_RATIO		1.1f //The width to height ratio

//Region of interest
#define PROCESSING_MID_MIN_Y		0 //(PROCESSING_CAMERA_HEIGHT / 6) //Minimum pixel height for contour to be in
#define PROCESSING_MID_MAX_Y		PROCESSING_CAMERA_HEIGHT //(PROCESSING_CAMERA_HEIGHT - 20) //Maximium pixel height for contour to be in

//Double tape detection and ratio calculations
#define PROCESSING_TAPE_WIDTH		16.0f //How many inches wide is the real tape
#define PROCESSING_TAPE_HEIGHT		2.0f //How many inches tall is the tape
#define PROCESSING_TAPE_VERT_DIFF	7.0f //The vertical inches differential between the tapes
#define PROCESSING_TAPE_FIX_DIFF	20 //The amount of pixels (vertically) to accommodate for the errors in the height differential
#define PROCESSING_WIDTH_FIX_DIFF	50 //The amount of pixel width differential allowed between the higher and lower object
#define PROCESSING_OFFSET_FIX_DIFF	30 //The amount of pixels difference allowed in the x coordinate location (offsets)

//Declare processing namespace
namespace processing {

	class Target {
	public:
		unsigned int area, sides, perim, x, y;
		float width, height, distance, horz_angle, vert_angle;
		bool found;
	};

	//All documentation provided in source file
	template<typename T>
	void inline PLOG(T, bool error = false);

	//OLDSRC
	//void kinectDepth2Mat(cv::Mat &, const kinect::DepthData &, const float);
	//void kinectScalar(cv::Mat &, cv::Mat &);

	void preProcessing(cv::Mat &, cv::Mat &);
	void processFrame(cv::Mat &, std::vector<Target> &, cv::Mat &);
	void processTargets(std::vector<Target> &, Target &);
	void settingsUpdate(std::shared_ptr<NetworkTable>);
} /* namespace processing */

#endif /* INCLUDE_PROCESSING_HPP_ */
