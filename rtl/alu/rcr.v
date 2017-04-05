task do_rcr;
    output [15:0] out;
    input is_8_bit;
    input [15:0] a;
    input [15:0] b;
    input [15:0] flags_in;
    output [15:0] flags_out;

    begin
        flags_out = flags_in;
        if (!is_8_bit) begin
            reg [5:0] i;
            out = a;
            for (i = 6'b0; i < {1'b0, b[4:0]}; ++i) begin
                {out, flags_out[CF_IDX]} = {flags_out[CF_IDX], out[15:0]};
            end
        end else begin
            reg [5:0] i;
            out = {8'b0, a[7:0]};
            for (i = 6'b0; i < {1'b0, b[4:0]}; ++i) begin
                {out[7:0], flags_out[CF_IDX]} = {flags_out[CF_IDX], out[7:0]};
            end
        end
        flags_out[OF_IDX] = is_8_bit ? a[7] ^ flags_in[CF_IDX] :
            a[15] ^ flags_in[CF_IDX];
        shift_flags(flags_out, is_8_bit, out, a, b);

        if (~|b)
            flags_out = flags_in;
    end
endtask
