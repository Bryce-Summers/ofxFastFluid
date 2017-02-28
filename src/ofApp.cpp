#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    string noop = "test_shaders/noop.vert";
    s_circle.load(noop, "test_shaders/circle.frag");
    s_quad.load(  noop, "test_shaders/quad.frag");
    s_blit.load(  noop, "test_shaders/blit.frag");
    
    s_red.load(noop, "test_shaders/red.frag");

    fluid = new Fluid2D(600);
    
    // render_interpolated_quad();
    // Initialize the fluid density with a circle.
    
    fluid -> begin_density();
    render_circle();
    fluid -> end_density();


    /*
    fluid -> begin_density();
    render_red();
    fluid -> end_density();
    */

    fluid_pos.x = 100;
    fluid_pos.y = 100;
}

//--------------------------------------------------------------
void ofApp::update()
{
    fluid -> step();
}


// Here is the plan.
// I will port my stam fluid code to graphic cards, by putting u, v, density into rgb.


inline string const BoolToString(bool b)
{
    return b ? string("true") : string("false");
}

//--------------------------------------------------------------
void ofApp::draw(){

    // Draw the color frame buffer to the screen.
    //fbo_color.draw(0, 0);
    //fbo_color.draw(0, 0);

    fluid -> draw(fluid_pos.x, fluid_pos.y);

    ofDrawBitmapString("Project   = " + BoolToString(fluid -> doProject),   ofPoint(0, 20));
    ofDrawBitmapString("Boundary  = " + BoolToString(fluid -> setBoundary), ofPoint(0, 40));
    ofDrawBitmapString("Advection = " + BoolToString(fluid -> doAdvection), ofPoint(0, 60));
    ofDrawBitmapString("Diffusion = " + BoolToString(fluid -> doDiffusion), ofPoint(0, 80));
}

void ofApp::render_interpolated_quad()
{
    // Bind the shader to the next rendering.
    s_quad.begin();

    // Supply data to the shader.
    s_quad.setUniform1f("screen_width", ofGetWidth());
    s_quad.setUniform1f("screen_height", ofGetHeight());

    // Draw a quad over the entire screen.
    ofRect(0, 0, ofGetWidth(), ofGetHeight());

    // Stop using this shader for rendering.
    s_quad.end();
}

void ofApp::render_circle()
{
    // Bind the shader to the next rendering.
    s_circle.begin();

    // Supply data to the shader.
    s_circle.setUniform1f("screen_width", fluid -> width);
    s_circle.setUniform1f("screen_height", fluid -> height);

    // Draw a quad over the entire screen.
    ofRect(0, 0, fluid -> width, fluid -> height);

    // Stop using this shader for rendering.
    s_circle.end();
}

void ofApp::render_red()
{
    s_red.begin();
    // Supply data to the shader.
    s_red.setUniform1f("screen_width", ofGetWidth());
    s_red.setUniform1f("screen_height", ofGetHeight());

    // Draw a quad over the entire screen.
    ofRect(0, 0, ofGetWidth(), ofGetHeight());

    // Stop using this shader for rendering.
    s_red.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == 'p')
    {
        fluid -> doProject = !fluid -> doProject;
    }

    if (key == 'b')
    {
        fluid -> setBoundary = !fluid -> setBoundary;
    }

    if (key == 'd')
    {
         fluid -> doDiffusion = !fluid -> doDiffusion;
    }

    if (key == 'a')
    {
        fluid -> doAdvection = !fluid -> doAdvection;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

    // Gradually move the last point back to the mouse.
    last.x = x - fluid_pos.x;
    last.y = y - fluid_pos.y;

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

    x -= fluid_pos.x;
    y -= fluid_pos.y;

    float dx = x - last.x;
    float dy = y - last.y;

    int delay = 20;
    last.x = x;
    last.y = y;

    dx = CLAMP(127 + dx*15, 0, 255);
    dy = CLAMP(127 + dy*15, 0, 255);


    fluid -> begin_forces();

    ofSetColor(dx, dy, 0);
    //ofSetColor(255, 255, 0);

    ofFill();
    ofDrawCircle(x, y, 30);

    fluid -> end_forces();

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

    last.x = x;
    last.y = y;

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
