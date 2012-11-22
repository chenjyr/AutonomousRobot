/*
 * ImgProcessor.h
 *
 *  Created on: Feb 27, 2011
 *      Author: jonathan
 */
#include <vector>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <stdio.h>
#include <iostream>
#include <boost/thread.hpp>

#ifndef IMGPROCESSOR_H
#define IMGPROCESSOR_H


using namespace std;
using namespace boost;
class ImgProcessor {
public:

	ImgProcessor();
	virtual ~ImgProcessor();
	void Get_Obstacles();
	IplImage * getFrame();
	void bufferFrame();
	void Process_Frame(int * hsv_thresh);
	void start_processing(int * hsv_thresh);
	void join_processing();
	void Process_Frame2(int * hsv_thresh);
	void start_processing2(int * hsv_thresh);
	void join_processing2();
	void start_getting();
	void join_getting();
	void get_Coordinates(vector<int> ** rf,
					vector<int>** rb , 
					vector<int>** b, 
					vector<int>** o);
	void lock_coord();
	void unlock_coord();
private:
	vector<int> * robot_front;
	vector<int>* robot_back;
	vector<int>* balls;
	vector<int>* obstacles;
	mutex frame_mutex;
	mutex coord_mutex;
	thread proc_thread2;
	thread proc_thread;
	thread get_thread;
	CvCapture * capture;
	vector<int> * find_blobs(IplImage* thresh, IplImage* dst, CvScalar color);

};

#endif