#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
	
    ofEnableAlphaBlending();
    //ofEnableSmoothing();
	ofSetFrameRate(30);
    
    
    // Show Management
    showXML.loadFile("show.xml");
    pathIndex = showXML.getValue("PATH_INDEX", 0);
	
    
	ofSetDataPathRoot("../Resources/data/");
	//ofSetDataPathRoot("./data/");
	//if we need to package up program
	
	//Syphon
	mClient.setup();
    mClient.setApplicationName("Simple Server");
    mClient.setServerName("");
    spanServer.setName("Span");
     
    //UI
	drawFill = true;
	float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING; 
    float length = 320-xInit;
    vector<string> dropdownNames;
    
	for (int i = 0; i < NUM_PATHS; i ++) {
        dropdownNames.push_back("PATH " + ofToString(i+1));
    }
    
    editGui = new ofxUICanvas(0,0,length+xInit*2.0,ofGetHeight());
    recordGui = new ofxUICanvas(0,0,length+xInit*2.0,ofGetHeight());
    
	editGui->addWidgetDown(new ofxUILabel("EDIT MODE", OFX_UI_FONT_LARGE)); 
	editGui->addSpacer(length, 2); 
    editGui->addWidgetDown(new ofxUILabel("BOID CONTROL", OFX_UI_FONT_MEDIUM));
	editGui->addMinimalSlider("SEPARATION", 0, 2, setSeparation, 95, dim);
    editGui->addMinimalSlider("MAXFORCE", 0, 1, setForce, 95, dim);
    editGui->addMinimalSlider("MAXSPEED", 0, 15, setMaxSpeed, 95, dim);
	 
    ddl = new ofxUIDropDownList(100, "PATHS", dropdownNames, OFX_UI_FONT_MEDIUM);
	ddl->setAutoClose(true);
    ddl->setAllowMultiple(false);
    editGui->addWidgetDown(ddl);
    
    
    //Record GUI
    
    recordGui->addWidgetDown(new ofxUILabel(" RECORD MODE ", OFX_UI_FONT_MEDIUM));
	recordGui->addMinimalSlider("THRESHOLD", 0, 100, threshold, 95, dim);
    recordGui->addSpacer(length, 2);
	recordGui->addWidgetDown(new ofxUILabel("RECORD FLYING", OFX_UI_FONT_MEDIUM));
    recordGui->addMinimalSlider("END TIMER", 0, 10, endSpeed, 95, dim);
	ofAddListener(recordGui->newGUIEvent,this,&testApp::guiEvent);
    ofAddListener(editGui->newGUIEvent,this,&testApp::guiEvent);
    recordGui->loadSettings("GUI/recordSettings.xml");
    editGui->loadSettings("GUI/editSettings.xml");
    recordGui->toggleVisible();
    editGui->toggleVisible();
	
    //Midi
    // print input ports to console
	midiIn.listPorts();
	
	// open port by number
	midiIn.openPort("PhotonX25");
	osculatorIn.openPort("OSCulator Out");
    
	// don't ignore sysex, timing, & active sense messages,
	// these are ignored by default
	midiIn.ignoreTypes(false, false, false);
	
	// add testApp as a listener
	midiIn.addListener(this);
  
    // print received messages to the console
	//midiIn.setVerbose(true);
    osculatorIn.ignoreTypes(false,false,false);
    
	osculatorIn.addListener(this);
    
    //osculatorIn.setVerbose(true);
    
	
    //Kinect Thread
    camWidth 	= 640;
	camHeight 	= 480;
    kinThread.kinect.init();
    kinThread.kinect.setVerbose(true);
    kinThread.kinect.open();
    kinThread.kinect.setCameraTiltAngle(0);
    kinThread.kinect.setUseTexture(false);
    kinThread.colorPix.allocate(640, 480, OF_PIXELS_RGB);
    kinThread.depthPix.allocate(640, 480, OF_PIXELS_MONO);
   
    kinThread.depthPixFloat = new float[kinThread.kinect.width*kinThread.kinect.height];
    kinThread.depthPixChar = new unsigned char[kinThread.kinect.width*kinThread.kinect.height];
    
   // cout << "kienct width : " << kinThread.kinect.width << endl;
    kinThread.initAndSleep();
    
	
    	
	// OpenCV
	getBackground = false; // true is auto, false is manual
	highBlob = 9999999;
	lowBlob = 100;
	cvColor.allocate(camWidth,camHeight);
	depthGray.allocate(camWidth,camHeight);
	cvBackground.allocate(camWidth,camHeight);
	cvThresh.allocate(camWidth,camHeight);
    colorBackground.allocate(camWidth,camHeight);
	diffMode = 1;
    
	
	// Recording
	nrDisplaySequences = NUM_SEQUENCES;
	cutoutTex.allocate(camWidth,camHeight,GL_RGBA);
	cutoutPixels = new unsigned char[camWidth*camHeight*4];
	record = 0;
	index = 0;  
	
    
	endRecordSequence = false;
	endRecordSequenceTime = 0;
	bufferFull = false;
    
    numberFbo.allocate(camWidth, camHeight);
    numberFont.loadFont("verdana.ttf", 120);
    numberPixels.allocate(camWidth, camHeight, OF_PIXELS_BGRA);
    
    spanWidth = 1280*4;
    spanHeight = 960;
    fourScreenSpan.allocate(spanWidth, spanHeight);
    
	
	//Playback
	playbackIndex = 0;
    frameDirection = true;
	
	
    
    //Paths
    paths = new Path*[NUM_PATHS];
    
    for(int i = 0; i < NUM_PATHS; i ++){
        
        paths[i] = new Path();
        string  l  = ofToString(i+1)+".png";
        cout << "trying to load image: " + l << endl;
        
        paths[i]->background.loadImage(l);
        paths[i]->background.resize(spanWidth, spanHeight);

    
    }
    
   
    if( pathsXML.loadFile("paths.xml") ) {
   
    int availPaths = pathsXML.getNumTags("PATH");
    
        cout << "availPaths: " << availPaths << endl;
        
        for (int p = 0; p < availPaths; p ++) {
            
            pathsXML.pushTag("PATH", p);
            
            for (int s = 0; s < pathsXML.getNumTags("STROKE"); s ++) {
                
                pathsXML.pushTag("STROKE",s);
                
                ofPolyline pl;
                
                if( pathsXML.getNumTags("PT") > 0){
                    
                    for(int i = 0; i < pathsXML.getNumTags("PT"); i++){
                        
                        int x = pathsXML.getValue("PT:X", 0, i);
                        int y = pathsXML.getValue("PT:Y", 0, i);
                        int z = pathsXML.getValue("PT:Z", 0, i);
                        
                        pl.addVertex(x, y, z);
                        
                    }
                    cout << " PL.size: " << pl.size() << endl;
                    
                    paths[p]->addPath(pl);
                    
                }
                
                pathsXML.popTag();
                paths[p]->rot = pathsXML.getValue("ROTATE", 0);
            }
            
            //this pops us out of the STROKE tag
            //sets the root back to the xml document
            
            pathsXML.popTag();
        }
    
    }else{
        pathsXML.addTag("PATH");
        pathsXML.pushTag("PATH", pathIndex);
    
    }
	pathZ = MIN_VIDEO_SIZE;
	

    
    
	//Debug 
	cvImgDisp = false;
    
    numberFbo.begin();
    ofPushStyle();
    ofClear(255, 100, 100,120);
    ofSetColor(0);
    numberFont.drawString(ofToString(showBoidsHead), camWidth/2, camHeight/2);
    ofPopStyle();
    numberFbo.end();
    numberFbo.readToPixels(numberPixels);
    
    
	//Flock
	for(int i = 0; i < NUM_SEQUENCES; i ++) {
		flock.addBoid(ofRandom(100, 500),ofRandom(100,2*ofGetHeight()/3), NUM_FRAMES+1);
         flock.boids[i].updateValues(setSeparation, setForce, setMaxSpeed); //setDesiredSeparation,
	}
    
    flock.follow( paths[pathIndex]);
    
	showBoidsHead = 0;
	showBoidsTail = 0;
	showBoids = false;
	removeLastBoid = false;
    
    
    
       
}


