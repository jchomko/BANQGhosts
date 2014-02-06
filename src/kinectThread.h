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
    
    ofxKinect kinect;
    //ofVideoGrabber cam;
    
    
    kinectThread(){
        
    }
    
    void updateThread(){
        
        kinect.update();
        
        
        if(kinect.isFrameNew()){
            
            //kinectColor.setFromPixels(cam.getPixelsRef());
            colorPix.setFromExternalPixels(kinect.getCalibratedRGBPixels(), kinect.width, kinect.height, OF_PIXELS_RGB);
            depthPix.setFromExternalPixels(kinect.getDepthPixels(), kinect.width, kinect.height, OF_PIXELS_MONO);

            
        }
        
        
        
    }
    
 
    

};


#endif
