#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#define PROCESSING_LIFECAM_DPP 0.09375



int mainCODE(){

	cv::VideoCapture cap(0);
	
	for(;;){
		std::vector< std::vector< cv::Point > > contours;
		cv::Mat frame,frameThresh,original;

		cap>>frame;
		cap>>original;
		cv::resize(frame,frame,cv::Size(640,480));

		cv::cvtColor(frame,frame,CV_BGR2HSV);
		cv::inRange(frame,cv::Scalar(49,227,57),cv::Scalar(85,255,255),frame);
		cv::imshow("HSV", frame);
//		cv::threshold(frame,frame,150,255,CV_THRESH_BINARY);
		cv::GaussianBlur(frame,frame,cv::Size(5,5),0,0);
		cv::medianBlur(frame,frame,9);
		cv::findContours(frame,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);

		std::vector<cv::Rect> boundRect( contours.size() );
		std::vector<float>radius( contours.size() );
		std::vector<cv::Point2f>center( contours.size() );
		std::vector<cv::Point> approx;
		cv::Mat drawn = cv::Mat::zeros(frame.rows,frame.cols,CV_8UC3);	



		for(int i =0;i<contours.size();i++){
				cv::approxPolyDP(contours[i],approx,5,true);
		
		
		unsigned int sides = approx.size();
		double area;
		area = cv::contourArea(contours[i]);
		if(sides==4 && area>400){
			
			cv::drawContours(drawn,contours,i,cv::Scalar(255,255,255),1);
			minEnclosingCircle( contours[i], center[i], radius[i] );
		}

}
			float x_val = 666;
			float x_valL = 666;
			float x_valR = 666;
			float tarDif = 666;
			if(center.size()==1){
				x_val = center[0].x;
			}

			if(center.size()==2){
				x_val = ((center[0].x)+(center[1].x))/2;
				if(center[0].x<center[1].x){
					x_valL = center[0].x;
					x_valR = center[1].x;
				}
				else if(center[0].x>center[1].x){
					x_valL = center[1].x;
					x_valR = center[0].x;
				}
				if(x_valR == 666)
					tarDif = std::abs(x_valL-x_valR);
			
			}
			
			float horz_angle = x_val*PROCESSING_LIFECAM_DPP;
			float horz_angleL = x_valL*PROCESSING_LIFECAM_DPP;
			float horz_angleR = x_valR*PROCESSING_LIFECAM_DPP;

			horz_angle= horz_angle-30;
			horz_angleL = horz_angleL-30;
			horz_angleR = horz_angleR-30;
		//	if(x_val!=666){
			std::cout<<"X VALUE : "<<x_val<<"  HORZ_ANGLE : "<<horz_angle<<"    NUM_CONT : "<<center.size()<<std::endl;
			std::cout<<"X VAL LEFT : "<<x_valL<<"   X VAL RIGHT : "<<x_valR<<std::endl;
			std::cout<<"HORZ ANGLE LEFT : "<<horz_angleL<<"  HORZ ANGLE RIGHT : "<< horz_angleR<<std::endl;
			std::cout<<"TARGETS PIXEL DIFFERENCE : "<<tarDif<<std::endl;
		//	float distance = ((0.003259)*(std::pow(tarDif,2)))-((1.244*tarDif)+149.23);
		//	std::cout<<distance<<std::endl;	
	//	}
			cv::imshow("FRAME",frame);
			cv::imshow("contours",drawn);
			cv::imshow("original",original);
			cv::waitKey(15);
		}
			
	return 0;
}
