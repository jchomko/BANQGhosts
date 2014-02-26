/*
 *  ofxFlocking.cpp
 *  flock
 *
 *  Created by Jeffrey Crouse on 3/30/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *  Modified by Rick Companje
 */

#include "ofxFlocking.h"

void ofxFlocking::update() {
	
	for(int i=0; i<boids.size(); i++) {
		//boids[i].update(boids);
		
	}
}

void ofxFlocking::draw() {
	for(int i=0; i<boids.size(); i++) {
		boids[i].draw();
	}
}



//void ofxFlocking::addBoid() {
//	boids.push_back(Boid());
//}

void ofxFlocking::addBoid(int x, int y, int nf) {
	boids.push_back(Boid(x,y,nf));
}

//Include number of boids? 
void ofxFlocking::resetLoc(Path * path){
    
  for(int i=0; i<boids.size(); i++) {
		
        boids[i].resetLoc(path, i);
		
	}
    
    
}

void ofxFlocking::follow(Path * path){
    
    for(int i=0; i<boids.size(); i++) {
		
        boids[i].follow(path, i);
		
	}
    
    
}