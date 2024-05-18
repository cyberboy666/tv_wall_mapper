OF_GLSL_SHADER_HEADER

uniform sampler2D u_tex0;
varying vec2 v_texcoord;
uniform vec2 u_resolution;
uniform float u_time;
uniform ivec2 u_canvas;
uniform ivec4 u_tvdata0;

vec4 crop_tv(sampler2D tex, vec2 pos, ivec4 tvdata){

    float tv_width = float(tvdata[0]) / u_canvas.x;
    float tv_height = float(tvdata[1]) / u_canvas.y;
    float tv_x_pos = float(tvdata[2]) / u_canvas.x;
    float tv_y_pos = float(tvdata[3]) / u_canvas.y;

    if (abs(tv_width) < abs(tv_height)) {
    // Rotate 90 degrees counter-clockwise
        float temp = pos.x;
        pos.x = tv_x_pos + tv_width - tv_width*pos.y;
        pos.y = tv_y_pos + tv_height*temp;
    }
    else{
        pos.x =  tv_x_pos + tv_width*pos.x;
        pos.y = tv_y_pos + tv_height*pos.y;
    }

    // pos.x = pos.x * u_resolution.x;
    // pos.y = pos.y * u_resolution.y;

    return texture2D(tex, pos);
}

void main() {
    vec4 texColour;
    vec2 pos;

    pos.x = v_texcoord.x;
    pos.y = v_texcoord.y;

    texColour = crop_tv(u_tex0, pos, u_tvdata0);

    gl_FragColor = texColour;
}
