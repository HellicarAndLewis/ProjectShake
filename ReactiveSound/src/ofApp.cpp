#include "ofApp.h"

void ofApp::setup() {
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
	for(int i = 0; i < sounds.size(); i++) {
		sounds[i].update();
	}
}

void ofApp::draw() {
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