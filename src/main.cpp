/*
 *  CSCI 441, Computer Graphics, Fall 2017
 *
 *  Project: FINAL PROJECT
 *  File: main.cpp
 *
 *  Description:
 *      This file contains the basic setup to work with VAOs & VBOs using a
 *    MD5 model.
 *
 *  Author: Dr. Paone, Colorado School of Mines, 2017
 *
 *
 */

//******************************************************************************

#include <GL/glew.h>
#include <GLFW/glfw3.h>            // include GLFW framework header

// include GLM libraries and matrix functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SOIL/SOIL.h>        // for image loading

#include <stdio.h>                // for printf functionality
#include <stdlib.h>                // for exit functionality
#include <time.h>                // for time functionality

#include <vector>                    // for vector

#include <CSCI441/modelLoader3.hpp> // to load in OBJ models
#include <CSCI441/OpenGLUtils3.hpp> // to print info about OpenGL

#include <CSCI441/FramebufferUtils3.hpp>
#include <CSCI441/objects3.hpp>
#include <CSCI441/ShaderProgram3.hpp>
#include <CSCI441/TextureUtils.hpp>

#include "include/Marble.h"

//******************************************************************************
//
// Global Parameters
// has the window been moved? Move it once to make the mac window work 
// (hack in main.cpp main loop. This flag makes it only happen once.)
bool movedWindow = false;
// Clock
double start_time = glfwGetTime();

//Window Varibles
int windowWidth, windowHeight;
bool controlDown = false;
bool leftMouseDown = false;
glm::vec2 mousePosition( -9999.0f, -9999.0f );


//Camera Variables
glm::vec3 cameraAngles( 1.82f, 2.01f, 25.0f );  // Angle of camera
glm::vec3 eyePoint(   5.0f, 5.0f, 5.0f );       // Location of eye. Based on angle of camera
glm::vec3 lookAtPoint( 0.0f,  0.0f,  0.0f );    // Where the camera is looking. Based on hero location.
glm::vec3 upVector(    0.0f,  1.0f,  0.0f );    // Up vector. Changes with drunkness.
glm::vec3 lightPos(10.0f, 10.0f, 10.0f);        // Light based on locatio of beer 1
glm::vec3 lightPos1(10.0f, 10.0f, 10.0f);       // Light based on location of beer 2
glm::vec3 lightPos2(10.0f, 10.0f, 10.0f);       // Light based on location of beer 3
float cameraDis = 15;                           // Distance Camera is away from the hero
int ctrlPress = 0;                              // State of the CTRL key

// Platform Variables
struct VertexTextured {
    float x, y, z;
    float s, t;
};
GLuint platformVAOd;
GLuint platformTextureHandle;
GLfloat groundSize = 50;    // Size of ground

// Skybox Variables
GLuint skyboxVAOds[6];                      // all of our skybox VAOs
GLuint skyboxHandles[6];                    // all of our skybox handles
float skyBoxSize = 300;     // Size of Skybox

// Shader Programs
CSCI441::ShaderProgram* textureShaderProgram = NULL;
GLint uniform_modelMtx_loc, uniform_viewProjetionMtx_loc, uniform_tex_loc, uniform_color_loc;
GLint attrib_vPos_loc, attrib_vTextureCoord_loc;
GLuint brickTexHandle;
CSCI441::ShaderProgram* marbleShaderProgram = NULL;
GLint uniform_m_modelMtx_loc, uniform_m_viewProjetionMtx_loc, uniform_m_tex_loc, uniform_m_color_loc;
GLint attrib_m_vPos_loc, attrib_m_vTextureCoord_loc;

// Marble Variables
std::vector< Marble* > marbles;
GLfloat marbleRadius = 2.5;                 // How big the enemies are
GLint numMarbles = 4;                       // Number of objects in our particle system
float bump = 0.1;                           // Used for collision resolution
glm::vec3 marbleStart = glm::vec3(12,0,12); // Start location of the hero
GLuint beverageTextureHandle;
GLuint playerTextureHandle;
GLuint enemyTextureHandle;

//OreKart Variables
glm::vec3 OKlocation(20,0,20);  // Starting location of the OreKart
glm::vec3 OKdirection;          // Direction the OreKart is facing
GLfloat OK_rotation = 0;        // Direction the OreKart is rotating
GLfloat OKradius = 1;           // Collision radius of the OreKart
GLfloat OKk = 0.1;              // Spring constant for the OreKart
GLfloat OKrest_length = 8.0;    // Default resting length of the OreKart
GLuint OKTextureHandle;

// Rope Variables
GLuint ropeVAOd;
GLuint ropeTextureHandle;
GLuint ropeVbod;
const int ropeSize = 20;                    // Number of segments in the rope
VertexTextured ropeVertices[ropeSize];      // List to hold the rope segments
float ropeMass = 0.001;                     // Mass of each rope segment
float ropeK = 10.0;                         // Spring constant for the rope
float ropeRest = 
    .6*OKrest_length/(float) (ropeSize - 1);// Resting length of the rope springs
float ropeTimeScale = 0.1;                  // How much the rope moves 
int ropeRender = 30;                        // Number of times the rope physics is exectued

// Movement Variables
int goingForward = 0;           // State of W Key
int goingBackward = 0;          // State of S Key
int turnLeft = 0;               // State of A Key
int turnRight = 0;              // State of D Key
int lookBack = 0;               // State of F Key
float speedRatio = 0.01;        // Speed at which the cops start out
float speedIncrease = 0.2;      // Speed at which cops speed increase. Logistic growth.

// For model
CSCI441::ShaderProgram *lightingProgramHandle = NULL;
GLint mvp_lights_location = -1;
GLint vpos_light_attrib_location = -1;
GLint lightPosUniLoc;
GLint lightPos1UniLoc;
GLint lightPos2UniLoc;
GLint normalAttLoc;
GLint viewUniformLoc;
GLint modelUniLoc;
CSCI441::ModelLoader* model = NULL;
float templeX = 7;  // Length of the Temple
float templeZ = 5;  // Width of the Temple

//FBO Stuff
GLuint fbo;
int framebufferWidth = 1024, framebufferHeight = 1024;
GLuint framebufferTextureHandle;

CSCI441::ShaderProgram *postprocessingShaderProgram = NULL;
GLint uniform_post_proj_loc, uniform_post_fbo_loc, uniform_post_time_loc;
GLint attrib_post_vpos_loc, attrib_post_vtex_loc;
GLint uniform_post_dist_loc, uniform_post_a_loc;
GLuint texturedQuadVAO;
int beers = 0;                  // Number of beers collected
float b_distance=0;             // Blurr distance
float b_dist_inc = 0.003;       // Blur distance increment
float alpha=1;                  // Beginning ratio of clearness
float alpha_ratio = 0.7;        // Rate at which we distribute our focus to seeing double
float sys_time = 0;             // Timing variable to vary over time

//Trees
CSCI441::ShaderProgram *treeShaderProgram = NULL;
GLint modelview_tree_uniform_location, projection_tree_uniform_location;
GLint vpos_tree_attrib_location;
const int NUM_POINTS = 50;
struct Vertex { GLfloat x, y, z; };
Vertex points[NUM_POINTS];
GLuint pointsVAO, pointsVBO;

GLuint treeTextureHandle;

//******************************************************************************
//
// Helper Functions

