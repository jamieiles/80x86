`ifndef FLAGSENUM
`define FLAGSENUM

typedef enum bit [3:0] {
    CF_IDX = 4'd0,
    PF_IDX = 4'd2,
    AF_IDX = 4'd4,
    ZF_IDX = 4'd6,
    SF_IDX = 4'd7,
    TF_IDX = 4'd8,
    IF_IDX = 4'd9,
    DF_IDX = 4'd10,
    OF_IDX = 4'd11
} FLAG_t;

`endif // FLAGSENUM
