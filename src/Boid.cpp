/*
 *  Boid.cpp
 *  boid
 *
 *  Created by Jeffrey Crouse on 3/29/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *  Updated by Rick Companje
 *
 */

#include "Boid.h"


Boid::Boid(int x, int y, int nf) {
    loc.set(x,y);
	
    num_frames = nf;
    
    vel.set(0.1,0);
	
    acc.set(0,0);
	
    maxspeed = 0;
	
    maxforce = 0;
	
    xNoise = 0;
	yNoise = 0;
	zNoise = 0;
	xNoiseInc = ofRandom(0.001, 0.007);
	yNoiseInc = ofRandom(0.01, 0.07);
	zNoiseInc = ofRandom(0.001, 0.007);
    
    
    pixels = new ofTexture[num_frames];
    
    for(int i = 0; i < num_frames;  i ++){
        pixels[i].allocate(640,480,GL_RGBA);
    }
    //Sequence variables
    
     maxSpeedMult = ofRandom(0.1, 0.7);
    
	
}

void Boid::updateValues(float _separation,float _maxforce, float _maxspeed) {
	
    separationMult = _separation ; //Separat ion scale +
	maxforce = _maxforce; //maxSteerForce
	maxspeed = _maxspeed * maxSpeedMult;
	
}



// Method to update location
void Boid::update(vector<Boid> &boids, Path  * path) {

    
    //Adjust for perspective
    maxspeed = ofMap(velAvg.z, 10, 300, 1, maxspeed);
    
    seekPath(path);
    
    flock(boids);
    
    addNoise();
    
    vel += acc;   // Update velocity
    vel.x =  ofClamp(vel.x, -maxspeed, maxspeed);  // Limit speed -maxspeed
    vel.y = ofClamp(vel.y, -maxspeed, maxspeed);  // Limit speed
    loc += vel;
    acc = 0;  // Reset accelertion to 0 each cycle
    
    averages();

}



void Boid::averages(){
    
    //Perspective Y Rotate Average
//    float diff =  lastZ - videoScale;
//    avgPerspective.push_back(2*diff);
//    
//    if( avgPerspective.size() > 40){
//        avgPerspective.erase(avgPerspective.begin());
//    }
//    perspectiveAvg = 0;
//    
//    for(int i = 0; i < avgPerspective.size(); i ++){
//        perspectiveAvg += avgPerspective[i];
//	}
//    
//    perspectiveAvg /= avgPerspective.size();
//    lastZ = videoScale;
    
    //Position Average
    ofVec3f a;
    a.set(loc.x, loc.y, videoScale);
    
    locAvgs.push_back(a); // vel
	
	if(locAvgs.size()> 20){
    	locAvgs.erase(locAvgs.begin());
    }
	locAvg.set(0, 0,0);
    for(int i = 0; i < locAvgs.size(); i ++){
    	locAvg += locAvgs[i];
    }
	locAvg /= locAvgs.size();
    
    
    //Velocity Average
    ofVec3f v;
    v.set(vel.x, vel.y, videoScale);
    
    velAvgs.push_back(v); // vel
	
	if(velAvgs.size()> 30){
    	velAvgs.erase(velAvgs.begin());
    }
	velAvg.set(0, 0,0);
    for(int i = 0; i < velAvgs.size(); i ++){
    	velAvg += velAvgs[i];
    }
	velAvg /= velAvgs.size();
    
    
}

void Boid::drawVideo(int index){
	
    //float angle = (float)atan2(-vel.y, vel.x);
    float angle = (float)atan2(-velAvg.y, velAvg.x);
    float theta =  -1.0*angle;
	float heading2D = ofRadToDeg(theta)+90;
	
    
    //Draw the Sequence
    ofPushMatrix();
    
    ofTranslate(locAvg.x, locAvg.y + noise.y*(videoScale/6));
    
    ofRotate(heading2D - 90); // rot
    // ofRotateY(perspectiveAvg);
    
    // 1.333 is the aspect of the kinect
    pixels[index].setAnchorPoint((velAvg.z*1.3333)/2, velAvg.z/3);
	
    pixels[index].draw(0, 0,velAvg.z *1.3333 , velAvg.z);
    
    ofPopMatrix();
     
}


void Boid::seekPath(Path * path){
    
   
    predict.set(velAvg.x, velAvg.y); //videoScale
	predict.normalize();
    predict *= videoScale; 
    predict += loc;
    
	if( path->polylines.size() > 0){
        
        //rot = path->rot;
        //Tests for no turnback
        
        if(path->isAllocated){
        closestPoint = path->getNearestPoint(predict, pathFollowIndex);
        }
        
        if(loc.distance(ofVec2f(closestPoint.x, closestPoint.y)) > videoScale/4){
            seek(ofVec2f(closestPoint.x, closestPoint.y));
        }
        
        videoScale = closestPoint.z;
        
        //Make sure always goes offscreen
        if(loc.distance(path->endPoints[pathFollowIndex]) <  velAvg.z){ // || loc.x > 5120 + videoScale/2
            
                follow(path, ofRandom(2));
         }

    }
    
}


