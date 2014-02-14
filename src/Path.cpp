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
    
    if(p.size() > 0){
    
        isAllocated = true;
        
    }
    
    pathColors.push_back(ofColor(ofRandom(100,255),ofRandom(100,255),ofRandom(100,255)));
    

}


void Path::clear(){
    
    polylines.clear();
    startPoints.clear();
    endPoints.clear();
    videoScales.clear();
    pathColors.clear();
    
    
}

void Path::reset(){


    lastNearestIndex = 10;
    nearestIndex = 0;
    
    
}



ofVec3f Path::getNearestPoint(ofVec2f target, int targetPath){
    
   vector<ofPoint> verts = polylines[targetPath].getVertices();
    
   
    ofVec3f closestPoint;
    float minDist = 1000000;
    
    for (int i = 0; i < verts.size(); i ++) {
        
        float dist = target.distance(ofVec2f(verts[i].x, verts[i].y));
        // float distX = (target.x - verts[i].x);
    
        // float distY = (predict.y - verts[i].y);
        //float dist = abs(distX ) + abs(distY);
        
        if(dist < minDist){
            
            nearestIndex = i;
            closestPoint = verts[i];
            
            minDist = dist;
            
            
        }
        
        
    }
    
    videoScales[nearestIndex];
    
    return ofVec3f(closestPoint);
    
    

}




