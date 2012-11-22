/*
 * PathFinder.cpp
 *
 *  Created on: Feb 27, 2011
 *      Author: jonathan
 */

#include "PathFinder.h"

PathFinder::PathFinder(ImgProcessor * imp) {
	endpoint = new vector<int>;
	my_imp = imp;
	my_ser = new Serial("COM8");
	pi = 3.14159265;
	// TODO Auto-generated constructor stub

}

PathFinder::~PathFinder() {
	// TODO Auto-generated destructor stub
}

int PathFinder::find_delta_angle(int angle1, int angle2)

{

	int delta_angle = angle1 - angle2;

	if (delta_angle < -180) {

		delta_angle += 360;

	}

	else if (delta_angle > 180) {

		delta_angle -= 360;

	}

	return delta_angle;
}

int PathFinder::find_closest(vector<int> * ref, vector<int> * dest, int &distance)

{
	int j = 0;
	double lowest = 1000000000;
	if (dest->size() >= 2)

	{



		for (int i = 0; i < dest->size(); i += 2)

		{

			double x_dist = (ref->at(0) - dest->at(i))*(ref->at(0) - dest->at(i));

			double y_dist = (ref->at(1) - dest->at(i + 1))*(ref->at(1) - dest->at(i + 1));

			if (x_dist + y_dist < lowest)

			{

				lowest = x_dist + y_dist;
				//cout << "LOWEST: " << lowest << endl;
				j = i;

			}

		}

	}

	distance = (int)sqrt(lowest);
	return j;

}
int PathFinder::find_angle_width(vector<int> * robot_back, vector<int> * obstacles, int i, int &distance)
{
	double delta_x = robot_back->at(0)-obstacles->at(i);
	double delta_y = robot_back->at(1)-obstacles->at(i+1);
	double dist = sqrt(delta_x*delta_x+delta_y*delta_y);
	double angle = (180 / pi) * atan(dist/80);
	distance = (int) dist;
	return (int) 90-angle;
}

int PathFinder::  find_angle(vector<int> * ref, vector<int> * dest, int j)

{

	double x_1 = dest->at(j);

	double x_2 = ref->at(0);

	double y_1 = 500 - dest->at(j + 1);

	double y_2 = 500 - ref->at(1);

	double x_delta = abs(x_1 - x_2);

	double y_delta = abs(y_1 - y_2);

	double angle = (180 / pi) * atan(y_delta / x_delta);

	if (x_1 < x_2)

	{

		if (y_1 > y_2)

			angle = 360 - angle;

	}

	else

	{

		if (y_1 < y_2)

			angle = 180 - angle;

		else

			angle = 180 + angle;

	}

	return (int) angle;

}
void PathFinder::goHome()
{
	int distance;
	int amount = 0;
	int wait_time = 0;
	int past_instruction = -1;
	bool done = false;
	my_imp->get_Coordinates(&robot_front,&robot_back,&balls,&obstacles);
	int last_size = balls->size();
	while(!GoToLocation(210,105))
	{
		if (balls->size() > last_size)
			break;
	}	
	my_imp->get_Coordinates(&robot_front,&robot_back,&balls,&obstacles);
	if(last_size < balls->size()) return;
	
	cout << "SHOOTING" << endl;
	vector <int>* goal = new vector<int>;
	goal->push_back(210);
	goal->push_back(0);

	while(!done)
	{
		my_imp->get_Coordinates(&robot_front,&robot_back,&balls,&obstacles);
		if (robot_front->size() == 2 && robot_back->size() == 2)
		{
			int goal_index = find_closest(robot_back, goal, distance);
			int goal_dir = find_angle(robot_back, goal, 0);
			int robot_dir = find_angle(robot_back, robot_front, 0);
			int robot_to_goal = find_delta_angle(robot_dir, goal_dir);
				
			if (robot_to_goal < -8)

			{
				instruction = 2;
				amount = abs(robot_to_goal);
				wait_time = amount*3.3;
			}

			else if (robot_to_goal > 8)

			{
				instruction = 3;
				amount = abs(robot_to_goal);
				wait_time = amount*3.3;
			}

			else

			{
				instruction = 6;
				amount = abs(robot_to_goal);
				wait_time = 400;
				done = true;
			}	

			char gnaw[8];
			sprintf ( gnaw, "%.1d%.3d", instruction, amount);
			my_ser->WriteData(gnaw, 4);
			cout << gnaw << endl;
			past_instruction = gnaw[0];
			cvWaitKey(750+7*wait_time);
		}
	}
	cout << "done!" << endl;
	return;	
}
void PathFinder::Update_instruction() 
{
	int distance = 0;
	int last_ball_size = 0;
	cvWaitKey(2000);
	bool checkpoint = false;
	
	do
	{
		my_imp->get_Coordinates(&robot_front,&robot_back,&balls,&obstacles);
	}while(robot_front->size()== 0 || robot_back->size() == 0);

	while(1)
	{	
		my_imp->get_Coordinates(&robot_front,&robot_back,&balls,&obstacles);
		if (robot_front->size() == 2 && robot_back->size() == 2)
		{
			cout << last_ball_size << ", " << balls->size() << endl;
			if (last_ball_size > balls->size())
			{
				cout <<"how?" << endl;
				int j = 0;
				for(unsigned int k = 0;k<=5;k++)
				{
					my_imp->get_Coordinates(&robot_front,&robot_back,&balls,&obstacles);
					cout << "hey! " << last_ball_size << ", " << balls->size() << endl;
					if (last_ball_size > balls->size())
						j++;
				}
				if (j > 3)
				{	
					goHome();
					checkpoint = false;
				}
			}
			int ball_index = find_closest(robot_back, balls, distance);
			if (distance > 15 && balls->size() > 0)
			{
				last_ball_size = balls->size();
				GoToLocation(balls->at(ball_index), balls->at(ball_index+1));
			}
		
			else if (balls->size() == 0)
			{
				last_ball_size = balls->size();
				GoToLocation(215, 300);
			}
		}
	}
}

