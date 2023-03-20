#type vertex
#version 450 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;
layout(location = 5) in int a_EntityID;
// Animation
layout(location = 6) in ivec4 a_BoneIds; 
layout(location = 7) in vec4 a_BoneWeights;

uniform bool u_Animated;

struct VertexOutput
{
	vec3 Normal;
	vec3 WorldPos;
	vec2 TexCoord;
};
layout (location = 0) out VertexOutput Output;
layout (location = 3) out flat int v_EntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

uniform mat4 model;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
    Output.TexCoord = a_TexCoord;

    if (u_Animated)
    {
        mat4 boneTransform = finalBonesMatrices[a_BoneIds[0]] * a_BoneWeights[0];
        boneTransform += finalBonesMatrices[a_BoneIds[1]] * a_BoneWeights[1];
        boneTransform += finalBonesMatrices[a_BoneIds[2]] * a_BoneWeights[2];
        boneTransform += finalBonesMatrices[a_BoneIds[3]] * a_BoneWeights[3];

        vec4 localPosition = boneTransform * vec4(a_Pos, 1.0);

        vec4 WorldPos = model * localPosition;
	    Output.WorldPos = WorldPos.xyz;
	    Output.Normal = mat3(boneTransform) * a_Normal;
	    v_EntityID = a_EntityID;
	    gl_Position = u_ViewProjection * WorldPos;
    }
    else
    {
        Output.WorldPos = vec3(model * vec4(a_Pos, 1.0));
        Output.Normal = mat3(model) * a_Normal;
        v_EntityID = a_EntityID;

        gl_Position =  u_ViewProjection * vec4(Output.WorldPos, 1.0);
    }
}

#type fragment
#version 450 core

out vec4 FragColor;
out int color2;

struct VertexOutput
{
	vec3 Normal;
	vec3 WorldPos;
	vec2 TexCoord;
};

layout (location = 0) in VertexOutput Input;
layout (location = 3) in flat int v_EntityID;

// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

// Point lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform vec3 camPos;

// Directional light
uniform vec3 lightDir;
uniform float dirLightIntensity;
layout (std140, binding = 1) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};


const float F0_NON_METAL = 0.04f;
const float PI = 3.14159265359;

// --------------------------PBR Function--------------------------------------

// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, Input.TexCoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(Input.WorldPos);
    vec3 Q2  = dFdy(Input.WorldPos);
    vec2 st1 = dFdx(Input.TexCoord);
    vec2 st2 = dFdy(Input.TexCoord);

    vec3 N   = normalize(Input.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   
// ----------------------------------------------------------------------------

// Lambert diffuse
vec3 LambertDiffuse(vec3 Ks, vec3 albedo, float metallic)
{
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    vec3 Kd = (vec3(1.0f, 1.0f, 1.0f) - Ks) * (1 - metallic);
    return (Kd * albedo / PI);
}

// Cook-Torrance Specular
vec3 CookTorrance(vec3 n, vec3 l, vec3 v, float roughness, float metalness, vec3 f0, out vec3 kS)
{
    vec3 h = normalize(v + l);

    float D = DistributionGGX(n, h, roughness);
    float G = GeometrySmith(n, v, l, roughness);
    vec3 F  = fresnelSchlick(max(dot(h, v), 0.0), f0);

    // kS is equal to Fresnel  
    kS = F;

    float NdotV = max(dot(n, v), 0.0f);
    float NdotL = max(dot(n, l), 0.0f);
    return (D * G * F) / (4.0 * max(NdotV * NdotL, 0.01f));
}

// --------------------------End PBR Function----------------------------------

void main()
{		
    // material properties
    //反射率(albedo)纹理在美术人员创建的时候就已经在sRGB空间了，因此我们需要在光照计算之前先把他们转换到线性空间。
    //一般来说，环境光遮蔽贴图(ambient occlusion maps)也需要我们转换到线性空间。不过金属性(Metallic)和粗糙度(Roughness)贴图大多数时间都会保证在线性空间中。
    vec3 albedo = pow(texture(albedoMap, Input.TexCoord).rgb, vec3(2.2));
    float metallic = texture(metallicMap, Input.TexCoord).r;
    float roughness = texture(roughnessMap, Input.TexCoord).r;
    float ao = texture(aoMap, Input.TexCoord).r;
       
    // input lighting data
    vec3 N = getNormalFromMap();
    vec3 V = normalize(camPos - Input.WorldPos);
    vec3 R = reflect(-V, N); 

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = mix(vec3(F0_NON_METAL), albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);

    // Point Light PBR
    for(int i = 0; i < 4; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPositions[i] - Input.WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - Input.WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;
        
        vec3 kS;
        vec3 specularBRDF = CookTorrance(N, L, V, roughness, metallic, F0, kS);

        // kS is equal to Fresnel  
        vec3 diffuseBRDF = LambertDiffuse(kS, albedo, metallic);
            
        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (diffuseBRDF + specularBRDF) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   

    // Directional Light PBR
    {
        vec3 L = lightDir;

        vec3 kS;
        vec3 specularBRDF = CookTorrance(N, L, V, roughness, metallic, F0, kS);

        // kS is equal to Fresnel  
        vec3 diffuseBRDF = LambertDiffuse(kS, albedo, metallic);
            
        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);   

        vec3 radiance = vec3(10.0) * dirLightIntensity;

        Lo += (diffuseBRDF + specularBRDF) * radiance * NdotL;
    }
    
    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedo;
    
    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;

    // gamma correct
    vec3 color = ambient + Lo;
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color , 1.0);

    color2 = v_EntityID;
}