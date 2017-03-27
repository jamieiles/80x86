task do_aaa;
    output [15:0] out;
    input [15:0] a;
    input [15:0] flags_in;
    output [15:0] flags_out;

    begin
        flags_out = flags_in;
        out = a;
        if (a[3:0] > 4'd9 || flags_in[AF_IDX]) begin
            out[3:0] += 4'd6;
            out[7:4] = 4'b0;
            out[15:8] += 8'b1;
            flags_out[CF_IDX] = 1'b1;
            flags_out[AF_IDX] = 1'b1;
        end else begin
            flags_out[CF_IDX] = 1'b0;
            flags_out[AF_IDX] = 1'b0;
        end
    end
endtask