void Boid::seek(ofVec2f target) {
    acc += steer(target, false); //neighbordist*
}

void Boid::avoid(ofVec2f target) {
    acc -= steer(target, false);
}

void Boid::arrive(ofVec2f target) {
    acc += steer(target, true);
}


// A method that calculates a steering vector towards a target
// Takes a second argument, if true, it slows down as it approaches the target
ofVec2f Boid::steer(ofVec2f target, bool slowdown) {
   
    ofVec2f steer;  // The steering vector
    ofVec2f desired = target - loc;  // A vector pointing from the location to the target
    
	float d = ofDist(target.x, target.y, loc.x, loc.y); // Distance from the target is the magnitude of the vector
	
	
	// If the distance is greater than 0, calc steering (otherwise return zero vector)
    if (d > 0) {
		
		desired /= d; // Normalize desired
		// Two options for desired vector magnitude (1 -- based on distance, 2 -- maxspeed)
		if ((slowdown) && (d < 100.0f)) {
			desired *= maxspeed * (d/100.0f); // This damping is somewhat arbitrary
		} else {
			desired *= maxspeed;
		}
		// Steering = Desired minus Velocity
		steer = desired - vel;
		
        //Used to have a max steer force 
		//steer.x = ofClamp(steer.x, -maxSteerForce, maxSteerForce); // Limit to maximum steering force
		//steer.y = ofClamp(steer.y, -maxSteerForce, maxSteerForce);
        steer.x = ofClamp(steer.x, -maxforce, maxforce);
		steer.y = ofClamp(steer.y, -maxforce, maxforce);

		
	}
    
    return steer;
}



void Boid::addNoise(){
	
	xNoise += xNoiseInc;
	yNoise += yNoiseInc;
	//zNoise += zNoiseInc;
	
	noise.x = ofNoise(xNoise);
	noise.y = sin(yNoise);  // up an down
	
}




void Boid::draw() {
    // Draw a triangle rotated in the direction of velocity
    
    float angle = (float)atan2(-vel.y, vel.x);
    float theta =  -1.0*angle;
	float heading2D = ofRadToDeg(theta)+90;
	
    float r = videoScale;
   
    ofPushStyle();
    ofFill();
    ofLine(predict.x, predict.y, closestPoint.x, closestPoint.y);
    ofPushMatrix();
    ofTranslate(loc.x, loc.y);
    ofRotateZ(heading2D);
    
    ofBeginShape();
    ofVertex(0, -r*2);
    ofVertex(-r, r*2);
    ofVertex(r, r*2);
    ofEndShape(true);	
    ofPopMatrix();
	ofPopStyle();

}



void Boid::flock(vector<Boid> &boids) {
    
	ofVec2f sep = separate(boids);
	sep *= separationMult;
    acc += sep;
    
}



// Separation
// Method checks for nearby boids and steers away
ofVec2f Boid::separate(vector<Boid> &boids) {
    
    float desiredseparation = videoScale;
   
    ofVec2f steer;
    int count = 0;
	
    // For every boid in the system, check if it's too close
    for (int i = 0 ; i < boids.size(); i++) {
		Boid &other = boids[i];
		
		float d = ofDist(loc.x, loc.y, other.loc.x, other.loc.y);
		
		// If the distance is greater than 0 and less than an arbitrary amount (0 when you are yourself)
		if ((d > 0) && (d < desiredseparation)) {
			// Calculate vector pointing away from neighbor
			ofVec2f diff = loc - other.loc;
			diff.normalize();			// normalize
			diff /= d;        //Weight by distance
			steer += diff;
			count++;            // Keep track of how many
		}
    }
   
    // Average -- divide by how many
    if (count > 0) {
		steer /= (float)count;
    }
	
	float mag = sqrt(steer.x*steer.x + steer.y*steer.y);
    if (mag > 0) {
		// Implement Reynolds: Steering = Desired - Velocity
		steer /= mag;
		steer *= maxspeed;
		steer -= vel;
		steer.x = ofClamp(steer.x, -maxforce, maxforce);
		steer.y = ofClamp(steer.y, -maxforce, maxforce);
    }
    return steer;
    
}



//Reset boids on a path - also takes random path
void Boid::follow(Path * path, int i){
    
    ofVec3f startVel;
    
    velAvgs.clear();
    locAvgs.clear();
    
    videoScale = 10;
    
    pathFollowIndex = ofRandom(path->polylines.size());
    
    if(path->startPoints.size() > 0){
        
        startVel = path->polylines[pathFollowIndex].getVertices()[1] - path->polylines[pathFollowIndex].getVertices()[0];
        
        startVel.normalize();
        
        loc.set(path->startPoints[pathFollowIndex] - startVel * i * ofRandom(80,130));
        
        vel.set(startVel);
        
    }
    
}

