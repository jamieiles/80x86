task do_enter_frame_temp_addr;
    output [15:0] out;
    input [15:0] a;
    input [4:0] b;

    begin
        out = a - ({10'b0, b, 1'b0});
    end
endtask
