uniform sampler2D ShadowMap;
varying vec4 ShadowCoord;

void main()
{
	vec4 Light = vec4(1,1,1,1);
	vec2 smTCoords = ShadowCoord.xy / ShadowCoord.w / 2.0 + vec2(0.5,0.5);
	vec2 clampedSMPos = clamp(smTCoords, vec2(0.0,0.0), vec2(1.0,1.0));
	if(clampedSMPos.x == smTCoords.x && clampedSMPos.y == smTCoords.y)
	{
		Light = texture2D(ShadowMap, smTCoords);
	}

	gl_FragColor = Light;
}