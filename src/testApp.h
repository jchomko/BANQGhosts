#pragma once

#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxVectorMath.h"
#include "ofxSyphon.h"
#include "ofxUI.h"
#include "ofxFlocking.h"
#include "ofxKinect.h"
#include "Path.h"
#include "kinectThread.h"
#include "ofxMidi.h"




#define NUM_FRAMES 50
#define NUM_PATHS 10
#define MIN_VIDEO_SIZE 30
#define NUM_SEQUENCES 24
#define PATH_RECT_IMAGE_PAD 300






class testApp : public ofBaseApp, public ofxMidiListener{
	
public:
	
		
	void setup();
	void update();
	void draw();
	void exit();

	void updateVideo();
    void drawCVImages();
    void drawBoids();
    void drawMonitor();
    void recordBlanks();
    
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	
	//Syphon
	 ofxSyphonServer spanServer;
	ofxSyphonClient mClient;
	
    
	// UI 
	ofxUICanvas *editGui;
    ofxUICanvas *recordGui;
    ofxUIDropDownList *ddl; 
	void guiEvent(ofxUIEventArgs &e);
	bool drawFill; 
	
     //Midi Input
    ofxMidiIn	midiIn;
    ofxMidiIn   osculatorIn;
    
	ofxMidiMessage midiMessage;
    void newMidiMessage(ofxMidiMessage& eventArgs);
    
    //Threaded Kinect
    int 				camWidth;
	int 				camHeight;
    kinectThread        kinThread;
	
    
    //OpenCV
	int	highBlob;
	int	lowBlob;
	int threshold;
	int diffMode;
	ofxCvGrayscaleImage cvBackground;
	ofxCvGrayscaleImage depthGray;
    ofxCvGrayscaleImage cvThresh;
	ofxCvColorImage		cvColor;
	ofxCvContourFinder 	contourFinder;
    ofxCvColorImage     colorBackground;
	
    //int blockSize;
	bool getBackground;
	
	
	//Recording
    float endSpeed;
	int record;
	int index;
	
	
	int endRecordSequenceTime;
	int endRecordSequenceDelay;
	bool endRecordSequence;
    bool bufferFull;
	
    ofFbo numberFbo;
    ofTrueTypeFont numberFont;
    ofPixels numberPixels;
    float removeOpacity;
    
	    

	//Playback
    int spanWidth;
    int spanHeight;
    ofFbo fourScreenSpan;
    ofTexture spanTex;
    int showBoidsHead;
	int showBoidsTail;
	bool showBoids;
	bool removeLastBoid;
	unsigned char *  cutoutPixels;
	ofTexture cutoutTex;
	int nrDisplaySequences;
	int playbackIndex;  
	int camHeightScale;
	int camWidthScale;
    bool frameDirection;
	

	//Path
	Path pth;
    Path **paths;
    int pathIndex;
    ofxXmlSettings pathsXML;
    ofPolyline tempPl;
    float pathZ;
    bool pathZoom;
    float boidRotation;
    ofRectangle pathScaleRect;
   
    
	//Settings
	string message;
	int lastTagNumber;
		
	//Flocking
	ofxFlocking flock;
	float setSeparation;
	float setMaxSpeed;
	float setForce;
	
    //Show Managment
	int showState;
	bool cvImgDisp;
	int mode;
	ofxXmlSettings showXML;
 
    
		
	
};

#endif


