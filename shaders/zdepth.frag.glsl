uniform float camFarZ;
varying float d;

void main()
{
    float camZ = 0.0;
	float d2 = d / camZ;
	gl_FragColor = vec4(d,d,d,1.0);
}