module VGAController(input logic clk,
                     input logic sys_clk,
		     input logic reset,
                     // CPU port
                     input logic cs,
                     input logic data_m_access,
                     output logic data_m_ack,
                     input logic [19:1] data_m_addr,
                     input logic data_m_wr_en,
                     input logic [15:0] data_m_data_in,
                     output logic [15:0] data_m_data_out,
                     input logic [1:0] data_m_bytesel,
                     // VGA signals
		     output logic vga_hsync,
		     output logic vga_vsync,
		     output logic [3:0] vga_r,
		     output logic [3:0] vga_g,
		     output logic [3:0] vga_b,
                     input logic cursor_enabled,
                     input logic [14:0] cursor_pos,
                     input logic [2:0] cursor_scan_start,
                     input logic [2:0] cursor_scan_end);

wire hsync;
wire vsync;
wire is_blank;
wire [9:0] row;
wire [9:0] col;
logic fdata;

// Putting a 640x400 display into a 640x480 screen.  Black bars of 40 pixels
// at the top and bottom.
wire [9:0] shifted_row = row - 10'd40;
wire is_border = row < 10'd40 || row >= 10'd440;

wire [3:0] fb_background;
wire [3:0] fb_foreground;
wire [7:0] fb_glyph;
logic [2:0] fb_fcl_col;
logic [2:0] fb_fcl_row;
wire render_cursor;

logic [2:0] vsync_pipe;
assign vga_vsync = vsync_pipe[0];
logic [2:0] hsync_pipe;
assign vga_hsync = hsync_pipe[0];

// 2 vertical pixels per horizontal pixel to scale out.
wire [10:0] frame_buffer_address = (shifted_row / 10'd16) * 10'd80 + (col / 10'd8);

VGASync VGASync(.*);

FrameBuffer FrameBuffer(.address(frame_buffer_address),
                        .glyph(fb_glyph),
                        .background(fb_background),
                        .foreground(fb_foreground),
                        .glyph_row(shifted_row[3:1]),
			.*);

FontColorLUT FontColorLUT(.glyph(fb_glyph),
                          .glyph_row(fb_fcl_row),
                          .glyph_col(fb_fcl_col),
                          .foreground(fb_foreground),
                          .background(fb_background),
                          .r(vga_r),
                          .g(vga_g),
                          .b(vga_b),
			  .*);

always_ff @(posedge clk) begin
    fb_fcl_col <= col[2:0];
    fb_fcl_row <= shifted_row[3:1];
end

always_ff @(posedge clk) begin
    vsync_pipe <= {vsync, vsync_pipe[2:1]};
    hsync_pipe <= {hsync, hsync_pipe[2:1]};
end

endmodule
