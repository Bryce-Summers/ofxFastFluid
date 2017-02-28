#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){

    ofGLWindowSettings settings;

    // Allows me to Use OpenGL version 4.2
    settings.setGLVersion(4, 2);
    ofCreateWindow(settings);

    ofSetWindowShape(1000, 1000);
    ofSetWindowPosition(100, 100);

    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    ofRunApp(new ofApp());
    
}
