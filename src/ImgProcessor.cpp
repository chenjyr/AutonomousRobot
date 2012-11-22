/*
 * ImgProcessor.cpp
 *
 *  Created on: Feb 27, 2011
 *      Author: jonathan
 */

#include "ImgProcessor.h"

ImgProcessor::ImgProcessor() {
	obstacles = new vector<int>;
	robot_back= new vector<int>;
	robot_front= new vector<int>;
	balls= new vector<int>;
	this->capture = cvCreateFileCapture("http://137.82.120.10:8008.mjpeg");
	if (!capture)
		cout << "Could not open stream!!\n";
	this->Get_Obstacles();
}

ImgProcessor::~ImgProcessor() {
	// TODO Auto-generated destructor stub
}

vector<int> * ImgProcessor:: find_blobs(IplImage* thresh, IplImage* dst, CvScalar color)

{
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour;

	int num_cont = cvFindContours(thresh, storage, &contour,
	sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
	vector<int> * coord = new vector<int> ;
	CvPoint2D32f cent = cvPoint2D32f(0, 0);

	float rad = 0;

	for (; contour != 0; contour = contour->h_next)

	{

		cvMinEnclosingCircle(contour, &cent, &rad);

		if (rad >= 3.0 && rad <= 5000)//20

		{

			if (coord->size() <= 2 || abs((int) coord->at(coord->size() - 1)
					- (int) cent.y) > 20 || abs((int) coord->at(coord->size()
					- 2) - (int) cent.x) > 20)

			{
				coord->push_back(cent.x);
				coord->push_back(cent.y);
			}

		}

	}

	cvReleaseMemStorage(&storage);

	return coord;

}

void ImgProcessor::Get_Obstacles()

{
	IplImage* frame = getFrame();
	cvSetImageROI(frame, cvRect(160, 0, 420, 480));

	cvSmooth(frame, frame, CV_GAUSSIAN, 3, 0);

	IplImage* imgHSV = cvCreateImage(cvGetSize(frame), 8, 3);

	IplImage* imgThreshed = cvCreateImage(cvGetSize(frame), 8, 1);

	cvCvtColor(frame, imgHSV, CV_BGR2HSV);

	cvInRangeS(imgHSV, cvScalar(100, 70, 70), cvScalar(140, 255, 255),
			imgThreshed); //obstacle - blue

	obstacles = find_blobs(imgThreshed, frame, cvScalar(50, 50, 200));
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgThreshed);
}
void ImgProcessor::bufferFrame()
{
	while(1){
		frame_mutex.lock();
		cvGrabFrame(capture);
		frame_mutex.unlock();
		cvWaitKey(35);
	}
}

IplImage * ImgProcessor::getFrame()
{
		frame_mutex.lock();
		IplImage * temp = cvQueryFrame(capture);
		frame_mutex.unlock();
		return temp;
}


void ImgProcessor:: Process_Frame(int * hsv_thresh) 
{
	while(1)
	{
		IplImage* frame = getFrame();
		cvSetImageROI(frame, cvRect(160, 0, 420, 480));
		cvSmooth(frame, frame, CV_GAUSSIAN, 3, 0);
		IplImage* imgHSV = cvCreateImage(cvGetSize(frame), 8, 3);
		cvCvtColor(frame, imgHSV, CV_BGR2HSV);
		IplImage* imgThreshed = cvCreateImage(cvGetSize(frame), 8, 1);

		
		cvInRangeS(imgHSV, cvScalar(hsv_thresh[0], hsv_thresh[1], hsv_thresh[2]), cvScalar(
						hsv_thresh[3], hsv_thresh[4], hsv_thresh[5]), imgThreshed); //ball - green
		vector<int> * ball_temp = find_blobs(imgThreshed, frame, cvScalar(0, 255,0));
		coord_mutex.lock();
		for (unsigned int i = 0; i < ball_temp->size() ; i+=2)
		{
			bool close_to_obstacle = false;
			for (unsigned int j = 0; j < obstacles->size() ; j+=2)
			{
				if (abs(ball_temp->at(i) - obstacles->at(j)) + abs(ball_temp->at(i+1) - obstacles->at(j+1)) < 50)
				{
					close_to_obstacle = true;
					break;
				}
			}
			if (!close_to_obstacle)
			{
				balls = new vector<int>;
				balls->push_back(ball_temp->at(i));
				balls->push_back(ball_temp->at(i+1));
			}
						

		}

	
		coord_mutex.unlock();

		cvReleaseImage(&imgHSV);
		cvReleaseImage(&imgThreshed);

	}
}

