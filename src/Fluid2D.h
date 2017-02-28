#pragma once
#include "ofMain.h"

class Fluid2D
{
public:
    Fluid2D(int size);
    ~Fluid2D();

public:

    // Opens the force field up for rendering new values.
    void begin_forces();
    void end_forces();

    void begin_density();
    void end_density();

    void step();
    void draw(float x, float y);

    bool doProject   = true;
    bool setBoundary = true;
    bool doAdvection = true;
    bool doDiffusion = true;

    int width, height;

private:

    


    ofShader s_blit;
    //ofShader s_density_renderer;

    ofShader s_jacobi; // Used to Solve Poisson equations.
    ofShader s_advection;
    ofShader s_addVectors;

    ofShader s_divergence;
    ofShader s_gradientSubtract;

    ofShader s_setboundary;


    // Frame Buffer Objects used to store data on the GPU.
    ofFbo * velocities; // (r, g, b) -> x_vel, y_vel, z_vel.
    ofFbo * density;    // (r, g, b) 3 channel densities.
    ofFbo * temp;
    ofFbo * temp2;
    ofFbo * pressure;

    // Used to add forces.
    ofFbo * forces;

    ofFloatColor zero_color = ofFloatColor(.5, .5, 0);

    // Swaps pointers to Fbo's.
    void swap(ofFbo ** a, ofFbo ** b);

    // Renders the density to the screen.
    void render_density(float x, float y);

    // diff is viscocity.
    void diffuse(ofFbo * src, ofFbo * temp, ofFbo * dest, float diff, float dt);
    void diffuseIteration(ofFbo * original, ofFbo * current, ofFbo * dest,
                          float viscocity, float dt);

    void advect(ofFbo * velocity, ofFbo * quantity, ofFbo * dest, float time);

    void project(ofFbo * velocity, ofFbo * dest);
    void computeDivergence(ofFbo * velocity, ofFbo * dest);
    void iteratePressure(ofFbo * velocity, ofFbo * divergence,
                         ofFbo * pressure_old, ofFbo * pressure_new);
    void subtractPressureGradient(ofFbo * velocity, ofFbo * pressure, ofFbo * dest);

    void clear_fbos();
    void clear_fbo(ofFbo * fbo, ofColor color = ofColor(0, 0, 0, 0));

    // Copies the src texture to the dest texture.
    void blit(ofFbo * src, ofFbo * dest);

    // Copies the src texture to the current render target.
    void blit(ofFbo * src);
    
    
    // Updates the Velocity texture from the force texture,
    // Then clears away the force texture.
    void apply_forces (ofFbo * src, ofFbo * forces, ofFbo * temp);

    // Sets all 4 boundaries to enforce the no slip condition.
    void set_boundary(ofFbo * src, ofFbo * dest, float scale);

    // Renders a slab in the interior.
    void Fluid2D::interior();

};

/*
 * Goals:
 *
 * - Draw a Box on the screen.
 * - Create a texture with ray directions from the view camera.
 * - Instantiate a 3D scalar field.
 * - Sample the 3D scalar field.
 */