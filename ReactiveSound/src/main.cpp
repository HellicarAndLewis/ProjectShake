#include "ofApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, 256, 512, OF_WINDOW);
	ofRunApp(new ofApp());
}
