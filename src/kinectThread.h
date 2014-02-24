//
//  kinectThread.h
//  BANQGhosts
//
//  Created by Playmind_SSD on 2014-02-06.
//
//

#ifndef BANQGhosts_kinectThread_h
#define BANQGhosts_kinectThread_h

#include "ofMain.h"
#include "ofxRuiThread.h"

class kinectThread : public ofxRuiThread {

public:

    ofPixels     colorPix;
    ofPixels     depthPix;
    float * depthPixFloat;
    unsigned char * depthPixChar;
    
    bool        isFrameNew;
    ofxKinect kinect;
    ofxKinectCalibration calibration;
    long timeSinceLastFrame;
    long grabTime;
   vector <unsigned char *> depthPixels;
    
    
    kinectThread(){
        
    }
    
    void updateThread(){
        
        isFrameNew = false;
        
        kinect.update();
        
        if(kinect.isFrameNew()){
            
            grabTime = ofGetElapsedTimeMillis();
           
            isFrameNew = true;
            
            //kinectColor.setFromPixels(cam.getPixelsRef());
            
            colorPix.setFromExternalPixels(kinect.getCalibratedRGBPixels(), kinect.width, kinect.height, OF_PIXELS_RGB);
            
           //depthPix.setFromExternalPixels(kinect.getDepthPixels(), kinect.width, kinect.height, OF_PIXELS_MONO);
            
          depthPixels.push_back(kinect.getDepthPixels());
            
            
            if(depthPixels.size() > 50){
            
                depthPixels.erase(depthPixels.begin());
            
            }
            
            for(int p  = 0; p < kinect.width * kinect.height; p ++){
                depthPixFloat[p] = 0;
            }
                                  
            for(int p  = 0; p < kinect.width * kinect.height; p++){
                for(int i = 0; i< depthPixels.size(); i ++){
                   depthPixFloat[p] += depthPixels[i][p];
               }
           }
            
                                  
           for(int p  = 0; p < kinect.width * kinect.height; p ++){
               
                  depthPixChar[p] = depthPixFloat[p] / depthPixels.size();
               
           }
                                  
           depthPix.setFromExternalPixels(depthPixChar, kinect.width, kinect.height, OF_PIXELS_MONO);
            
                                  
        
        timeSinceLastFrame = ofGetElapsedTimeMillis() - grabTime;

        }
    }
    
    

};


#endif
