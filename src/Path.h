/*
 *  Path.h
 *  CarteBlancheJoliette
 *
 *  Created by Jonathan Chomko on 12-11-01.
 *  
 *
 */

#ifndef _PATH
#define _PATH

#include "ofMain.h"
 
class Path{
public:
	Path();
	void addPath(ofPolyline p);
    void clear();
    void reset();
    
    ofPoint getNextPoint();
    ofPoint getNearestPoint(ofPoint _t, int _targetPath);
    int lastNearestIndex;
    int nearestIndex;
    
	ofPolyline points;
    vector <ofPolyline> polylines;
    vector <float> videoScales;
    vector <ofPoint> startPoints;
    vector <ofPoint> endPoints;
    
    ofImage background;
    
    int stuckInt;
    
	float radius;
	float rot;
    int pointIndex;
    void addPoint(float x, float y);
	void display();
	
private:
};

#endif