OF_GLSL_SHADER_HEADER

uniform sampler2D u_tex0;
varying vec2 v_texcoord;
uniform vec2 u_resolution;
uniform float u_time;
uniform ivec2 u_canvas;
uniform ivec4 u_tvdata0;
uniform ivec4 u_tvdata1;
uniform ivec4 u_tvdata2;
uniform ivec4 u_tvdata3;

vec4 crop_tv_and_resize_2x2(sampler2D tex, vec2 pos, ivec4 tvdata, int x_number, int y_number){

    float tv_width = float(tvdata[0]) / u_canvas.x;
    float tv_height = float(tvdata[1]) / u_canvas.y;
    float tv_x_pos = float(tvdata[2]) / u_canvas.x;
    float tv_y_pos = float(tvdata[3]) / u_canvas.y;

    if (abs(tv_width) < abs(tv_height)) {
    // Rotate 90 degrees counter-clockwise
        float temp = pos.x;
        pos.x = tv_x_pos + tv_width - 2.0*tv_width*(pos.y - y_number/2.0);
        pos.y = tv_y_pos + 2.0*tv_height*(temp - x_number/2.0);
    }
    else{
        pos.x =  tv_x_pos + 2.0*tv_width*(pos.x - x_number/2.0);
        pos.y = tv_y_pos + 2.0*tv_height*(pos.y - y_number/2.0);
    }


    // pos.x = pos.x * u_resolution.x;
    // pos.y = pos.y * u_resolution.y;

    vec4 texColour = texture2D(tex, pos);

    return texColour;
}

void main() {
    vec4 texColour;
    vec4 tex0;
    vec4 tex1;
    vec4 tex2;
    vec4 tex3;
    vec2 pos;

    pos.x = v_texcoord.x;
    pos.y = v_texcoord.y;

    tex0 = crop_tv_and_resize_2x2(u_tex0, pos, u_tvdata0, 0, 0);
    tex1 = crop_tv_and_resize_2x2(u_tex0, pos, u_tvdata1, 1, 0); 
    tex2 = crop_tv_and_resize_2x2(u_tex0, pos, u_tvdata2, 0, 1); 
    tex3 = crop_tv_and_resize_2x2(u_tex0, pos, u_tvdata3, 1, 1); 

    vec2 coord = v_texcoord; // / u_resolution;

         if(coord.x < 0.5 && coord.y < 0.5){texColour = tex0;}
    else if(coord.x >= 0.5 && coord.y < 0.5){texColour = tex1;}
    else if(coord.x < 0.5 && coord.y >= 0.5){texColour = tex2;}
    else if(coord.x >= 0.5 && coord.y >= 0.5){texColour = tex3;}

    gl_FragColor = texColour;
}
