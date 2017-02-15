/*
 * Kinect.cpp
 *
 *  Created on: Jan 13, 2017
 *      Author: root
 */

#include "../include/kinect.hpp"
#include "../include/log.hpp"

namespace kinect {

bool runKinectThread = true;

void kinectExit( int signum ) {
	std::cout << "Exiting Perception!" << signum << std::endl;
	runKinectThread = false;
	boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
	exit(signum);
}

Kinect::Kinect() {
	KLOG("Initializing Kinect device");

	this->__fps = 30;
	this->__is_init = false;

	this->listener = nullptr;
	this->frames = new libfreenect2::FrameMap;

	signal(SIGINT, kinectExit);
	signal(SIGKILL, kinectExit);
	signal(SIGABRT, kinectExit);
}

Kinect::~Kinect() {
	this->cleanFrames();

	KLOG("Cleaning up Kinect");

	this->__is_init = false;

	delete this->frames;
	delete this->listener;

	if(this->pipeline != nullptr) {
		delete this->pipeline;
	}

	runKinectThread = false;

	this->__wait(300);
	if(this->kinectThread != nullptr) {
		this->kinectThread->interrupt();
		delete this->kinectThread;
	}

	if(dev != nullptr) {
		dev->stop();
		dev->close();
	}
}

//Open Kinect
void Kinect::loadKinect(){
	if (this->freenect2.enumerateDevices() == 0) {
			std::cout << "No device connected" << std::endl;
		}

		std::string serial = this->freenect2.getDefaultDeviceSerialNumber();
		KLOG("Device Serial: " + serial);

		if (!this->pipeline) {
			this->pipeline = new libfreenect2::OpenGLPacketPipeline();
		}

		if (this->pipeline) {
			KLOG("Using pipeline!");
			this->dev = freenect2.openDevice(serial, this->pipeline);
		} else {
			KLOG("Using standard serial!");
			this->dev = freenect2.openDevice(serial);
		}

		if (this->dev == 0) {
			KLOG("Failed to open Kinectv2 device");
		}
}

void Kinect::setFrames(int frameType){
	if(this->__is_init) this->listener = nullptr;

	KLOG("Setting Kinect frames");

	this->listener = new libfreenect2::SyncMultiFrameListener(frameType);

	this->__is_init = true;

	//Set the color grame listener to the MultiFrame Listener
	this->dev->setColorFrameListener(this->listener);
	if(frameType > KINECT_COLOR) {
		this->dev->setIrAndDepthFrameListener(this->listener);
	}
	this->dev->start();

	KLOG("Started the listener");
}

bool Kinect::pullAll() {
	if(this->__is_init) {
		this->listener->waitForNewFrame(*this->frames);
		return (true);
	}
	KLOG("Kinect not initialized!");
	return (false);
}

void Kinect::cleanFrames() {
	if(!this->__is_init) {
		KLOG("Kinect not initialized!");
		return;
	}
	this->listener->release(*this->frames);
}

template<typename T>
void Kinect::KLOG(T toLog, bool err) {
	Logger::Log(SW(KINECT_LOGTAG).c_str(), SW(toLog), err);
}

char Kinect::getDepth(DepthData *data, int x, int y) {
	return (data->depthRaw[x + (y * data->width)]);
}

/*
 * KINECT FRAME LISTENERS
 *
 */

void Kinect::attachRGB(_attach_t callback) {
	this->rgbCallback = callback;
}

void Kinect::attachDepth(std::function< void(DepthData) > callback) {
	this->depthCallback = callback;
}

void Kinect::__wait(const long millis) {
	boost::this_thread::sleep_for(boost::chrono::milliseconds(millis));
}


void Kinect::__rgb_callback(libfreenect2::Frame *rgb) {
	cv::Mat rgbMat = cv::Mat(rgb->height, rgb->width, CV_8UC4, rgb->data);
	rgbCallback(&rgbMat);
}

void Kinect::__depth_callback(libfreenect2::Frame *depth) {
	struct kinect::DepthData depthData;
	depthData.height = depth->height;
	depthData.width = depth->width;
	depthData.depthRaw = depth->data;

	//cv::Mat irMat = cv::Mat(ir->height, ir->width, CV_32FC1, ir->data) / 4500.0f;
	//boost::thread(irCallback, depthData);
	depthCallback(depthData);
}

/*void Kinect::__depth_callback(libfreenect2::Frame *depth) {

	//cv::Mat depthMat = cv::Mat(depth->height, depth->width, CV_32FC1, depth->data) /4500.0f;
	depthCallback(&depthData);
}*/

void Kinect::__framePulling() {
	KLOG("Started the frame updates and pulling!");
	while(runKinectThread) {
		try {
			this->pullAll(); //Pull the new FrameMaps

			//Get frames from framemap
			libfreenect2::Frame
				//*rgb = ((*this->frames)[libfreenect2::Frame::Color]),
				//*ir = ((*this->frames)[libfreenect2::Frame::Ir]),
				*depth = ((*this->frames)[libfreenect2::Frame::Depth]);

			this->__depth_callback(depth);

			//Start callback threads
			//this->rgbThread = boost::thread(boost::bind(&Kinect::__rgb_callback, this, rgb));
			//this->depthThread = boost::thread(boost::bind(&Kinect::__depth_callback, this, depth));
			//this->depthThread = boost::thread(boost::bind(&Kinect::__depth_callback, this, depth));

			//Join all frame threads
			//this->rgbThread.join();
			//this->depthThread.join();
			//this->depthThread.join();

			this->cleanFrames();
		} catch(const std::exception &err) {
			KLOG(SW("Failed pulling new frame from the Kinect: ") + err.what(), true);
		}
	}

	if(dev != nullptr) {
		dev->stop();
		dev->close();
	}
}

void Kinect::setFPS(int fps) {
	this->__fps = fps;
}

long inline Kinect::__FPS2millis(long fps) {
	return ((1 / fps) * 1000);
}

void inline Kinect::__waitFPS(long fps) {
	__wait(__FPS2millis(fps));
}

void Kinect::startLoop() {
	runKinectThread = true;

	this->__framePulling();
	//this->kinectThread = new boost::thread(boost::bind(&Kinect::__framePulling, this));
}



/*void Kinect::ourWaitKey(int wait){
	int key = cv::waitKey(wait);
	protonect_shutdown = protonect_shutdown|| (key > 0 && ((key & 0xFF) == 27)); // shutdown on escape
	this->listener->release(*this->frames);
}

void Kinect::showAll(){
	cv::imshow("rgb", );
	cv::imshow("ir", irmat / 4500.0f);
	cv::imshow("depth", depthmat / 4500.0f);
}

void Kinect::showRGB(){
	cv::imshow("rgb",rgbmat);
}

void Kinect::showIR(){
	cv::imshow("ir",irmat);
}

void Kinect::showDepth(){
	cv::imshow("depth",depthmat);
}*/



} /* namespace kinect */
