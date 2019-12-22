# a simple implementation of tcp echo client
# using x86_64 assembly on linux
# caller-saved register are used for local variables
# callee-saved register are used for global variables
#
# compile it with `gcc -c client.s && ld client.o -o client` command

.global _start

.equ PORT, 0x0f27    # port number in network byte order(big endian), here is 9999 
.equ IP, 0x0100007f  # connect ip addresses in network byte order, here is 127.0.0.1

.equ STDIN, 0
.equ STDOUT, 1
.equ STDERR, 2
.equ AF_INET, 2
.equ SOCK_STREAM, 1

.data
.size sockaddr, 16
sockaddr:
    .short 2
    .short PORT
    .long IP
    .quad 0     # 8 bytes padding zero
    .equ addrlen, . - sockaddr

msg_connect:
    .ascii "server connected\n"
    .equ msg_connect_len, . - msg_connect

msg_disconnect:
    .ascii "server disconnected\n"
    .equ msg_disconnect_len, . - msg_disconnect

err_socket:
    .ascii "socket syscall error\n"
    .equ err_socket_len, . - err_socket

err_connect:
    .ascii "connect syscall error\n"
    .equ err_connect_len, . - err_connect


.bss
buffer:
    .space 1024
    .equ buffer_size, . - buffer


.text
# create a socket file descriptor
# return: connfd
_socket:
    # connfd = socket(AF_INET, SOCK_STREAM, 0)
    mov $41, %rax
    mov $AF_INET, %rdi
    mov $SOCK_STREAM, %rsi
    mov $0, %rdx
    syscall

    # check return value
    cmp $0, %rax
    jl _fail_socket

    ret

# connect to server
# args:
#     rdi: connfd(unmodified)
# return: void
_connect:
    mov $42, %rax
    lea sockaddr(%rip), %rsi
    mov $addrlen, %rdx
    syscall
    
    cmp $0, %rax
    jl _fail_connect

    # store connfd
    push %rax

    # print connected message
    # write(STDOUT, msg_connect, msg_connect_len)
    mov $1, %rax
    mov $STDOUT, %rdi
    lea msg_connect(%rip), %rsi
    mov $msg_connect_len, %rdx
    syscall

    # pop connfd back to rax and return
    pop %rax
    ret

# read from fd to buffer
# args:
#     rdi: fd(unmodified)
# return: read_cnt
_read:
    mov $0, %rax
    lea buffer(%rip), %rsi
    mov $buffer_size, %rdx
    syscall
    ret

# write from buffer to fd
# args:
#     rdi: fd(unmodified)
#     rsi: read_cnt(modified)
# return: void
_write:
    mov $1, %rax
    mov %rsi, %rdx
    lea buffer(%rip), %rsi
    syscall
    ret

# close connfd and print a message
# args:
#     rdi: connfd(modified)
# return: void
_close:
    mov $3, %rax
    syscall

    # print disconnected message
    # write(STDOUT, msg_disconnect, msg_disconnect_len)
    mov $1, %rax
    mov $STDOUT, %rdi
    lea msg_disconnect(%rip), %rsi
    mov $msg_disconnect_len, %rdx
    syscall
    ret

# main function
_start:
    call _socket	   # connfd = socket()
    mov %rax, %rbp     # save connfd at rbp
    mov %rbp, %rdi     # connect(connfd)
    call _connect
.loop:
    mov $STDIN, %rdi   # read(STDIN)
    call _read         # exit loop when read_cnt == 0
    cmp $0, %rax
    je .end
    mov %rbp, %rdi     # write(connfd, read_cnt)
    mov %rax, %rsi
    call _write
    call _read         # read(connfd)
    mov $STDOUT, %rdi  # write(STDOUT, read_cnt)
    mov %rax, %rsi
    call _write
    jmp .loop
.end:
    mov %rbp, %rdi     # close(connfd)
    call _close
    xor %rdi, %rdi     # exit(0)
    call _exit

# wrapper for exit syscall
# caller should provide status code in %rdi
# never returns
_exit:
    mov $60, %rax
    syscall

# error handlers
# handle socket syscall error
_fail_socket:
    lea err_socket(%rip), %rsi
    mov $err_socket_len, %rdx
    call _fail

# handle connect syscall error
_fail_connect:
    lea err_connect(%rip), %rsi
    mov $err_connect_len, %rdx
    call _fail

# on failure, print error message and exit with code 1
# this should only be called with _fail_xxx functions
_fail:
    # write(STDERR, errmsg, errmsg_len)
    mov $1, %rax
    mov $STDERR, %rdi
    syscall

    # exit(1)
    mov $1, %rdi
    call _exit
