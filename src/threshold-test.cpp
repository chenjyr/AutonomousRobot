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


#define MSG_BUF_SIZE  128
#define  WITH_SYNCHRONIZATION


class MessageBuffer
{
     
    char messageText[MSG_BUF_SIZE]; // the message buffer
    BOOL m_bContinueProcessing;      // used to control thread lifetime
    char szTemp_1[MSG_BUF_SIZE];
     
     #ifdef WITH_SYNCHRONIZATION
         HANDLE  m_hMutex;
         HANDLE  m_hEvent;
     #endif

     public:
         MessageBuffer()            // ctor
         {
             memset( messageText, '\0', sizeof( messageText ) );  // initially the message buffer is empty
             m_bContinueProcessing = true;   // will be changed upon a call to DieDieDie()

             #ifdef WITH_SYNCHRONIZATION
                 printf( "Creating mutex in MessageBuffer ctor.\n" );

                 m_hMutex = CreateMutex( NULL,    // no security attributes
                                false,   // BOOL bInitialOwner, we don't want the
                                         // thread that creates the mutex to
                                        // immediately own it.
                                _T(" MessageBufferMutex") // lpName
                              );
                 if ( m_hMutex == NULL )
                 {
//                        printf( "CreateMutex() failed in MessageBuffer ctor.\n" );
                 }

                 // Create the auto-reset event.

                 m_hEvent = CreateEvent( NULL,     // no security attributes
                                false,    // auto-reset event
                                false,    // initial state is non-signaled
                               _T( "MessageBufferEvent") );    // lpName

                 if (m_hEvent == NULL)
                 {
//                        printf( "CreateEvent() failed in MessageBuffer ctor.\n" );
                 }
             #endif
         }

         ~MessageBuffer()
         {
             #ifdef WITH_SYNCHRONIZATION
                 CloseHandle( m_hMutex );
                 CloseHandle( m_hEvent );
             #endif
         }

         void SetMessage(char * s)
         {
//                printf("in MessageBuffer::SetMessage()\n");

         #ifdef WITH_SYNCHRONIZATION
             DWORD   dwWaitResult = WaitForSingleObject( m_hMutex, INFINITE ); // Jaeschke's Monitor::Enter(this)
           
             if ( dwWaitResult != WAIT_OBJECT_0 )
             {
//                    printf( "WaitForSingleObject() failed in MessageBuffer::SetMessage().\n");
                 return;
             }

//                printf( "SetMessage() acquired mutex\n" );
         #endif

       // I intentionally use a very non-atomic method of copying
       // the new message into the message buffer, in order to
       // exacerbate the problem which occurs if the program doesn't
       // use a synchronization object between the producer and consumer.

         if ( strlen( s ) >= MSG_BUF_SIZE )
             s[MSG_BUF_SIZE-1] = '\0';    // make sure the caller doesn't overfill our buffer  
         
         char * pch = &messageText[0];
             
         while ( *s )
         {
             *pch++ = *s++;
             Sleep( 5 );
             //printf("ok\n");
         }

         *pch = '\0';
        
         // Since the message buffer now holds a message we can
         // allow the consumer thread to run.

         //printf("Set new message: %s.\n", messageText );

         #ifdef WITH_SYNCHRONIZATION
//                printf( "SetMessage() pulsing Event\n" );
       
             if ( ! SetEvent( m_hEvent ) )   // Jaeschke's Monitor::Pulse(this)
             {
//                    printf( "SetEvent() failed in SetMessage()\n" );
             }

//                printf( "SetMessage() releasing mutex\n" );
       
             if ( ! ReleaseMutex( m_hMutex ))   // Jaeschke's Monitor::Exit(this)
             {
//                    printf( "ReleaseMutex() failed in MessageBuffer::SetMessage().\n");
             }
         #endif
    }

