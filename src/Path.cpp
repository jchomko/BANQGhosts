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
    lastNearestIndex = 0;
    
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
    lastNearestIndex = 0;
    lastLastNearestIndex = 0;
}

//ofPoint Path::getNextPoint(){
//    
//    pointIndex ++;
//    
//    if (pointIndex > polylines[0].getVertices().size()) {
//        
//        pointIndex = 0;
//        
//    }
//    
//    return polylines[0].getVertices()[pointIndex];
//    
//}



ofPoint Path::getNearestPoint(ofPoint target){
    
    vector<ofPoint> verts = polylines[0].getVertices();
    float minDist = 100000;
    ofPoint nearest;
    
    
    for (int i = lastNearestIndex; i < verts.size(); i ++) {
        
        float dist = target.distance(verts[i]);
        
        if(dist < minDist ){
            
            lastNearestIndex = i;
            nearest = verts[i];
            minDist = dist;
        
        }
        
    }
    
   
       
    return nearest;
    
}




