#type vertex
#version 450 core

layout (location = 0) in vec3 a_Pos;

// Animation
layout (location = 6) in ivec4 a_BoneIds; 
layout (location = 7) in vec4 a_BoneWeights;

uniform mat4 model;

uniform bool u_Animated;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
if (u_Animated)
    {
	    mat4 boneTransform = finalBonesMatrices[a_BoneIds[0]] * a_BoneWeights[0];
        boneTransform += finalBonesMatrices[a_BoneIds[1]] * a_BoneWeights[1];
        boneTransform += finalBonesMatrices[a_BoneIds[2]] * a_BoneWeights[2];
        boneTransform += finalBonesMatrices[a_BoneIds[3]] * a_BoneWeights[3];

        vec4 localPosition = boneTransform * vec4(a_Pos, 1.0);

        gl_Position = model * localPosition;
	}
    else
    {
        gl_Position = model * vec4(a_Pos, 1.0);
    }
}

#type geometry
#version 450 core

layout(triangles, invocations = 5) in;
layout(triangle_strip, max_vertices = 3) out;

layout (std140, binding = 1) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};

void main()
{          
    // CSM深度图在生成的时候需要一个几何着色器来变换层数，和将点转换到对应的包围盒空间下
	for (int i = 0; i < 3; ++i)
	{ 
        // invocations表示迭代次数，gl_Layer表示GL_TEXTURE_2D_ARRAY的层数，gl_InvocationID表示当前在第几次迭代
		gl_Position = lightSpaceMatrices[gl_InvocationID] * gl_in[i].gl_Position;
		gl_Layer = gl_InvocationID;
		EmitVertex();
	}
	EndPrimitive();
}  

#type fragment
#version 450 core

void main()
{             
}