// convertSphericalToCartesian() ///////////////////////////////////////////////
//
// This function updates the camera's position in cartesian coordinates based
//  on its position in spherical coordinates. Should be called every time
//  cameraAngles is updated.
//
////////////////////////////////////////////////////////////////////////////////
void convertSphericalToCartesian() {
    // Define eye point based on camera distance and camera angle. 
    if (lookBack == GLFW_PRESS || lookBack == GLFW_REPEAT) {
        eyePoint.x = lookAtPoint.x + cameraDis * sinf( cameraAngles.x - M_PI ) * sinf( cameraAngles.y );
        eyePoint.y = lookAtPoint.y + cameraDis * -cosf( cameraAngles.y );
        eyePoint.z = lookAtPoint.z + cameraDis * -cosf( cameraAngles.x - M_PI) * sinf( cameraAngles.y );
    }
    else {
    eyePoint.x = lookAtPoint.x + cameraDis * sinf( cameraAngles.x ) * sinf( cameraAngles.y );
    eyePoint.y = lookAtPoint.y + cameraDis * -cosf( cameraAngles.y );
    eyePoint.z = lookAtPoint.z + cameraDis * -cosf( cameraAngles.x ) * sinf( cameraAngles.y );
    }

}

float randRange(float min, float max){
    // Return a random float in a given range
    return rand()/(float) RAND_MAX * (max-min) + min;
}

bool registerOpenGLTexture(unsigned char *textureData,
                           unsigned int texWidth, unsigned int texHeight,
                           GLuint &textureHandle) {
    if(textureData == 0) {
        fprintf(stderr,"Cannot register texture; no data specified.");
        return false;
    }

    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_2D, textureHandle);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);

    return true;
}

//******************************************************************************
//
// Event Callbacks

// error_callback() ////////////////////////////////////////////////////////////
//
//        We will register this function as GLFW's error callback.
//    When an error within GLFW occurs, GLFW will tell us by calling
//    this function.  We can then print this info to the terminal to
//    alert the user.
//
////////////////////////////////////////////////////////////////////////////////
static void error_callback(int error, const char* description) {
    fprintf(stderr, "[ERROR]: %s\n", description);
}

// key_callback() //////////////////////////////////////////////////////////////
//
//        We will register this function as GLFW's keypress callback.
//    Responds to key presses and key releases
//
////////////////////////////////////////////////////////////////////////////////
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if( (key == GLFW_KEY_ESCAPE || key == 'Q') && action == GLFW_PRESS )
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    switch( key ){
        case GLFW_KEY_W: goingForward   = action;
            break;
        case GLFW_KEY_S: goingBackward  = action;
            break;
        case GLFW_KEY_A: turnLeft       = action;
            break;
        case GLFW_KEY_D: turnRight      = action;
            break;
        case GLFW_KEY_LEFT_CONTROL: ctrlPress = action;
            break;
        case GLFW_KEY_F: lookBack = action;
    }
}

// mouse_button_callback() /////////////////////////////////////////////////////
//
//        We will register this function as GLFW's mouse button callback.
//    Responds to mouse button presses and mouse button releases.  Keeps track if
//    the control key was pressed when a left mouse click occurs to allow
//    zooming of our arcball camera.
//
////////////////////////////////////////////////////////////////////////////////
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS ) {
        leftMouseDown = true;
        controlDown = (mods & GLFW_MOD_CONTROL);
    } else {
        leftMouseDown = false;
        mousePosition.x = -9999.0f;
        mousePosition.y = -9999.0f;
        controlDown = false;
    }
}

// cursor_callback() ///////////////////////////////////////////////////////////
//
//        We will register this function as GLFW's cursor movement callback.
//    Responds to mouse movement.  When active motion is used with the left
//    mouse button an arcball camera model is followed.
//
////////////////////////////////////////////////////////////////////////////////
static void cursor_callback(GLFWwindow* window, double xpos, double ypos) {
    // make sure movement is in bounds of the window
    // glfw captures mouse movement on entire screen

    if( xpos > 0 && xpos < windowWidth ) {
        if( ypos > 0 && ypos < windowHeight ) {
            // active motion
            if( leftMouseDown ) {
                if( (mousePosition.x - -9999.0f) < 0.001f ) {
                    mousePosition.x = xpos;
                    mousePosition.y = ypos;
                } else {
                    if( ctrlPress == 0) {
                        cameraAngles.x += (xpos - mousePosition.x)*0.005f;
                        cameraAngles.y += (ypos - mousePosition.y)*0.005f;

                        if( cameraAngles.y < M_PI/2 ) cameraAngles.y = M_PI/2 + 0.001f;
                        if( cameraAngles.y >= M_PI*6/7 ) cameraAngles.y = M_PI*6/7 - 0.001f;
                    } else {
                        cameraDis -= (mousePosition.y - ypos)/10;
                    }
                    convertSphericalToCartesian();

                    mousePosition.x = xpos;
                    mousePosition.y = ypos;
                }
            }
            // passive motion
            else {

            }
        }
    }
}

// scroll_callback() ///////////////////////////////////////////////////////////
//
//        We will register this function as GLFW's scroll wheel callback.
//    Responds to movement of the scroll where.  Allows zooming of the arcball
//    camera.
//
////////////////////////////////////////////////////////////////////////////////
static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset ) {
    double totChgSq = yOffset;
    cameraAngles.z += totChgSq*0.2f;

    if( cameraAngles.z <= 2.0f ) cameraAngles.z = 2.0f;
    if( cameraAngles.z >= 30.0f ) cameraAngles.z = 30.0f;

    convertSphericalToCartesian();
}

//******************************************************************************
//
// Setup Functions

// setupGLFW() /////////////////////////////////////////////////////////////////
//
//        Used to setup everything GLFW related.  This includes the OpenGL context
//    and our window.
//
////////////////////////////////////////////////////////////////////////////////
GLFWwindow* setupGLFW() {
    // set what function to use when registering errors
    // this is the ONLY GLFW function that can be called BEFORE GLFW is initialized
    // all other GLFW calls must be performed after GLFW has been initialized
    glfwSetErrorCallback(error_callback);

    // initialize GLFW
    if (!glfwInit()) {
        fprintf( stderr, "[ERROR]: Could not initialize GLFW\n" );
        exit(EXIT_FAILURE);
    } else {
        fprintf( stdout, "[INFO]: GLFW initialized\n" );
    }

    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );                        // request forward compatible OpenGL context
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );    // request OpenGL Core Profile context
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );        // request OpenGL 3.x context
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );        // request OpenGL 3.3 context

    // create a window for a given size, with a given title
    GLFWwindow *window = glfwCreateWindow(640, 480, "Lab11: Collision Detection", NULL, NULL);
    if( !window ) {                        // if the window could not be created, NULL is returned
        fprintf( stderr, "[ERROR]: GLFW Window could not be created\n" );
        glfwTerminate();
        exit( EXIT_FAILURE );
    } else {
        fprintf( stdout, "[INFO]: GLFW Window created\n" );
    }

    glfwMakeContextCurrent(    window );    // make the created window the current window
    glfwSwapInterval( 1 );                    // update our screen after at least 1 screen refresh

    glfwSetKeyCallback(               window, key_callback                  );    // set our keyboard callback function
    glfwSetMouseButtonCallback( window, mouse_button_callback );    // set our mouse button callback function
    glfwSetCursorPosCallback(      window, cursor_callback              );    // set our cursor position callback function
    glfwSetScrollCallback(            window, scroll_callback              );    // set our scroll wheel callback function

    return window;                                        // return the window that was created
}

