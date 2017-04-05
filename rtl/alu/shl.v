task do_shl;
    output [15:0] out;
    input is_8_bit;
    input [15:0] a;
    input [15:0] b;
    input [15:0] flags_in;
    output [15:0] flags_out;

    begin
        flags_out = flags_in;
        out = 16'b0;
        if (!is_8_bit) begin
            {flags_out[CF_IDX], out} = {1'b0, a} << b[4:0];
            flags_out[OF_IDX] = a[15] ^ out[15];
        end else begin
            {flags_out[CF_IDX], out[7:0]} = {1'b0, a[7:0]} << b[4:0];
            flags_out[OF_IDX] = a[7] ^ out[7];
        end
        shift_flags(flags_out, is_8_bit, out, a, b);

        if (~|b)
            flags_out = flags_in;
    end
endtask
