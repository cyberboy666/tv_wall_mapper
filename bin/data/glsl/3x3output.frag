OF_GLSL_SHADER_HEADER

uniform sampler2D u_tex0;
in vec2 v_texcoord;
uniform vec2 u_resolution;
uniform float u_time;
uniform ivec2 u_canvas;
uniform ivec4 u_tvdata0;
uniform ivec4 u_tvdata1;
uniform ivec4 u_tvdata2;
uniform ivec4 u_tvdata3;
uniform ivec4 u_tvdata4;
uniform ivec4 u_tvdata5;
uniform ivec4 u_tvdata6;
uniform ivec4 u_tvdata7;
uniform ivec4 u_tvdata8;
out vec4 FragColor;

vec4 crop_tv_and_resize_3x3(sampler2D tex, vec2 pos, ivec4 tvdata, int x_number, int y_number){

    float tv_width = float(tvdata[0]) / u_canvas.x;
    float tv_height = float(tvdata[1]) / u_canvas.y;
    float tv_x_pos = float(tvdata[2]) / u_canvas.x;
    float tv_y_pos = float(tvdata[3]) / u_canvas.y;

    // // Check if width is less than height
    if (abs(tv_width) < abs(tv_height)) {
    // Rotate 90 degrees counter-clockwise
        float temp = pos.x;
        pos.x = tv_x_pos + tv_width - 3.0*tv_width*(pos.y - y_number/3.0);
        pos.y = tv_y_pos + 3.0*tv_height*(temp - x_number/3.0);
    }
    else{
        pos.x =  tv_x_pos + 3.0*tv_width*(pos.x - x_number/3.0);
        pos.y = tv_y_pos + 3.0*tv_height*(pos.y - y_number/3.0);
    }

    // pos.x = pos.x * u_resolution.x;
    // pos.y = pos.y * u_resolution.y;

    vec4 texColour = texture(tex, pos);

    return texColour;
}

void main() {
    vec4 texColour;
    vec4 tex0;
    vec4 tex1;
    vec4 tex2;
    vec4 tex3;
    vec4 tex4;
    vec4 tex5;
    vec4 tex6;
    vec4 tex7;
    vec4 tex8;
    vec2 pos;

    pos.x = v_texcoord.x;
    pos.y = v_texcoord.y;

    // tex = crop_tv(u_tex0, pos, u_tvdata0);
    tex0 = crop_tv_and_resize_3x3(u_tex0, pos, u_tvdata0, 0, 0);
    tex1 = crop_tv_and_resize_3x3(u_tex0, pos, u_tvdata1, 1, 0); 
    tex2 = crop_tv_and_resize_3x3(u_tex0, pos, u_tvdata2, 2, 0); 
    tex3 = crop_tv_and_resize_3x3(u_tex0, pos, u_tvdata3, 0, 1); 
    tex4 = crop_tv_and_resize_3x3(u_tex0, pos, u_tvdata4, 1, 1);
    tex5 = crop_tv_and_resize_3x3(u_tex0, pos, u_tvdata5, 2, 1); 
    tex6 = crop_tv_and_resize_3x3(u_tex0, pos, u_tvdata6, 0, 2); 
    tex7 = crop_tv_and_resize_3x3(u_tex0, pos, u_tvdata7, 1, 2);
    tex8 = crop_tv_and_resize_3x3(u_tex0, pos, u_tvdata8, 2, 2); 

    vec2 coord = v_texcoord;

         if(0/3.0 < coord.x && coord.x < 1/3.0 && 0/3.0 < coord.y && coord.y < 1/3.0){texColour = tex0;}
    else if(1/3.0 < coord.x && coord.x < 2/3.0 && 0/3.0 < coord.y && coord.y < 1/3.0){texColour = tex1;}
    else if(2/3.0 < coord.x && coord.x < 3/3.0 && 0/3.0 < coord.y && coord.y < 1/3.0){texColour = tex2;}
    else if(0/3.0 < coord.x && coord.x < 1/3.0 && 1/3.0 < coord.y && coord.y < 2/3.0){texColour = tex3;}
    else if(1/3.0 < coord.x && coord.x < 2/3.0 && 1/3.0 < coord.y && coord.y < 2/3.0){texColour = tex4;}
    else if(2/3.0 < coord.x && coord.x < 3/3.0 && 1/3.0 < coord.y && coord.y < 2/3.0){texColour = tex5;}
    else if(0/3.0 < coord.x && coord.x < 1/3.0 && 2/3.0 < coord.y && coord.y < 3/3.0){texColour = tex6;}
    else if(1/3.0 < coord.x && coord.x < 2/3.0 && 2/3.0 < coord.y && coord.y < 3/3.0){texColour = tex7;}
    else if(2/3.0 < coord.x && coord.x < 3/3.0 && 2/3.0 < coord.y && coord.y < 3/3.0){texColour = tex8;}

    FragColor = texColour;
}
