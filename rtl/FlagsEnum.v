`ifndef FLAGSENUM
`define FLAGSENUM

typedef enum bit [3:0] {
    CF_IDX = 0,
    PF_IDX = 2,
    AF_IDX = 4,
    ZF_IDX = 6,
    SF_IDX = 7,
    TF_IDX = 8,
    IF_IDX = 9,
    DF_IDX = 10,
    OF_IDX = 11
} FLAG_t;

`endif // FLAGSENUM
