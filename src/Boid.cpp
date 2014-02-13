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





Boid::Boid() {
    loc.set(ofRandomWidth(),ofRandomHeight());
	vel.set(0,0);
	acc.set(0,0);
	predict.set(0,0);
	locAvg.set(0,0);
	pushLoc.set(0,0);
	
    r = 3.0;
    maxspeed = 0.61;
	maxforce = 0.93;
	neighbordist = 48.42;
	maxSteerForce = 0;
	desiredseparation = 0;
	s = 0;
	a = 0;
	c = 0;
	angle = 0;
	
	xNoise = 0;
	yNoise = 0;
	zNoise = 0;
	xNoiseInc = 0;
	yNoiseInc = 0;
	zNoiseInc = 0;
    
    //Sequence();
    
	
	
}

Boid::Boid(int x, int y, int nf) {
    loc.set(x,y);
	num_frames = nf;
    
	origin.set(x,y);
	
	vel.set(0.1,0);
	acc.set(0,0);
	
    r = 3.0;
	
	maxspeed = 0.61;
	maxforce = 0.93;
	neighbordist = 48.42;
	
	xNoise = 0;
	yNoise = 0;
	zNoise = 0;
	xNoiseInc = ofRandom(0.001, 0.007);
	yNoiseInc = ofRandom(0.01, 0.07);
	zNoiseInc = ofRandom(0.001, 0.007);
    
    //pixels = new ofPixels[num_frames];
    
    pixels = new ofTexture[num_frames];
    
    for(int i = 0; i < num_frames;  i ++){
        pixels[i].allocate(640,480,GL_RGBA);
    }
    //Sequence variables
    //dispTex.allocate(640,360,GL_RGBA);
    
    lastPathId = 0;
    
    nearestIndex = 0;
    lastNearestIndex = -1;
    
	
}


void Boid::updateValues(float _separation,float _maxforce, float _maxspeed, float _seekforce) {
	
    s = _separation * 0.01; //Separation scale +
	//a = _a; //Cohesion
	//c = _c; //Alignement
    
	maxSteerForce = _maxforce;
	maxspeed = _maxspeed;
	neighbordist = _seekforce * 0.1;
    
    //desiredseparation = _dS;
}


void Boid::setLoc(ofVec2f p){
    
	loc.set(p.x,p.y);
	
}

//
//void Boid::push(float ms){
//	ofVec2f h;
//	
//	psh = ms;
//		//float a = ofMap(angle, -PI, PI, -1,1);
//	
//	predict *= (75*ms);
//	ofVec2f predictLoc = loc + predict;
//
//	ofLine(loc.x, loc.y, predictLoc.x, predictLoc.y);
//		
//	//pushLoc = predict;
//	loc = predictLoc;
//	
//	ofSetColor(244, 0, 0);
//	ofEllipse(loc.x, loc.y, ms*25, ms*25);
//	ofSetColor(255, 255, 255,255);
//	
//}




// Method to update location
void Boid::update(vector<Boid> &boids, Path  * path) {

    
    //Adjust for perspective
    maxspeed = maxspeed + ofMap(velAvg.z, 10, 300, 0.2, 10);
    
    seekPath(path);
    
    flock(boids);
    
    
    // addNoise();
    vel += acc;   // Update velocity
    
    vel.x =  ofClamp(vel.x, -maxspeed, maxspeed);  // Limit speed -maxspeed
    vel.y = ofClamp(vel.y, -maxspeed, maxspeed);  // Limit speed
    
    loc += vel;
    acc = 0;  // Reset accelertion to 0 each cycle
    
    averages();

}



