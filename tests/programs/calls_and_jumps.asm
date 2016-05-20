cpu 8086

    times 1024 db 0

    call near near_call_target
    call 0:far_call_target
    mov ax, 0
    cmp ax, 0
    jne end
    mov word    [0x1004], 3
end:
    int3

near_call_target:
    mov word    [0x1000], 1
    ret

far_call_target:
    mov word    [0x1002], 2
    retf
