task do_shr;
    output [31:0] out;
    input is_8_bit;
    input [15:0] a;
    input [4:0] shift_count;
    input [15:0] flags_in;
    output [15:0] flags_out;
    output busy;
    input multibit_shift;

    begin
        flags_out = flags_in;
        out = {11'b0, shift_count, a};

        if (|shift_count || !multibit_shift) begin
            {out[15:0], flags_out[CF_IDX]} = {a, 1'b0} >> 1'b1;
            shift_flags(flags_out, is_8_bit, out[15:0], a);
            if (is_8_bit)
                flags_out[OF_IDX] = a[7] ^ out[7];
            else
                flags_out[OF_IDX] = a[15] ^ out[15];

            out[31:16] = {11'b0, shift_count - 1'b1};
        end

        busy = multibit_shift && |out[20:16];
    end
endtask
