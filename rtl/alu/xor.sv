task do_xor;
    output [15:0] out;
    input is_8_bit;
    input [15:0] a;
    input [15:0] b;
    input [15:0] flags_in;
    output [15:0] flags_out;

    begin
        flags_out = flags_in;
        out = a ^ b;
        {flags_out[CF_IDX], flags_out[OF_IDX]} = 2'b0;
        common_flags(flags_out, is_8_bit, out, a, b);
    end
endtask
