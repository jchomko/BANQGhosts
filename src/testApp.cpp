#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
	
		
	ofEnableAlphaBlending();  
	ofSetFrameRate(30);
	
	//ofSetDataPathRoot("./data/");
	//if we need to package up program
	
	//Syphon
	mClient.setup();
    mClient.setApplicationName("Simple Server");
    mClient.setServerName("");

	backgroundServer.setName("Background");
	
    textServer.setName("Text");
	backgroundTex.allocate(ofGetWidth(), ofGetHeight(),GL_RGBA);
	
	textTex.allocate(ofGetWidth(),300, GL_RGBA);
	
	//UI
	
	drawFill = true;
	float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING; 
    float length = 320-xInit;
    vector<string> dropdownNames;
    
	for (int i = 0; i < 40; i ++) {
        dropdownNames.push_back("PATH " + ofToString(i));
    }
    editGui = new ofxUICanvas(0,0,length+xInit*2.0,ofGetHeight());
    recordGui = new ofxUICanvas(0,0,length+xInit*2.0,ofGetHeight());
    
	editGui->addWidgetDown(new ofxUILabel("EDIT MODE", OFX_UI_FONT_LARGE)); 
	editGui->addSpacer(length, 2);
    ddl = new ofxUIDropDownList(100, "PATHS", dropdownNames, OFX_UI_FONT_MEDIUM);
	ddl->setAutoClose(true);
    
    editGui->addWidgetDown(ddl);
	editGui->addWidgetDown(new ofxUILabel("BOID CONTROL", OFX_UI_FONT_MEDIUM));
	editGui->addMinimalSlider("SEPARATION", 0, 50, setSeparation, 95, dim);
	editGui->addMinimalSlider("COHESION", 0, 10, setCohesion, 95, dim);
	editGui->addMinimalSlider("MAXSPEED", 0, 5, setMaxSpeed, 95, dim);
	editGui->addMinimalSlider("MAXFORCE", 0, 1, setForce, 95, dim);
    editGui->addMinimalSlider("FLAP MAGNITUDE", 0, 1, flapMagnitude, 95, dim);
	editGui->addMinimalSlider("LINE FOLLOW", 0, 200, lineFollowMult, 95, dim);
    
    //gui->addWidgetDown(new ofxUILabel("VIDEO SETTINGS", OFX_UI_FONT_MEDIUM));
	//gui->addMinimalSlider("VIDEO SCALE", 0, 640, scale, 95, dim);
	//gui->addMinimalSlider("SCALE MAGNITUDE", 0, 300, scaleMagnitude, 95, dim);
	
    recordGui->addWidgetDown(new ofxUILabel(" RECORD MODE ", OFX_UI_FONT_MEDIUM));
	recordGui->addMinimalSlider("THRESHOLD", 0, 100, threshold, 95, dim);
    
	recordGui->addSpacer(length, 2);
	recordGui->addWidgetDown(new ofxUILabel("RECORD FLYING", OFX_UI_FONT_MEDIUM));
	recordGui->addMinimalSlider("UP MULTIPLIER", 0,10, downMult, 95, dim);
	recordGui->addMinimalSlider("DOWN MULTIPLIER", 0, 1, upMult, 95, 0);
	recordGui->addMinimalSlider("FLAP THRESHOLD", 0, 1, flapThresh, 95, dim);
	recordGui->addMinimalSlider("END RECORD SEQUENCE DELAY", 0, 100 , endRecordSequenceDelay, 95, dim);
    recordGui->addMinimalSlider("END SPEED", 0, 2, endSpeed, 95, dim);
		
	ofAddListener(recordGui->newGUIEvent,this,&testApp::guiEvent);
    ofAddListener(editGui->newGUIEvent,this,&testApp::guiEvent);
    
	recordGui->loadSettings("GUI/recordSettings.xml");
    editGui->loadSettings("GUI/editSettings.xml");
    
    recordGui->toggleVisible();
    editGui->toggleVisible();
	
	//OSC
    oscReceiver.setup(8212);
		
	
	//Video
	camWidth 	= 640; 
	camHeight 	= 360; 
	
	vidGrabber.setVerbose(true);
	vidGrabber.listDevices();
	
	vidGrabber.setDeviceID(10);  
	vidGrabber.initGrabber(camWidth,camHeight,true);
    
	camWidthScale = ofGetWidth();
	camHeightScale = camWidthScale *0.562; //should give proportions of 16:9

	
	
	// OpenCV
	getBackground = false; // true is auto, false is manual
	
    highBlob = 9999999;
	lowBlob = 100;
	
	diffMode  = 0;
	flap	  = 0;
    mappedFlap = 0;
    
	
	cvColor.allocate(camWidth,camHeight);
	cvGray.allocate(camWidth,camHeight);
	cvBackground.allocate(camWidth,camHeight);
	cvThresh.allocate(camWidth,camHeight);
	lastGray.allocate(camWidth,camHeight);
	frameDiff.allocate(camWidth,camHeight);
	
	
	
	
	// Recording
	
    nrDisplaySequences = 12;
	bufferSize = 24;
   
	cutoutTex.allocate(camWidth,camHeight,GL_RGBA);
	cutoutPixels = new unsigned char[camWidth*camHeight*4];
	record = 0;
	index = 0;  
	
	videoPos = 0;
	videoVel = 0;
	
	endRecordSequence = false;
	endRecordSequenceTime = 0;
	
	bufferFull = false;
    bufferFullDuringShow = false;
  	
	//Playback
	play = 0;
	playbackIndex = 0;
	playSequenceIndex = 0;
	sequenceIndex = 0;
	startIndex = 0;
    scale = 300;
	
	
	
	
	//XML Settings
	message = "loading paths.xml";
	
	if (XML.loadFile("paths.xml")) {
		message = "paths.xml loaded";
	}else{
		message = "unable to load paths.xml check data/ folder";
	}
		int numDragTags = XML.getNumTags("STROKE:PT");
		
		if(numDragTags > 0 ){
		
			XML.pushTag("STROKE", numDragTags-1);
			
			int numPtTags = XML.getNumTags("PT");
			
			if (numPtTags > 0) {
				
				int totalToRead = numPtTags;
				
				for (int i = 0; i < totalToRead; i ++) {
					
					int x = XML.getValue("PT:X", 0,i);
					int y = XML.getValue("PT:Y", 0,i);
					pth.addPoint(x, y);
					
				}
				
			}
			
			XML.popTag();
		
		}
	
	
	//Debug 
	cvImgDisp = false;

	
	// Show Management
	
	showState = 0;
	
	
    
	//Flock
	for(int i = 0; i < bufferSize; i ++) {
		flock.addBoid(ofRandom(100, 500),ofRandom(100,2*ofGetHeight()/3));
	}
    
	showBoidsHead = 0;
	showBoidsTail = 0;
	showBoids = false;
	removeLastBoid = false;
    
    
    
    
}