//--------------------------------------------------------------
void testApp::update(){
	
     kinThread.updateOnce();
    
//    cout << " new frame ? " << kinThread.timeSinceLastFrame << endl;
   
//    if(kinThread.timeSinceLastFrame > 1000){
//    
//        
//       
//        cout << "attempt reconnect";
//        kinThread.kinect.close();
//        
//        if(kinThread.)
//        kinThread.kinect.init();
//        kinThread.kinect.open();
//        kinThread.kinect.setCameraTiltAngle(0);
//        kinThread.kinect.setUseTexture(false);
////
//   }
    
  
    //Draw Flying Videos to Syphon Layer
    drawBoids();
    
  	
    //Frame Management
    updateVideo();
    
    //If the number of displayed boids is greater than the desired number
	if ( showBoidsHead - showBoidsTail > NUM_SEQUENCES - 6){
		removeLastBoid = true;
       
    }
    
    if(removeLastBoid){
        
           removeOpacity -= 3;
        
        if(removeOpacity <= 0){
            
            showBoidsTail ++;
            removeLastBoid = false;
            removeOpacity = 255;
            
            cout<< "last boid removed" << endl;

            
        }
    
    }
    
    
    for (int j = showBoidsTail; j < showBoidsHead; j ++){
        
        //Loop i around the size of the buffer using modulo
        int i = j % NUM_SEQUENCES;
        
        flock.boids[i].updateValues(setSeparation, setForce, setMaxSpeed); //setDesiredSeparation,
       
        if(paths[pathIndex]->polylines.size() > 0){
            flock.boids[i].update(flock.boids, paths[pathIndex]);
        }
            
    }
    
    
    
//    if(mode == 1){
//        if(pathZoom){
//            pathZ += 2;
//        }else{
//         if(pathZ > MIN_VIDEO_SIZE) {
//            pathZ -= 2;
//           }
//        }
//    
//    }
    
}


