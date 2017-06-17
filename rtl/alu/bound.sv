task do_bound;
    input [15:0] a;
    input [15:0] b;
    input [15:0] flags_in;
    output [15:0] flags_out;
    begin
        flags_out = flags_in;
        flags_out[CF_IDX] = $signed(a) < $signed(b);
    end
endtask
