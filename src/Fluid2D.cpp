#include "Fluid2D.h"

Fluid2D::Fluid2D(int size)
{
    width  = size;
    height = size;

    string noop = "test_shaders/noop.vert";

    s_jacobi.load(noop, "test_shaders/jacobi.frag");
    s_blit.load(noop,      "test_shaders/blit.frag");
    s_advection.load(noop, "test_shaders/advection.frag");

    s_addVectors.load(noop, "test_shaders/addVectors.frag");

    s_divergence.load(noop, "test_shaders/divergence.frag");
    s_gradientSubtract.load(noop, "test_shaders/gradientSub.frag");

    s_setboundary.load(noop, "test_shaders/boundary.frag");

    //s_density_renderer.load("test_shaders/noop.vert", "test_shaders/density_render.frag");

    velocities = new ofFbo();
    density    = new ofFbo();
    temp       = new ofFbo();
    forces     = new ofFbo();
    pressure   = new ofFbo();
    temp2      = new ofFbo();

    // with alpha, 8 bits red, 8 bits green, 8 bits blue, from 0 to 255 in 256 steps.
    velocities -> allocate(width, height, GL_RGB);
    density    -> allocate(width, height, GL_RGB);
    temp       -> allocate(width, height, GL_RGB);
    temp2      -> allocate(width, height, GL_RGB);

    forces     -> allocate(width, height, GL_RGB);
    pressure   -> allocate(width, height, GL_RGB);

    // fbo allocation works more like malloc than calloc, so we clear the memory.
    clear_fbos();
}


void Fluid2D::clear_fbos()
{
    clear_fbo(velocities, zero_color);
    clear_fbo(density);
    clear_fbo(temp);
    clear_fbo(temp2);

    clear_fbo(forces, zero_color);
    clear_fbo(pressure);
}

void Fluid2D::clear_fbo(ofFbo * fbo, ofColor color)
{
    fbo -> begin();
    ofClear(color);
    fbo -> end();
}

Fluid2D::~Fluid2D()
{
}

// -- Public Interface.
// Opens the force field up for rendering new values.
void Fluid2D::begin_forces()
{
    forces -> begin();
}

// Closes the force field.
void Fluid2D::end_forces()
{
    forces -> end();
}

void Fluid2D::begin_density()
{
    density -> begin();
}

void Fluid2D::end_density()
{
    density -> end();
}

void Fluid2D::step()
{

    // parameters //<>//
    float dt = 5;

    // The rate of diffusion.
    float viscocity = 1;
    

    apply_forces(velocities, forces, temp);
    swap(&velocities, &temp);

    /*
    apply_forces(density, forces, temp);
    swap(&density, &temp);
    */


    clear_fbo(forces, ofColor(128, 128, 0, 255));
    
    // -- Advection.
    if (doAdvection)
    {
        advect(velocities, density, temp, dt);
        swap(&density, &temp);

        advect(velocities, velocities, temp, dt);
        swap(&velocities, &temp);
    }

    // -- Diffusion.

    // -- Always set the boundary before projection.
    if (setBoundary)
    {
        set_boundary(velocities, temp, -1);
        swap(&velocities, &temp);
    }

    if(doProject)
    {
        project(velocities, temp);
        swap(&velocities, &temp);
    }    

    if (setBoundary)
    {
        set_boundary(velocities, temp, -1);
        swap(&velocities, &temp);
    }

    if(doDiffusion)
    {
        diffuse(velocities, temp2, temp, viscocity, dt);
        swap(&velocities, &temp);

        diffuse(density, temp2, temp, viscocity, dt);
        swap(&density, &temp);
    }

    if (setBoundary)
    {
        set_boundary(velocities, temp, -1);
        swap(&velocities, &temp);
    }
    
    if(doProject)
    {
        project(velocities, temp);
        swap(&velocities, &temp);
    }
    
    if(setBoundary)
    {
        set_boundary(velocities, temp, -1);
        swap(&velocities, &temp);
    }
}

void Fluid2D::draw(float x, float y)
{
    density -> draw(x, y);
    //forces -> draw(x, y);
    //velocities -> draw(x, y);
}


// -- Private Methods.

// Swaps pointers to Fbo's.
// This saves time a prevents the need to blit textures back.
void Fluid2D::swap(ofFbo ** a, ofFbo ** b)
{
    ofFbo * temp = *a;
    *a = *b;
    *b = temp;
}

void Fluid2D::interior()
{
    // Draw a quad over the entire screen.
    //ofRect(1, 1, width - 2, height - 2);
    ofRect(0, 0, width, height);
}