void testApp::updateVideo(){
		
        
	if(kinThread.isFrameNew){
        
                  
            //Check for new frame and increase index if frame is new
            // Dont go out of bounds
            
            if(playbackIndex >= NUM_FRAMES-1){
                
                frameDirection = false;
                
            }
            if(playbackIndex <= 0){
                frameDirection = true;
            }
            
         if(frameDirection){ // && playbackIndex < NUM_FRAMES-1 && playbackIndex > 0
             
             if(playbackIndex < NUM_FRAMES-1){
                 playbackIndex++;
              }
            }else{
                if(playbackIndex > 0){
             playbackIndex--;
                }
           }
        
        
		cvColor.setFromPixels(kinThread.colorPix.getPixels(), camWidth,camHeight);
        depthGray.setFromPixels(kinThread.depthPix.getPixels(), camWidth,camHeight);
    
        
      	depthGray.blur();
        
		
		//Grab background after ten frames
		if (getBackground == true && ofGetFrameNum() > 10) {
			
            colorBackground = cvColor;
            cvBackground = depthGray;
			getBackground = false;
			
		}
		
		//Background subtraction options
		if(diffMode == 0){
			cvThresh = depthGray;
			cvThresh.absDiff(cvBackground);
		}else if(diffMode == 1){
			cvThresh = depthGray;
			cvThresh -= cvBackground;
		}else if(diffMode == 2){
			cvThresh = cvBackground;
			cvThresh -= depthGray;
		}
		
		cvThresh.threshold(threshold);
        
        cvThresh.blur(10);
        
		
        //For flapping - may not be necessary
        //contourFinder.findContours(cvThresh, lowBlob, highBlob, 1, true, true);
		
        //Blurring the threshold image before using it as a cutout softens edges
        //cvThresh.blur();
       // cvThresh.blurHeavily();
        
		//Removing Background
               
		unsigned char * colorPix = cvColor.getPixels();
		unsigned char * grayPix = cvThresh.getPixels();
		//unsigned char * frameDiffPix = frameDiff.getPixels();
		
		
		for(int i = 0; i < (camWidth*camHeight); i++){
			
			//If pixel has content
			if(grayPix[i] > threshold){
				
				cutoutPixels[(i*4)+0] = colorPix[(i*3)+0];
				cutoutPixels[(i*4)+1] = colorPix[(i*3)+1];
				cutoutPixels[(i*4)+2] = colorPix[(i*3)+2];
				cutoutPixels[(i*4)+3] = grayPix[i]; //
            
            //If no content, set pixel to be translucent
			}else if(grayPix[i]  < threshold){
					
					cutoutPixels[(i*4)+0] =	0;
					cutoutPixels[(i*4)+1] = 0;
					cutoutPixels[(i*4)+2] = 0;
					cutoutPixels[(i*4)+3] = 0;
					
				}
			
		}
        
        
        //Check to see if all 24 spots are full
		if(showBoidsHead-showBoidsTail >= NUM_SEQUENCES-1){
			record = 0;
			bufferFull = true;
		}else {
			bufferFull= false;
		}
        
		
		//Record a sequence
       if (record == 1 ) 
		{
			
            //Texture goes on GPU, ofPixels on CPUs
            if(cvImgDisp){
                
                flock.boids[showBoidsHead%NUM_SEQUENCES].pixels[index].loadData(numberPixels.getPixels(), camWidth, camHeight, GL_RGBA);
                //flock.boids[showBoidsHead%NUM_SEQUENCES].pixels[index].setFromPixels(numberPixels.getPixels(), camWidth, camHeight, 4);
            
            }else{
                
                //flock.boids[showBoidsHead%NUM_SEQUENCES].pixels[index].setFromPixels(cutoutPixels, camWidth, camHeight, 4);
                flock.boids[showBoidsHead%NUM_SEQUENCES].pixels[index].loadData(cutoutPixels, camWidth, camHeight, GL_RGBA);
            
            }
            index ++;
			
			
			if (index == NUM_FRAMES)
			{	
				
				index = 0;
				record = 0;
				
                //Put that boid offscreen at start of path
                
                flock.boids[showBoidsHead%NUM_SEQUENCES].follow( paths[pathIndex], 0);
				
                //Increment display int
				showBoidsHead ++;
				
                //Start end record sequence
				endRecordSequence = true;
				endRecordSequenceTime = ofGetElapsedTimeMillis();
                
                // For debugging
                numberFbo.begin();
                ofPushStyle();
                ofClear(255, 100, 100,120);
                ofSetColor(0);
                numberFont.drawString(ofToString(showBoidsHead), camWidth/2, camHeight/2);
                ofPopStyle();
                numberFbo.end();
                numberFbo.readToPixels(numberPixels);

								
			}
			
		}//End Record
		
		//Load background subtracted image for displaying 
		cutoutTex.loadData(cutoutPixels,camWidth,camHeight,GL_RGBA); 
        
		
	}//End of New Frame Check
	
	
    //If the sequence is done recording
    if ( endRecordSequence && ofGetElapsedTimeMillis() - endRecordSequenceTime >= endRecordSequenceDelay*1000) {
        endRecordSequence = false;
    }
	
    //Draw Syphon client
    mClient.draw(50, 50);
	
    
       
}
//--------------------------------------------------------------
void testApp::draw(){
	
	ofClear(255, 255, 255, 0);
	ofSetColor(255, 255, 255,255);
	
	
    ///If in Record Mode
	if(mode == 1 && cvImgDisp){
    
               
        float wS = 0;
        
        wS = ofGetWidth();
        wS -= 2*PATH_RECT_IMAGE_PAD;
        wS /= spanWidth;
        
        pathScaleRect.width = spanWidth* wS;
        pathScaleRect.height = spanHeight * wS;
        pathScaleRect.x = PATH_RECT_IMAGE_PAD;
        pathScaleRect.y = ofGetHeight()/2;
        
        
        
        ofPushMatrix();
        
        ofTranslate(pathScaleRect.x, pathScaleRect.y);
        
        
        ofScale(wS, wS);
        
        fourScreenSpan.draw(0, 0);
        
        ofPopMatrix();
        ofPushStyle();
        ofNoFill();
        ofSetColor(200, 0, 0, 200);
        ofEllipse(ofGetMouseX(), ofGetMouseY(), pathZ/2, pathZ/2);
        ofPopStyle();
    }
    
       
    
    if(mode == 0 || mode == 2) { //Anything other than mode 1
        
        //Clear Background
        ofClear(0, 0, 0, 0); //Tranparent Background)
       
        
        //Draw Monitor/Cutout Video
        drawMonitor();
	}
    
	//Debug can be visible anywhere
    if (cvImgDisp) {
            drawCVImages();
        }else{
            ofSetColor(255, 255, 255,255); // White for display color
    }
    
//    //Debug Functions
//    ofSetColor(255,255,255, 255);
//	ofDrawBitmapString(ofToString(showState), ofGetWidth()-50, ofGetHeight()-50);
//	

}