    char* ProcessMessages()
    {
//         printf("in MessageBuffer::ProcessMessages()\n");

      while ( m_bContinueProcessing )   // state variable used to control thread lifetime
      {
          // We now want to enter an "alertable wait state" so that
          // this consumer thread doesn't burn any cycles except
          // upon those occasions when the producer thread indicates
          // that a message waits for processing.

     #ifdef WITH_SYNCHRONIZATION
          DWORD   dwWaitResult = WaitForSingleObject( m_hEvent, 100000 );
//             printf( "WaitForSingleObject(1) : %d\n", dwWaitResult);
          
          if ( ( dwWaitResult == WAIT_TIMEOUT ) && ( m_bContinueProcessing == false ) ) // WAIT_TIMEOUT = 258    
             break;    // we were told to die
               
          else if ( dwWaitResult == WAIT_ABANDONED ) // WAIT_ABANDONED = 80
          {
//                 printf( "WaitForSingleObject(1) failed in MessageBuffer::ProcessMessages().\n");
              return 0 ;
          }
          
          else if ( dwWaitResult == WAIT_OBJECT_0 )  // WAIT_OBJECT_0 = 0
          {
//                  printf( "ProcessMessages() saw Event\n" );
          }

          dwWaitResult = WaitForSingleObject( m_hMutex, INFINITE );
           
          if ( dwWaitResult != WAIT_OBJECT_0 )
          {
//                 printf( "WaitForSingleObject(2) failed in MessageBuffer::ProcessMessages().\n");
              return 0 ;
          }
           
//             printf( "ProcessMessages() acquired mutex\n" );
     #endif

     if ( strlen( messageText ) != 0 )
     {  
         sprintf( szTemp_1," %s\n", messageText );
         //printf(" processed new message:%s\n", messageText);
         messageText[0] = '\0';
     }



     #ifdef WITH_SYNCHRONIZATION
//            printf( "ProcessMessages() releasing mutex\n" );
         if ( ! ReleaseMutex( m_hMutex ))
         {
//                printf( "ReleaseMutex() failed in MessageBuffer::ProcessMessages().\n");
         }
     #endif

     return szTemp_1;
     } // end of while ( m_bContinueProcessing ) loop
    }

    void  DieDieDie( void )
    {
      m_bContinueProcessing = false;   // ProcessMessages() watches for this in a loop
    }
     
};





















class ThreadX
{
 MessageBuffer* msg;

 public:
     
     string threadName;
     ThreadX(MessageBuffer* m)    // ctor
     {
         msg = m;
     }

     double find_angle(vector<int> * ref, vector<int> * dest, int j)
     {



         double x_1 = dest->at(j);
         double x_2 = ref->at(0);
         double y_1 = 500 - dest->at(j+1);
         double y_2 = 500 - ref->at(1);

         double x_delta = abs(x_1 - x_2 );
         double y_delta = abs(y_1 - y_2);
         double angle = (180/pi)*atan(y_delta/x_delta);

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

         return angle;
     }


	 int find_closest(vector<int> * ref, vector<int> * dest, int * distance = NULL)
	 {
		  int j = 0;
          if (dest->size() > 2)
         {
             int lowest = 10000;   
             for (int i = 0; i < dest->size(); i+=2)
             {
                 int x_dist = abs(ref->at(0) - dest->at(i));
                 int y_dist = abs(ref->at(1) - dest->at(i+1));
     
                 if (x_dist + y_dist < lowest)
                 {
                     lowest = x_dist+y_dist;
                     j = i;
                 }
             }
         }
		  if (distance != NULL){
			*distance = lowest;
		  }
		 return j;
	 }
     vector<int> * find_blobs(IplImage* thresh, IplImage* dst, CvScalar color)
     {
         //cvErode(thresh,thresh);
     //    cvDilate(thresh,thresh);
     //    cvSmooth(thresh,thresh);
         CvMemStorage* storage = cvCreateMemStorage(0);
         CvSeq* contour;
         int num_cont = cvFindContours( thresh, storage, &contour,
         sizeof(CvContour),CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE  );

         vector<int> * coord = new vector<int>;
         cvDrawContours(dst, contour, CV_RGB(255,255,255), CV_RGB(255,255,255), 1);
         CvPoint2D32f cent = cvPoint2D32f(0,0);
         float rad = 0;

         for(; contour != 0; contour = contour->h_next )
         {
             cvMinEnclosingCircle(contour, &cent, &rad);

             if (rad >=3.0 && rad <= 20.0)
             {
                 if ( coord->size() <=1 || abs((int)coord->at(coord->size()-1) - (int)cent.y) > 5 || abs((int)coord->at(coord->size()-2) - (int)cent.x) > 5)
                 {
                     cvCircle(dst, cvPoint(cent.x, cent.y), 10, color, 1);
                     coord->push_back(cent.x); coord->push_back(cent.y);
                 }
             }
         }

         cvReleaseMemStorage(&storage);
         return coord;
     }
     vector<int> * Get_Obstacles(IplImage* frame)
	 {
		cvSetImageROI( frame, cvRect(160,0,420,480) );
		cvSmooth( frame, frame,CV_GAUSSIAN,3,0);
		IplImage* imgHSV= cvCreateImage( cvGetSize(frame), 8, 3);
        IplImage* imgThreshed = cvCreateImage(cvGetSize(frame), 8, 1);
		cvCvtColor(frame, imgHSV, CV_BGR2HSV);
        cvInRangeS(imgHSV, cvScalar(114, 19, 100), cvScalar(150, 130, 160),imgThreshed); //obstacle - blue
		return find_blobs(imgThreshed, frame,cvScalar(50,50,200));
	 }
     
