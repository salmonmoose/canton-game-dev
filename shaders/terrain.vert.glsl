varying vec3 vNormal;
varying vec4 vColor;

varying vec4 ShadowCoord;

uniform mat4 mWorldViewProj;
uniform mat4 mLightViewProj;

uniform vec3 mLightPos;

varying vec4 vTexCoord;

varying vec4 diffuse,ambient;
varying vec3 normal,lightDir,halfVector;

varying float shadowDist;

void main() {

	ShadowCoord = mLightViewProj * gl_Vertex;

    vec4 shadowPos=mLightViewProj * gl_Vertex/256.0;
    shadowDist = shadowPos.z/2.0;

    normal = normalize(gl_NormalMatrix * gl_Normal);
    
    lightDir = normalize(gl_LightSource[0].position.xyz);
    halfVector = normalize(gl_LightSource[0].halfVector.xyz);

    diffuse = gl_FrontMaterial.diffuse;
    ambient = gl_FrontMaterial.ambient;
    ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;

    vNormal = gl_Normal;
    
    gl_TexCoord[0].xy = gl_MultiTexCoord1.xy;
    gl_TexCoord[1].x = gl_MultiTexCoord1.z;
    gl_TexCoord[1].y = gl_MultiTexCoord2.z;
    gl_TexCoord[2].xy = gl_MultiTexCoord2.xy;

    gl_TexCoord[3] = gl_MultiTexCoord0;

    vTexCoord = gl_Vertex;

    vColor = gl_Color;

    gl_Position = ftransform();
}