void testApp::drawBoids(){
	
    
    fourScreenSpan.begin();
    ofPushStyle();
    
    //Set to 0,0,0,0 for black transparent
    ofClear(0, 0, 0, 0);
    
    //Draw lines
    if(mode == 1 && cvImgDisp){
        
        ofPushStyle();
        
        
        ofNoFill();
        //Draw Temp Line
        
        ofSetColor(255, 255, 255, 255);
        
        //Draw guide image
        if(paths[pathIndex]->background.isAllocated()){
            
            paths[pathIndex]->background.draw(0,0,spanWidth,spanHeight);
            
        }
        
        ofSetColor(200, 0, 0, 200);
        
        tempPl.draw();
        
          
        
        vector<ofPoint> pth = tempPl.getVertices();
        
        for (int p = 0; p < pth.size(); p++ ) {
            
            ofEllipse(pth[p].x, pth[p].y, pth[p].z, pth[p].z);
            
        }
        
        //Draw Saved Lines
        ofPushStyle();
        
        for (int i = 0; i < paths[pathIndex]->polylines.size(); i ++) {
            
            ofSetColor(paths[pathIndex]->pathColors[i]);
            
            vector<ofPoint> pth = paths[pathIndex]->polylines[i].getVertices();
          
            
            ofBeginShape();
            
            for (int p = 0; p < pth.size(); p++ ) {
                
                float z = pth[p].z;
                ofEllipse(pth[p].x, pth[p].y , z, z);
                ofVertex(pth[p].x, pth[p].y);
                
            }
            ofEndShape();
            
              
            
        }
        ofPopStyle();
        
    }
    

   
	//Set background to be transparent
	 //Color affects image tint, set to full opacity
	ofSetColor(255, 255, 255,255);  
	
	//Variables for boid displaying
	//ofPoint pathPoint;
	
    //ofVec3f loc;
	
	//float zScale;
	//float lFlap;
    //float lLastFlap;
	
	//Boid display boolean
//    if(showBoids)
//	{
    
	for (int j = showBoidsTail; j < showBoidsHead; j ++)
		{   
        
        //Loop i around the size of the buffer using modulo
        int i = j % NUM_SEQUENCES;
        
            
        ofPushStyle();
            
        //Fadeout last boid
        if(removeLastBoid && i == showBoidsTail%NUM_SEQUENCES){
          
            ofSetColor(255, 255, 255, removeOpacity);
            
        }
            flock.boids[i].drawVideo(playbackIndex);
            ofPopStyle();
        }
    
//    }

    ofPopStyle();
    
    fourScreenSpan.end();
    
    spanServer.publishTexture(&fourScreenSpan.getTextureReference());

}


