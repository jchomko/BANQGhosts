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
	
    ofPushStyle();
    ofSetColor( 0, 0, 200, 170);
    
//    for (int i = 0; i >polylines.size(); i ++) {
//        
//        polylines[i].draw();
//        
//        vector<ofPoint> pth = polylines[i].getVertices();
//        
//        for (int p = 0; p < pth.size(); p++ ) {
//            
//            float z = pth[p].z;
//            
//            ofEllipse(pth[p].x, pth[p].y , z, z);
//            
//        }
//        
//    
//        ofPushStyle();
//        ofFill();
//        ofSetColor(255, 0, 0);
//        ofEllipse(endPoints[i], 40, 40);
//        ofPopStyle();
//        
//        ofPushStyle();
//    ofFill();
//        ofSetColor(0, 255, 0);
//        ofEllipse(startPoints[i], 40, 40);
//        ofPopStyle();
    
        
//    }
    
    ofPopStyle();
    
	
	
}

void Path::clear(){
    
    polylines.clear();
    startPoints.clear();
    endPoints.clear();
    lastNearestIndex = 0;
    lastLastNearestIndex = 0;
}



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




