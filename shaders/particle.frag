uniform sampler2D particleTex;

void main()
{
    gl_FragColor = texture2D(particleTex, vec2(gl_TexCoord[0]));
}