void Fluid2D::blit(ofFbo * src)
{
    // Bind the shader to the next rendering.
    s_blit.begin();

    // Supply data to the shader.
    s_blit.setUniformTexture("src_texture", src -> getTexture(), 1);

    // Supply data to the shader.
    s_blit.setUniform1f("screen_width", ofGetWidth());
    s_blit.setUniform1f("screen_height", ofGetHeight());


    // Stop using this shader for rendering.
    s_blit.end();
}

// Blits (copies) from the src fbo to the dest fbo.
void Fluid2D::blit(ofFbo * src, ofFbo * dest)
{
    // Set the dest buffer as the target for rendering.
    dest -> begin();
    blit(src);
    
    // Finish the rendering of the dest buffer.
    dest -> end();
}

// Assumes an appropriate render target has alread been set.
void Fluid2D::render_density(float x, float y)
{
    // For now, we directly blit all three channels.
    //blit(density);
    density -> draw(x, y);

    // FIXME: handle shader translation, if I ever go back to a shader solution.


    /*
    // Bind the shader to the next rendering.
    s_density_renderer.begin();

    // Supply data to the shader.
    s_blit.setUniformTexture("src_texture", density.getTexture(), 1);

    // Supply data to the shader.
    s_blit.setUniform1f("screen_width", ofGetWidth());
    s_blit.setUniform1f("screen_height", ofGetHeight());

    // Draw a quad over the entire screen.
    interior();

    // Stop using this shader for rendering.
    s_blit.end();

    // Revert back to the screen as the render target.
    dest.end();
    */
}

// Computes diffusion as the solution to a poisson equation.
void Fluid2D::diffuse(ofFbo * src, ofFbo * temp, ofFbo * dest, float diff, float dt)
{
    /* First, we need to copy our current velocities int the temp texture
     * as our first guess.
     */
    temp -> begin();
    src  -> draw(0, 0);
    temp -> end();

    for (int i = 0; i < 20; i++)
    {
        diffuseIteration(src, temp, dest, diff, dt);
        swap(&temp, &dest);
    }

    swap(&temp, &dest);

}

void Fluid2D::diffuseIteration(ofFbo * original, ofFbo * current, ofFbo * dest, float viscocity, float dt)
{
    dest -> begin();

    s_jacobi.begin();

    // Supply data to the shader.
    s_jacobi.setUniformTexture("x", current  -> getTexture(), 0);
    s_jacobi.setUniformTexture("b", current -> getTexture(), 1);

    float grid_cell_len = 1.0;
    float alpha = grid_cell_len / (viscocity*dt);
    float beta = 1.0 / (4 + alpha);

    s_jacobi.setUniform1f("alpha", alpha);
    s_jacobi.setUniform1f("rBeta", beta);

    interior();

    s_jacobi.end();

    dest -> end();

}

// In this function, we use a vector add shader, rather than additive blending,
// because negative numbers are encoded as 0.0 - .5 and positives as .5 - 1.0
void Fluid2D::apply_forces(ofFbo * src, ofFbo * forces, ofFbo * dest)
{
    dest -> begin();

    s_addVectors.begin();

    s_addVectors.setUniformTexture("tex1", src    -> getTexture(), 0);
    s_addVectors.setUniformTexture("tex2", forces -> getTexture(), 1);

    interior();

    s_addVectors.end();

    dest -> end();
}

void Fluid2D::advect(ofFbo * velocity, ofFbo * quantity, ofFbo * dest, float dt)
{

    dest -> begin();

    s_advection.begin();

    // Supply data to the shader.
    s_advection.setUniformTexture("v", velocity -> getTexture(), 0);
    s_advection.setUniformTexture("x", quantity -> getTexture(), 1);

    // Supply data to the shader.
    s_advection.setUniform1f("timestep", dt);
    s_advection.setUniform1f("rdx", 1);// 1 / grid scale.

    // Draw a quad over the entire screen.
    interior();

    s_advection.end();

    dest -> end();
}

void Fluid2D::project(ofFbo * velocity, ofFbo * dest)
{


    ofFbo * divergence = temp2;
    computeDivergence(velocity, divergence); // Divergence gets stored in temp.

    clear_fbo(pressure, zero_color);

    for (int i = 0; i < 20; i++)
    {
        iteratePressure(velocity, divergence, pressure, temp);
        swap(&pressure, &temp);
    }
    
    subtractPressureGradient(velocity, pressure, dest);
}

