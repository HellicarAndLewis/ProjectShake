#include "ofApp.h"

void ofApp::setup() {
	ofSetDataPathRoot("../../../../../SharedData/");
	ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);
	loadSounds();
}

void ofApp::loadSounds() {
	sounds.clear();
	ofxXmlSettings xml;
	xml.loadFile("sounds.xml");
	int n = xml.getNumTags("sound");
	for(int i = 0; i < n; i++) {
		Pulse cur;
		xml.pushTag("sound", i);
		cur.load(xml);
		xml.popTag();
		sounds.push_back(cur);
	}
}

void ofApp::update() {
	float endVolume = ofMap(mouseX, 0, ofGetWidth(), -1, 1, true);
	float pulseSpeed = ofMap(mouseY, 0, ofGetWidth(), .5, 1.5, true);
	for(int i = 0; i < sounds.size(); i++) {
		sounds[i].update();
		
		float curVolume = ofMap(i, 0, sounds.size() - 1, 1, endVolume);
		sounds[i].setVolume(MAX(curVolume, 0));
		sounds[i].setPulseSpeed(pulseSpeed);
	}
}

void ofApp::draw() {
	ofBackground(ofMap(mouseX, 0, ofGetWidth(), 0, 255, true));
	for(int i = 0; i < sounds.size(); i++) {
		sounds[i].draw();
		ofTranslate(0, 20);
	}
}

void ofApp::keyPressed(int key) {
	if(key == 'l') {
		loadSounds();
	}
}