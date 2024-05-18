OF_GLSL_SHADER_HEADER

// these are for the programmable pipeline system
uniform mat4 modelViewProjectionMatrix;

in vec4 position;
in vec2 texcoord;

out vec2 v_texcoord;

void main()
{
    v_texcoord = texcoord;
    
	gl_Position = modelViewProjectionMatrix * position;
}