void testApp::drawMonitor(){
    
    
    //Cutout for Recording
	ofClear(0, 0, 0, 255);
	
    ofPushStyle();
    
    //Opacity Mapping
    
    colorBackground.draw(0, (ofGetHeight()/2)-(camHeightScale/2) , camWidthScale, camHeightScale);
    
  
    float t = ofMap(index, 0, NUM_FRAMES, 255, 0);
    ofSetColor(255,  t ); //ofSetColor can change the tint of an image
   
    
    if( endRecordSequence){
        ofSetColor(255, 0); //See if we get monochrome
    }
    
    cutoutTex.draw(0, (ofGetHeight()/2)-(camHeightScale/2) , camWidthScale, camHeightScale);  // correct proportions
   
    if(bufferFull){
        ofSetColor(255, 0, 0, 255);
    }
    
    ofRect(800,(ofGetHeight()/2), 20, (NUM_FRAMES-index)*2);
	ofRect(800,(ofGetHeight()/2), 20, -(NUM_FRAMES-index)*2);
	
    ofPopStyle();
    
}


void testApp::guiEvent(ofxUIEventArgs &e){
	
	
	string name = e.widget->getName();
	int kind = e.widget ->getKind();
    
    //Boid Control
	
	if( name == "SEPARATION"){
		
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		setSeparation = slider -> getScaledValue();
	}
    else if( name == "MAXSPEED"){
		
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		setMaxSpeed = slider -> getScaledValue();
	}
	else if( name == "MAXFORCE"){
		
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		setForce = slider -> getScaledValue();
	}
	
    //CV Settings
	else if( name == "THRESHOLD"){
		
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		threshold = slider -> getScaledValue();
	}
	
	
    //Record Flying
	else if(name == "END TIMER" ){
		
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		endRecordSequenceDelay = slider->getScaledValue();
	}
    else if(name == "PATHS"){
    
    ofxUIDropDownList *ddlist = (ofxUIDropDownList *) e.widget;
    vector<ofxUIWidget *> &selected = ddlist->getSelected();
  
    for(int i = 0; i < selected.size(); i++)
    
    {
        
        int in = selected[i]->getID();
        
        in /= 2;
        in -= 5;
        
        cout << "pathIndex: " << in << "rawIndex: " << selected[i]->getID() << endl;
        
        if(pathIndex != in){
          pathIndex = in;
          flock.follow(paths[pathIndex]);
            
            cout << " path DROPDOWN called " << endl;
            if(pathsXML.getPushLevel() > 0){
                pathsXML.popTag();
            }
            
            if (!pathsXML.tagExists("PATH", pathIndex)) {
            pathsXML.addTag("PATH");
            }
            
            pathsXML.pushTag("PATH", pathIndex);


            
        }
        
     }
        
    }
    
    
			
}

