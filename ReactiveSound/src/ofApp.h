#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

class Pulse : public ofSoundPlayer {
public:
	Pulse()
	:pulse(0)
	,name("")
	,lastTrigger(0)
	,seed((int) ofRandom(127))
	,tremolo(0)
	,pulseSpeed(1) {
	}
	void load(ofxXmlSettings xml) {
		name = xml.getValue("name", "");
		loadSound(name, false);
		if(xml.getValue("loop", 0)) {
			setLoop(true);
			setMultiPlay(false);
		} else {
			setLoop(false);
			setMultiPlay(true);
		}
		pulse = xml.getValue("pulse", 0);
		tremolo = xml.getValue("tremolo", 0.0);
		setPan(0);
		play();
		ofLogVerbose() << "Loaded sound " << name;
	}
	void setPulseSpeed(float pulseSpeed) {
		this->pulseSpeed = pulseSpeed;
	}
	void update() {
		if(pulse > 0) {
			unsigned long curTrigger = ofGetSystemTime();
			unsigned long curPulse = pulseSpeed * pulse;
			if(curTrigger - lastTrigger > curPulse) {
				if(getIsPlaying()) { // loops
					setPosition(ofRandom(0, 1));
				} else { // sample
					play();
				}
				lastTrigger = curTrigger;
			}
		}
		setPan(ofSignedNoise(ofGetElapsedTimef() + seed));
		if(tremolo > 0) {
			setSpeed(1. + tremolo * ofSignedNoise(ofGetElapsedTimef() + seed));
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
		ofRect(playPosition, 0, 3, 3);
		ofSetColor(ofColor::red);
		ofRect(pulsePosition, 0, 3, 3);
		ofSetColor(255);
		ofDrawBitmapString(name, 4, 12);
	}
	float getPulsePosition() {
		unsigned long curTrigger = ofGetSystemTime();
		unsigned long curPulse = pulseSpeed * pulse;
		return (float) (curTrigger - lastTrigger) / curPulse;
	}
	
private:
	unsigned long pulse, lastTrigger;
	string name;
	int seed;
	float tremolo;
	float pulseSpeed;
};

class ofApp : public ofBaseApp {
public:
	void setup();
	void loadSounds();
	void update();
	void draw();
	void keyPressed(int key);
	
	vector<Pulse> sounds;
};
