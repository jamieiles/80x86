typedef enum bit [1:0] {
    VIDEO_MODE_TEXT,
    VIDEO_MODE_4_COLOR,
    VIDEO_MODE_256_COLOR
} VideoMode_t;

function VideoMode_t get_video_mode;
    input logic graphics_enabled;
    input logic vga_256_color;

    if (vga_256_color)
        get_video_mode = VIDEO_MODE_256_COLOR;
    else if (graphics_enabled)
        get_video_mode = VIDEO_MODE_4_COLOR;
    else
        get_video_mode = VIDEO_MODE_TEXT;
endfunction
