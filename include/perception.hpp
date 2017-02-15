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
 * @file perception.hpp
 * @date 01/23/2017
 * @brief Header to control the main functions for perception
 *
 * @details This header file contains all the main functions for the perception
 * processing.
 */

#ifndef INCLUDE_PERCEPTION_HPP_
#define INCLUDE_PERCEPTION_HPP_

//STANDARD C INCLUDES
#include <stdio.h>
#include <iomanip>
#include <time.h>
#include <signal.h>

//STANDARD C++ INCLUDES
#include <iostream>
#include <memory>

//PERCEPTION INLCUDES
//OLDSRC
//#include "../include/kinect.hpp"
#include "log.hpp"
//OLDSRC
//#include "../include/mjpgserver.hpp"
#include "processing.hpp"
#include "tables.hpp"

//BOOST INCLUDES
#include <boost/lexical_cast.hpp>

//OPENCV INCLUDES
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

//LIBFREENECT INCLUDES (OLDSRC)
/*
#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>
*/

#define PERCEPTION_LOG_TAG 			"PERCEP" //Logger tag name
#define PERCEPTION_TABLE_NAME 		"perception" //The network table name
#define PERCEPTION_PULL_URL 		"http://10.54.31.25/mjpg/video.mjpg"
#define PERCEPTION_MAX_FAIL_COUNT	30 //Maximum empty frame count

#endif /* INCLUDE_PERCEPTION_HPP_ */
