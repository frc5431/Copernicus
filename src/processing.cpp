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
 * @file processing.cpp
 * @date 01/23/2017
 * @brief Source file for the processing of Perception
 *
 * @details This source file contains all the definitions for the vision
 * processing of Perception and its components.
 */

#include "../include/processing.hpp"

/*
 * Everything labeled OLDSRC is for the kinect version of the code
 * which is currently deprecated, if other teams want to try it out
 * just uncomment the OLDSRC headed parts.
 */

//OLDSRC
/*
//Depth processing and checking
#define PROCESSING_DEPTH_Y_OFFSET	-20 //Subtract 20 pixels from target center
#define PROCESSING_DEPTH_MINIMUM	2000 //Minimum distance in millimeters to be from target
#define PROCESSING_DEPTH_MAXIMUM	4500 //Maximum distance in millimeters to be from target
*/


//Processing namespace (@TODO Split off common functions in separate source files to decrease compile time) 
namespace processing {

	//Mutex lock for settings
	boost::mutex proc_mutex;
	//float undistort_k_arr[] = PROCESSING_CAMERA_UNDISTORT;
	//float undistort_d_arr[] = PROCESSING_CAMERA_D_UNDISTORT;

	//cv::Mat undistort_K = cv::Mat(3, 3, CV_32F, undistort_k_arr);
	//cv::Mat undistort_D = cv::Mat(1, 4, CV_32F, undistort_d_arr);
	//cv::Mat undistort_O = PROCESSING_CAMERA_GET_OPTIMAL(undistort_K, undistort_D);

	//Settings for processing
	unsigned int loop_time = PROCESSING_LOOP_TIME,
				 threshMinH = PROCESSING_MIN_THRESH_H,
				 threshMinS = PROCESSING_MIN_THRESH_S,
				 threshMinV = PROCESSING_MIN_THRESH_V,
				 threshMaxH = PROCESSING_MAX_THRESH_H,
				 threshMaxS = PROCESSING_MAX_THRESH_S,
				 threshMaxV = PROCESSING_MAX_THRESH_V,
				 medianBlue = PROCESSING_MEDIAN_BLUR,
				 dilationSize = PROCESSING_DILATION_SIZE,
				 minArea = PROCESSING_MIN_AREA,
				 maxArea = PROCESSING_MAX_AREA,
				 minSides = PROCESSING_MIN_SIDES,
				 maxSides = PROCESSING_MAX_SIDES,
				 minPerim = PROCESSING_MIN_PERIM,
				 maxPerim = PROCESSING_MAX_PERIM,
				 midYMin = PROCESSING_MID_MIN_Y,
				 midYMax = PROCESSING_MID_MAX_Y;
			
				//OLDSRC 
				//depthYOffset = PROCESSING_DEPTH_Y_OFFSET;

	float maxHeight = PROCESSING_MAX_HEIGHT,
		  widthRatio = PROCESSING_WIDTH_RATIO,
		  tapeWidth = PROCESSING_TAPE_WIDTH,
		  tapeHeight = PROCESSING_TAPE_HEIGHT,
		  tapeVertDist = PROCESSING_TAPE_VERT_DIFF,
		  tapeVertFix = PROCESSING_TAPE_FIX_DIFF,
		  tapeHorzFix = PROCESSING_OFFSET_FIX_DIFF,
		  tapeWidthFix = PROCESSING_WIDTH_FIX_DIFF;

	template<typename T>
	void inline PLOG(T toLog, bool error) {
		Logger::Log(PROCESSING_LOG_TAG, SW(toLog), error);
	}

	//OLDSRC
	/*void kinectDepth2Mat(cv::Mat &frame, const kinect::DepthData &depth, const float maxDepth) {
		frame = cv::Mat(depth.height, depth.width, CV_32FC1, depth.depthRaw) / maxDepth;
	}

	void kinectScalar(cv::Mat &frame, cv::Mat &newFrame) {
		frame.convertTo(newFrame, CV_8UC3);
	}*/

