uniform mat4 mLightViewProj;
varying vec4 ShadowCoord;

void main()
{
	gl_Position = ftransform();
	ShadowCoord = mLightViewProj * gl_Vertex;
}