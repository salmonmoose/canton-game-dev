varying vec4 vNormal;
varying vec4 vColor;

varying vec4 ShadowCoord;

uniform mat4 mLightViewProj;

varying vec4 diffuse,ambient;
varying vec3 normal,lightDir,halfVector;

void main() {

	ShadowCoord = mLightViewProj * gl_Vertex;

    normal = normalize(gl_NormalMatrix * gl_Normal);
    lightDir = normalize(vec3(gl_LightSource[0].position));

    halfVector = normalize(gl_LightSource[0].halfVector.xyz);

    diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
    ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
    ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;

    vNormal.xyz = gl_Normal;
    gl_TexCoord[1] = gl_Vertex;
    vColor = gl_Color;

    gl_Position = ftransform();
}