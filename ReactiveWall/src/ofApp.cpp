#include "ofApp.h"

using namespace cv;
using namespace ofxCv;

void ofApp::setup() {
	kinect.setup();
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	
	loadDmxSettings();
	setupControlPanel();
	
	dmx.connect(port, modules * channelsPerModule);
	dmx.update(true); // black on startup
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
	
	panel.addPanel("settings 0");
	panel.addToggle("saveCurves", false);
	panel.addToggle("loadCurves", false);
	panel.addSlider("red", 1, 0, 1);
	panel.addSlider("green", 1, 0, 1);
	panel.addSlider("blue", 1, 0, 1);
	for(int module = 1; module <= modules; module++) {
		string label = "mod" + ofToString(module);
		panel.addSlider(label, 0, 0, 255, true);
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

float ofApp::_(string name) {
	return panel.getValueF(name);
}

void ofApp::update() {
	updateVirtualKinect();
	updateDmx();
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
	
	float red = _("red");
	float green = _("green");
	float blue = _("blue");
	int channel = 1;
	for(int module = 1; module <= modules; module++) {
		int cur = _("mod" + ofToString(module));
		dmx.setLevel(channel++, redCurve[cur * red]);
		dmx.setLevel(channel++, greenCurve[cur * green]);
		dmx.setLevel(channel++, blueCurve[cur * blue]);
		channel++;
	}
	if(dmx.isConnected()) {
		dmx.update();
	} else {
		panel.msg = "Could not connect to port " + port;
	}
}

void ofApp::drawVirtualKinect() {
	ofSetColor(255);
	kinect.draw(512, 0);
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
		int r = dmx.getLevel(rc);
		int g = dmx.getLevel(gc);
		int b = dmx.getLevel(bc);
		ofSetColor(r, g, b);
		ofFill();
		ofRect(4, module * 16 + 6, 14, 14);
		ofSetColor(255);
		ofNoFill();
		ofRect(4, module * 16 + 6, 14, 14);
		string rs = ofToString(rc) + ":" + ofToString(r);
		string gs = ofToString(gc) + ":" + ofToString(g);
		string bs = ofToString(bc) + ":" + ofToString(b);
		string text = label + " (" + rs + ", " + gs + ", " + bs + ")";
		drawHighlightString(text, 24, module * 16 + 2);
	}
	
	ofPopMatrix();
}

void ofApp::draw() {
	ofBackground(127);
	drawDmx();
	drawVirtualKinect();
}

