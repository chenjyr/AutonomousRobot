/*
 * my_GUI.h
 *
 *  Created on: Feb 27, 2011
 *      Author: jonathan
 */

#include <boost/thread.hpp>  
#include <vector>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <stdio.h>
#include <iostream>
#include "ImgProcessor.h"
#include "PathFinder.h"
using namespace boost;
using namespace std;

class My_GUI {


public:
	My_GUI(ImgProcessor * imp,PathFinder * pfi);
	virtual ~My_GUI();
	void update_image(int * hsv_thresh);
	void join_updating();
	void start_updating(int * hsv_thresh);
private:
	thread GUI_thread;
	ImgProcessor * my_imp;
	PathFinder * my_pfi;
	vector<int>* robot_front;
	vector<int>* robot_back;
	vector<int>* balls;
	vector<int>* obstacles;

};