//Used as a debug function
void testApp::drawCVImages()
{
	camWidth  = camWidth/2;
	camHeight = camHeight/2;
	
     
    //numberFbo.draw(camWidth*2, 100, camWidth, camHeight);
    
	//cvColor.draw(100+camWidth, 100, camWidth, camHeight);
	//depthGray.draw(350, 100+camHeight, camWidth, camHeight);
	//cvBackground.draw(100+camWidth, 100+camHeight, camWidth, camHeight);
	//cvThresh.draw(100, 100+camHeight*2, camWidth, camHeight);
	
	camWidth = camWidth*2;
	camHeight = camHeight*2;
	
	ofFill();
	ofSetColor(255, 255, 255,255);
	
	string info = "";
	info += "XML Push Level" + ofToString(pathsXML.getPushLevel()) + "\n";
	
	info += "number of boids displayed : " + ofToString(showBoidsHead-showBoidsTail) + "\n";
	//info += "showState: " + ofToString(showState) + "\n";
	
    info += "showBoidsHead : " + ofToString(showBoidsHead) + "\n";
	info += "showBoidsTail : " + ofToString(showBoidsTail) + "\n";
	
	info += "showBoidsHead % NUM_SEQUENCES " +ofToString(showBoidsHead % NUM_SEQUENCES) + "\n";
	info += "showBoidsTail % NUM_SEQUENCES: " +ofToString(showBoidsTail % NUM_SEQUENCES) + "\n";
	
    info += "Record: " +ofToString(record) + "\n";
	
    info += "Framerate: " + ofToString(ofGetFrameRate()) + "\n";
    info += "Remove Last Boid: " + ofToString(removeLastBoid) + "\n";
	
    info += "\n \n \n";
    
    info += "Keyboard mappings:  \n";
    info += "Clear paths = x \n";
    info += "Switch Mode = m \n";
    info += "Trigger Record = r \n";
    info += "Capture Background = b \n";
    info += "Increase Path Size = UP \n";
    info += "Decreas Path Size = DOWN \n";
    
    
    
    
    
	info += message +"\n";
	
	ofSetHexColor(0x444342);
	ofDrawBitmapString(info, 350, 30);
	

}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	
	// in fullscreen mode, on a pc at least, the
	// first time video settings the come up
	// they come up *under* the fullscreen window
	// use alt-tab to navigate to the settings
	// window. we are working on a fix for this...
	
    if(key == OF_KEY_UP){
        if(mode == 1){
        pathZ += 6;
        }else{
        threshold += 2;
        }
    }
    
    if(key == OF_KEY_DOWN){
        
        if(mode == 1 && pathZ > MIN_VIDEO_SIZE){
            pathZ -= 6;
        }
        
        if(mode == 0 || mode == 2){
             threshold -= 2;
        }
        
    }
    
	if (key == 'f' || key == 'F') {
		ofToggleFullscreen();
	}
	
	//Grab Background
	if(key == 'b' || key == 'B'){
		getBackground = true;
	}
	
	//Show debug 
	if(key == 'u' || key == 'U'){
		if(cvImgDisp){
			cvImgDisp = false;
		}else {
			cvImgDisp = true;
		}
	}
    
	//Clear path
    if (key == 'x' || key == 'X') {
        
        if(mode == 1 && pathsXML.getPushLevel() > 0){
            
            int n = pathsXML.getNumTags("STROKE");
            
            for (int i = 0; i < n;  i ++) {
                pathsXML.clearTagContents("STROKE");
            }
            //Test viability of this
            pathsXML.popTag();
            pathsXML.clearTagContents("PATH", pathIndex);
            pathsXML.pushTag("PATH", pathIndex);
            pathsXML.saveFile("paths.xml");
            paths[pathIndex]->clear();
            
        }
	}
	
	
  
	
    //Background sub mode
	if(key =='1'){
		diffMode = 0;
	}else if(key =='2'){
		diffMode = 1;
	}else if(key == '3'){
		diffMode = 2;
	}
    
    
