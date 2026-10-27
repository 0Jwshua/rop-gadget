OPTION PROLOGUE:NONE
OPTION EPILOGUE:NONE

EXTERN g_saved_rsp:QWORD
EXTERN g_rop_result:DWORD

PUBLIC run_rop_chain
PUBLIC exec_and_return

.code

run_rop_chain PROC
    lea rax, [g_saved_rsp]
    mov [rax], rsp
    mov rsp, rcx
    ret
run_rop_chain ENDP

exec_and_return PROC
    sub rsp, 020h
    call rax
    lea r10, [g_rop_result]
    mov [r10], eax
    lea r10, [g_saved_rsp]
    mov rsp, [r10]
    ret
exec_and_return ENDP

END