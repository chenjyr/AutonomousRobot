#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <iostream>
#include <stdio.h>
#include "PathFinder.h"
#include "My_GUI.h"
using namespace std;



int main(void)
{
	int hsv_thresh[] = 
		 {25,135,135,35,215,215,
		 167,20,177,187,255,227,
		 40,20,150,80,170,250};
	ImgProcessor * mImgPr = new ImgProcessor();
	PathFinder * mPathFr = new PathFinder(mImgPr);

	My_GUI * mGUI = new My_GUI(mImgPr,mPathFr);

	mImgPr->start_getting();
	mImgPr->start_processing(hsv_thresh);
	mImgPr->start_processing2(hsv_thresh);
	mGUI->start_updating(hsv_thresh);
	mPathFr->start_updating();

	mImgPr->join_getting();
	mImgPr->join_processing();
	mImgPr->join_processing2();
	mGUI->join_updating();
	return 0;
}

/*
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <iostream>
#include <math.h>
#include <string.h>
#include <math.h>
#include "serial.h"
#include <stdio.h>
#include <string>             // for STL string class
#include <windows.h>          // for HANDLE
#include <process.h>          // for _beginthread()
#include<TCHAR.h>


#define pi 3.14159265
using namespace std;


     int main()
     {
         IplImage  *frame = 0;
         CvCapture* capture = 0;
         CvSize size = cvSize(600,400);
		cvNamedWindow("FAG", CV_WINDOW_AUTOSIZE);
        while(1)
         {
             capture = cvCreateFileCapture("http://137.82.120.10:8008.mjpeg");
             if (capture){break;}
         }
// 160,130,150,255,190,190,		 35,80,110,50,170,170};
         int hue_b_low = 35;
         int sat_b_low = 80;
         int val_b_low = 50;
		 int hue_b_high = 50;
		 int sat_b_high = 200;
		 int val_b_high = 200;
         char key = 0;
		 cvDestroyWindow("Robot Front HSV");
		 cvDestroyWindow("Robot Back HSV");
		 cvNamedWindow("Tennis Ball HSV", CV_WINDOW_AUTOSIZE );
		 cvResizeWindow("Tennis Ball HSV", 650, 350);
		 cvMoveWindow("Tennis Ball HSV", 0, 400);
		 cvCreateTrackbar( "Ball HueL", "Tennis Ball HSV", &hue_b_low, 255, NULL);
		 cvCreateTrackbar( "Ball SatL", "Tennis Ball HSV", &sat_b_low, 255, NULL);
		 cvCreateTrackbar( "Ball ValL", "Tennis Ball HSV", &val_b_low, 255, NULL);
		 cvCreateTrackbar( "Ball HueH", "Tennis Ball HSV", &hue_b_high, 255, NULL);
		 cvCreateTrackbar( "Ball SatH", "Tennis Ball HSV", &sat_b_high, 255, NULL);
		 cvCreateTrackbar( "Ball ValH", "Tennis Ball HSV", &val_b_high, 255, NULL);

         while (key!='q')
         {

			 for(int i = 0 ; i <10; i++)
				frame = cvQueryFrame( capture );
             if ( !frame ) continue;
 			 cvSetImageROI( frame, cvRect(160,0,420,480) );
             cvSmooth( frame, frame,CV_GAUSSIAN,3,0);
             IplImage* imgHSV= cvCreateImage( cvGetSize(frame), 8, 3);
             IplImage* imgThreshed = cvCreateImage(cvGetSize(frame), 8, 1);
             cvCvtColor(frame, imgHSV, CV_BGR2HSV);
             cvInRangeS(imgHSV, cvScalar(hue_b_low, sat_b_low, val_b_low), cvScalar(hue_b_high, sat_b_high, val_b_high),imgThreshed); //ball - green
			 cvShowImage("FAG", imgThreshed);
			 cvWaitKey(1);
         }

     }


*/