void Fluid2D::computeDivergence(ofFbo * velocity, ofFbo * dest)
{
    dest -> begin();

    s_divergence.begin();
    
    s_divergence.setUniformTexture("w", velocity -> getTexture(), 0);
    s_divergence.setUniform1f("rdx", .5);// .5 / grid scale.

    // Draw a quad over the entire screen.
    interior();
        
    s_divergence.end();

    dest -> end();
}

void Fluid2D::iteratePressure(ofFbo * velocity,     ofFbo * divergence,
                              ofFbo * pressure_old, ofFbo * pressure_new)
{
    pressure_new -> begin();

    s_jacobi.begin();

    // Supply data to the shader.
    s_jacobi.setUniformTexture("x", pressure_old -> getTexture(), 0);
    s_jacobi.setUniformTexture("b", divergence -> getTexture(), 1);

    float alpha = 1.0;
    float beta  = .25;
    float rBeta = 1.0/beta;

    s_jacobi.setUniform1f("alpha", alpha);
    s_jacobi.setUniform1f("rBeta", beta);

    // Draw a quad over the entire screen.
    interior();

    s_jacobi.end();

    pressure_new -> end();
    
}

void Fluid2D::subtractPressureGradient(ofFbo * velocity, ofFbo * pressure, ofFbo * dest)
{
    dest -> begin();
    s_gradientSubtract.begin();

    s_gradientSubtract.setUniform1f("rdx", .5);// .5 / grid scale.
    s_gradientSubtract.setUniformTexture("p", pressure -> getTexture(), 0);
    s_gradientSubtract.setUniformTexture("w", velocity -> getTexture(), 1);

    // Draw a quad over the entire screen.
    interior();
    s_gradientSubtract.end();
    dest -> end();
}

void Fluid2D::set_boundary(ofFbo * src, ofFbo * dest, float scale)
{
    
    dest -> begin();
    
    // -- First deposit a copy of the src into the dest.
    src -> draw(0, 0);

    // Right boundary shader.
    s_setboundary.begin();
    s_setboundary.setUniform1f("scale", scale);
    s_setboundary.setUniform2f("offset", ofVec2f(-1, 0));
    s_setboundary.setUniformTexture("x", src -> getTexture(), 0);
    ofLine(width - 1, 1, width - 1, height - 1);
    s_setboundary.end();

    // Left Boundary Shader.
    s_setboundary.begin();
    s_setboundary.setUniform1f("scale", scale);
    s_setboundary.setUniform2f("offset", ofVec2f(1, 0));
    s_setboundary.setUniformTexture("x", src -> getTexture(), 0);
    ofLine(1, 1, 1, height - 1);
    s_setboundary.end();

    // Top Boundary Shader.
    s_setboundary.begin();
    s_setboundary.setUniform1f("scale", scale);
    s_setboundary.setUniform2f("offset", ofVec2f(0, 1));
    s_setboundary.setUniformTexture("x", src -> getTexture(), 0);
    ofLine(1, 1, width - 1, 1);
    s_setboundary.end();

    // Bottom Boundary Shader.
    s_setboundary.begin();
    s_setboundary.setUniform1f("scale", scale);
    s_setboundary.setUniform2f("offset", ofVec2f(0, -1));
    s_setboundary.setUniformTexture("x", src->getTexture(), 0);
    ofLine(1, height - 1, width - 1, height - 1);
    s_setboundary.end();

    // Bottom right corner.
    s_setboundary.begin();
    s_setboundary.setUniform1f("scale", scale);
    s_setboundary.setUniform2f("offset", ofVec2f(-1, -1));
    s_setboundary.setUniformTexture("x", src -> getTexture(), 0);
    ofLine(width - 1, height - 1, width - 1, height - 1);
    s_setboundary.end();

    // Top right corner.
    s_setboundary.begin();
    s_setboundary.setUniform1f("scale", scale);
    s_setboundary.setUniform2f("offset", ofVec2f(-1, 1));
    s_setboundary.setUniformTexture("x", src->getTexture(), 0);
    ofLine(width - 1, 0, width - 1, 0);
    s_setboundary.end();

    // Top left corner.
    s_setboundary.begin();
    s_setboundary.setUniform1f("scale", scale);
    s_setboundary.setUniform2f("offset", ofVec2f(1, 1));
    s_setboundary.setUniformTexture("x", src->getTexture(), 0);
    ofLine(0, 0, 0, 0);
    s_setboundary.end();

    // Bottom left corner.
    s_setboundary.begin();
    s_setboundary.setUniform1f("scale", scale);
    s_setboundary.setUniform2f("offset", ofVec2f(1, -1));
    s_setboundary.setUniformTexture("x", src -> getTexture(), 0);
    ofLine(0, height - 1, 0, height - 1);
    s_setboundary.end();

    dest -> end();
}