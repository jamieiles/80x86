module Counter #(parameter count_width = 0,
                 parameter count_max = 255)
                (input logic clk,
                 input logic reset,
                 output logic [count_width - 1:0] count);

reg [count_width - 1:0] count_reg;

always @(posedge clk) begin
	if (reset)
		count_reg <= 0;
	else if (count_reg != count_max[count_width - 1:0])
		count_reg <= count_reg + 1'b1;
end

assign count = count_reg;

endmodule
