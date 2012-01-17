#include "ofApp.h"

vector<ofSoundPlayer> loadSounds(string path) {
	ofDirectory dir(path);
	dir.listDir();
	vector<ofSoundPlayer> sounds;
	for(int i = 0; i < dir.size(); i++) {
		ofLogVerbose() << "Loading " << dir.getPath(i);
		ofSoundPlayer cur;
		cur.loadSound(dir.getPath(i), false);
		if(path == "samples") {
			cur.setLoop(false);
			cur.setMultiPlay(true);
		}
		if(path == "loops") {
			cur.setLoop(true);
			cur.setMultiPlay(false);
		}
		cur.setPan(0);
		sounds.push_back(cur);
	}
	return sounds;
}

void ofApp::setup() {
	ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);
	samples = loadSounds("samples");
	loops = loadSounds("loops");
}

void ofApp::update() {

}

void ofApp::draw() {

}

void ofApp::keyPressed(int key) {
	if(key - 'a' >= 0 && key - 'a' < loops.size()) {
		int cur = key - 'a';
		loops[cur].play();
		loops[cur].setPosition(ofMap(mouseX, 0, ofGetWidth(), 0, 1));
	} else if(key - '1' >= 0 && key - '1' < samples.size()) {
		int cur = key - '1';
		samples[cur].play();
	}
}