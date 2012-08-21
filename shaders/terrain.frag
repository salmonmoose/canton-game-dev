uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;
uniform sampler2D tex5;

varying vec4 vNormal;
varying vec4 vColor;
void main(void)
{
	vec4 blend_weights = abs(vNormal);
	blend_weights = (blend_weights - 0.3) * 9.0;
	blend_weights = max(blend_weights, 0);
	blend_weights/= blend_weights.x + blend_weights.y + blend_weights.z;

	vec4 blended_color;
	float tex_scale = 1.0/8.0;

	vec2 coord1 = gl_TexCoord[1].yz * tex_scale;
	vec2 coord2 = gl_TexCoord[1].zx * tex_scale;
	vec2 coord3 = gl_TexCoord[1].xy * tex_scale;

	vec4 dirt_x = texture2D(tex0, coord1);
	vec4 dirt_y = texture2D(tex2, coord2);
	vec4 dirt_z = texture2D(tex0, coord3);

	blended_color = 
		dirt_x.xyzw * blend_weights.xxxx +
		dirt_y.xyzw * blend_weights.yyyy +
		dirt_z.xyzw * blend_weights.zzzz;

    gl_FragColor = blended_color;
}


/* Old shader for reference
void fshader(
	in float3 l_texcoord0 : TEXCOORD0, //normals
	in float4 l_texcoord1 : TEXCOORD1,
	in float4 l_color : COLOR,

	uniform sampler2D tex_0,
	uniform sampler2D tex_1,
	uniform sampler2D tex_2,
	uniform sampler2D tex_3,
	uniform sampler2D tex_4,

	uniform float4x4 dlight_sun_rel_view,

	out float4 o_color : COLOR
)
{ 
	float4 blend_weights = abs(l_texcoord1);
	blend_weights = (blend_weights - 0.3) * 9;
	blend_weights = max(blend_weights, 0);
	blend_weights /= (blend_weights.x + blend_weights.y + blend_weights.z).xxxx;

	float4 blended_color;
	float tex_scale = 1.0/8.0;
	
	float2 coord1 = l_texcoord0.yz * tex_scale;
	float2 coord2 = l_texcoord0.zx * tex_scale;
	float2 coord3 = l_texcoord0.xy * tex_scale;

	float4 rock_x = tex2D(tex_0, coord1);
	float4 rock_y = tex2D(tex_0, coord2);
	float4 rock_z = tex2D(tex_0, coord3);

	float4 dirt_x = tex2D(tex_1, coord1);
	float4 dirt_y = tex2D(tex_1, coord2);
	float4 dirt_z = tex2D(tex_1, coord3);

	float4 clay_x = tex2D(tex_2, coord1);
	float4 clay_y = tex2D(tex_2, coord2);
	float4 clay_z = tex2D(tex_2, coord3);

	float4 sand_x = tex2D(tex_3, coord1);
	float4 sand_y = tex2D(tex_3, coord2);
	float4 sand_z = tex2D(tex_3, coord3);

	float4 grass_x = tex2D(tex_4, coord1);
	float4 grass_y = tex2D(tex_4, coord2);
	float4 grass_z = tex2D(tex_4, coord3);

	float4 triplanar_rock = 
		rock_x.xyzw * blend_weights.xxxx +
		rock_y.xyzw * blend_weights.yyyy +
		rock_z.xyzw * blend_weights.zzzz;

	float4 triplanar_dirt =
		dirt_x.xyzw * blend_weights.xxxx +
		dirt_y.xyzw * blend_weights.yyyy +
		grass_z.xyzw * blend_weights.zzzz;

	float4 triplanar_clay =
		clay_x.xyzw * blend_weights.xxxx +
		clay_y.xyzw * blend_weights.yyyy +
		clay_z.xyzw * blend_weights.zzzz;

	float4 triplanar_sand =
		sand_x.xyzw * blend_weights.xxxx +
		sand_y.xyzw * blend_weights.yyyy +
		sand_z.xyzw * blend_weights.zzzz;

	blended_color =
		triplanar_rock * l_color.x +
		triplanar_dirt * l_color.y +
		triplanar_clay * l_color.z +
		triplanar_sand * l_color.w;

	//This code, deals with lights
	float4 lcolor, lspec, lvec, lpoint, latten, ldir, leye, lhalf;

	float4 tot_ambient  = float4(0,0,0,0);
	float4 tot_diffuse  = float4(0,0,0,0);
	float4 tot_specular = float4(0,0,0,0);
	float4 result       = float4(0,0,0,0);

	lcolor = dlight_sun_rel_view[0];
	lspec  = dlight_sun_rel_view[1];
	lvec   = dlight_sun_rel_view[2];

	lcolor *= saturate(dot(l_texcoord1, lvec.xyz));
	tot_diffuse += lcolor;
	lhalf = dlight_sun_rel_view[3];
	lspec *= pow(saturate(dot(l_texcoord1, lhalf.xyz)), 0);
	tot_specular += lspec;

	result += tot_ambient;
	result += tot_diffuse * lcolor;
	result = saturate(result);

	result.rgb = result.rgb + tot_specular.rgb;


	o_color = result * blended_color;// * blend_weights;
}
*/