//    if(key == OF_KEY_RIGHT){
//		highBlob += 1000;
//	}else if(key == OF_KEY_LEFT){
//		highBlob -= 1000;
//	}
//	
//	if(key == '='){
//		lowBlob += 1000;
//	}else if(key == '-'){
//		lowBlob -= 1000;
//	}
//	
    
//	if(key == OF_KEY_UP){
//		threshold += 2;
//	}else if(key == OF_KEY_DOWN){
//		threshold -= 2;
//		if(threshold < 0)threshold = 0;
//	}
    
	if(key == 'r'){
		 
     record = 1;
          
	}
    
    if(key == 'm'){
        
        mode += 1;
        
        if(mode > 2){
            mode = 0;
        }
        
        if(mode == 1){
            
            if ( pathsXML.tagExists("PATH", pathIndex) == false) {
            cout<< "pathIndex " <<  pathIndex << endl;
            
            pathsXML.addTag("PATH");
                
            }
            
            pathsXML.pushTag("PATH", pathIndex);
            
         
            cvImgDisp = true;
            editGui->toggleVisible();
        }
        if(mode == 2){
            
            cvImgDisp = false;
            pathsXML.popTag();
            editGui->toggleVisible();
            recordGui->toggleVisible();
        }
        if(mode == 0){
            recordGui->toggleVisible();
        }
        
    }
    
    
    if(key == ' '){
        if (mode == 1) {
            pathZoom = true;
        }
    
    }
    
    if(key == 'z'){
        recordBlanks();
    }
	
    if(key == 'n'){
        flock.follow(paths[pathIndex]);
        
    }
	
	
	
	 
}


//--------------------------------------------------------------
void testApp::keyReleased(int key){
	pathZoom = false;
    //pathsXML.saveFile("paths.xml");
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

		
	if(button == 2){
	
		//This mapping allows for offscreen drawing, which enables removing of the boids
		int mx = (int)ofMap(x, 0, ofGetWidth(), PATH_RECT_IMAGE_PAD*-2,  spanWidth + 2*PATH_RECT_IMAGE_PAD);
        int my = (int)ofMap(y, pathScaleRect.y,  pathScaleRect.y + pathScaleRect.height,  0, spanHeight );
		
        tempPl.addVertex(mx, my, pathZ); //, pathZ

        
    if(pathsXML.pushTag("STROKE", lastTagNumber)){
    
        int tagNum = pathsXML.addTag("PT");
        pathsXML.setValue("PT:X", mx, tagNum);
        pathsXML.setValue("PT:Y", my, tagNum);
        pathsXML.setValue("PT:Z", pathZ, tagNum);
        pathsXML.popTag();
      
        }
        
    }
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
    if(button == 2){
        lastTagNumber = pathsXML.addTag("STROKE");
    }
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
    if (tempPl.size() > 0) {
        
        tempPl.simplify();
        
        paths[pathIndex]->addPath(tempPl);
        paths[pathIndex]->rot = boidRotation;
        pathsXML.setValue("ROTATE", boidRotation);
        
        tempPl.clear();
        
    }
     pathsXML.saveFile("paths.xml");
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    //This doesn't need to be calculated for each update
    camWidthScale = ofGetWidth();
	camHeightScale = camWidthScale * 0.75; //should give proportions of 4:3

    
    
}