//--------------------------------------------------------------
void testApp::update(){
	 
    
    //Show management
	if (showState > 1) {  // Check at beginning of update because multiple functions use this
		showState = 0;
	}
		
	
	if (showState == 0 ) {
		showBoids = false;
 
    }
    
    //Reset show variables
	if (showState == 1) {
        
        if (showBoidsHead > nrDisplaySequences) {
            showBoidsTail = showBoidsHead - nrDisplaySequences;
		}
		
		showBoids = true;
	
    }
	
	
	updateVideo();
    
    
    //Frame Management
    if(showBoids) {
		
		//Check for new frame and increase index if frame is new
		if(vidGrabber.isFrameNew()){
			// Dont go out of bounds
			if(playbackIndex == 99){
				playbackIndex = 0;
			}else {
				playbackIndex++;
			}
		}
	}
	
    
    //If the number of displayed boids is greater than the desired number
	if ( showBoidsHead - showBoidsTail > nrDisplaySequences ){
		removeLastBoid = true;
	}

    
	//Update Boids
    for  (int i = 0; i < nrDisplaySequences; i ++){
    	//This function can be removed once the preferred values are set
        
        //TODO update values in Update(), only if something's changed
        
        flock.boids[i].updateValues(setSeparation,setAlignment,setCohesion,
                                    setForce*0.001,setMaxSpeed,setDesiredSeparation, lineFollowMult);
        flock.boids[i].update(flock.boids);
		
        //Get boids location
       
       
        //TODO
        // paths[pathIndex].points.get etc
        
        flock.boids[i].seek(pth.points.getClosestPoint(flock.boids[i].getPredictLoc()));
        
	}
    
    
    
    if(oscReceiver.hasWaitingMessages()){
        
        ofxOscMessage om;
        if( oscReceiver.getNextMessage(&om)){
         //cout << om.getAddress();
         // get address and then do something with that value
         // we'll just parse out the column numbe or something
            
        }
        
    }

}


