#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <glm/vec3.hpp>
#include <GL3/gl3.h>

#include <cmath>
#include <iostream>

enum RenderModes
{
    Standard = 0,
    Wireframe = 1,
    Points = 2
};


enum CullingModes
{
    NoCulling = 0,
    BackfaceCulling = -1,
    FrontFaceCulling = 1
};

enum LightingModes
{
    NoShader,
    Gouraud_AD,
    Gouraud_ADS,
    Phong
};

enum MovementOptions
{
    NoMovement = 0,
    Up = 1,
    Down = 2,
    Left = 3,
    Right = 4,
    Forward = 5,
    Backwards = 6
};

enum ResamplingModes
{
    NearestNeighbor = 0,
    Bilinear = 1,
    Trilinear = 2
};

enum Engines
{
    OpenGL,
    Close2GL
};

typedef struct
{
    int renderMode = Standard;
    int orientation = GL_CW;
    int cullingMode = BackfaceCulling;
    int engine = OpenGL;
    int lightingMode = NoShader;
    int resamplingMode = NearestNeighbor;

    bool lightOn = true;
    bool textureOn = false;

    glm::vec3 modelDiffuseColor;
    float diffuseIntensity = 1.0f;
    glm::vec3 modelAmbientColor;
    float ambientIntensity = 0.2f;
    glm::vec3 modelSpecularColor;
    float specularIntensity = 0.2f;
    float modelShineCoefficient;
    glm::vec3 backgroundColor = glm::vec3(0.0f);

    bool resetCamera = false;
    bool keepLookingAtModel = false;

    float rotationPitch = 0.0f;
    float rotationYaw = 0.0f;
    float rotationRoll = 0.0f;

    bool shouldMove = false;
    float speed = 0.1f;
    MovementOptions movementDirection = NoMovement;

    float horizontalFieldOfView = M_PI / 2.0f;
    float verticalFieldOfView = M_PI / 2.0f;
    float nearPlane = 0.1f;
    float farPlane = 10000.0f;

    bool reloadFile = false;
    std::string modelFilePath;

} Options;

#endif
