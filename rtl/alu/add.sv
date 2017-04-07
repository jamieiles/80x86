task do_add;
    output [15:0] out;
    input is_8_bit;
    input [15:0] a;
    input [15:0] b;
    input [15:0] flags_in;
    output [15:0] flags_out;

    begin
        flags_out = flags_in;
        if (!is_8_bit) begin
            {flags_out[CF_IDX], out} = a + b;
            flags_out[OF_IDX] = ~(a[15] ^ b[15]) & (out[15] ^ b[15]);
        end else begin
            out = a + b;
            flags_out[CF_IDX] = a[8] ^ b[8] ^ out[8];
            flags_out[OF_IDX] = ~(a[7] ^ b[7]) & (out[7] ^ b[7]);
        end
        common_flags(flags_out, is_8_bit, out, a, b);
    end
endtask
