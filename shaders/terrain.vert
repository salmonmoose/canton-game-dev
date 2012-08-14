varying vec4 vNormal;

void main(void)
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    vNormal.xyz = gl_Normal;
    gl_TexCoord[1] = gl_Vertex;
}


/* Old shader for reference
void vshader(
	in float4	vtx_position : POSITION,
	in float3	vtx_normal : NORMAL,
	in float4   vtx_color : COLOR,

	uniform float4x4 tps_projection,
	uniform float4x4 mat_modelproj,

	out float4 	l_position : POSITION,
	out float4	l_texcoord0 : TEXCOORD0,
	out float3	l_texcoord1 : TEXCOORD1,
	out float4  l_color : COLOR
)
{   
	l_position = mul(mat_modelproj, vtx_position);
	l_texcoord1 = vtx_normal;
	l_texcoord0 = mul(tps_projection, vtx_position);
	l_color = vtx_color;
}
*/