	void preProcessing(cv::Mat &frame, cv::Mat &newFrame) {
		//Undistort the camera image using the undistort matrix
		//cv::fisheye::undistortImage(frame, frame, undistort_K, 
		//	undistort_D, undistort_O);

		if(frame.empty()) return;
		//Logger::LogWindow("Undistorted", frame);

		//Turn the BGR image into HSV
		cv::cvtColor(frame, frame, CV_BGR2HSV);

		//HSV threshold
		cv::inRange(frame, cv::Scalar(threshMinH, threshMinS, threshMinV), 
						cv::Scalar(threshMaxH, threshMaxS, threshMaxV), newFrame);

		//Blur the threshed image to remove random depth static
		cv::medianBlur(newFrame, newFrame, PROCESSING_MEDIAN_BLUR);

		//Smooth back the thicker parts of the image to crisp out the target
		cv::Mat dilater = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * dilationSize + 1,
					2 * dilationSize + 1),cv::Point(dilationSize, dilationSize));

		//Apply erode method
		cv::dilate(newFrame, newFrame, dilater);
	}

	void processFrame(cv::Mat &frame, std::vector<Target> &targets, cv::Mat &contoured) {

		//Create the contour hierarchical status
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;

		//Find all contours on processed frame
		cv::findContours(frame, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

		unsigned int ind = 0; //Set contour count

		contoured = cv::Mat::zeros(frame.size(), CV_8UC3);

		//Loop through each contour found
		for(std::vector<cv::Point> contour : contours) {

			//Get the current contour area
			unsigned int contour_area = cv::contourArea(contour);

			//Check if the contour could be a possible target match with the area
			if(contour_area >= minArea && contour_area <= maxArea) {
				//Create an approx poly
				std::vector<cv::Point> approx;

				//Find the approx poly
				cv::approxPolyDP(contour, approx, 5, true);

				//Get the current poly side count
				unsigned int contour_sides = approx.size();

				//Check to see if the contour matches the correct side count
				if(contour_sides >= minSides && contour_sides <= maxSides) {

					//Get the contour perimeter size
					unsigned int contour_perim = cv::arcLength(contour, true);

					//Check to see if the perimeter length is the correct size
					if(contour_perim >= minPerim && contour_perim <= maxPerim) {

						//Get bounding rectangle for contour
						cv::Rect contour_bound = cv::boundingRect(contour);

						//Get contour bounding dimensions
						float contour_width = static_cast<float>(contour_bound.width);
						float contour_height = static_cast<float>(contour_bound.height);

						//Get the target width for a rectangle
						float targetX = contour_height * widthRatio;

						//Check to see if it's a valid target with the proper aspect ratio
						if(contour_width >= targetX && contour_height <= maxHeight) {

							//Get the moments of the contour
							cv::Moments mu = cv::moments(contour, false);

							//Get the mass center of the contour for both X and Y
							unsigned int contour_x = (mu.m10 / mu.m00);
							unsigned int r_contour_y = (mu.m01 / mu.m00);
							unsigned int contour_y = static_cast<int>(PROCESSING_CAMERA_HEIGHT) - r_contour_y;

							//Check to see if the contour is within our region of interest
							if(contour_y >= midYMin && contour_y <= midYMax) {

								//OLDSRC
								//Get current contour depth
								//int contour_depth = depthData.getDepth(&frame, contour_x, (contour_y + depthYOffset));

								//Add contour to possible target object
								Target target;
								target.area = contour_area;
								target.width = contour_width;
								target.height = contour_height;
								target.perim = contour_perim;
								target.sides = contour_sides;
								target.x = contour_x;
								target.y = contour_y;

								//OLDSRC
								//target.depth = contour_depth;

								//Draw a contour index for the found contour 
								cv::Scalar color = cv::Scalar(255, 0, 0);
								cv::drawContours(contoured, contours, ind, color, 2, 8, hierarchy, 0, cv::Point());
								cv::putText(contoured, boost::lexical_cast<std::string>(ind), cv::Point(contour_x - 10, r_contour_y),
									CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(255, 255, 255));

								//Add current target to target calculation list
								targets.push_back(target);
							}

						}

					}

				}
			}

			ind++; //Add to current contour index
		}
	}

	void processTargets(std::vector<Target> &targets, Target &finalTarget) {
		unsigned int target_ind = 0, secondary_ind = 0;

		float differencial_calculation = PROCESSING_TAPE_VERT_DIFF / PROCESSING_TAPE_WIDTH;
		//@TODO add height differential calculation as a secondary check to the width ratio since the target might be temporarily cut off

		Target toRet;
		toRet.found = false;

		bool target_found = false;

		for(Target target : targets) {
			secondary_ind =0;

			for(Target second_target : targets) {

				//Don't continue the loop since the target widths are different and that the first target is higher than the other and if they're near the same X pixel location

			if(fabs(target.width - second_target.width) < PROCESSING_WIDTH_FIX_DIFF &&
						target.y > second_target.y && fabs(target.x - target.x) < PROCESSING_OFFSET_FIX_DIFF) {
					//Absolute target vertical differential
					float height_differential = fabs(target.width * differencial_calculation); //Width to wanted height in pixels
					float target_differential = fabs(target.y - second_target.y); //Get current target height difference

					//Get the current difference between the real height and calculated height
					float calculated_differential = fabs(height_differential - target_differential);

					//If the differential is within our error limit then we have found both retro reflective tape targets
					if((calculated_differential) < PROCESSING_WIDTH_FIX_DIFF) {
						//Get center point between two targets						
						float new_y = (target.y + second_target.y) / 2.0f;
						float new_x = (target.x + second_target.x) / 2.0f;
						float new_width = (target.width + second_target.width) / 2.0f;
						float new_height = (target.height + second_target.height) / 2.0f;
						float vertical_dist = fabs(target.y - second_target.y);

						toRet.found = true;
						toRet.x = new_x;
						toRet.y = new_y;
						toRet.width = new_width;
						toRet.height = new_height;

						float degrees = PROCESSING_GET_DEGREES(new_x); //Get horizontal degrees from target
						float vert_degrees = PROCESSING_GET_VDEGREES(new_y); //Get vertical degrees from target
						float distance = PROCESSING_GET_DISTANCE(vert_degrees); //Get the distance of the robot from the tower
						std::cout << "FOUND TARGET PAIRS ONE: " << target_ind << " TWO: "
						 << secondary_ind << " X: " << toRet.x << " Y: " << toRet.y << " DEG: "
						 << degrees << "VERT DEG: " << vert_degrees << " VERT: " << vertical_dist << " DIST: " 
						 << distance << std::endl;

						toRet.distance = distance;
						toRet.horz_angle = degrees;
						toRet.vert_angle = vert_degrees;

						target_found = true;
						break; //Break out of this for loop
					}

				}

				secondary_ind++;
			}

			//Break out of loop since target has been found
			if(target_found) break;

			target_ind++;
		}
		
		finalTarget = toRet;
	}

	/*
	void __settingsLoop(std::shared_ptr<NetworkTable> table) {
		while(1) {
			try {

				//Scoped mutex lock
				{
					MUTEX_S_LOCK(proc_mutex);
					PLOG("Pulling new settings!");
					loop_time = boost::lexical_cast<int>(table->GetNumber(PROCESSING_LOOP_TAG, PROCESSING_LOOP_TIME));
					PLOG(SW("LOOP TIME: ") + SW(loop_time));
				}

				boost::this_thread::sleep_for(boost::chrono::milliseconds(loop_time));
			} catch(const std::exception &err) {
				PLOG("Failed pulling new settings!");
			}
		}
	}

	void settingsUpdate(std::shared_ptr<NetworkTable> table) {
		PLOG("Starting the settings updater");
		boost::thread(processing::__settingsLoop, table);
	}*/

} /* namespace processing */
