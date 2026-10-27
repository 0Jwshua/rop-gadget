#include <Windows.h>
#include <cstdio>
#include <cstdint>

extern "C" {
    uint64_t g_saved_rsp = 0;
    DWORD g_rop_result = 0;

    void run_rop_chain( uint64_t* chain );
    void exec_and_return();
}

static char g_cmd[] = "calc";

static constexpr size_t CHAIN_BUF_SIZE = 0x20000;

int main() {
    uint8_t* buf = ( uint8_t* )VirtualAlloc(
        nullptr, CHAIN_BUF_SIZE,
        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
    if ( !buf ) {
        printf( "virtualalloc failed: %lu\n", GetLastError() );
        return 1;
    }

    HMODULE oBase = GetModuleHandleA( "KernelBase.dll" );
    HMODULE k32 = GetModuleHandleA( "kernel32.dll" );
    uintptr_t pWinExec = ( uintptr_t )GetProcAddress( k32, "WinExec" );
    if ( !pWinExec ) { 
        printf( "win exec not found\n" ); 
        return 1; 
    }

    uint64_t* chain = ( uint64_t* )( buf + 0x1F808 );

    if ( ( ( uintptr_t )chain & 0xF ) != 8 ) {
        printf( "chain misaligned: %p (need 8 mod 16)\n", chain );
        return 1;
    }

    uint64_t pop_rcx_gadget = ( uint64_t )oBase + 0x26af3;
    uint64_t pop_rdx_gadget = ( uint64_t )oBase + 0x155212;
    uint64_t pop_rax_gadget = ( uint64_t )oBase + 0x55d5;

    int i = 0;
    chain[ i++ ] = pop_rcx_gadget;
    chain[ i++ ] = ( uint64_t )g_cmd;
    chain[ i++ ] = pop_rdx_gadget;
    chain[ i++ ] = 1;
    chain[ i++ ] = pop_rax_gadget;
    chain[ i++ ] = pWinExec;
    chain[ i++ ] = ( uint64_t )exec_and_return;

    run_rop_chain( chain );

    printf( "winexec return : %lu (0x%lx)\ndone :)\n", ( unsigned long )g_rop_result, ( unsigned long )g_rop_result );

    VirtualFree( buf, 0, MEM_RELEASE );

    return 0;
}