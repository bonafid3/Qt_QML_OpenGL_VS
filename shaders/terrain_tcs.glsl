#version 430

layout (vertices = 4) out;

uniform mat4 qt_modelViewMatrix;
uniform mat4 qt_projectionMatrix;

uniform vec3 qt_eyeVector;

in vec2 mapCoords_TCS[];
in vec4 shadowCoords_TCS[];
in vec4 worldPos_TCS[];

out vec2 mapCoords_TES[];
out vec4 shadowCoords_TES[];
out vec4 worldPos_TES[];

float lodFactor(float dist) {
    return clamp(600/pow(dist, 1.6)-0.1, 0, 1);
}

void main()
{
    if (gl_InvocationID == 0) {

	const float maxTessLevel = 32;

        vec3 cameraPosition = vec3(qt_modelViewMatrix * vec4(qt_eyeVector, 1));

        vec3 abMid = vec3(gl_in[3].gl_Position + gl_in[0].gl_Position)/2.0;
        vec3 bcMid = vec3(gl_in[0].gl_Position + gl_in[1].gl_Position)/2.0;
        vec3 cdMid = vec3(gl_in[1].gl_Position + gl_in[2].gl_Position)/2.0;
        vec3 daMid = vec3(gl_in[2].gl_Position + gl_in[3].gl_Position)/2.0;

        float distanceAB = distance(abMid, cameraPosition);
        float distanceBC = distance(bcMid, cameraPosition);
        float distanceCD = distance(cdMid, cameraPosition);
        float distanceDA = distance(daMid, cameraPosition);

        gl_TessLevelOuter[0] = mix(1, maxTessLevel, lodFactor(distanceAB));
        gl_TessLevelOuter[1] = mix(1, maxTessLevel, lodFactor(distanceBC));
        gl_TessLevelOuter[2] = mix(1, maxTessLevel, lodFactor(distanceCD));
        gl_TessLevelOuter[3] = mix(1, maxTessLevel, lodFactor(distanceDA));

        gl_TessLevelInner[0] = gl_TessLevelOuter[0] + gl_TessLevelOuter[2] / 4;
        gl_TessLevelInner[1] = gl_TessLevelOuter[1] + gl_TessLevelOuter[3] / 4;
    }

    mapCoords_TES[gl_InvocationID] = mapCoords_TCS[gl_InvocationID];
    shadowCoords_TES[gl_InvocationID] = shadowCoords_TCS[gl_InvocationID];
    worldPos_TES[gl_InvocationID] = worldPos_TCS[gl_InvocationID];

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
