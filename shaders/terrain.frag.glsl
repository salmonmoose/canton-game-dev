uniform sampler2D ShadowMap;

uniform sampler2D topTex0;
uniform sampler2D sideTex0;
uniform sampler2D topTex1;
uniform sampler2D sideTex1;
uniform sampler2D topTex2;
uniform sampler2D sideTex2;
uniform sampler2D topTex3;
uniform sampler2D sideTex3;

uniform vec4 mLightColor;

varying vec3 vNormal;
varying vec4 vColor;

varying vec4 diffuse,ambient;
varying vec3 normal,lightDir,halfVector;

varying vec4 ShadowCoord;

varying float shadowDist;

void main()
{
    vec4 shadow = vec4(1.0, 0.0, 1.0, 0);

    vec4 ambientColor = vec4(0.5,0.5,0.75,0);
    
    /**
    * Lights
    */
    vec3 n,halfV;
    float NdotL,NdotHV;

    vec4 color = ambientColor;
    n = normalize(vNormal);

    NdotL = max(dot(lightDir,-vNormal),0.0);

    if (NdotL > 0.0) {
        color += NdotL;
        NdotHV = max(dot(vNormal, halfVector), 0.0);
        color += gl_FrontMaterial.specular *
            mLightColor *
            pow(NdotHV, gl_FrontMaterial.shininess);
    }

    vec4 shadowCoordinateWdivide = ShadowCoord / ShadowCoord.w ;
    
    shadowCoordinateWdivide.z -= 0.005;

    vec4 shadowTexture = texture2D(ShadowMap, 0.5 * ShadowCoord.st + 0.5);

    float distanceFromLight = texture2D(ShadowMap,0.5 * shadowCoordinateWdivide.st + 0.5).z;
    
    if (ShadowCoord.w > 0.0)
    {
        float offset = (distanceFromLight - shadowCoordinateWdivide.z);
        //shadow = mix(ambientColor, mLightColor, offset);
        shadow = distanceFromLight < shadowCoordinateWdivide.z ? color : mLightColor ;
    }   
    
    vec3 blend_weights = abs(vNormal);
    blend_weights = (blend_weights) - 0.5f;//0.2679f;
    blend_weights = max(blend_weights, 0);
    blend_weights /= blend_weights.x + blend_weights.y + blend_weights.z;

    vec4 blended_color;

    vec2 coord_x_r = gl_TexCoord[3].zy + gl_TexCoord[0].xy;
    vec2 coord_y_r = gl_TexCoord[3].zx + gl_TexCoord[0].xy;
    vec2 coord_z_r = gl_TexCoord[3].xy + gl_TexCoord[0].xy;

    vec2 coord_x_g = gl_TexCoord[3].zy + gl_TexCoord[1].xy;
    vec2 coord_y_g = gl_TexCoord[3].zx + gl_TexCoord[1].xy;
    vec2 coord_z_g = gl_TexCoord[3].xy + gl_TexCoord[1].xy;

    vec2 coord_x_b = gl_TexCoord[3].zy + gl_TexCoord[2].xy;
    vec2 coord_y_b = gl_TexCoord[3].zx + gl_TexCoord[2].xy;
    vec2 coord_z_b = gl_TexCoord[3].xy + gl_TexCoord[2].xy;

    vec4 map_r_x = texture2D(sideTex0, coord_x_r);
    vec4 map_r_y = texture2D(topTex0,  coord_y_r);
    vec4 map_r_z = texture2D(sideTex0, coord_z_r);

    vec4 map_g_x = texture2D(sideTex0, coord_x_g);
    vec4 map_g_y = texture2D(topTex0,  coord_y_g);
    vec4 map_g_z = texture2D(sideTex0, coord_z_g);

    vec4 map_b_x = texture2D(sideTex0, coord_x_b);
    vec4 map_b_y = texture2D(topTex0,  coord_y_b);
    vec4 map_b_z = texture2D(sideTex0, coord_z_b);

    vec4 blended_color_r = 
        map_r_x.xyzw * blend_weights.xxxx +
        map_r_y.xyzw * blend_weights.yyyy +
        map_r_z.xyzw * blend_weights.zzzz;

    vec4 blended_color_g = 
        map_g_x.xyzw * blend_weights.xxxx +
        map_g_y.xyzw * blend_weights.yyyy +
        map_g_z.xyzw * blend_weights.zzzz;

    vec4 blended_color_b =  
        map_b_x.xyzw * blend_weights.xxxx +
        map_b_y.xyzw * blend_weights.yyyy +
        map_b_z.xyzw * blend_weights.zzzz;

    gl_FragColor = shadow * color * blended_color_r;
}