// setupOpenGL() ///////////////////////////////////////////////////////////////
//
//      Used to setup everything OpenGL related.
//
////////////////////////////////////////////////////////////////////////////////
void setupOpenGL() {
    glEnable( GL_DEPTH_TEST );                    // enable depth testing
    glDepthFunc( GL_LESS );                            // use less than depth test

    glFrontFace( GL_CCW );

    glEnable(GL_BLEND);                                    // enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    // use one minus blending equation

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );    // clear the frame buffer to black

    glEnable( GL_TEXTURE_2D );

    glPointSize( 4.0 );
}

// setupGLEW() /////////////////////////////////////////////////////////////////
//
//      Used to initialize GLEW
//
////////////////////////////////////////////////////////////////////////////////
void setupGLEW() {
    glewExperimental = GL_TRUE;
    GLenum glewResult = glewInit();

    /* check for an error */
    if( glewResult != GLEW_OK ) {
        printf( "[ERROR]: Error initalizing GLEW\n");
        /* Problem: glewInit failed, something is seriously wrong. */
      fprintf( stderr, "[ERROR]: %s\n", glewGetErrorString(glewResult) );
        exit(EXIT_FAILURE);
    } else {
         fprintf( stdout, "[INFO]: GLEW initialized\n" );
         fprintf( stdout, "[INFO]: Status: Using GLEW %s\n", glewGetString(GLEW_VERSION) );
    }

    if( !glewIsSupported( "GL_VERSION_2_0" ) ) {
        printf( "[ERROR]: OpenGL not version 2.0+.  GLSL not supported\n" );
        exit(EXIT_FAILURE);
    }
}

// setupTextures() /////////////////////////////////////////////////////////////
//
//      Load and register all the tetures for our program
//
////////////////////////////////////////////////////////////////////////////////
void setupTextures() {

    // and get handles for our full skybox
  printf( "[INFO]: registering skybox..." );
  fflush( stdout );
  skyboxHandles[0] = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/skybox2/back.png"   );   printf( "." ); fflush( stdout );
  skyboxHandles[1] = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/skybox2/left.png"  );   printf( "." ); fflush( stdout );
  skyboxHandles[2] = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/skybox2/front.png"  );   printf( "." ); fflush( stdout );
  skyboxHandles[3] = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/skybox2/right.png"   );   printf( "." ); fflush( stdout );
  skyboxHandles[4] = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/skybox2/bottom.png" );        printf( "." ); fflush( stdout );
  skyboxHandles[5] = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/skybox2/top.png"    );   printf( "." ); fflush( stdout );
  printf( "skybox textures read in and registered!\n\n" );

    unsigned char *brickTexData;
    int brickTexWidth, brickTexHeight;
    CSCI441::TextureUtils::loadPPM("textures/brick.ppm", brickTexWidth, brickTexHeight, brickTexData);
    registerOpenGLTexture(brickTexData, brickTexWidth, brickTexHeight, brickTexHandle);
    printf( "[INFO]: brick texture read in and registered\n" );

    platformTextureHandle = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/grass.png" );
    beverageTextureHandle = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/coors-b.png" );
    playerTextureHandle = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/Mines.jpg" );
    enemyTextureHandle  = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/ends.png" );
    ropeTextureHandle   = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/rope.png" );
    treeTextureHandle   = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/goodboi.png" );
    OKTextureHandle   = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/MinesLogo.png" );
}

void setupShaders() {
    //Model Shading
    lightingProgramHandle = new CSCI441::ShaderProgram("shaders/customShader.v.glsl", "shaders/customShader.f.glsl");
    mvp_lights_location = lightingProgramHandle->getUniformLocation( "mvpMatrix");
    if (mvp_lights_location < 0)
        printf("[ERROR]: mvp_lights_location is negative\n");
    vpos_light_attrib_location = lightingProgramHandle->getAttributeLocation("vPosition");
    if (vpos_light_attrib_location < 0)
        printf("[ERROR]: vpos_light_attrib_location is negative\n");
    //Lighting things
    lightPosUniLoc = lightingProgramHandle -> getUniformLocation("lightPos");
    lightPos1UniLoc = lightingProgramHandle->getUniformLocation("lightPos1");
    lightPos2UniLoc = lightingProgramHandle->getUniformLocation("lightPos2");
    modelUniLoc = lightingProgramHandle -> getUniformLocation( "model");
    normalAttLoc = lightingProgramHandle -> getAttributeLocation("aNormal");
    viewUniformLoc = lightingProgramHandle->getUniformLocation("viewPos");
    //Texturing Things
    textureShaderProgram = new CSCI441::ShaderProgram( "shaders/textureShader.v.glsl", "shaders/textureShader.f.glsl" );
    uniform_modelMtx_loc         = textureShaderProgram->getUniformLocation( "modelMtx" );
    uniform_viewProjetionMtx_loc = textureShaderProgram->getUniformLocation( "viewProjectionMtx" );
    uniform_tex_loc              = textureShaderProgram->getUniformLocation( "tex" );
    uniform_color_loc             = textureShaderProgram->getUniformLocation( "color" );
    attrib_vPos_loc                 = textureShaderProgram->getAttributeLocation( "vPos" );
    attrib_vTextureCoord_loc      = textureShaderProgram->getAttributeLocation( "vTextureCoord" );
    //Texturing Solid Object
    marbleShaderProgram = new CSCI441::ShaderProgram( "shaders/marbleShader.v.glsl", "shaders/marbleShader.f.glsl" );
    uniform_m_modelMtx_loc         = textureShaderProgram->getUniformLocation( "modelMtx" );
    uniform_m_viewProjetionMtx_loc = textureShaderProgram->getUniformLocation( "viewProjectionMtx" );
    uniform_m_tex_loc              = textureShaderProgram->getUniformLocation( "tex" );
    uniform_m_color_loc             = textureShaderProgram->getUniformLocation( "color" );
    attrib_m_vPos_loc                 = textureShaderProgram->getAttributeLocation( "vPos" );
    attrib_m_vTextureCoord_loc      = textureShaderProgram->getAttributeLocation( "vTextureCoord" );
    // Texturing trees
    treeShaderProgram   = new CSCI441::ShaderProgram("shaders/billboardQuadShader.v.glsl",
                                                "shaders/billboardQuadShader.g.glsl",
                                                "shaders/billboardQuadShader.f.glsl" );
    modelview_tree_uniform_location  = treeShaderProgram->getUniformLocation( "mvMatrix" );
    projection_tree_uniform_location = treeShaderProgram->getUniformLocation( "projMatrix" );
    vpos_tree_attrib_location        = treeShaderProgram->getAttributeLocation( "vPos" );
    // Drunk Shader
    postprocessingShaderProgram = new CSCI441::ShaderProgram("shaders/blurShader.v.glsl", "shaders/blurShader.f.glsl");
    uniform_post_proj_loc = postprocessingShaderProgram->getUniformLocation("projectionMtx");
    uniform_post_fbo_loc = postprocessingShaderProgram->getUniformLocation("fbo");
    uniform_post_time_loc = postprocessingShaderProgram->getUniformLocation("systime");
    uniform_post_dist_loc = postprocessingShaderProgram->getUniformLocation("distance");
    uniform_post_a_loc = postprocessingShaderProgram->getUniformLocation("a");
    attrib_post_vpos_loc = postprocessingShaderProgram->getAttributeLocation("vPos");
    attrib_post_vtex_loc = postprocessingShaderProgram->getAttributeLocation("vTexCoord");

    }

