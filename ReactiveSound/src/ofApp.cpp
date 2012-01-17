#include "ofApp.h"

vector<Pulse> loadSounds(string path) {
	ofDirectory dir(path);
	dir.listDir();
	vector<Pulse> sounds;
	for(int i = 0; i < dir.size(); i++) {
		Pulse cur;
		cur.loadSound(dir.getPath(i), false);
		cur.setName(dir.getName(i));
		if(path == "samples") {
			cur.setLoop(false);
			cur.setMultiPlay(true);
			cur.setPulse(ofRandom(2000, 8000));
		} else if(path == "loops") {
			cur.setLoop(true);
			cur.setMultiPlay(false);
			cur.play();
		} else if(path == "textures") {
			cur.setLoop(true);
			cur.setMultiPlay(false);
			cur.setPulse(ofRandom(4000, 8000));
			cur.play();
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
	textures = loadSounds("textures");
	for(int i = 0; i < samples.size(); i++) {
		all.push_back(&samples[i]);
	}
	for(int i = 0; i < loops.size(); i++) {
		all.push_back(&loops[i]);
	}
	for(int i = 0; i < textures.size(); i++) {
		all.push_back(&textures[i]);
	}
}

void ofApp::update() {
	for(int i = 0; i < all.size(); i++) {
		all[i]->update();
	}
}

void ofApp::draw() {
	for(int i = 0; i < all.size(); i++) {
		all[i]->draw();
		ofTranslate(0, 20);
	}
}

void ofApp::keyPressed(int key) {
}