     void cvShowManyImages(char* title, int nArgs, ...)
     {
         // img - Used for getting the arguments
         IplImage *img;

         // DispImage - the image in which input images are to be copied
         IplImage *DispImage;
         int size;
         int i;
         int m, n;
         int x, y;

         // w - Maximum number of images in a row
         // h - Maximum number of images in a column
         int w, h;

         // scale - How much we have to resize the image
         float scale;
         int max;

         // If the number of arguments is lesser than 0 or greater than 12
         // return without displaying
         
         if(nArgs <= 0)
         {
             printf("Number of arguments too small....\n");
             return;
         }
  
         else if(nArgs > 12)
         {
             printf("Number of arguments too large....\n");
             return;
         }
         
         // Determine the size of the image,
         // and the number of rows/cols
         // from number of arguments

         else if (nArgs == 1)
         {
             w = h = 1;
             size = 300;
         }
  
         else if (nArgs == 2)
         {
             w = 2;
             h = 1;
             size = 420;
         }
  
         else if (nArgs == 3 || nArgs == 4)
         {
             w = 2;
             h = 2;
             size = 300;
         }

         else if (nArgs == 5 || nArgs == 6)
         {
             w = 3;
             h = 2;
             size = 200;
         }
  
         else if (nArgs == 7 || nArgs == 8)
         {
             w = 4;
             h = 2;
             size = 200;
         }
  
         else
         {
             w = 4;
             h = 3;
             size = 150;
         }

         // Create a new 3 channel image
         DispImage = cvCreateImage( cvSize( 840, 540), 8, 3 );

         // Used to get the arguments passed
         va_list args;
         va_start(args, nArgs);

         // Loop for nArgs number of arguments
         for (i = 0, m = 20, n = 20; i < nArgs; i++, m += (20 + size))
         {
             // Get the Pointer to the IplImage
             img = va_arg(args, IplImage*);

             // Check whether it is NULL or not
             // If it is NULL, release the image, and return
             if(img == 0)
             {
                 printf("Invalid arguments");
                 cvReleaseImage(&DispImage);
                 return;
             }

             // Find the width and height of the image
             x = img->width;
             y = img->height;

             // Find whether height or width is greater in order to resize the image
             max = (x > y)? x: y;

             
             // Find the scaling factor to resize the image
             scale = (float) ( (float) max / size );

             // Used to Align the images
             if( i % w == 0 && m!= 20)
             {
                 m = 20;
                 n+= 20 + size;
             }

             // Set the image ROI to display the current image
             cvSetImageROI(DispImage, cvRect(m, n, (int)( (x)/scale ) , (int)(y) ));

             // Resize the input image and copy the it to the Single Big Image
             cvResize(img, DispImage);

             // Reset the ROI in order to display the next image
             cvResetImageROI(DispImage);
         }

         // Create a new window, and show the Single Big Image
         cvNamedWindow( title, CV_WINDOW_AUTOSIZE );
         cvShowImage( title, DispImage);


         //  cvWaitKey();
         //  cvDestroyWindow(title);

         // End the number of arguments   
         va_end(args);

         // Release the Image Memory
         cvReleaseImage(&DispImage);
     }
	 int find_delta_angle(int angle1, int angle2)
	 {
			int delta_angle = angle1 - angle2;
			if (delta_angle < -180){
				delta_angle += 360;			
			}
			else if (delta_angle > 180){
				delta_angle -= 360;
			}
			return delta_angle;
/*			else
			{
				ball_dir = 555;
				robot_dir  = (int) find_angle(robot_back_coord, robot_front_coord);
				delta_angle = robot_dir - goal_dir;

				if (delta_angle < -180)
					delta_angle += 360;
				else if (delta_angle > 180)
					delta_angle -= 360;
			}
*/
	}

