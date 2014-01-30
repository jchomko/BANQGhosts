/*
 *  Path.cpp
 *  Carte Blanche Joliette
 *
 *  Created by Jonathan Chomko on 12-11-01.
 *
 */

#include "Path.h"

Path::Path()
{
    rot = 0;
    
}


void Path::addPoint(float x, float y){

	ofPoint p;
    
	p.set(x,y);
	
	points.addVertex(p);
	
	
}

void Path::addPath(ofPolyline p){
    
    vector <ofPoint> verts =  p.getVertices();
    
    startPoints.push_back(verts[0]);
    endPoints.push_back(verts[verts.size()-1]);
    
    polylines.push_back(p);
    

}

void Path::display(){
	
	ofPushMatrix();
	
	ofSetColor(255, 0, 0);
	
	points.draw();
	
	ofPopMatrix();
}

void Path::clear(){
    
    polylines.clear();
    startPoints.clear();
    endPoints.clear();

}




