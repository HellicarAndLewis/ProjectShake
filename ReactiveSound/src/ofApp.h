#pragma once

#include "ofMain.h"
#include "Pulse.h"
#include "ofxXmlSettings.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void loadSounds();
	void update();
	void draw();
	void keyPressed(int key);
	
	vector<Pulse> sounds;
};
