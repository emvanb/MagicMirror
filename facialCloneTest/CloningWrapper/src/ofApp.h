#pragma once

#include "ofMain.h"
#include "Clone.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	
	int w, h;
	ofFbo src, mask;
	ofVideoGrabber dst;
	Clone clone;
};