void ImgProcessor:: Process_Frame2(int * hsv_thresh) 
{
	while(1)
	{
		IplImage* frame = getFrame();
		cvSetImageROI(frame, cvRect(160, 0, 420, 480));
		cvSmooth(frame, frame, CV_GAUSSIAN, 3, 0);
		IplImage* imgHSV = cvCreateImage(cvGetSize(frame), 8, 3);
		cvCvtColor(frame, imgHSV, CV_BGR2HSV);
		IplImage* imgThreshed = cvCreateImage(cvGetSize(frame), 8, 1);

		coord_mutex.lock();
			cvInRangeS(imgHSV, cvScalar(hsv_thresh[6], hsv_thresh[7], hsv_thresh[8]), cvScalar(
						hsv_thresh[9], hsv_thresh[10], hsv_thresh[11]), imgThreshed); //robot front - red
			vector<int> * robot_front_temp = find_blobs(imgThreshed, frame, cvScalar(0, 0, 255));
			
			cvInRangeS(imgHSV, cvScalar(hsv_thresh[12], hsv_thresh[13], hsv_thresh[14]), cvScalar(
						hsv_thresh[15], hsv_thresh[16], hsv_thresh[17]), imgThreshed); //robot back - dark green		
			vector<int> * robot_back_temp = find_blobs(imgThreshed, frame, cvScalar(	255, 0, 0));
			int dist = 1000;
			for (unsigned int i = 0; i < robot_back_temp->size() ; i+=2)
			{
				for (unsigned int j = 0; j < robot_front_temp->size() ; j+=2)
				{
					int new_dist = abs(robot_back_temp->at(i) - robot_front_temp->at(j)) + abs(robot_back_temp->at(i+1) - robot_front_temp->at(j+1));
					if (new_dist < 50 && new_dist > 20)
					{
						robot_back = new vector<int>;
						robot_back->push_back(robot_back_temp->at(i));
						robot_back->push_back(robot_back_temp->at(i+1));
						robot_front = new vector<int>;
						robot_front->push_back(robot_front_temp->at(j));
						robot_front->push_back(robot_front_temp->at(j+1));
						break;
					}
				}
			}
		coord_mutex.unlock();

		cvReleaseImage(&imgHSV);
		cvReleaseImage(&imgThreshed);

	}
}
void ImgProcessor::start_processing2(int * hsv_thresh)
{
	proc_thread2 = thread(&ImgProcessor:: Process_Frame2,
						this,
						hsv_thresh);
}
void ImgProcessor::join_processing2(){
	proc_thread2.join();
}
void ImgProcessor::start_processing(int * hsv_thresh)
{
	proc_thread = thread(&ImgProcessor:: Process_Frame,
						this,
						hsv_thresh);
}
void ImgProcessor::join_processing(){
	proc_thread.join();
}
void ImgProcessor::start_getting(){
	get_thread = thread(&ImgProcessor::bufferFrame,
						this);
}
void ImgProcessor::join_getting(){
	get_thread.join();
}

void ImgProcessor::get_Coordinates(vector<int> ** rf,
					vector<int>** rb, 
					vector<int>** b, 
					vector<int>** o)
{
	coord_mutex.lock();
	*rf = robot_front;
	*rb = robot_back;
	*b = balls;
	*o = obstacles;
	coord_mutex.unlock();
}

void ImgProcessor::lock_coord(){
	coord_mutex.lock();
}

void ImgProcessor::unlock_coord(){
	coord_mutex.unlock();
}