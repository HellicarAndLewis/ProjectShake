#include "ofApp.h"

using namespace cv;
using namespace ofxCv;

float ofApp::_(string name) {
	return panel.getValueF(name);
}

void ofApp::_(string name, float x) {
	panel.setValueF(name, x);
}

void ofApp::setup() {
	ofSetDataPathRoot("../../../../../SharedData/");
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	
	loadDmxSettings();
	setupControlPanel();
	
	dmx.connect(port, modules * channelsPerModule);
	dmx.update(true); // black on startup
	
	previous.allocate(kinect.getWidth(), kinect.getHeight(), OF_IMAGE_GRAYSCALE);
	diff.allocate(kinect.getWidth(), kinect.getHeight(), OF_IMAGE_GRAYSCALE);
	kinect.setup();
	
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

void ofApp::loadDmxSettings() {
	ofxXmlSettings xml("dmx.xml");
	port = xml.getValue("port", "");
	modules = xml.getValue("modules", 10);
	channelsPerModule = xml.getValue("channelsPerModule", 4);
}

void ofApp::setupControlPanel() {
	panel.setup(256, 768);
	panel.setPosition(0, 0);
	panel.addPanel("sound and movement");
	panel.addToggle("useAdapt", true);
	panel.addSlider("adaptRate", .01, .0001, .05);
	panel.addSlider("energyGrow", .1, 0, 1);
	panel.addSlider("energyDecay", -.1, -.5, 0);
	panel.addSlider("energyMax", 20, 0, 48);
	panel.addSlider("overallEnergy", 0, 0, 1);
	panel.addSlider("minimumPresence", -.8, -2, 0);
	panel.addSlider("pulseRange", .6, 0, 1);
	
	panel.addPanel("virtual camera");
	float maxPosition = 4000;
	panel.addSlider("x", 0, -maxPosition, maxPosition);
	panel.addSlider("y", 0, -maxPosition, maxPosition);
	panel.addSlider("z", 0, -maxPosition, maxPosition);
	panel.addSlider("xcr", 0, -180, 180);
	panel.addSlider("ycr", 0, -180, 180);
	panel.addSlider("zcr", 0, -180, 180);
	panel.addSlider("xsr", 0, -180, 180);
	panel.addSlider("ysr", 0, -180, 180);
	panel.addSlider("zsr", 0, -180, 180);
	panel.addSlider("near", 0, 0, maxPosition);
	panel.addSlider("far", maxPosition, 0, maxPosition);
	panel.addSlider("maxLen", 100, 0, maxPosition);
	panel.addSlider("stepSize", 2, 1, 16, true);
	panel.addSlider("orthoScale", 10, 0, 32);
	panel.addToggle("horizontalFlip", false);
	
	panel.addPanel("lighting modules");
	panel.addToggle("saveCurves", false);
	panel.addToggle("loadCurves", false);
	panel.addSlider("saturation", 255, 0, 255);
	panel.addSlider("brightness", 255, 0, 255);
	for(int module = 1; module <= modules; module++) {
		string label = "mod" + ofToString(module);
		panel.addSlider(label, 0, 0, 1);
	}	
	panel.loadSettings("settings.xml");
	
	redCurve.setup(256);
	greenCurve.setup(256);
	blueCurve.setup(256);
	panel.setValueB("loadCurves", true);
}

void ofApp::exit() {
	dmx.clear();
	dmx.update(true); // black on shutdown
}

ofColor ofApp::rastafari(float x) {
	return ofColor::fromHsb(x * 255. / 3., _("saturation"), _("brightness"));
}

void ofApp::update() {
	updateVirtualKinect();
	updateDmx();
	updateSounds();
}

void ofApp::updateVirtualKinect() {
	ofVec3f position(_("x"), _("y"), _("z"));
	ofVec3f cameraRotation(_("xcr"), _("ycr"), _("zcr"));
	ofVec3f sceneRotation(_("xsr"), _("ysr"), _("zsr"));
	kinect.setPosition(position);
	kinect.setCameraRotation(cameraRotation);
	kinect.setSceneRotation(sceneRotation);
	kinect.setClipping(_("near"), _("far"));
	kinect.setStepSize(_("stepSize"));
	kinect.setMaxLen(_("maxLen"));
	kinect.setOrthoScale(_("orthoScale"));
	kinect.setHorizontalFlip(_("horizontalFlip"));
	
	kinect.update();
	if(kinect.isFrameNew()) {
		absdiff(kinect, previous, diff);
		// only use pixels that have data both frames
		//Mat kinectMat = toCv(kinect), previousMat = toCv(previous), diffMat = toCv(diff);
		//diffMat &= (kinectMat > 0) & (previousMat > 0);
		diff.update();
		copy(kinect, previous);
		rawMean = (Mat_<float>) meanCols(toCv(diff));
		if(runningMean.rows != rawMean.rows) {
			runningMean = Mat_<float>::zeros(rawMean.rows, 1);
			energy = Mat_<float>::zeros(rawMean.rows, 1);
		}
		accumulateWeighted(rawMean, runningMean, _("adaptRate"));
		subtract(rawMean, runningMean, adaptedMean);
		Mat& cur = _("useAdapt") ? adaptedMean : rawMean;
		max(cur, 0, cur);
		addWeighted(cur, _("energyGrow"), energy, 1, _("energyDecay"), energy);
		min(max(energy, 0), _("energyMax"), energy);
		resize(energy, moduleEnergy, cv::Size(1, modules), 0, 0, INTER_AREA);
		moduleEnergy /= _("energyMax");
		
		for(int i = 0; i < moduleEnergy.rows; i++) {
			int module = i + 1;
			_("mod" + ofToString(module), moduleEnergy.at<float>(i));
		}
	}
}

void ofApp::updateDmx() {
	if(panel.getValueB("saveCurves")) {
		redCurve.save("redCurve.yml");
		greenCurve.save("greenCurve.yml");
		blueCurve.save("blueCurve.yml");
		panel.setValueB("saveCurves", false);
	}
	if(panel.getValueB("loadCurves")) {
		redCurve.load("redCurve.yml");
		greenCurve.load("greenCurve.yml");
		blueCurve.load("blueCurve.yml");
		panel.setValueB("loadCurves", false);
	}
	
	int channel = 1;
	for(int module = 1; module <= modules; module++) {
		ofColor cur = rastafari(_("mod" + ofToString(module)));
		dmx.setLevel(channel++, redCurve[cur.r]);
		dmx.setLevel(channel++, greenCurve[cur.g]);
		dmx.setLevel(channel++, blueCurve[cur.b]);
		channel++;
	}
	cout << endl;
	if(dmx.isConnected()) {
		dmx.update();
	} else {
		panel.msg = "Could not connect to port " + port;
	}
}

void ofApp::updateSounds() {
	float endVolume = ofMap(_("overallEnergy"), 0, 1, _("minimumPresence"), 1, true);
	float pulseSpeed = ofMap(_("overallEnergy"), 0, 1, 1 + _("pulseRange"), 1 - _("pulseRange"), true);
	for(int i = 0; i < sounds.size(); i++) {
		float curVolume = ofMap(i, 0, sounds.size() - 1, 1, endVolume);
		sounds[i].setVolume(MAX(curVolume, 0));
		sounds[i].setPulseSpeed(pulseSpeed);
		sounds[i].update();
	}
}

void ofApp::drawVirtualKinect() {
	ofPushMatrix();
	ofSetColor(255);
	ofTranslate(512, 0);
	kinect.draw(0, 0);
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofSetColor(yellowPrint);
	diff.draw(0, 0);
	ofEnableAlphaBlending();
	ofDisableBlendMode();
	
	ofPushStyle();
	ofSetLineWidth(3);
	ofNoFill();
	
	ofPushMatrix();
	ofTranslate(0, kinect.getHeight());
	ofScale(1, -10);
	
	ofSetColor(ofColor::white);
	ofBeginShape();
	for(int i = 0; i < energy.rows; i++) {
		ofVertex(i, energy.at<float>(i));
	}
	ofEndShape();
	
	ofSetColor(128);
	ofLine(0, _("energyMax"), kinect.getWidth(), _("energyMax"));
	
	ofSetColor(yellowPrint);
	ofBeginShape();
	for(int i = 0; i < rawMean.rows; i++) {
		ofVertex(i, rawMean.at<float>(i));
	}
	ofEndShape();
	
	if(_("useAdapt")) {
		ofSetColor(cyanPrint);
		ofBeginShape();
		for(int i = 0; i < runningMean.rows; i++) {
			ofVertex(i, runningMean.at<float>(i));
		}
		ofEndShape();
		
		ofSetColor(magentaPrint);
		ofBeginShape();
		for(int i = 0; i < adaptedMean.rows; i++) {
			ofVertex(i, adaptedMean.at<float>(i));
		}
		ofEndShape();
	}
	
	ofPopMatrix();
	
	drawHighlightString("energy", 4, 4);
	drawHighlightString("raw", 4, 24, ofColor::black, yellowPrint);
	if(_("useAdapt")) {
		drawHighlightString("running", 4, 44, ofColor::black, cyanPrint);
		drawHighlightString("adapt", 4, 64, ofColor::black, magentaPrint);
	}
	
	ofPopStyle();
	ofPopMatrix();
}

void ofApp::drawDmx() {
	ofPushMatrix();
	ofTranslate(256, 0);
	redCurve.draw(0, 0);
	greenCurve.draw(0, 256);
	blueCurve.draw(0, 512);
	
	ofTranslate(256, 480);
	int channel = 1;
	for(int module = 1; module <= modules; module++) {
		string label = "mod" + ofToString(module);
		int rc = channel++;
		int gc = channel++;
		int bc = channel++;
		int ac = channel++;
		int r = dmx.getLevel(rc), g = dmx.getLevel(gc), b = dmx.getLevel(bc);
		ofSetColor(r, g, b);
		ofFill();
		ofRect(4, module * 16, 14, 14);
		ofSetColor(255);
		ofNoFill();
		ofRect(4, module * 16, 14, 14);
		string rs = ofToString(rc) + ":" + ofToString(r);
		string gs = ofToString(gc) + ":" + ofToString(g);
		string bs = ofToString(bc) + ":" + ofToString(b);
		string text = label + " (" + rs + ", " + gs + ", " + bs + ")";
		drawHighlightString(text, 24, module * 16 - 2);
	}
	
	ofPopMatrix();
}

void ofApp::drawSounds() {
	ofPushMatrix();
	ofTranslate(800, 480 + 16);
	for(int i = 0; i < sounds.size(); i++) {
		sounds[i].draw();
		ofTranslate(0, 20);
	}
	ofPopMatrix();
}

void ofApp::draw() {
	if(panel.hidden) {
		ofPushMatrix();
		float spacing = ofGetWidth() / modules;
		int channel = 1;
		for(int module = 1; module <= modules; module++) {
			string label = "mod" + ofToString(module);
			int rc = channel++;
			int gc = channel++;
			int bc = channel++;
			int ac = channel++;
			int r = dmx.getLevel(rc), g = dmx.getLevel(gc), b = dmx.getLevel(bc);
			ofSetColor(r, g, b);
			ofFill();
			ofRect(0, 0, spacing, ofGetHeight());
			ofTranslate(spacing, 0);
		}
		ofPopMatrix();
	} else {
		ofBackground(127);
		drawVirtualKinect();
		drawDmx();
		drawSounds();
	}
}

