task do_shr;
    output [15:0] out;
    input is_8_bit;
    input [15:0] a;
    input [15:0] b;
    input [15:0] flags_in;
    output [15:0] flags_out;

    begin
        flags_out = flags_in;
        {out, flags_out[CF_IDX]} = {a, 1'b0} >> b[4:0];
        common_flags(flags_out, is_8_bit, out, a, b);

        if (~|b)
            flags_out = flags_in;
    end
endtask