// setupBuffers() //////////////////////////////////////////////////////////////
//
//      Create our VAOs & VBOs. Send vertex data to the GPU for future rendering
//
////////////////////////////////////////////////////////////////////////////////
void setupBuffers() {
    //////////////////////////////////////////
    //
    // Load Temple Model
    model = new CSCI441::ModelLoader();
    CSCI441::ModelLoader::enableAutoGenerateNormals();
    model->loadModelFile("models/temple.obj");


    //////////////////////////////////////////
    //
    // PLATFORM

    GLfloat platformSize = groundSize;

    VertexTextured platformVertices[4] = {
            { -platformSize, 0.0f, -platformSize,   0.0f,  0.0f }, // 0 - BL
            {  platformSize, 0.0f, -platformSize,   1.0f,  0.0f }, // 1 - BR
            { -platformSize, 0.0f,  platformSize,   0.0f,  1.0f }, // 2 - TL
            {  platformSize, 0.0f,  platformSize,   1.0f,  1.0f }  // 3 - TR
    };

    unsigned short platformIndices[4] = { 0, 1, 2, 3 };

    glGenVertexArrays( 1, &platformVAOd );
    glBindVertexArray( platformVAOd );

    GLuint vbods[2];
    glGenBuffers( 2, vbods );

    glBindBuffer( GL_ARRAY_BUFFER, vbods[0] );
    glBufferData( GL_ARRAY_BUFFER, sizeof( platformVertices ), platformVertices, GL_STATIC_DRAW );

    glEnableVertexAttribArray( attrib_vPos_loc );
    glVertexAttribPointer( attrib_vPos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) 0 );

    glEnableVertexAttribArray( attrib_vTextureCoord_loc );
    glVertexAttribPointer( attrib_vTextureCoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) (sizeof(float) * 3) );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbods[1] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( platformIndices ), platformIndices, GL_STATIC_DRAW );

    //////////////////////////////////////////
    //
    // SKYBOX

    unsigned short groundIndices[4] = {
        0, 1, 2, 3
    };

    VertexTextured groundVertices[4] = {
            { -skyBoxSize, -skyBoxSize, -skyBoxSize,   0.0f,  0.0f }, // 0 - BL
            {  skyBoxSize, -skyBoxSize, -skyBoxSize,   -1.0f,  0.0f }, // 1 - BR
            { -skyBoxSize, -skyBoxSize,  skyBoxSize,   0.0f,  -1.0f }, // 2 - TL
            {  skyBoxSize, -skyBoxSize,  skyBoxSize,   -1.0f,  -1.0f }  // 3 - TR
    };

    VertexTextured leftWallVerts[4] = {
      { -skyBoxSize, -skyBoxSize, -skyBoxSize,   0.0f,  0.0f }, // 0 - BL
      {  skyBoxSize, -skyBoxSize, -skyBoxSize,   1.0f,  0.0f }, // 1 - BR
      { -skyBoxSize,  skyBoxSize, -skyBoxSize,   0.0f,  1.0f }, // 2 - TL
      {  skyBoxSize,  skyBoxSize, -skyBoxSize,   1.0f,  1.0f }  // 3 - TR
    };

    VertexTextured rightWallVerts[4] = {
      { -skyBoxSize, -skyBoxSize,  skyBoxSize,   0.0f,  0.0f }, // 0 - BL
      {  skyBoxSize, -skyBoxSize,  skyBoxSize,   -1.0f,  0.0f }, // 1 - BR
      { -skyBoxSize,  skyBoxSize,  skyBoxSize,   0.0f,  1.0f }, // 2 - TL
      {  skyBoxSize,  skyBoxSize,  skyBoxSize,   -1.0f,  1.0f }  // 3 - TR
    };

    VertexTextured backWallVerts[4] = {
      { -skyBoxSize, -skyBoxSize, -skyBoxSize,   0.0f,  0.0f }, // 0 - BL
      { -skyBoxSize, -skyBoxSize,  skyBoxSize,   -1.0f,  0.0f }, // 1 - BR
      { -skyBoxSize,  skyBoxSize, -skyBoxSize,   0.0f,  1.0f }, // 2 - TL
      { -skyBoxSize,  skyBoxSize,  skyBoxSize,   -1.0f,  1.0f }  // 3 - TR
    };

    VertexTextured frontWallVerts[4] = {
      {  skyBoxSize, -skyBoxSize, -skyBoxSize,   0.0f,  0.0f }, // 0 - BL
      {  skyBoxSize, -skyBoxSize,  skyBoxSize,   1.0f,  0.0f }, // 1 - BR
      {  skyBoxSize,  skyBoxSize, -skyBoxSize,   0.0f,  1.0f }, // 2 - TL
      {  skyBoxSize,  skyBoxSize,  skyBoxSize,   1.0f,  1.0f }  // 3 - TR
    };

    VertexTextured topWallVerts[4] = {
            { -skyBoxSize,  skyBoxSize, -skyBoxSize,   1.0f,  -1.0f }, // 0 - BL
            {  skyBoxSize,  skyBoxSize, -skyBoxSize,   1.0f,  0.0f }, // 1 - BR
            { -skyBoxSize,  skyBoxSize,  skyBoxSize,   0.0f,  -1.0f }, // 2 - TL
            {  skyBoxSize,  skyBoxSize,  skyBoxSize,   0.0f,  0.0f }  // 3 - TR
    };

    glGenVertexArrays( 6, skyboxVAOds );

  glBindVertexArray( skyboxVAOds[4] );
  glGenBuffers(2, vbods);
  glBindBuffer( GL_ARRAY_BUFFER, vbods[0] );
  glBufferData( GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbods[1] );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW );
  glEnableVertexAttribArray(attrib_vPos_loc);
  glVertexAttribPointer(attrib_vPos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) 0);
  glEnableVertexAttribArray(attrib_vTextureCoord_loc);
  glVertexAttribPointer(attrib_vTextureCoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) (sizeof(float) * 3));

  glBindVertexArray( skyboxVAOds[3] );
  glGenBuffers(2, vbods);
  glBindBuffer( GL_ARRAY_BUFFER, vbods[0] );
  glBufferData( GL_ARRAY_BUFFER, sizeof(leftWallVerts), leftWallVerts, GL_STATIC_DRAW );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbods[1] );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW );
  glEnableVertexAttribArray(attrib_vPos_loc);
  glVertexAttribPointer(attrib_vPos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) 0);
  glEnableVertexAttribArray(attrib_vTextureCoord_loc);
  glVertexAttribPointer(attrib_vTextureCoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) (sizeof(float) * 3));

  glBindVertexArray( skyboxVAOds[1] );
  glGenBuffers(2, vbods);
  glBindBuffer( GL_ARRAY_BUFFER, vbods[0] );
  glBufferData( GL_ARRAY_BUFFER, sizeof(rightWallVerts), rightWallVerts, GL_STATIC_DRAW );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbods[1] );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW );
  glEnableVertexAttribArray(attrib_vPos_loc);
  glVertexAttribPointer(attrib_vPos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) 0);
  glEnableVertexAttribArray(attrib_vTextureCoord_loc);
  glVertexAttribPointer(attrib_vTextureCoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) (sizeof(float) * 3));

  glBindVertexArray( skyboxVAOds[0] );
  glGenBuffers(2, vbods);
  glBindBuffer( GL_ARRAY_BUFFER, vbods[0] );
  glBufferData( GL_ARRAY_BUFFER, sizeof(backWallVerts), backWallVerts, GL_STATIC_DRAW );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbods[1] );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW );
  glEnableVertexAttribArray(attrib_vPos_loc);
  glVertexAttribPointer(attrib_vPos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) 0);
  glEnableVertexAttribArray(attrib_vTextureCoord_loc);
  glVertexAttribPointer(attrib_vTextureCoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) (sizeof(float) * 3));

  glBindVertexArray( skyboxVAOds[2] );
  glGenBuffers(2, vbods);
  glBindBuffer( GL_ARRAY_BUFFER, vbods[0] );
  glBufferData( GL_ARRAY_BUFFER, sizeof(frontWallVerts), frontWallVerts, GL_STATIC_DRAW );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbods[1] );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW );
  glEnableVertexAttribArray(attrib_vPos_loc);
  glVertexAttribPointer(attrib_vPos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) 0);
  glEnableVertexAttribArray(attrib_vTextureCoord_loc);
  glVertexAttribPointer(attrib_vTextureCoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) (sizeof(float) * 3));

  glBindVertexArray( skyboxVAOds[5] );
  glGenBuffers(2, vbods);
  glBindBuffer( GL_ARRAY_BUFFER, vbods[0] );
  glBufferData( GL_ARRAY_BUFFER, sizeof(topWallVerts), topWallVerts, GL_STATIC_DRAW );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbods[1] );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW );
  glEnableVertexAttribArray(attrib_vPos_loc);
  glVertexAttribPointer(attrib_vPos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) 0);
  glEnableVertexAttribArray(attrib_vTextureCoord_loc);
  glVertexAttribPointer(attrib_vTextureCoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) (sizeof(float) * 3));

    //////////////////////////////////////////
    //
    // ROPE
    glm::vec3 begin = marbleStart;
    glm::vec3 end = OKlocation;
    for (int i = 0; i < ropeSize; i++){
        float a = i/(float) ropeSize;
        VertexTextured b = {a*begin.x + (1-a)*end.x, 1.0, a*begin.z + (1-a)*end.z, i%2, i%2};
        ropeVertices[i] = b;
    }

    glGenVertexArrays( 1, &ropeVAOd );
    glBindVertexArray( ropeVAOd );

    glGenBuffers( 1, &ropeVbod);
    glBindBuffer( GL_ARRAY_BUFFER, ropeVbod );
    glBufferData( GL_ARRAY_BUFFER, sizeof( ropeVertices ), ropeVertices, GL_STREAM_DRAW );
    glEnableVertexAttribArray( attrib_vPos_loc );
    glVertexAttribPointer( attrib_vPos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) 0 );
    glEnableVertexAttribArray( attrib_vTextureCoord_loc );
    glVertexAttribPointer( attrib_vTextureCoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) (sizeof(float) * 3) );

  //////////////////////////////////////////
  //
  // FBO Box

  VertexTextured texturedQuadVerts[4] = {
      { -1.0f, -1.0f, 0.0f, 0.0f, 0.0f }, // 0 - BL
      { 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f }, // 1 - BR
      { -1.0f, 1.0f,  0.0f, 0.0f, 1.0f }, // 2 - TL
      { 1.0f,  1.0f,  0.0f, 1.0f, 1.0f }  // 3 - TR
  };

  unsigned short texturedQuadIndices[4] = { 0, 1, 2, 3 };

  glGenVertexArrays(1, &texturedQuadVAO);
  glBindVertexArray(texturedQuadVAO);
  glGenBuffers(2, vbods);
  glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(texturedQuadVerts), texturedQuadVerts, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(texturedQuadIndices), texturedQuadIndices, GL_STATIC_DRAW);
  postprocessingShaderProgram->useProgram();
  glEnableVertexAttribArray(attrib_post_vpos_loc);
  glVertexAttribPointer(attrib_post_vpos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)0);
  glEnableVertexAttribArray(attrib_post_vtex_loc);
  glVertexAttribPointer(attrib_post_vtex_loc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void *)(sizeof(float) * 3));

    //////////////////////////////////////////
    //
    // Trees

    for( int i = 0; i < NUM_POINTS; i++ ) {
        Vertex v = {    randRange(-groundSize, groundSize),
                        0,
                        randRange(-groundSize, groundSize) };
        while (abs(v.x) < 1.5*templeX and abs(v.z) < 1.5*templeZ) {
            Vertex vnew = {   randRange(-groundSize, groundSize),
                    0,
                    randRange(-groundSize, groundSize) };
            v = vnew;
        }
        points[i] = v;
    }

    glGenVertexArrays( 1, &pointsVAO );
    glBindVertexArray( pointsVAO );

    glGenBuffers( 1, &pointsVBO );
    glBindBuffer( GL_ARRAY_BUFFER, pointsVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW );
    glEnableVertexAttribArray( vpos_tree_attrib_location );
    glVertexAttribPointer( vpos_tree_attrib_location, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0 );



}

