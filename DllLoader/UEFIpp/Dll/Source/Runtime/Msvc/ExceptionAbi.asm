.data

PUBLIC EhSavedOriginalRbp
PUBLIC EhSavedEstablisher
PUBLIC EhSavedContinuation
PUBLIC EhSavedRsp

EhSavedOriginalRbp dq 0
EhSavedEstablisher dq 0
EhSavedContinuation dq 0
EhSavedRsp dq 0

.code

PUBLIC CallCatchAndContinue

CallCatchAndContinue PROC
    mov EhSavedEstablisher, rdx
    mov EhSavedContinuation, r8
    mov EhSavedRsp, r9

    sub rsp, 28h
    call rcx
    add rsp, 28h

    mov rdx, EhSavedEstablisher
    mov r8,  EhSavedContinuation
    mov r9,  EhSavedRsp

    mov rsp, r9
    mov rbp, rdx

    jmp r8
CallCatchAndContinue ENDP

EXTERN CxxThrowExceptionImpl : PROC
PUBLIC CxxThrowExceptionShim

CxxThrowExceptionShim PROC
    mov r8, [rsp]
    lea r9, [rsp+8]
    jmp CxxThrowExceptionImpl
CxxThrowExceptionShim ENDP

END