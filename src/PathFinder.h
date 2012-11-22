/*
 * PathFinder.h
 *
 *  Created on: Feb 27, 2011
 *      Author: jonathan
 */
#ifndef PathFinder_H
#define PathFinder_H
#include "Serial.h"
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <boost/thread.hpp> 
#include "ImgProcessor.h"
using namespace std;
using namespace boost;

class PathFinder {
public:
	PathFinder(ImgProcessor * imp);
	virtual ~PathFinder();
	void Update_instruction();
	void start_updating();
	void join_updating();
	void getPathInfo(vector<int> ** end);
	int instruction;
private:
	mutex endpoint_mutex;
	vector<int> * robot_front;
	vector<int> * robot_back;
	vector<int> * balls;
	vector<int> * obstacles;
	vector <int>* endpoint;
	thread update_thread;
	Serial * my_ser;
	ImgProcessor * my_imp;
	void goHome();
	bool GoToLocation(int x, int y);
	int find_angle_width(vector<int> * robot_back, vector<int> * obstacles, int i, int& distance);
	int find_delta_angle(int angle1, int angle2);
	int find_closest(vector<int> * ref, vector<int> * dest, int & distance);
	int find_angle(vector<int> * ref, vector<int> * dest, int j);
	double pi; 
};
#endif