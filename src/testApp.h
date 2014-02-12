#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxVectorMath.h"
#include "ofxSyphon.h"
#include "ofxUI.h"
#include "ofxFlocking.h"
#include "ofxKinect.h"
#include "ofxOsc.h"
#include "Path.h"
#include "kinectThread.h"
#include "ofxMidi.h"



#define NUM_FRAMES 30
#define NUM_PATHS 20
#define MIN_VIDEO_SIZE 10
#define NUM_SEQUENCES 24
#define NUM_SERVERS 4
#define PATH_RECT_IMAGE_PAD 100






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
    
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	void recordBlanks();
    
	//Syphon
	ofTexture backgroundTex; 
	ofTexture foregroundTex;	
	ofTexture textTex;
	ofxSyphonServer spanServer;
	ofxSyphonServer backgroundServer;
	//ofxSyphonServer textServer;
    ofxSyphonServer servers[NUM_SERVERS];
	ofxSyphonClient mClient;
	float testX, testY;
	ofPixels spanPixels;
    ofTexture spanTextures[NUM_SERVERS];
    
    
	// UI 
	ofxUICanvas *editGui;
    ofxUICanvas *recordGui;
    ofxUIDropDownList *ddl; 
	void guiEvent(ofxUIEventArgs &e);
	bool drawFill; 
	string currentPath;
	
	
	//OSC Communication
    ofxOscReceiver oscReceiver;
    //Midi Input
    
    ofxMidiIn	midiIn;
	ofxMidiMessage midiMessage;
    void newMidiMessage(ofxMidiMessage& eventArgs);
	
	
    
    //Video
	//ofVideoGrabber 		vidGrabber;
	//ofVideoPlayer		video;
	
    
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
	ofxCvGrayscaleImage cvGray;
	ofxCvGrayscaleImage cvThresh;
	ofxCvColorImage		cvColor;
	ofxCvContourFinder 	contourFinder;
	ofxCvGrayscaleImage lastGray;
	ofxCvGrayscaleImage frameDiff;
	int blockSize;
	bool getBackground;
	
	
	//Recording
    
  	float endSpeed;
	float videoPos;
	float videoVel;
	float videoGravity;
	bool newSequence;
	int record;
	int index;
	float flap;
	float lastFlap;
    float mappedFlap;
	float flapThresh;
	float downMult;
	float upMult;
	
	int endRecordSequenceTime;
	int endRecordSequenceDelay;
	bool endRecordSequence;
    bool bufferFull;
	bool bufferFullDuringShow;
    ofFbo numberFbo;
    ofTrueTypeFont numberFont;
    ofPixels numberPixels;
    
	    

	//Playback
    int spanWidth;
    int spanHeight;
    ofFbo fourScreenSpan;
    ofTexture spanTex;
    int showBoidsHead;
	int showBoidsTail;
	bool showBoids;
	bool removeLastBoid;
	float staticFlyingPush;
	float staticFlyingPull;
	unsigned char *  cutoutPixels;
	ofTexture cutoutTex;
	int nrDisplaySequences;
	int bufferSize;
	int play;
	int playbackIndex;  
	int sequenceIndex;
	int playSequenceIndex;
	int startIndex;
	int scale;
	float flapMagnitude;
	float scaleMagnitude; 
	int camHeightScale;
	int camWidthScale;
    long frameRateTimer;
    bool frameDirection;
	

	//Path
	Path pth;
    Path **paths;
    int pathIndex;
    string pathId;
    ofxXmlSettings pathsXML;
    ofPolyline tempPl;
    float pathZ;
    bool pathZoom;
    float boidRotation;
    ofPoint closestPoint;
    ofVec2f predict;
    ofRectangle pathScaleRect;
   
    
	//Settings
	string message;
	//string xmlStructure;
	int lastTagNumber;
		
	//Flocking
	ofxFlocking flock;
	float setSeparation;
	float setAlignment;
	float setCohesion;
	float setMaxSpeed;
	float setForce;
	float setDesiredSeparation;
	int nrBoids;
	float lineFollowMult;
	float pushMaxSpeed;
    
	
    //Show Managment
	int showState;
	bool cvImgDisp;
	int mode;
	ofxXmlSettings showXML;
    //ofImage testImage;
    
		
	
};

#endif


