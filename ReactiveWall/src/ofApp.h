#pragma once

#include "ofMain.h"
#include "ofxVirtualKinect.h"
#include "ofxCv.h"
#include "ofxDmx.h"
#include "ofxCurvesTool.h"
#include "ofxAutoControlPanel.h"

class ofApp : public ofBaseApp {
public:
	
	void setup();
	void loadDmxSettings();
	void setupControlPanel();
	
	void exit();
	
	void update();
	void updateVirtualKinect();
	void updateDmx();
	
	void draw();
	void drawVirtualKinect();
	void drawDmx();
	
	ofxAutoControlPanel panel;
	float _(string name); // panel.getValueF() shortcut
	
	ofxVirtualKinect kinect;
	ofPixels previous;
	ofImage diff;
	cv::Mat mask;
	cv::Mat columnMean;
	
	ofxCurvesTool redCurve, greenCurve, blueCurve;
	ofxDmx dmx;
	string port;
	int modules, channelsPerModule;
};
