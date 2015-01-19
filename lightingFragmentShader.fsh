//=============================================================================
// This file is part of Simple3D
//
// (c) Copyright 2014-2015 Borislav Karaivanov. All rights reserved.
//
// The code is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//=============================================================================

uniform highp vec4 ambientColor;
uniform highp vec4 diffuseColor;
uniform highp vec4 specularColor;
uniform highp float ambientReflection;
uniform highp float diffuseReflection;
uniform highp float specularReflection;
uniform highp float shininess;

varying highp vec3 varyingNormal;
varying highp vec3 varyingLightDirection;
varying highp vec3 varyingViewerDirection;

void main(void)
{
    highp vec3 normal = normalize(varyingNormal);
    highp vec3 lightDirection = normalize(varyingLightDirection);
    highp vec3 viewerDirection = normalize(varyingViewerDirection);
    highp vec4 ambientIllumination = ambientReflection * ambientColor;
    highp vec4 diffuseIllumination = diffuseReflection * max(0.0, dot(lightDirection, normal)) * diffuseColor;
    highp vec4 specularIllumination = specularReflection *
            pow(max(0.0, dot(-reflect(lightDirection, normal), viewerDirection)), shininess) * specularColor;
    gl_FragColor = ambientIllumination + diffuseIllumination + specularIllumination;
}
