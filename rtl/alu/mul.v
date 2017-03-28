task do_mul;
    output [31:0] out;
    input is_8_bit;
    input [15:0] a;
    input [15:0] b;
    input [15:0] flags_in;
    output [15:0] flags_out;
    input is_signed;

    begin
        flags_out = flags_in;
        flags_out[ZF_IDX] = 1'b0;
        if (!is_8_bit) begin
            if (is_signed)
                out[31:0] = {{16{a[15]}}, a} * {{16{b[15]}}, b};
            else
                out[31:0] = {16'b0, a} * {16'b0, b};
            flags_out[CF_IDX] = |out[31:16];
            flags_out[OF_IDX] = |out[31:16];
        end else begin
            if (is_signed)
                out[15:0] = {{8{a[7]}}, a[7:0]} * {{8{b[7]}}, b[7:0]};
            else
                out[15:0] = {8'b0, a[7:0]} * {8'b0, b[7:0]};
            flags_out[CF_IDX] = |out[15:8];
            flags_out[OF_IDX] = |out[15:8];
        end
    end
endtask
