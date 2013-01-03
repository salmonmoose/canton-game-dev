uniform sampler2D topTex0;
uniform sampler2D sideTex0;
uniform sampler2D topTex1;
uniform sampler2D sideTex1;
uniform sampler2D topTex2;
uniform sampler2D sideTex2;
uniform sampler2D topTex3;
uniform sampler2D sideTex3;

uniform sampler2D ShadowMap;

varying vec4 vNormal;
varying vec4 vColor;

varying vec4 diffuse,ambient;
varying vec3 normal,lightDir,halfVector;

varying vec4 ShadowCoord;

void main()
{
	vec4 shadowCoordinateWDivide = ShadowCoord / ShadowCoord.w;

	//shadowCoordinateWDivide.z += 0.0005;

    vec4 map_shadow = texture2D(topTex0, shadowCoordinateWDivide.st);

	float distanceFromLight = map_shadow.z;

	float shadow = 1.0;

	if (ShadowCoord.w > 0.0)
		shadow = distanceFromLight < shadowCoordinateWDivide.z ? 0.5 : 1.0;

    vec4 blend_weights = abs(vNormal);
    blend_weights = (blend_weights) - 0.5f;//0.2679f;
    blend_weights = max(blend_weights, 0);
    blend_weights /= blend_weights.x + blend_weights.y + blend_weights.z;

    vec4 blended_color;
    float tex_scale = 1.0 / 16.0;

    vec2 coord_x = -gl_TexCoord[1].zy * tex_scale;
    vec2 coord_y = gl_TexCoord[1].zx * tex_scale;
    vec2 coord_z = -gl_TexCoord[1].xy * tex_scale;

    vec4 map0_x = texture2D(sideTex0, coord_x);
    vec4 map0_y = texture2D(ShadowMap, coord_y);
    vec4 map0_z = texture2D(sideTex0, coord_z);

    vec4 map1_x = texture2D(sideTex1, coord_x);
    vec4 map1_y = texture2D(topTex1, coord_y);
    vec4 map1_z = texture2D(sideTex1, coord_z);

    vec4 map2_x = texture2D(sideTex2, coord_x);
    vec4 map2_y = texture2D(topTex2, coord_y);
    vec4 map2_z = texture2D(sideTex2, coord_z);

    vec4 map3_x = texture2D(sideTex3, coord_x);
    vec4 map3_y = texture2D(topTex3, coord_y);
    vec4 map3_z = texture2D(sideTex3, coord_z);

    blended_color = 
        map0_x.xyzw * blend_weights.xxxx +
        map0_y.xyzw * blend_weights.yyyy +
        map0_z.xyzw * blend_weights.zzzz;

    /**
    * Lights
    */
    vec3 n,halfV;
    float NdotL,NdotHV;

    vec4 color = ambient;
    n = normalize(normal);

    NdotL = max(dot(n,lightDir),0.0);

    if (NdotL > 0.0) {
        color += NdotL;
        //halfV = normalize(halfVector);
        NdotHV = max(dot(n,halfV),0.0);
        color += gl_FrontMaterial.specular *
                gl_LightSource[0].specular *
                pow(NdotHV, gl_FrontMaterial.shininess);
    }


    gl_FragColor = shadow * color * blended_color;
    //gl_FragColor = map_shadow;
}