bool PathFinder::GoToLocation(int x, int y)
{
	endpoint_mutex.lock();
	endpoint = new vector<int>;
	endpoint->push_back(x);
	endpoint->push_back(y);
	endpoint_mutex.unlock();
	int distance = 0;
	int obs_distance = 1000;
	int instruction = 0;
	int amount = 0;
	int wait_time = 0;
	int time = 0;
	int time_limit = -1;
	int past_instruction = -1;
//	while(1)
//	{
		my_imp->get_Coordinates(&robot_front,&robot_back,&balls,&obstacles);
		int last_ball_size = balls->size();
		if (robot_front->size() == 2 && robot_back->size() == 2)
		{
				find_closest(robot_back, endpoint, distance);
				if (distance < 20) return true;
				cout << "Heading Towards: " << endpoint->at(0) << "," << endpoint->at(1) << endl;
				int end_dir = find_angle(robot_back, endpoint, 0);
				int robot_dir = find_angle(robot_back, robot_front, 0);
				int robot_to_end= find_delta_angle(robot_dir, end_dir);
				bool obstacle_in_way = false;
		
				
				for (int i = 0; i < obstacles->size() && !obstacle_in_way; i += 2)
				{
					int angle_width = find_angle_width(robot_back, obstacles, i, obs_distance);
					int obstacle_dir = find_angle(robot_back, obstacles, i);
					int robot_to_obstacle = find_delta_angle(robot_dir, obstacle_dir);
					if(distance > obs_distance && abs(robot_to_obstacle-robot_to_end) < 15)
					{
						//Find way point
						double theta = (atan(50.0/robot_to_obstacle)) + (obstacle_dir*pi/180);
						theta = - theta;
						cout << "ANGLE: " << theta*180/pi;
						double r = sqrt(50*50.0+obs_distance*obs_distance);
						int x = robot_back->at(0) + r*cos(theta) ;  
						int y=  robot_back->at(1) - r*sin(theta);
						if (x > 0 && y>0)
							while(!GoToLocation(x,y));
					}
				}

				if (!obstacle_in_way) 
				{
					
					if (robot_to_end < -20)
					{
						instruction = 2;
						amount = abs(robot_to_end);
						wait_time = amount* 3.0;
					}

					else if (robot_to_end > 20)
					{
						instruction = 3;
						amount = abs(robot_to_end);
						wait_time = amount* 3.0;
					}

					else
					{
						instruction = 1;
						amount = abs(distance)-20;
						wait_time = amount*8.5;
					}
				}
					char inst[8];
					sprintf ( inst, "%.1d%.3d", instruction, amount);
					my_ser->WriteData(inst, 4);		
					cout << inst << endl;									
					past_instruction = inst[0];
					while(my_ser->ReadData(inst,4) == -1);
					cvWaitKey(500 + 4*wait_time);	
		}
	my_imp->get_Coordinates(&robot_front,&robot_back,&balls,&obstacles);
	if (robot_back->size() == 2)
		if (abs(robot_back->at(0) - x ) < 20 && abs(robot_back->at(1) - y) < 20)
			return true;
	return false;
}


void PathFinder::start_updating()
{

	update_thread = thread(&PathFinder::Update_instruction,
						this);
}

void PathFinder::join_updating(){
	update_thread.join();
}

void PathFinder::getPathInfo(vector<int> ** end){
	endpoint_mutex.lock();
	*end = endpoint;
	endpoint_mutex.unlock();
}