void setupFramebuffer() {
glGenFramebuffers(1, &fbo);
glBindFramebuffer(GL_FRAMEBUFFER, fbo);

GLuint rbo;
glGenRenderbuffers(1, &rbo);
glBindRenderbuffer(GL_RENDERBUFFER, rbo);
glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, framebufferWidth, framebufferHeight);
glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,    rbo);

glGenTextures(1, &framebufferTextureHandle);
glBindTexture(GL_TEXTURE_2D, framebufferTextureHandle);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,    framebufferWidth, framebufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTextureHandle, 0);
CSCI441::FramebufferUtils::printFramebufferStatusMessage(GL_FRAMEBUFFER);
CSCI441::FramebufferUtils::printFramebufferInfo(GL_FRAMEBUFFER, fbo);

}

void setLights() {
    // Set lighting based on beer location
    glm::vec3 beerLoc = marbles[1]->location;
    glm::vec3 beerLoc1 = marbles[2]->location;
    glm::vec3 beerLoc2 = marbles[3]->location;
    lightPos = glm::vec3(beerLoc.x, 10, beerLoc.z);
    lightPos1 = glm::vec3(beerLoc1.x, 10, beerLoc1.z);
    lightPos2 = glm::vec3(beerLoc2.x, 10, beerLoc2.z);
}
void populateMarbles() {
    srand( time(NULL) );
    float rangeX = groundSize*2;
    float rangeZ = groundSize*2;
    for(int i = 0; i < numMarbles; i++) {
        Marble* m = new Marble( glm::vec3( rand()/(float)RAND_MAX * rangeX - rangeX/2.0f,
                                0.0f,
                               (rangeZ * (i/(float)numMarbles)) - rangeZ/2.0f),
                                glm::vec3( rand()/(float)RAND_MAX, 0.0, rand()/(float)RAND_MAX ),
                                marbleRadius );
        marbles.push_back( m );
    }
    // Our Hero
    marbles[0]->location    = marbleStart;
    marbles[0]->direction   = glm::vec3(-1,0,-1);
    marbles[0]->radius      = 0.5;
    // First Goal
    marbles[1]->location    = glm::vec3(randRange(-groundSize, groundSize),
                                        0,
                                        randRange(-groundSize, groundSize));
    marbles[1]->radius      = 0.5;
    marbles[1]->type        = 1;
    // Second Goal
    marbles[1]->location    = glm::vec3(randRange(-groundSize, groundSize),
                                        0,
                                        randRange(-groundSize, groundSize));
    marbles[2]->radius      = 0.5;
    marbles[2]->type        = 1;
    // Third Goal
    marbles[3]->location    = glm::vec3(randRange(-groundSize, groundSize),
                                        0,
                                        randRange(-groundSize, groundSize));
    marbles[3]->radius      = 0.5;
    marbles[3]->type        = 1;
    printf("Num Marbles %d. Expected %d\n", numMarbles, marbles.size());
    for (int i =0; i<numMarbles; i++){
        float xloc = marbles[i]->location.x;
        float yloc = marbles[i]->location.y;
        float zloc = marbles[i]->location.z;
        printf("Marble at %f %f %f\n", xloc, yloc, zloc);
    }
}

