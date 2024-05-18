OF_GLSL_SHADER_HEADER

varying vec2 v_texcoord;

void main()
{
    v_texcoord = gl_MultiTexCoord0.xy;
	gl_Position = ftransform();
}