     void ImageProcessing()
     {
         IplImage  *frame = 0;
         CvCapture* capture = 0;
         CvSize size = cvSize(600,400);
		

         char szTemp[MSG_BUF_SIZE];

         //http://137.82.120.10:8008.mjpeg
         //D:/temp.mpg

         while(1)
         {
             capture = cvCreateFileCapture("http://137.82.120.10:8008.mjpeg");
             if (capture){break;}
         }
		 // Get obstacles coordinates
		 vector<int> * obstacle_coord = Get_Obstacles(cvQueryFrame( capture ));
         int hue_b_low = 20;
         int sat_b_low = 125;
         int val_b_low = 130;
         int hue_b_high = 35;
         int sat_b_high = 225;
         int val_b_high = 180;
         
         int hue_rf_low = 160;
         int sat_rf_low = 130;
         int val_rf_low = 150;
         int hue_rf_high = 255;
         int sat_rf_high = 190;
         int val_rf_high = 190;

         int hue_rb_low = 35;
         int sat_rb_low = 80;
         int val_rb_low = 120;
         int hue_rb_high = 50;
         int sat_rb_high = 140;
         int val_rb_high = 150;

          CvFont font1;
             cvInitFont( &font1, CV_FONT_VECTOR0, 0.4f, 0.4f, 2,1 );
             
             CvFont font2;
             cvInitFont( &font2, CV_FONT_HERSHEY_SIMPLEX, 1.2f, 1.2f, 10 ,7 );

             char text_s1[] = " AWESOME";
             char text_s2[] = " FUCKING";
             char text_s3[] = "   GUI";
             char text_q[] = "[q] Quit";
             char text_1[] = "[1] Open Ball HSV Sliders";
             char text_2[] = "[2] Open Robot Front HSV Sliders";
             char text_3[] = "[3] Open Robot Back HSV Sliders";
             char text_4[] = "[4] Close All Slider Windows";
             char text_5[] = "Robot is currently:";
             char text_6[] = "Robot, Ball, Robot to Ball";
         char wrobot[20] = "Robot Not Detected";

         char key = 0;
     //    cvNamedWindow( "pre", CV_WINDOW_AUTOSIZE );
     //    cvNamedWindow( "post", CV_WINDOW_AUTOSIZE );
         vector<int> * goal = new vector<int>;
		 goal->push_back(130);
		 goal->push_back(0);
		
         while (key!='q')
         {
			 for(int i = 0 ; i <10; i++)
				frame = cvQueryFrame( capture );
             if ( !frame ) continue;
         //    IplImage *ResizedImage = cvCreateImage( size, 8, 3 );
         //  cvResize(frame,ResizedImage,CV_INTER_LINEAR);
         //  cvSetImageROI(ResizedImage, cvRect(135,0,350,400));
 			 cvSetImageROI( frame, cvRect(160,0,420,480) );
             cvSmooth( frame, frame,CV_GAUSSIAN,3,0);
        //   cvSmooth( ResizedImage, ResizedImage,CV_GAUSSIAN,3,0);
      //     IplImage* out= cvCreateImage( size, 8, 3);
//           cvSetImageROI(out, cvRect(100,0,260,300));
//           IplImage* title= cvCreateImage(cvGetSize(out), 8, 3);
             IplImage* texts= cvCreateImage( cvGetSize(frame), 8, 3);
             IplImage* imgHSV= cvCreateImage( cvGetSize(frame), 8, 3);
             IplImage* imgThreshed = cvCreateImage(cvGetSize(frame), 8, 1);
             cvCvtColor(frame, imgHSV, CV_BGR2HSV);
        //   cvCvtColor(ResizedImage, imgHSV, CV_BGR2HSV);

             cvInRangeS(imgHSV, cvScalar(hue_b_low, sat_b_low, val_b_low), cvScalar(hue_b_high, sat_b_high, val_b_high),imgThreshed); //ball - green
             vector<int> * ball_coord = find_blobs(imgThreshed, frame,cvScalar(0,255,0));

             cvInRangeS(imgHSV, cvScalar(hue_rf_low, sat_rf_low, val_rf_low), cvScalar(hue_rf_high, sat_rf_high, val_rf_high),imgThreshed); //robot front - red
             vector<int> * robot_front_coord = find_blobs(imgThreshed, frame, cvScalar(0,0,255));
			 
             cvInRangeS(imgHSV, cvScalar(hue_rb_low,sat_rb_low, val_rb_low), cvScalar(hue_rb_high, sat_rb_high, val_rb_high),imgThreshed); //robot back - dark green
             vector<int> * robot_back_coord = find_blobs(imgThreshed, frame, cvScalar(255,0,0));
		
             if (robot_front_coord->size() >= 2 && robot_back_coord->size() >= 2)
             {
				 int ball_dir;
                 int robot_dir ;
				 int delta_angle;
//				 int goal_dir = (int) find_angle(robot_back_coord,goal);

				 if (ball_coord->size() >= 2)
				 {
					 int last_distance = *distance;
					 int ball_index = find_closest(robot_back_coord, ball_coord, distance);
					 ball_dir = (int) find_angle(robot_back_coord, ball_coord, ball_index);
					 robot_dir  = (int) find_angle(robot_back_coord, robot_front_coord, 0);
					 int robot_to_ball = find_delta_angle(robot_dir,ball_dir);
					 bool obstacle_in_way = false;
					 for (int i = 0; i < obstacle_coord->size() ; i+=2)
					 {
						 cvCircle(frame, cvPoint(obstacle_coord->at(i), obstacle_coord->at(i+1)), 15, cvScalar(250,50,50), 4);
						 int obstacle_dir = find_angle(robot_back_coord, obstacle_coord, i);
						 int robot_to_obstacle = find_delta_angle(robot_dir, obstacle_dir);
						 if (robot_to_obstacle > -20 && robot_to_obstacle < 0)
						 {
							 //Turn away
							 instruction = 3;
							 char tmp[] = "Turning Right";
							 
							 obstacle_in_way = true;
						 }
						else if (robot_to_obstacle < 20 && robot_to_obstacle > 0)
						 {
							 //Turn away
							 instruction = 2;
							 char tmp[] = "Turning Left";
							 obstacle_in_way = true;
						 }
						 else if (abs(robot_to_obstacle - robot_to_ball) < 20)
						 {
							 //just keep going
							 instruction = 1;
							 char tmp[] = "Going Straight";
							 obstacle_in_way = true;
						 }
					 }
			 				 if (robot_to_ball < -20)
							 {
								  instruction = 2;
							      char tmp[] = "Turning Left";
								   strcpy(wrobot,tmp);			 
	   						 }
							 else if (robot_to_ball > 20)
							 {
								  instruction = 3;
								  char tmp[] = "Turning Right";
								  strcpy(wrobot,tmp);
							 }
							 else if (robot_to_ball <= 20 && robot_to_ball >= -20)
							 {
								  instruction = 1;
								  char tmp[] = "Going Straight";
								  strcpy(wrobot,tmp);
							 }
						  }
				 }
				 sprintf(szTemp,"%d", instruction);
                 msg->SetMessage(szTemp );
                 //printf("[%s]\n", szTemp);
                 //printf("angle to ball is: %d\n", robot_dir-ball_dir);



             }
                     
            
             
//             cvPutText( title, text_s1, cvPoint( 20, 40 ), &font2,CV_RGB(50,200,0) );
//             cvPutText( title, text_s2, cvPoint( 20, 100 ), &font2,CV_RGB(50,200,0) );
//             cvPutText( title, text_s3, cvPoint( 20, 160 ), &font2,CV_RGB(50,200,0) );

             cvPutText( texts, text_q, cvPoint( 20, 20 ), &font1,CV_RGB(230,0,25) );
             cvPutText( texts, text_1, cvPoint( 20, 40 ), &font1,CV_RGB(0,0,250) );
             cvPutText( texts, text_2, cvPoint( 20, 60 ), &font1,CV_RGB(0,0,250) );
             cvPutText( texts, text_3, cvPoint( 20, 80 ), &font1,CV_RGB(0,0,250) );
             cvPutText( texts, text_4, cvPoint( 20, 100 ), &font1,CV_RGB(230,0,25) );
             cvPutText( texts, text_5, cvPoint( 20, 130 ), &font1,CV_RGB(0,0,0) );
             cvPutText( texts, wrobot, cvPoint( 20, 150 ), &font1,CV_RGB(0,56,168) );
             cvPutText( texts, text_6, cvPoint( 20, 170 ), &font1,CV_RGB(0,0,0) );
             cvPutText( texts, szTemp , cvPoint( 20, 190 ), &font1,CV_RGB(0,56,168) );
          
             //cvMerge(imgThreshedBall, imgThreshedRobotFront, imgThreshedRobotBack, NULL, imgThreshed);
            
             if (key == '1')
             {
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
             }

             else if (key == '2')
             {
                 cvDestroyWindow("Robot Front HSV");
                 cvNamedWindow("Robot Front HSV", CV_WINDOW_AUTOSIZE );
                 cvResizeWindow("Robot Front HSV", 650, 350);
                 cvMoveWindow("Robot Front HSV", 0, 400);
                 cvCreateTrackbar( "RF HueL", "Robot Front HSV", &hue_rf_low, 255, NULL);
                 cvCreateTrackbar( "RF SatL", "Robot Front HSV", &sat_rf_low, 255, NULL);
                 cvCreateTrackbar( "RF ValL", "Robot Front HSV", &val_rf_low, 255, NULL);  
                 cvCreateTrackbar( "RF HueH", "Robot Front HSV", &hue_rf_high, 255, NULL);
                 cvCreateTrackbar( "RF SatH", "Robot Front HSV", &sat_rf_high, 255, NULL);
                 cvCreateTrackbar( "RF ValH", "Robot Front HSV", &val_rf_high, 255, NULL);   
             }

             else if (key == '3')
             {
                 cvDestroyWindow("Tennis Ball HSV");
                 cvDestroyWindow("Robot Front HSV");
                 cvNamedWindow("Robot Back HSV", CV_WINDOW_AUTOSIZE );
                 cvResizeWindow("Robot Back HSV", 650, 350);
                 cvMoveWindow("Robot Back HSV", 0, 400);
                 cvCreateTrackbar( "RB HueL", "Robot Back HSV", &hue_rb_low, 255, NULL);
                 cvCreateTrackbar( "RB SaTL", "Robot Back HSV", &sat_rb_low, 255, NULL);
                 cvCreateTrackbar( "RB ValL", "Robot Back HSV", &val_rb_low, 255, NULL);
                 cvCreateTrackbar( "RB HueH", "Robot Back HSV", &hue_rb_high, 255, NULL);
                 cvCreateTrackbar( "RB SaTH", "Robot Back HSV", &sat_rb_high, 255, NULL);
                 cvCreateTrackbar( "RB ValH", "Robot Back HSV", &val_rb_high, 255, NULL);             
             }

             else if (key == '4')
             {
                 cvDestroyAllWindows();
             }

          
             cvShowManyImages("Image Processing", 2, frame, texts);    
    //       cvMoveWindow("Image Processing", 510, 5);
  
   //          cvShowImage("text", texts);
   //          cvShowImage("hsv", imgHSV);
   //          cvShowImage("pre", ResizedImage);
   //         cvShowImage("pre", frame);
//              cvShowImage("Image Processing", frame);
	//		 cvReleaseImage(&ResizedImage);
//			 cvReleaseImage(&title);
             cvReleaseImage(&texts);
             cvReleaseImage(&imgHSV);
             cvReleaseImage(&imgThreshed);
        //     cvReleaseImage(&imgThreshedBall);
        //     cvReleaseImage(&imgThreshedRobotFront);
        //     cvReleaseImage(&imgThreshedRobotBack);
//             cvReleaseImage(&out);
             key = cvWaitKey(1);

			 
         }

     }


     void Wireless()
     {

         Serial * my_serial = new Serial("COM8");
         char * my_chars = new char[2];

         while (true)
         {
             char* temp =msg->ProcessMessages();
             char a[] = {*(temp+1),*(temp+2),*(temp+3)};
             int instruction = atoi(a);
			 char sent[1] = {instruction+48};
			 printf("%c", sent[0]);
	         my_serial->WriteData(sent,1);
         }

     }



     static unsigned __stdcall ThreadStaticEntryPoint(void * pThis)
     {
         ThreadX * pthX =(ThreadX *)pThis;   // the tricky cast
         pthX->ImageProcessing();           // now call the true entry-point-function

         return 1;          // the thread exit code
     }


     static unsigned __stdcall ThreadStaticEntryPoint2(void * pThis)
     {
         ThreadX * pthX =(ThreadX *)pThis;   // the tricky cast
         pthX->Wireless();           // now call the true entry-point-function

         return 2;          // the thread exit code
     }


};