//******************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

// renderScene() ///////////////////////////////////////////////////////////////
//
//        This method will contain all of the objects to be drawn.
//
////////////////////////////////////////////////////////////////////////////////
void drawWheels(glm::mat4 modelMtx) {
    CSCI441::drawSolidTorus(0.15, 0.30, 10, 12);
    glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &(glm::translate(modelMtx, glm::vec3(0, 0, -1)))[0][0]);
    CSCI441::drawSolidTorus(0.15, 0.30, 10, 12);
    glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &(glm::translate(modelMtx, glm::vec3(-1, 0, 0)))[0][0]);
    CSCI441::drawSolidTorus(0.15, 0.30, 10, 12);
    glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &(glm::translate(modelMtx, glm::vec3(-1, 0, -1)))[0][0]);
    CSCI441::drawSolidTorus(0.15, 0.30, 10, 12);
}
void drawOreKart(glm::mat4 modelMtx, GLint uniform_modelMtx_loc, GLint uniform_color_loc ) {
    glBindTexture( GL_TEXTURE_2D, OKTextureHandle);
    modelMtx = glm::translate( modelMtx, OKlocation );
    modelMtx = glm::translate( modelMtx, glm::vec3( 0, OKradius, 0 ) );
    modelMtx = glm::rotate( modelMtx, (float)OK_rotation - (float)M_PI_4, glm::vec3(0, 1, 0) );
    glUniformMatrix4fv( uniform_modelMtx_loc, 1, GL_FALSE, &modelMtx[0][0] );

    CSCI441::drawSolidCube(1);

    modelMtx = glm::translate(modelMtx, glm::vec3(0.5 * OKradius, -0.5, 0.5 * OKradius));
    glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtx[0][0] );
    drawWheels(modelMtx);
}

glm::vec3 spring(float k, float rest, glm::vec3 source, glm::vec3 dest){
    // Function to calulate spring force
    glm::vec3 dir = dest-source;
    float dist = glm::length(dir);
    if (dist-rest > -OKrest_length)
        return k*(dist-rest)*dir;
    return glm::vec3(0,0,0);
}

void moveOreKart(){
    glm::vec3 heading = marbles[0]->location - OKlocation;
    float mag = OKk*(glm::length(heading) - OKrest_length);
    // Apply force to Orekart equal to spring from Hero
    if (mag > 0){
        OKdirection = OKk*(glm::length(heading) - OKrest_length)*heading;
        OKlocation = OKlocation + OKdirection;
    }

}

void moveRope(){
    // Move the Rope System
    // Set the ends of the rope to the Hero and the OreKart
    glm::vec3 heroLoc = marbles[0]->location;
    ropeVertices[0].x = heroLoc.x;
    ropeVertices[0].y = marbles[0]->radius;
    ropeVertices[0].z = heroLoc.z;

    ropeVertices[ropeSize - 1].x = OKlocation.x;
    ropeVertices[ropeSize - 1].z = OKlocation.z;

    glm::vec3 g = glm::vec3(0,ropeMass*-9.81,0); // gravity
    for (int j = 0; j < ropeRender; j++){
        for (int i = 1; i < ropeSize-1; i++){
            // Sum of forces
            VertexTextured r1 = ropeVertices[i-1];
            VertexTextured r2 = ropeVertices[i];
            VertexTextured r3 = ropeVertices[i+1];
            // Force from previous point
            glm::vec3 f1 = spring(ropeK, ropeRest, glm::vec3(r2.x, r2.y, r2.z), glm::vec3(r1.x, r1.y, r1.z));
            // Force from current point
            glm::vec3 f2 = spring(ropeK, ropeRest, glm::vec3(r2.x, r2.y, r2.z), glm::vec3(r3.x, r3.y, r3.z));
            glm::vec3 sumF = g + f1 + f2;
            // If the force is large, reduce it
            if (glm::length(sumF) > ropeTimeScale)
                sumF = glm::normalize(sumF);
            ropeVertices[i].x += sumF.x * ropeTimeScale;
            ropeVertices[i].y += sumF.y * ropeTimeScale;
            if (ropeVertices[i].y < 0){
                ropeVertices[i].y = 0;
            }
            ropeVertices[i].z += sumF.z * ropeTimeScale;
        }
    }
}

void renderScene( glm::mat4 viewMatrix, glm::mat4 projectionMatrix ) {
    // stores our model matrix
    glm::mat4 modelMtx;
    //glEnable(GL_TEXTURE_2D);                    // enable 2D texturing
    // use our shader program
    // precompute our MVP CPU side so it only needs to be computed once
    glm::mat4 mvpMtx = projectionMatrix * viewMatrix * modelMtx;

    lightingProgramHandle->useProgram();
    glUniformMatrix4fv(mvp_lights_location, 1, GL_FALSE, &mvpMtx[0][0]);
    glUniformMatrix4fv(modelUniLoc, 1, GL_FALSE, &modelMtx[0][0]);
    glUniform3fv(lightPosUniLoc, 1, &lightPos[0]);
    glUniform3fv(lightPos1UniLoc, 1, &lightPos1[0]);
    glUniform3fv(lightPos2UniLoc, 1, &lightPos2[0]);
    glUniform3fv(viewUniformLoc, 1, &eyePoint[0]);
    model->draw(vpos_light_attrib_location, normalAttLoc);

    CSCI441::setVertexAttributeLocations( attrib_vPos_loc, -1, attrib_vTextureCoord_loc );
    // Draw Scenery
    textureShaderProgram->useProgram();

    glm::mat4 m, vp = projectionMatrix * viewMatrix;
    glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &m[0][0]);
    glUniformMatrix4fv(uniform_viewProjetionMtx_loc, 1, GL_FALSE, &vp[0][0]);
    glUniform1ui(uniform_tex_loc, GL_TEXTURE0);

    glm::vec3 white(1,1,1);
    glUniform3fv( uniform_color_loc, 1, &white[0] );

    for( unsigned int i = 0; i < 6; i++ ) {
        glBindTexture( GL_TEXTURE_2D, skyboxHandles[i] );
        glBindVertexArray(skyboxVAOds[i]);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void*)0);
    }
    // Platform
    glBindTexture( GL_TEXTURE_2D, platformTextureHandle );
    glBindVertexArray( platformVAOd );
    glDrawElements( GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void*)0 );

    // Rope
    glBindVertexArray( ropeVAOd );
    glBindBuffer( GL_ARRAY_BUFFER, ropeVbod );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(ropeVertices), ropeVertices);
    glBindTexture( GL_TEXTURE_2D, ropeTextureHandle);
    glDrawArrays( GL_LINE_STRIP, 0, ropeSize);

    // Draw Marbles
    marbleShaderProgram -> useProgram();
    glUniformMatrix4fv(uniform_m_modelMtx_loc, 1, GL_FALSE, &m[0][0]);
    glUniformMatrix4fv(uniform_m_viewProjetionMtx_loc, 1, GL_FALSE, &vp[0][0]);
    glUniform1ui(uniform_m_tex_loc, GL_TEXTURE0);

    for( unsigned int i = 0; i < marbles.size(); i++ ) {
        if (i == 0)
            glBindTexture( GL_TEXTURE_2D, playerTextureHandle);
        else if (i < 4)
            glBindTexture( GL_TEXTURE_2D, beverageTextureHandle );
        else{
            glBindTexture( GL_TEXTURE_2D, enemyTextureHandle );
            //printf("Bricks loaded\n");
        }
        marbles[i]->draw( m, uniform_m_modelMtx_loc, uniform_m_color_loc );
    }
    drawOreKart( m, uniform_modelMtx_loc, uniform_m_color_loc);

    // Trees
    CSCI441::setVertexAttributeLocations( vpos_tree_attrib_location );
    modelMtx = glm::mat4(1.0,0.0,0.0,0.0,
                         0.0,1.0,0.0,0.0,
                         0.0,0.0,1.0,0.0,
                         0.0,0.0,0.0,1.0);

    treeShaderProgram->useProgram();
    glm::mat4 mvMtx = viewMatrix * modelMtx;;
    glUniformMatrix4fv(modelview_tree_uniform_location, 1, GL_FALSE, &mvMtx[0][0]);
    glUniformMatrix4fv(projection_tree_uniform_location, 1, GL_FALSE, &projectionMatrix[0][0]);
    glBindVertexArray( pointsVAO );
    glBindBuffer( GL_ARRAY_BUFFER, pointsVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
    glBindTexture( GL_TEXTURE_2D, treeTextureHandle );
    glDrawArrays( GL_POINTS, 0, NUM_POINTS );


}