void testApp::updateVideo(){
		
	
	vidGrabber.update();	
	
    if (vidGrabber.isFrameNew()){
		
		cvColor.setFromPixels(vidGrabber.getPixels(), camWidth, camHeight);
		cvGray = cvColor;
		
		
		//Grab background after ten frames
		if (getBackground == true && ofGetFrameNum() > 10) {
			
			cvBackground = cvGray;
			getBackground = false;
			
		}
		
		//Background subtraction options
		if(diffMode == 0){
			cvThresh = cvGray;
			cvThresh.absDiff(cvBackground);
		}else if(diffMode == 1){
			cvThresh = cvGray;
			cvThresh -= cvBackground;
		}else if(diffMode == 2){
			cvThresh = cvBackground;
			cvThresh -= cvGray;
		}
		
		cvThresh.threshold(threshold);
        
		frameDiff.absDiff(cvGray, lastGray);
		
        //For flapping - may not be necessary
        contourFinder.findContours(cvThresh, lowBlob, highBlob, 1, true, true);
		
        //Blurring the threshold image before using it as a cutout softens edges
        cvThresh.blur();
       
		
		//Removing Background
		unsigned char * colorPix = cvColor.getPixels();
		unsigned char * grayPix = cvThresh.getPixels();
		unsigned char * frameDiffPix = frameDiff.getPixels();
		
		
		for(int i = 0; i < (camWidth*camHeight); i++){
			
			//If pixel has content
			if(grayPix[i] > 1){ 
				
				cutoutPixels[(i*4)+0] = colorPix[(i*3)+0];
				cutoutPixels[(i*4)+1] = colorPix[(i*3)+1];
				cutoutPixels[(i*4)+2] = colorPix[(i*3)+2];
				cutoutPixels[(i*4)+3] = 255;
            
            //If no content, set pixel to be translucent
			}else if(grayPix[i]  < 1){
					
					cutoutPixels[(i*4)+0] =	0;
					cutoutPixels[(i*4)+1] = 0;
					cutoutPixels[(i*4)+2] = 0;
					cutoutPixels[(i*4)+3] = 0;
					
				}
			
		}
        
//	    //Check to see if all 24 spots are full
//		if(showBoids == true && showBoidsHead-showBoidsTail >= bufferSize){
//			record = 0;
//			bufferFull = true;
//		}else {
//			bufferFull= false;
//		}
        
		
		//Record a sequence
       if (record == 1 ) 
		{
			//Accessing the pixels directly was the quickest method I could find
		    bufferSequences[showBoidsHead%bufferSize].pixels[index].setFromPixels(cutoutPixels, camWidth, camHeight, 4);
			bufferSequences[showBoidsHead%bufferSize].flaps[index] = mappedFlap;
			index ++;
			
			//Size of a Sequence is hard-coded here and in Sequence.h
			if (index == 100) 
			{	
				
				index = 0;
				record = 0;
				
				
				//Increment display int
				showBoidsHead ++;
				
				//Put that boid offscreen
                flock.boids[showBoidsHead%bufferSize].setLoc(ofVec2f(ofRandom(-100, -400),ofRandom(100,2*ofGetHeight()/3)));
				
				//Start end record sequence
				endRecordSequence = true;
				endRecordSequenceTime = ofGetElapsedTimeMillis();
								
			}
			
		}//End Record	
		
		//Load background subtracted image for displaying 
		cutoutTex.loadData(cutoutPixels,camWidth,camHeight,GL_RGBA); 
        
		
	}//End of New Frame Check
	
	
	//Automatic Recording 
	if (videoPos > 300 && !endRecordSequence ) {
		record = 1;
	}
	
	//If the sequence is done recording
	if (endRecordSequence) {
        
		//Pull the image offscreen
		if (videoPos < ofGetHeight()) {
			videoVel += endSpeed;
		}
		
		//Once the allotted time as passed, reset the values & stop showing the text
		if (ofGetElapsedTimeMillis() - endRecordSequenceTime >= endRecordSequenceDelay*1000) {
			endRecordSequence = false;
			videoPos = 0;
			videoVel = 0;
		}
		
	}
	
	//Getting Contour Height Data (Flap)
	if (contourFinder.nBlobs > 0) {
		
		flap = contourFinder.blobs[0].boundingRect.height;
		mappedFlap = ofMap(flap, 0, camHeight, 0, 1);
		
	}
	
	//If a user as raised their arms, and lowered them
	if (mappedFlap < lastFlap-flapThresh) { //+0.01
        
		//Set velocity to flap value
		videoVel = mappedFlap*upMult;
		
	}
	
	//Add velocity to positon
	videoPos += videoVel;
	
	
	//If the video position is above zero
	if(videoPos > 0) { 
		//Add gravity
		videoVel -= downMult;
	}
	
	//If we're below the ground
	if(videoPos <= 0) {
		//Set postion to 0
		videoVel  = 0;
	
	}
	
	//Get last flap to compare with new flap
	lastFlap = mappedFlap;
	
    //Draw Syphon client
    mClient.draw(50, 50);
	
}
//--------------------------------------------------------------
void testApp::draw(){
	
	ofClear(255, 255, 255, 0);
	ofSetColor(255, 255, 255,255);
	
	//Draw Flying Videos to Syphon Layer
	drawBoids();
	
	//Clear Background
	ofClear(255, 255, 255, 0); //Tranparent Background)
	ofBackground(255, 255, 255, 0); //Tranparent Background
	
	
    //Draw Monitor/Cutout Video
	drawMonitor();
		
	
    //Debug
    if (cvImgDisp) {
        drawCVImages();
	
    }else{
       ofSetColor(255, 255, 255,255); // White for display color
    }
    
    //Debug Functions
    ofSetColor(255,255,255, 255);
	ofDrawBitmapString(ofToString(showState), ofGetWidth()-50, ofGetHeight()-50);
	

}





