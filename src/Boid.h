/*
 *  Boid.h
 *  boid
 *
 *  Created by Jeffrey Crouse on 3/29/10.
 *  Copyright 2010 Eyebeam. All rights reserved.
 *
 */

#ifndef BOID_H
#define BOID_H

#include "ofMain.h"
#include "Path.h"

class Boid {
public:
	//Boid();
	
    Boid(int x, int y, int _nf);
	
	void update(vector<Boid> &boids, Path * path);
    void seekPath(Path * path);
    void draw();
    void seek(ofVec2f target);
    void avoid(ofVec2f target);
    void arrive(ofVec2f target);
	void updateValues(float separation, float maxforce, float maxspeed);
	
    void flock(vector<Boid> &boids);
	void follow(Path * path, int i);
    void resetLoc(Path * path, int i);
    
    void averages();
    void addNoise();
    void drawVideo(int index);
    
	ofVec2f steer(ofVec2f target, bool slowdown);	
	ofVec2f separate(vector<Boid> &boids);
	
  
	ofVec3f loc,vel,acc;
	ofVec3f predict;
	ofVec3f closestPoint;
   
    vector <ofVec3f> locAvgs;
    vector<float> avgPerspective;
    vector<ofVec3f> velAvgs;
    ofVec3f velAvg;
    ofVec3f noise;
	ofVec3f locAvg;
    
    float perspectiveAvg;
  	
    float xNoise;
	float yNoise;
	float zNoise;
	float xNoiseInc;
	float yNoiseInc;
	float zNoiseInc;
   
    
    float maxforce;
	float maxspeed;
	float separationMult;
	
    float videoScale;
    
    int num_frames;
   
    
    ofTexture * pixels;
    ofTexture dispTex;
    
    float maxSpeedMult;
    int pathFollowIndex;
    
    
};

#endif