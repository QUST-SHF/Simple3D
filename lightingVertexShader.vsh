//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

uniform highp mat4 mvpMatrix;
uniform highp mat4 mvMatrix;
uniform highp mat3 normalMatrix;
uniform highp vec3 lightPosition;
attribute highp vec4 vertex;
attribute highp vec3 normal;

varying highp vec3 varyingNormal;
varying highp vec3 varyingLightDirection;
varying highp vec3 varyingViewerDirection;

void main(void)
{
    // Compute the (homogeneous) coordinates of the vertices in the camera
    // space, and normalize the last coordinate to 1, unless the point is at
    // infinity.
    highp vec4 eyeVertex = mvMatrix * vertex;
    if (eyeVertex.w != 0.0f)
        eyeVertex /= eyeVertex.w;

    // Compute the coordinates of the normals in the camera space.
    varyingNormal = normalMatrix * normal;

    // Compute the vectors from the vertices to the light in the camera space.
    varyingLightDirection = lightPosition - eyeVertex.xyz;
    // Compute the vectors from the vertices to the camera in the camera space.
    varyingViewerDirection = -eyeVertex.xyz;

    // Compute the (homogeneous) coordinates of the vertices in screen space.
    gl_Position = mvpMatrix * vertex;
}