int main()
{
  MessageBuffer* m = new MessageBuffer();

    ThreadX * o1 = new ThreadX(m );
    ThreadX * o2 = new ThreadX(m );
//   ThreadX * o3 = new ThreadX(m );
    HANDLE   hth1;
    HANDLE   hth2;
    unsigned  uiThread1ID;
    unsigned  uiThread2ID;

    hth1 = (HANDLE)_beginthreadex( NULL,         // security
                                0,            // stack size
                                ThreadX::ThreadStaticEntryPoint,
                                o1,           // arg list
                                CREATE_SUSPENDED,  // so we can later call ResumeThread()
                                &uiThread1ID );

    hth2 = (HANDLE)_beginthreadex( NULL,         // security
                                0,            // stack size
                                ThreadX::ThreadStaticEntryPoint2,
                                o2,           // arg list
                                CREATE_SUSPENDED,  // so we can later call ResumeThread()
                                &uiThread2ID );

    if ( hth1 == 0 )
     printf("Failed to create thread 1\n");
     
    if ( hth2 == 0 )
      printf("Failed to create thread 1\n");

    DWORD   dwExitCode;
    DWORD   dwExitCode2;
     
    GetExitCodeThread( hth1, &dwExitCode );  // should be STILL_ACTIVE = 0x00000103 = 259
    GetExitCodeThread( hth2, &dwExitCode2 );
     
    printf( "initial thread 1 exit code = %u\n", dwExitCode );
    printf( "initial thread 2 exit code = %u\n", dwExitCode2 );
     
    // The System::Threading::Thread object in C++/CLI has a "Name" property.
    // To create the equivalent functionality in C++ I added a public data member
     
    o1->threadName = "t1";
    o2->threadName = "t2";

    // named threadName.

    ResumeThread( hth1 );
    ResumeThread( hth2 );
     
    WaitForSingleObject( hth1, INFINITE );
    WaitForSingleObject( hth2, INFINITE );
     
    GetExitCodeThread( hth1, &dwExitCode );
    GetExitCodeThread( hth2, &dwExitCode2 );
     
    printf( "thread 1 exited with code %u\n", dwExitCode );
    printf( "thread 2 exited with code %u\n", dwExitCode2 );
     
    CloseHandle( hth1 );
    CloseHandle( hth2 );
     
    printf("Primary thread terminating.\n");
  
}


