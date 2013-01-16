uniform sampler2D ShadowMap;

uniform sampler2D topTex0;
uniform sampler2D sideTex0;
uniform sampler2D topTex1;
uniform sampler2D sideTex1;
uniform sampler2D topTex2;
uniform sampler2D sideTex2;
uniform sampler2D topTex3;
uniform sampler2D sideTex3;

varying vec4 vNormal;
varying vec4 vColor;

varying vec4 diffuse,ambient;
varying vec3 normal,lightDir,halfVector;

varying vec4 ShadowCoord;

varying vec2 texCoords;
varying float shadowDist;

void main()
{
    vec4 shadow = vec4(1.0, 0.0, 1.0, 0);

    vec4 ambientColor = vec4(0.25,0.25,0.5,0);
    vec4 sunColor = vec4(1.0,0.75,0.25, 0);

    vec4 shadowCoordinateWdivide = ShadowCoord / ShadowCoord.w ;
    
    shadowCoordinateWdivide.z -= 0.005;

    vec4 shadowTexture = texture2D(ShadowMap, 0.5 * ShadowCoord.st + 0.5);

    float distanceFromLight = texture2D(ShadowMap,0.5 * shadowCoordinateWdivide.st + 0.5).z;
    
    if (ShadowCoord.w > 0.0)
        shadow = distanceFromLight < shadowCoordinateWdivide.z ? ambientColor : sunColor ;
    
    vec4 blend_weights = abs(vNormal);
    blend_weights = (blend_weights) - 0.5f;//0.2679f;
    blend_weights = max(blend_weights, 0);
    blend_weights /= blend_weights.x + blend_weights.y + blend_weights.z;

    vec4 blended_color;
    float tex_scale = 1.0 / 32.0;

    vec2 coord_x = -gl_TexCoord[1].zy * tex_scale;
    vec2 coord_y = gl_TexCoord[1].zx * tex_scale;
    vec2 coord_z = -gl_TexCoord[1].xy * tex_scale;

    vec4 map0_x = texture2D(sideTex0, coord_x);
    vec4 map0_y = texture2D(topTex0, coord_y);
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
                sunColor *
                pow(NdotHV, gl_FrontMaterial.shininess);
    }

    //gl_FragColor = color * blended_color;
    gl_FragColor = shadow * color * blended_color;
    //gl_FragColor = shadowTexture;
    //gl_FragColor = shadow;
}