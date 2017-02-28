#pragma once

#include "ofMain.h"
#include "Fluid2D.h"

class ofApp : public ofBaseApp
{

    public:
        void setup();
        void update();
        void draw();

        void keyPressed(int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y );
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void mouseEntered(int x, int y);
        void mouseExited(int x, int y);
        void windowResized(int w, int h);
        void dragEvent(ofDragInfo dragInfo);
        void gotMessage(ofMessage msg);


    private:
        ofShader s_circle;
        ofShader s_quad;
        ofShader s_blit;
        ofShader s_red;

        Fluid2D * fluid;

        // Populates the color_fbo with a screen space red-green
        // interpolated quad.
        void render_interpolated_quad();

        // Renders a circle into the color frame buffer.
        void render_circle();
        void render_red();

        void blit(ofFbo & src, ofFbo & dest);

        ofPoint last;

        ofPoint fluid_pos;

};