glm::vec3 collide(glm::vec3 vec_in, glm::vec3 norm){
    // Given a directino and a normal, provide the angle of incidence
    return vec_in - 2*glm::dot(vec_in, norm)*norm;
}

void moveMarbles() {
    // Move Hero
    if (goingForward == GLFW_PRESS || goingForward == GLFW_REPEAT || marbles[0]->direction.y == -1){
        marbles[0]->moveForward();
    }
    if ((goingBackward == GLFW_PRESS || goingBackward == GLFW_REPEAT) && marbles[0]->direction.y != -1){
        marbles[0]->moveBackward();
    }
    if (turnLeft == GLFW_PRESS || turnLeft == GLFW_REPEAT){
        GLfloat angle = 0.08;
        glm::mat4 rotationMat(1); // Creates a identity matri
        rotationMat = glm::rotate(rotationMat, angle, glm::vec3(0.0, 1.0, 0.0));
        marbles[0]->direction = glm::vec3(rotationMat * glm::vec4(marbles[0]->direction,1));
    }
    if (turnRight == GLFW_PRESS || turnRight == GLFW_REPEAT){
        GLfloat angle = -0.08;
        glm::mat4 rotationMat(1); // Creates a identity matri
        rotationMat = glm::rotate(rotationMat, angle, glm::vec3(0.0, 1.0, 0.0));
        marbles[0]->direction = glm::vec3(rotationMat * glm::vec4(marbles[0]->direction,1));
    }

    // Move Police
    for (int i = 4; i < marbles.size(); i++){
        glm::vec3 cur_dir = glm::normalize(marbles[i]->direction);
        glm::vec3 target_head = glm::normalize(marbles[0]->location - marbles[i]->location);
        float epsilon = 0.01;
        GLfloat angle = 0;
        if (glm::length(cur_dir - target_head) > epsilon){
            angle = glm::acos(glm::dot(cur_dir, target_head));
        }
        angle *= -0.1;
        if (glm::cross(cur_dir, target_head).y > 0) angle *= -1;
        //printf("Angle %f\n",angle);
        glm::mat4 rotationMat(1); // Creates a identity matri
        rotationMat = glm::rotate(rotationMat, angle, glm::vec3(0.0, 1.0, 0.0));
        marbles[i]->direction = speedRatio * glm::vec3(rotationMat*glm::vec4(cur_dir,0));
        marbles[i]->moveForward();
    }

    // Move Beer
    sys_time += 0.03;
    for (int i = 1; i < 4; i++){
        marbles[i]->radius = 0.5 + 0.3*sin(sys_time);
    }
    moveOreKart();
    moveRope();
}

void collideMarblesWithWall() {
    // Player collides with Temple
    for (int i=0; i< numMarbles; i++){
        if  (abs(marbles[i]->location.x) < templeX && abs(marbles[i]->location.z) < templeZ){
            if (0 > marbles[i]->location.z && marbles[i]->location.z > -templeZ){
                marbles[i]->location.z = -templeZ;
                marbles[i]->direction = collide(marbles[i]->direction, glm::vec3(0,0,1));
            }
            if (0 < marbles[i]->location.z && marbles[i]->location.z < templeZ){
                marbles[i]->location.z =  templeZ;
                marbles[i]->direction = collide(marbles[i]->direction, glm::vec3(0,0,-1));
            }
            if (0 > marbles[i]->location.x && marbles[i]->location.x > -templeX){
                marbles[i]->location.x = -templeX;
                marbles[i]->direction = collide(marbles[i]->direction, glm::vec3(1,0,0));
            }
            if (0 < marbles[i]->location.x && marbles[i]->location.x < templeX){
                marbles[i]->location.x =  templeX;
                marbles[i]->direction = collide(marbles[i]->direction, glm::vec3(-1,0,0));
            }
        }
    }

    // Player Goes over edge
    for (int i = 0; i < numMarbles; i++){
        if (marbles[i]->location.z < -groundSize
                || marbles[i]->location.z > groundSize
                || marbles[i]->location.x < -groundSize
                || marbles[i]->location.x > groundSize){
            marbles[i]->direction.y = -1;
        }
    }

    // Game Over
    if (marbles[0]->location.y < -3){
        printf("Game over at level %d\n", numMarbles - 4);
        printf("You survived %f seconds\n", sys_time);
        //printf("Rope Rest: %f\n",ropeRest);
        exit(EXIT_SUCCESS);
    }

}

