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
    bool        isFrameNew;
    ofxKinect kinect;
    ofxKinectCalibration calibration;
    
    kinectThread(){
        
    }
    
    void updateThread(){
        
        isFrameNew = false;
        
        kinect.update();
        
        
        if(kinect.isFrameNew()){
            
            isFrameNew = true;
            //kinectColor.setFromPixels(cam.getPixelsRef());
            colorPix.setFromExternalPixels(kinect.getCalibratedRGBPixels(), kinect.width, kinect.height, OF_PIXELS_RGB);
            depthPix.setFromExternalPixels(kinect.getDepthPixels(), kinect.width, kinect.height, OF_PIXELS_MONO);
            

            
        }
        
    }

};


#endif
