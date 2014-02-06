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
	Boid();
	Boid(int x, int y, int _nf);
	
	void update(vector<Boid> &boids);
	void draw();
    void drawDebug();
    
	
    void seek(ofVec2f target);
    void avoid(ofVec2f target);
    void arrive(ofVec2f target);
	void updateValues(float separation, float maxforce, float maxspeed, float seekforce);
	void push(float ms);
	void flock(vector<Boid> &boids);
	void follow(Path path, int i);
    
	// setLoc Bug
	void setLoc(ofVec2f p);
	
	
	ofVec2f getPredictLoc();
	float getScale();
	
	ofVec3f getLoc();
	ofVec2f steer(ofVec2f target, bool slowdown);	
	ofVec2f separate(vector<Boid> &boids);
	
	ofVec2f align(vector<Boid> &boids);
	ofVec2f cohesion(vector<Boid> &boids);
	
	ofVec2f loc,vel,acc;
	
	ofVec2f predict;
	ofVec2f origin;
	ofVec2f pushLoc;
	
	float psh;
	vector <ofVec2f> avgVel;
	ofVec3f addNoise();

	
	float p;
	int hold;
	//float scale;
	float xNoise;
	float yNoise;
	float zNoise;
	float xNoiseInc;
	float yNoiseInc;
	float zNoiseInc;
	ofVec3f noise;
	ofVec2f avg;
    
	float r;
	float maxforce;
	float maxSteerForce;
	float maxspeed;
	float s; //Separation
	float a; //Cohesion
	float c; //Alignement
	float neighbordist;
	float desiredseparation;
		
	float angle;
    
    float videoScale;
    
    float rot;
    
    //Sequence Variables
    int num_frames;
    void drawVideo(int index);
	//ofPixels * pixels;
    ofTexture * pixels;
    ofTexture dispTex;
    
};

#endif