void collideMarblesWithEachother() {
    // Collide Hero
    for (int i = 1; i < numMarbles; i++){
        float dist = glm::distance(
                                glm::vec3(  marbles[i]->location.x,
                                    marbles[i]->radius + marbles[i]->location.y,
                                    marbles[i]->location.z),
                                glm::vec3(  marbles[0]->location.x,
                                    marbles[0]->radius + marbles[0]->location.y,
                                    marbles[0]->location.z));
        if( dist < marbles[i]->radius + marbles[0]->radius){
            if (i < 4){
                //Collect Beers
                beers++;
                b_distance+=b_dist_inc;
                alpha *= alpha_ratio;
                marbles[i]->location = glm::vec3(   randRange(-groundSize+5, groundSize-5),
                                                    0,
                                                    randRange(-groundSize+5, groundSize-5));

                while (abs(marbles[i]->location.x) < templeX || abs(marbles[i]->location.z) < templeZ){
                    marbles[i]->location = glm::vec3(   randRange(-groundSize+5, groundSize-5),
                                                        0,
                                                        randRange(-groundSize+5, groundSize-5));

                }
                // Spawn officer
                if ( (beers%2 == 0) || numMarbles > 6){
                    glm::vec3 loc = marbles[i]->location;
                    float inside = 0.7;
                    speedRatio += speedIncrease*(1.1 - speedRatio);
                    Marble* m = new Marble( glm::vec3(randRange(-inside*groundSize, inside*groundSize),
                                                        0,
                                                       randRange(-inside*groundSize, inside*groundSize)),
                                            glm::vec3(randRange(-inside*groundSize, inside*groundSize),
                                                        0,
                                                       randRange(-inside*groundSize, inside*groundSize)),
                                            marbleRadius );
                    marbles.push_back( m );
                    numMarbles++;
                    printf("Level %d Unocked at %f %f %f\n", numMarbles - 4, m->location.x, m->location.y, m->location.z);
                }
            }
            else {
                // Death
                marbles[0]->direction.y = -1;
            }
        }
    }

    // Collide Marbles
    for (int i = 4; i < numMarbles-1; i++){
        for (int j = i+1; j < numMarbles; j++){
            if (i != j){
                float dist = glm::distance(
                                glm::vec3(  marbles[i]->location.x,
                                            marbles[i]->radius,
                                            marbles[i]->location.z),
                                glm::vec3(  marbles[j]->location.x,
                                            marbles[j]->radius,
                                            marbles[j]->location.z));
                if( dist < marbles[i]->radius + marbles[j]->radius){
                    marbles[i]->direction = collide(marbles[i]->direction, marbles[i]->location - marbles[j]->location);
                    marbles[i]->location =  marbles[i]->location
                                            + bump*glm::normalize(marbles[i]->location - marbles[j]->location);
                    marbles[j]->direction = collide(marbles[j]->direction, marbles[j]->location - marbles[i]->location);
                    marbles[j]->location =  marbles[j]->location
                                            + bump*glm::normalize(marbles[j]->location - marbles[i]->location);
                }
            }
        }
    }

    //Collide OreKart
    for (int i = 0; i < numMarbles; i++){
        float dist = glm::distance( 
                            glm::vec3(  marbles[i]->location.x,
                                        marbles[i]->radius,
                                        marbles[i]->location.z),
                            glm::vec3(  OKlocation.x,
                                        OKradius,
                                        OKlocation.z));
        if ( dist < marbles[i]->radius + OKradius ){
            marbles[i]->direction = collide(marbles[i]->direction, marbles[i]->location - OKlocation);
            marbles[i]->location =  marbles[i]->location
                + bump*glm::normalize(marbles[i]->location - OKlocation);
        }
    }

}

///*****************************************************************************
//
// Our main function

// main() ///////////////////////////////////////////////////////////////
//
//        Really you should know what this is by now.
//
////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] ) {
    // GLFW sets up our OpenGL context so must be done first
    GLFWwindow *window = setupGLFW();    // initialize all of the GLFW specific information releated to OpenGL and our window
    setupOpenGL();                                        // initialize all of the OpenGL specific information
    setupGLEW();                                            // initialize all of the GLEW specific information
    setupShaders();                                        // load our shaders into memory
    setupBuffers();                                        // load all our VAOs and VBOs into memory
    setupTextures();                                    // load all textures into memory
    setupFramebuffer();
    populateMarbles();                                // generate marbles
    CSCI441::setVertexAttributeLocations(vpos_light_attrib_location);
    convertSphericalToCartesian();        // set up our camera position

    CSCI441::drawSolidSphere( 1, 16, 16 );    // strange hack I need to make spheres draw - don't have time to investigate why..it's a bug with my library
    CSCI441::drawSolidCylinder( 1, 1, 1, 16, 16 );    // strange hack I need to make spheres draw - don't have time to investigate why..it's a bug with my library
    CSCI441::drawSolidTorus( 0.5, 1, 10, 12 );    // strange hack I need to make spheres draw - don't have time to investigate why..it's a bug with my library
    //
    CSCI441::drawSolidCube(1);
    //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
    //    until the user decides to close the window and quit the program.  Without a loop, the
    //    window will display once and then the program exits.
    double last_update  = glfwGetTime();
    while( !glfwWindowShouldClose(window) ) {    // check if the window was instructed to be closed
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, framebufferWidth, framebufferHeight);
        glClear(GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT);    // clear the current color contents and depth buffer in the framebuffer

        // set the projection matrix based on the window size
        // use a perspective projection that ranges
        // with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 1000].
        glm::mat4 projectionMatrix = glm::perspective(45.0f, framebufferWidth / (float)framebufferHeight, 0.001f, 10000.0f);

        //postprocessingShaderProgram->useProgram();
        // set up our look at matrix to position our camera
        // Camera looks at Marble
        lookAtPoint = marbles[0]->location;
        convertSphericalToCartesian();
                glm::mat4 viewMatrix = glm::lookAt(eyePoint, lookAtPoint, upVector);
        // pass our view and projection matrices
        if (glfwGetTime() - last_update > 0.016) {
            last_update = glfwGetTime();
            collideMarblesWithWall();
            collideMarblesWithEachother();
            moveMarbles();
        }
        renderScene(viewMatrix, projectionMatrix);

        glFlush();
        /////////////////////////////
        // SECOND PASS
        /////////////////////////////
        // TODO #3
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Show it all on the screen now!
        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT);    // clear the current color contents and depth buffer in the framebuffer
        postprocessingShaderProgram->useProgram();
        projectionMatrix = glm::ortho(-1, 1, -1, 1);
        glUniformMatrix4fv(uniform_post_proj_loc, 1, GL_FALSE, &projectionMatrix[0][0]);
        sys_time += 0.01;
                glUniform1f(uniform_post_time_loc, sys_time);
                glUniform1f(uniform_post_dist_loc, b_distance);
                glUniform1f(uniform_post_a_loc, alpha);
                glBindTexture(GL_TEXTURE_2D, framebufferTextureHandle);
        glBindVertexArray(texturedQuadVAO);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void *)0);

        // Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
        // when using a Retina display the actual window can be larger than the requested window.  Therefore
        // query what the actual size of the window we are rendering to is.
        glfwGetFramebufferSize( window, &windowWidth, &windowHeight );

        // update the viewport - tell OpenGL we want to render to the whole window
        glViewport( 0, 0, windowWidth, windowHeight );
        glfwSwapBuffers(window);// flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents();                // check for any events and signal to redraw screen

        // THIS IS WHERE THE MAGICAL MAGIC HAPPENS!  Move everything
        if (glfwGetTime() - last_update > 0.016) {
            last_update = glfwGetTime();
            collideMarblesWithWall();
            collideMarblesWithEachother();
            moveMarbles();
            setLights();
        }
        // hack to make the window work on the mac without manually dragging
        if (! movedWindow) {
            movedWindow = true;
            int xpos, ypos;
            glfwGetWindowPos(window, &xpos, &ypos);
            glfwSetWindowPos(window, xpos + 1, ypos);
        }
    }

    glfwDestroyWindow( window );// clean up and close our window
    glfwTerminate();                        // shut down GLFW to clean up our context

    return EXIT_SUCCESS;                // exit our program successfully!
}
