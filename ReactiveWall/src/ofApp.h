#pragma once

#include "ofMain.h"
#include "ofxVirtualKinect.h"
#include "ofxCv.h"
#include "ofxDmx.h"
#include "ofxCurvesTool.h"
#include "ofxAutoControlPanel.h"
#include "Pulse.h"

class ofApp : public ofBaseApp {
public:
	
	void setup();
	void loadDmxSettings();
	void setupControlPanel();
	
	void exit();
	
	ofColor rastafari(float x);

	void update();
	void updateVirtualKinect();
	void updateDmx();
	
	void draw();
	void drawVirtualKinect();
	void drawDmx();
	
	ofxAutoControlPanel panel;
	float _(string name); // panel.getValueF() shortcut
	void _(string name, float x); // panel.setValueF() shortcut
	
	ofxVirtualKinect kinect;
	ofPixels previous;
	ofImage diff;
	cv::Mat rawMean, runningMean, adaptedMean, energy;
	cv::Mat moduleEnergy;
	
	ofxCurvesTool redCurve, greenCurve, blueCurve;
	ofxDmx dmx;
	string port;
	int modules, channelsPerModule;
};
