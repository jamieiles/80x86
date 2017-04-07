task do_extend;
    output [15:0] out;
    input is_8_bit;
    // verilator lint_off UNUSED
    input [15:0] a;
    // verilator lint_on UNUSED

    begin
        out = is_8_bit ? {16{a[7]}} : {16{a[15]}};
    end
endtask
