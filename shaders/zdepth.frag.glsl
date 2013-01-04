uniform float camFarZ;
varying float d;

void main()
{
    float camZ = 128.0;
	float d2 = d / camZ;
	gl_FragColor = vec4(d2,d2,d2,1.0);
}