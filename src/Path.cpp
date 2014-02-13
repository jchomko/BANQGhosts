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
//    ofPolyline temp;
//    
//    for(int i = 0; i < verts.size();  i++ ){
//        
//        temp.addVertex(verts[i].x, verts[i].y);
//        
//        videoScales.push_back(verts[i].z);
//        
//    }
    
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
    videoScales.clear();
    
    
}

void Path::reset(){


    lastNearestIndex = 10;
    nearestIndex = 0;
    
    
}



ofVec3f Path::getNearestPoint(ofVec2f target, int targetPath){
    
   vector<ofPoint> verts = polylines[targetPath].getVertices();
    
   // polylines[targetPath].getClosestPoint(<#const ofPoint &target#>)
   
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




