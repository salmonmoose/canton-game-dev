uniform mat4 mLightViewProj;
varying float d;

void main()
{
	vec4 tPos = mLightViewProj * gl_Vertex;
	d = tPos.z;

	gl_Position = ftransform();
}