void Boid::averages(){
    
    //Perspective Y Rotate Average
    float diff =  lastZ - videoScale;
    avgPerspective.push_back(2*diff);
    
    if( avgPerspective.size() > 40){
        avgPerspective.erase(avgPerspective.begin());
    }
    perspectiveAvg = 0;
    
    for(int i = 0; i < avgPerspective.size(); i ++){
        perspectiveAvg += avgPerspective[i];
	}
    
    perspectiveAvg /= avgPerspective.size();
    lastZ = videoScale;
    
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
    
    //Set coordinates
    //ofTranslate(loc.x, loc.y + noise.y*50 );
    ofTranslate(locAvg.x, locAvg.y + noise.y*50);
    
    ofRotate(heading2D + 90); // rot
    // ofRotateY(perspectiveAvg);
    
    // 1.333 is the aspect of the kinect
    // should not be hard-coded
    pixels[index].setAnchorPoint((velAvg.z*1.3333)/2, velAvg.z/3);
	pixels[index].draw(0, 0,velAvg.z *1.3333, velAvg.z);
    
    ofPopMatrix();
    
    lastZ = videoScale;
    
    
}


void Boid::seekPath(Path * path){
    
    predict.set(velAvg.x, velAvg.y); //videoScale
	predict.normalize();
    predict *= 75; //videoScale*2 averaged video size
    predict += loc;
    
	if( path->polylines.size() > 0){
        
        
        rot = path->rot;
        
       
//        closestPoint = path->polylines[pathFollowIndex].getClosestPoint(ofVec3f(predict.x, predict.y, predict.z));
//        
//        videoScale = closestPoint.z;
//        
//        
//        if(loc.distance(ofVec2f(closestPoint.x, closestPoint.y)) > videoScale/4){
//            
//            seek(ofVec2f(closestPoint.x, closestPoint.y));
//            
//        }
//
//        
//        if(loc.distance(path->endPoints[pathFollowIndex]) < 140 || loc.x > 5120 ){
//            
//            //path->reset();
//            follow(path, 3);
//        
//        }
//        

        //Tests for no turnback
        
        //closestPoint = path->getNearestPoint(predict, pathFollowIndex);
        vector<ofPoint> verts = path->polylines[pathFollowIndex].getVertices();
        
        
        float minDist = 1000000;
        
        for (int i = nearestIndex; i < verts.size(); i ++) {
        
                    float dist = predict.distance(verts[i]);
            
                    float distX = (predict.x - verts[i].x);
                   // float distY = (predict.y - verts[i].y);
                    //float dist = abs(distX ) + abs(distY);
            
                    if(dist < minDist){
        
                        closestPoint = verts[i];
                        minDist = dist;
        
            
                    }
            
            }
        
        //ofVec3f d =  - locAvg;
//        
//        if( closestPoint.x < locAvg.x){
//            
//            nearestIndex ++;
//            
//            closestPoint = verts[nearestIndex];
//            
//            cout << "diff : " << closestPoint.x - locAvg.x<< " index: " << nearestIndex << endl;
//
//        }
        
        
      //  lastClosestPoint = closestPoint;
        
         videoScale = closestPoint.z;
        
       // if(loc.distance(ofVec2f(closestPoint.x, closestPoint.y)) > videoScale/4){
              seek(ofVec2f(closestPoint.x, closestPoint.y));
        // }
        
        
        if(loc.distance(path->endPoints[pathFollowIndex]) < 140 || loc.x > 5120 ){
            
            //path->reset();
            
            follow(path, 3);
            
        }


        
        
        //        cout << "nearestIndex: " << nearestIndex << " diff: " <<  nearestIndex - lastNearestIndex  << " minDist: " << minDist << endl;
        
        
        //        if( nearestIndex == lastNearestIndex && minDist < 50){
        //
        //            nearestIndex ++;
        //
        //            closestPoint = verts[nearestIndex];
        //
        //        }
        //
        //        lastNearestIndex = nearestIndex;
        //
        
        // closestPoint = path->polylines[pathFollowIndex].getClosestPoint(predict);
        //        float d = predict.distance(verts[nearestIndex]);
        //
        //        if( d < 40){
        //
        //            nearestIndex ++;
        //            closestPoint = verts[nearestIndex];
        //        }
        //
        //seek(closestPoint);
        
        // lastPathId = n;
        
        
    }



}