void testApp::drawBoids(){
	
	//Set background to be transparent
	ofBackground(255, 255, 255, 0); 
	
    	
	//Color affects image tint, set to full opacity
	ofSetColor(255, 255, 255,255);  
	
	//Variables for boid displaying
	//ofPoint pathPoint;
	ofVec3f loc;
	
	float zScale;
	float lFlap;
    float lLastFlap;
	
	//Boid display boolean
    if(showBoids)
	{
        
	for (int j = showBoidsTail; j < showBoidsHead; j ++)
		{   
        
        //Loop i around the size of the buffer using modulo
        int i = j % bufferSize;
            
        loc.set(flock.boids[i].getLoc());
        
        //Draw the Sequence
        ofPushMatrix();
        
        //TODO 
		//Retrieve scale from the path info
            
       
		//Set coordinates
		ofTranslate(loc.x, loc.y);  	
		
		//Add 90 degrees of rotation to account for the camera's orientation
		//this may need to change depending on where the camera is placed in future
		ofRotate(loc.z+90); 
		
		bufferSequences[i].playBack(playbackIndex, scale);
        
        ofPushStyle();
        ofSetColor(255, 0, 0, 255);
        ofEllipse(0,0,100,100);
        ofPopStyle();
            
        ofPopMatrix();
		
         
		//TODO change to opacity fade out
        if(removeLastBoid ){
           
              //flock.boids[showBoidsTail%bufferSize].fadeOpacity Down
                //if opacity == 0
                 // then showBoidsTail ++;
				//Stop removing the last boid
                // removeLastBoid = false;
                
            }
		}
        
        ofPushStyle();
        ofSetColor(0, 0, 255, 255);
        ofEllipse(400,400,100,100);
        ofPopStyle();
        
    }
	
   
	
	//Load screen data
	backgroundTex.loadScreenData(0, 0, ofGetWidth(),ofGetHeight());
	//Publish screen data to syphon 
	backgroundServer.publishTexture(&backgroundTex);
	
	
	
}