void testApp::recordBlanks(){
    
   
        int numberX = 0;
        int numberY = 0;
        
        for (int i = 0; i < NUM_SEQUENCES; i ++) {
            
            showBoidsHead = i;
            numberX = 100;
            numberY = 100;
            
            
            for(int f  = 0; f < NUM_FRAMES; f++){
                
                numberFbo.begin();
                
                ofPushStyle();
                
                ofClear(255, 100, 100,120);
                
                ofSetColor(0);
                
                
                if(numberX > camWidth){
                    numberX = 0;
                }
                if(numberY > camHeight){
                    numberY = 0;
                }
                numberX += 10;
                numberY += 10;
                
                numberFont.drawString(ofToString(showBoidsHead), camWidth/2, camHeight/2);
                
                ofEllipse(numberX, numberY, 100, 100);
                
                ofPopStyle();
                
                numberFbo.end();
                numberFbo.readToPixels(numberPixels);
                
                // flock.boids[showBoidsHead%NUM_SEQUENCES].pixels[f].setFromPixels(numberPixels.getPixels(), camWidth, camHeight, 4);
                flock.boids[showBoidsHead%NUM_SEQUENCES].pixels[f].loadData(numberPixels.getPixels(), camWidth, camHeight, GL_RGBA);
                
            }
        }
        
        flock.follow(paths[pathIndex]);
       
    


}

void testApp::newMidiMessage(ofxMidiMessage& msg) {
    
	// make a copy of the latest message
	midiMessage = msg;
     
    if(msg.channel == 1){
        
        switch (msg.pitch) {
            case 48:
                pathIndex = 0;
                 flock.follow(paths[pathIndex]);
                break;
            case 50:
                pathIndex = 1;
                 flock.follow(paths[pathIndex]);
                break;
            case 52:
                pathIndex = 2;
                 flock.follow(paths[pathIndex]);
                break;
            case 53:
                pathIndex = 3;
                flock.follow(paths[pathIndex]);
                break;
            case 55:
                pathIndex = 4;
                 flock.follow(paths[pathIndex]);
                break;
            case 57:
                pathIndex = 5;
                flock.follow(paths[pathIndex]);
                break;
            case 59:
                pathIndex = 6;
                 flock.follow(paths[pathIndex]);
                break;
            case 60:
                pathIndex = 7;
                 flock.follow(paths[pathIndex]);
                break;
            case 62:
                pathIndex = 8;
                 flock.follow(paths[pathIndex]);
                break;
            case 64:
                pathIndex = 9;
                 flock.follow(paths[pathIndex]);
                break;
//            case 65:
//                pathIndex = 10;
//                break;
            
            
        }
        
       
        
       
        
    }
    
    //Space Navigator Button
    if(msg.channel == 10){
        record = 1;
        cout << "record: " << record << endl;
    }
    
}


//void testApp::connexionMoved(ConnexionData& data) {
//	// this would be a good place to get data from multiple devices
//	cout << data.getButton(0) << " " << data.getButton(1) << endl;
//	
//	// use the button state to set the LED
//	ofxConnexion::setLed(data.getButton(0) || data.getButton(1));
//}

void testApp::exit(){
	
    kinThread.pauseUpdate();
    kinThread.stop();
    kinThread.stopThread();
    
    
    showXML.setValue("PATH_INDEX", pathIndex);
    showXML.saveFile("show.xml");
   
    pathsXML.saveFile("paths.xml");
	recordGui->saveSettings("GUI/recordSettings.xml");
    editGui->saveSettings("GUI/editSettings.xml");
    
    delete editGui;
    delete recordGui;
    
    mClient.unbind();
   // ofxConnexion::stop();
	

}