void Boid::seek(ofVec2f target) {
    acc += neighbordist*steer(target, false);
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
		
		
		//steer.x = ofMap(steer.x, 0, steer.x, -maxforce, +maxforce);
		//steer.y = ofMap(steer.y, 0, steer.y, -maxforce, +maxforce);
		
		steer.x = ofClamp(steer.x, -maxSteerForce, maxSteerForce); // Limit to maximum steering force
		steer.y = ofClamp(steer.y, -maxSteerForce, maxSteerForce); 
		
	}
    
    return steer;
}

void  Boid::addNoise(){
	
	xNoise += xNoiseInc;
	yNoise += yNoiseInc;
	//zNoise += zNoiseInc;
	
	noise.x = ofNoise(xNoise);
	noise.y = sin(yNoise);  // up an down
	
    
	
}

ofVec2f Boid::getPredictLoc(){
    
	ofVec2f p(predict);
    p *= videoScale;
    p += loc;
	return p;

}

float Boid::getScale(){
	
	zNoise += zNoiseInc;
	float z = sin(zNoise);
	return z;
	

}

ofVec3f Boid::getLoc(){
	
	ofVec3f l;
	
	   
	//l += pushLoc;
	
	return l;
	

}
void Boid::draw() {
    // Draw a triangle rotated in the direction of velocity
    
    angle = (float)atan2(-vel.y, vel.x);
    float theta =  -1.0*angle;
	float heading2D = ofRadToDeg(theta)+90;
	
   
    
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
	//ofVec2f ali = align(boids);
	//ofVec2f coh = cohesion(boids);
	
	// Arbitrarily weight these forces
	sep *= s;
 	//ali *= 1.0; //a
	//coh *= 1.0;//c
	
	acc += sep; //+ ali + coh;
}


void Boid::follow(Path * path, int i){

    ofVec3f startVel;
    
    //path->reset();
    nearestIndex = 0;
    lastNearestIndex = -1;
    
    velAvgs.clear();
    locAvgs.clear();
    
    //videoScale = 0;
    
    pathFollowIndex = ofRandom(path->polylines.size());
    

    if(path->startPoints.size() > 0){
         
        startVel = path->polylines[pathFollowIndex].getVertices()[1] - path->polylines[pathFollowIndex].getVertices()[0];
        
        startVel.normalize();
        
        loc.set(path->startPoints[pathFollowIndex] - startVel * i * 100);
        
        vel.set(startVel);
    }

}

// Separation
// Method checks for nearby boids and steers away
ofVec2f Boid::separate(vector<Boid> &boids) {
    
    float desiredseparation = videoScale*1.7;
    
    //float desiredseparation = ;
    
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
			diff /= d;        // Weight by distance
			steer += diff;
			count++;            // Keep track of how many
		}
    }
    // Average -- divide by how many
    if (count > 0) {
		steer /= (float)count;
    }
	
	
    // As long as the vector is greater than 0
	//float mag = sqrt(steer.x*steer.x + steer.y*steer.y);
	
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

// Alignment
// For every nearby boid in the system, calculate the average velocity
ofVec2f Boid::align(vector<Boid> &boids) {
    //float neighbordist = 50.0;
    ofVec2f steer;
    int count = 0;
    for (int i = 0 ; i < boids.size(); i++) {
		Boid &other = boids[i];
		
		float d = ofDist(loc.x, loc.y, other.loc.x, other.loc.y);
		if ((d > 0) && (d < neighbordist)) {
			steer += (other.vel);
			count++;
		}
    }
    if (count > 0) {
		steer /= (float)count;
    }
	
    // As long as the vector is greater than 0
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

// Cohesion
// For the average location (i.e. center) of all nearby boids, calculate steering vector towards that location
ofVec2f Boid::cohesion(vector<Boid> &boids) {
    
   // float neighbordist = 50.0;
    ofVec2f sum;   // Start with empty vector to accumulate all locations
    int count = 0;
    for (int i = 0 ; i < boids.size(); i++) {
		Boid &other = boids[i];
		float d = ofDist(loc.x, loc.y, other.loc.x, other.loc.y);
		if ((d > 0) && (d < neighbordist)) {
			sum += other.loc; // Add location
			count++;
		}
    }
    if (count > 0) {
		sum /= (float)count;
		return steer(sum, false);  // Steer towards the location
    
    return sum;
}
	
}
