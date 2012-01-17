#pragma once

#include "ofMain.h"

class Pulse : public ofSoundPlayer {
public:
	Pulse()
	:pulse(0)
	,name("")
	,lastTrigger(0) {
	}
	void setPulse(unsigned long pulse) {
		this->pulse = pulse;
	}
	void setName(string name) {
		this->name = name;
	}
	void update() {
		if(pulse > 0) {
			unsigned long curTrigger = ofGetSystemTime();
			if(curTrigger - lastTrigger > pulse) {
				if(getIsPlaying()) { // loops
					setPosition(ofRandom(0, 1));
				} else { // sample
					play();
				}
				lastTrigger = curTrigger;
			}
		}
	}
	void draw() {
		float width = 256;
		float height = 18;
		float playPosition = width * getPosition();
		float pulsePosition = width * (1 - getPulsePosition());
		ofFill();
		ofSetColor(0);
		ofRect(0, 0, width, height);
		ofNoFill();
		ofSetColor(128);
		ofRect(0, 0, width, height);
		ofSetColor(255);
		ofFill();
		ofRect(playPosition, 0, 3, height);
		ofSetColor(ofColor::red);
		ofRect(pulsePosition, 0, 3, height);
		ofSetColor(255);
		ofDrawBitmapString(name, 4, 12);
	}
	float getPulsePosition() {
		unsigned long curTrigger = ofGetSystemTime();
		return (float) (curTrigger - lastTrigger) / pulse;
	}
	
private:
	unsigned long pulse, lastTrigger;
	string name;
};

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
	vector<Pulse> loops, samples, textures;
	vector<Pulse*> all;
};
