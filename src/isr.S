.global isr_stub_0
.type isr_stub_0, @function
isr_stub_0:
    cli
    hlt

.global isr_stub_13
.type isr_stub_13, @function
isr_stub_13:
    call isr13_handler
    iretq

.global isr_stub_14
.type isr_stub_14, @function
isr_stub_14:
    call isr14_handler
    iretq

.global isr_stub_6
.type isr_stub_6, @function
isr_stub_6:
    call isr6_handler
    iretq