void testApp::drawMonitor(){
    
    
    //Cutout for Recording
	ofSetColor(210, 229, 247, 255);  //BACKGROUND FOR FLYERS
	ofRect(0,0, ofGetWidth(), ofGetHeight());
	ofSetColor(255, 255, 255,255);
	
    
    ofSetColor(255, 255, 255,255); //ofSetColor can change the tint of an image
	cutoutTex.draw(0-videoPos,(ofGetHeight()/2)-(camHeightScale/2) , camWidthScale, camHeightScale);  // correct proportions
	
    
    
    
	//Show the 'End Record Sequence'
    
	if (endRecordSequence == true) {
		
		string goodbye = "Tu voles maintenant sur la Carte Blanche!";
		ofPushStyle();
		ofSetColor(28, 20, 255);
		ofPushMatrix();
		ofTranslate(900, (ofGetHeight()/2)+350);
		ofRotateZ(-90);
		ofPopMatrix();
		ofPopStyle();
		
	}
    
	//Tell people that the buffer is full if that is true
    if (bufferFull && !endRecordSequence) {
        
        string full = "La carte est plein! Jouer, mais le system t'enrigistre pas";
		ofPushStyle();
		ofSetColor(28, 20, 255);
		ofPushMatrix();
		ofTranslate(900, (ofGetHeight()/2)+480);
		ofRotateZ(-90);
        ofPopMatrix();
		ofPopStyle();
        
    }
	
	//Progress bar for recording
    //TODO remove hardcoding
    
    ofRect(800,(ofGetHeight()/2), 20, (100-index)*2);
	ofRect(800,(ofGetHeight()/2), 20, -(100-index)*2);
	
}


void testApp::guiEvent(ofxUIEventArgs &e){
	
	//Remove some of these
	
	string name = e.widget->getName();
	int kind = e.widget ->getKind();
    
    //Boid Control
	
	if( name == "SEPARATION"){
		
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		setSeparation = slider -> getScaledValue();
	}
	else if( name == "COHESION"){
		
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		setCohesion = slider -> getScaledValue();
	}
	else if( name == "MAXSPEED"){
		
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		setMaxSpeed = slider -> getScaledValue();
	}
	else if( name == "MAXFORCE"){
		
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		setForce = slider -> getScaledValue();
	}
	else if( name == "FLAP MAGNITUDE"){
		
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		flapMagnitude = slider -> getScaledValue();
	}
	else if( name == "LINE FOLLOW"){
		
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		lineFollowMult = slider -> getScaledValue();
	}
	
	//Video Settings
//	else if( name == "VIDEO SCALE"){
//		
//		ofxUISlider *slider = (ofxUISlider *) e.widget;
//		scale = slider -> getScaledValue();
//	}
	else if( name == "SCALE MAGNITUDE"){
		
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		scaleMagnitude = slider -> getScaledValue();
	}
	
	//CV Settings
	else if( name == "THRESHOLD"){
		
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		threshold = slider -> getScaledValue();
	}
	
	//Record Flying
	else if(name == "UP MULTIPLIER" ){
		
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		upMult = slider->getScaledValue();
	}
	else if(name == "DOWN MULTIPLIER" ){
		
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		downMult = slider->getScaledValue();
	}
	else if(name == "FLAP THRESHOLD" ){
		
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		flapThresh = slider->getScaledValue();
	}
	
	else if(name == "END RECORD SEQUENCE DELAY" ){
		
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		endRecordSequenceDelay = slider->getScaledValue();
	}
	
	else if(name == "END SPEED"){
		
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		endSpeed =slider -> getScaledValue();
		
	}
	
	//Text Speed
	else if(name == "TEXT SPEED" ){
		
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		textSpeed = slider->getScaledValue();
	
	}else if(name == "PATHS"){
    
    ofxUIDropDownList *ddlist = (ofxUIDropDownList *) e.widget;
    vector<ofxUIWidget *> &selected = ddlist->getSelected();
    for(int i = 0; i < selected.size(); i++)
    {
        cout << "SELECTED: " << selected[i]->getName() << endl;
    }
    }
		
	
			
}

