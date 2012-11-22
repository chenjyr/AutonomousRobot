/*
 * my_GUI.cpp
 *
 *  Created on: Feb 27, 2011
 *      Author: jonathan
 */

#include "My_GUI.h"

My_GUI::My_GUI(ImgProcessor * imp, PathFinder * pfi) {
		my_imp = imp;
		my_pfi = pfi;
		robot_front = new vector<int>;
		robot_back = new vector<int>;
		balls = new vector<int>;
		obstacles = new vector<int>;
}

My_GUI::~My_GUI() {
	// TODO Auto-generated destructor stub
}

void My_GUI::update_image(int * hsv_thresh)
{
	cvNamedWindow("FAG", CV_WINDOW_AUTOSIZE);

while(1)
{
	IplImage * frame = my_imp->getFrame();
	my_imp->get_Coordinates(&robot_front,&robot_back,&balls,&obstacles);
	vector<int> * endpoint = new vector<int>;
	my_pfi->getPathInfo(&endpoint);

	if (endpoint->size() >= 2)
	{
		cvCircle(frame, cvPoint(endpoint->at(0), endpoint->at(1)),10, cvScalar(133,21,199));
		if (robot_back->size() >=2)
			cvLine(frame, cvPoint(endpoint->at(0), endpoint->at(1)), cvPoint(robot_back->at(0), robot_back->at(1)),cvScalar(133,21,199), 3);
	}
	cvCircle(frame, cvPoint(210, 0), 20, cvScalar(100,100,100), 3);
	
	for (int i = 0; i< balls->size(); i+=2)
	{
		cvCircle(frame, cvPoint(balls->at(i), balls->at(i+1)), 15, cvScalar(50,200,50), 4);
	}

	for (int i = 0; i< obstacles->size(); i+=2)
	{
		cvCircle(frame, cvPoint(obstacles->at(i), obstacles->at(i+1)), 15, cvScalar(150,60,50), 4);
	}
	for (int i = 0; i< robot_front->size(); i+=2)
	{
		cvCircle(frame, cvPoint(robot_front->at(i), robot_front->at(i+1)), 15, cvScalar(40,20,90), 4);
	}
	for (int i = 0; i< robot_back->size(); i+=2)
	{
		cvCircle(frame, cvPoint(robot_back->at(i), robot_back->at(i+1)), 15, cvScalar(110,90,100), 4);
	}
	
		
	
	cvShowImage("FAG", frame);
	char fag = cvWaitKey(1);
	if (fag == '1')
             {
                 cvDestroyWindow("Robot Front HSV");
                 cvDestroyWindow("Robot Back HSV");
                 cvNamedWindow("Tennis Ball HSV", CV_WINDOW_AUTOSIZE );
                 cvResizeWindow("Tennis Ball HSV", 650, 350);
                 cvMoveWindow("Tennis Ball HSV", 0, 400);
                 cvCreateTrackbar( "Ball HueL", "Tennis Ball HSV", &hsv_thresh[0], 255, NULL);
                 cvCreateTrackbar( "Ball SatL", "Tennis Ball HSV", &hsv_thresh[1], 255, NULL);
                 cvCreateTrackbar( "Ball ValL", "Tennis Ball HSV", &hsv_thresh[2], 255, NULL);
                 cvCreateTrackbar( "Ball HueH", "Tennis Ball HSV", &hsv_thresh[3], 255, NULL);
                 cvCreateTrackbar( "Ball SatH", "Tennis Ball HSV", &hsv_thresh[4], 255, NULL);
                 cvCreateTrackbar( "Ball ValH", "Tennis Ball HSV", &hsv_thresh[5], 255, NULL);
             }

     else if (fag == '2')
     {
         cvDestroyWindow("Robot Front HSV");
         cvNamedWindow("Robot Front HSV", CV_WINDOW_AUTOSIZE );
         cvResizeWindow("Robot Front HSV", 650, 350);
         cvMoveWindow("Robot Front HSV", 0, 400);
         cvCreateTrackbar( "RF HueL", "Robot Front HSV", &hsv_thresh[6], 255, NULL);
         cvCreateTrackbar( "RF SatL", "Robot Front HSV", &hsv_thresh[7], 255, NULL);
         cvCreateTrackbar( "RF ValL", "Robot Front HSV", &hsv_thresh[8], 255, NULL);  
         cvCreateTrackbar( "RF HueH", "Robot Front HSV", &hsv_thresh[9], 255, NULL);
         cvCreateTrackbar( "RF SatH", "Robot Front HSV", &hsv_thresh[10], 255, NULL);
         cvCreateTrackbar( "RF ValH", "Robot Front HSV", &hsv_thresh[11], 255, NULL);   
     }

     else if (fag == '3')
     {
         cvDestroyWindow("Tennis Ball HSV");
         cvDestroyWindow("Robot Front HSV");
         cvNamedWindow("Robot Back HSV", CV_WINDOW_AUTOSIZE );
         cvResizeWindow("Robot Back HSV", 650, 350);
         cvMoveWindow("Robot Back HSV", 0, 400);
         cvCreateTrackbar( "RB HueL", "Robot Back HSV", &hsv_thresh[12], 255, NULL);
         cvCreateTrackbar( "RB SaTL", "Robot Back HSV", &hsv_thresh[13], 255, NULL);
         cvCreateTrackbar( "RB ValL", "Robot Back HSV", &hsv_thresh[14], 255, NULL);
         cvCreateTrackbar( "RB HueH", "Robot Back HSV", &hsv_thresh[15], 255, NULL);
         cvCreateTrackbar( "RB SaTH", "Robot Back HSV", &hsv_thresh[16], 255, NULL);
         cvCreateTrackbar( "RB ValH", "Robot Back HSV", &hsv_thresh[17], 255, NULL);             
     }

     else if (fag == '4')
     {
         cvDestroyAllWindows();
     }
	 
}
}


void My_GUI::start_updating(int * hsv_thresh)
{

	GUI_thread = thread(&My_GUI::update_image,
						this,
						hsv_thresh);
}
void My_GUI::join_updating(){
	GUI_thread.join();
}