//Used as a debug function
void testApp::drawCVImages()
{
	camWidth  = camWidth/2;
	camHeight = camHeight/2;
	
	vidGrabber.draw(100,  100, camWidth, camHeight);
	cvColor.draw(100+camWidth, 100, camWidth, camHeight);
	cvGray.draw(100, 100+camHeight, camWidth, camHeight);
	cvBackground.draw(100+camWidth, 100+camHeight, camWidth, camHeight);
	cvThresh.draw(100, 100+camHeight*2, camWidth, camHeight);
	
	camWidth = camWidth*2;
	camHeight = camHeight*2;
	
	ofFill();
	ofSetColor(255, 255, 255,255);
	
	string info = "";
		
	info += "number of boids displayed : " + ofToString(showBoidsHead-showBoidsTail) + "\n";
	info += "showState: " + ofToString(showState) + "\n";
	
	info += "showBoidsHead : " + ofToString(showBoidsHead) + "\n";
	info += "showBoidsTail : " + ofToString(showBoidsTail) + "\n";
	
	info += "showBoidsHead % bufferSize " +ofToString(showBoidsHead % bufferSize) + "\n";
	info += "showBoidsTail % bufferSize: " +ofToString(showBoidsTail % bufferSize) + "\n";
	
	info += "Number of Messages : " +ofToString(messages.size()) + "\n";
	
	info += "Record: " +ofToString(record) + "\n";
	info += "Index: " + ofToString(index) + "\n";
	
	info += message +"\n";
	
	ofSetHexColor(0x444342);
	ofDrawBitmapString(info, 30, 30);
	
	ofSetColor(255, 0,0);
	ofLine(ofGetWidth()-100, 0, ofGetWidth()-100, ofGetHeight());
    pth.display();
	
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	
	// in fullscreen mode, on a pc at least, the
	// first time video settings the come up
	// they come up *under* the fullscreen window
	// use alt-tab to navigate to the settings
	// window. we are working on a fix for this...
	
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
		pth.points.clear();
		XML.clear();
		lastTagNumber	= XML.addTag("STROKE");
		xmlStructure	= "<STROKE>\n";
	}
	
	
	if(key == 'g' || key == 'G'){
		editGui->toggleVisible();
        recordGui->toggleVisible();
	}
	
    //Increase showstate
	if (key == 's') { 
		showState += 1;
	}
	
	if(key == 'p'){
		vidGrabber.videoSettings();
	}
	
    //Background sub mode
	if(key =='1'){
		diffMode = 0;
	}else if(key =='2'){
		diffMode = 1;
	}else if(key == '3'){
		diffMode = 2;
	}
    
    
    if(key == OF_KEY_RIGHT){
		highBlob += 1000;
	}else if(key == OF_KEY_LEFT){
		highBlob -= 1000;
	}
	
	if(key == '='){
		lowBlob += 1000;
	}else if(key == '-'){
		lowBlob -= 1000;
	}
	
    
	if(key == OF_KEY_UP){
		threshold += 2;
	}else if(key == OF_KEY_DOWN){
		threshold -= 2;
		if(threshold < 0)threshold = 0;
	}
     
	if(key == 'r'){
		 
     record = 1;
          
	}
    
    if(key == 'm'){
        mode += 1;
        if(mode > 2){
            mode = 0;
        }
        if(mode == 1){
            editGui->toggleVisible();
        }
        if(mode == 2){
            editGui->toggleVisible();
            recordGui->toggleVisible();
        }
        if(mode == 0){
            recordGui->toggleVisible();
        }
        
    }
	

	
	
	
	 
}


//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

		
	if(button == 2){
	
		//This mapping allows for offscreen drawing, which enables removing of the boids
		int mx = (int)ofMap(x,0, ofGetWidth(),-100,  ofGetWidth()+100);
		int my = (int)ofMap(y,0, ofGetHeight(),   0, ofGetHeight());
		
		pth.addPoint(mx, my);
		
	if (XML.pushTag("STROKE", lastTagNumber)) {
		
		int tagNum = XML.addTag("PT");
		XML.setValue("PT:X", mx, tagNum);
		XML.setValue("PT:Y", my, tagNum);
		XML.popTag();
		
		}
	}
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
	if(button == 1){
		flock.addBoid(x,y);
	}
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
						
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}

void testApp::exit(){
	
	
	XML.saveFile("paths.xml");
	recordGui->saveSettings("GUI/recordSettings.xml");
    editGui->saveSettings("GUI/editSettings.xml");
    
    delete editGui;
    delete recordGui;
    
    mClient.unbind();
	

}

