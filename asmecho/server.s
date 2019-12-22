# a simple implementation of tcp echo server
# using x86_64 assembly on linux
# caller-saved register are used for local variables
# callee-saved register are used for global variables
#
# compile it with `gcc -c server.s && ld server.o` command
# use `-o filename` in ld to specify output filename

.global _start

.equ PORT, 0x0f27    # port number in network byte order(big endian), here is 9999 
.equ IP, 0           # all ip addresses in network byte order, 0 means listen all ips

.equ STDOUT, 1
.equ STDERR, 2
.equ AF_INET, 2
.equ SOCK_STREAM, 1
.equ SOL_SOCKET, 1
.equ SO_REUSEADDR, 2
.equ BACKLOG, 1024

.data
.size sockaddr, 16
sockaddr:
    .short 2
    .short PORT
    .long IP
    .quad 0          # 8 bytes padding zero
    .equ SA_LEN, . - sockaddr

optval:
    .long 1
    .equ optval_size, . - optval

msg_connect:
    .ascii "client connected\n"
    .equ msg_connect_len, . - msg_connect

msg_disconnect:
    .ascii "client disconnected\n"
    .equ msg_disconnect_len, . - msg_disconnect

err_socket:
    .ascii "socket syscall error\n"
    .equ err_socket_len, . - err_socket

err_bind:
    .ascii "bind syscall error\n"
    .equ err_bind_len, . - err_bind

err_listen:
    .ascii "listen syscall error\n"
    .equ err_listen_len, . - err_listen

err_accept:
    .ascii "accept syscall error\n"
    .equ err_accept_len, . - err_accept


.bss
buffer:
    .space 1024
    .equ buffer_size, . - buffer


.text
# create a socket file descriptor
# return: listenfd
_socket:
    # listenfd = socket(AF_INET, SOCK_STREAM, 0)
    mov $41, %rax    # socket
    mov $AF_INET, %rdi
    mov $SOCK_STREAM, %rsi
    mov $0, %rdx
    syscall

    # check return value
    cmp $0, %rax
    jl _fail_socket

    # store listenfd
    push %rax

    # eliminates "Address already in use" error from bind
    # setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int))
    mov $54, %rax
    mov (%rsp), %rdi
    mov $SOL_SOCKET, %rsi
    mov $SO_REUSEADDR, %rdx
    mov $optval, %rcx
    mov $optval_size, %r8
    syscall

    # pop listenfd back to rax and return
    pop %rax
    ret


# accept connection
# args:
#     rdi: listenfd(unmodified)
# return: void
_listen:
    # bind(listenfd, sockaddr, SA_LEN)
    mov $49, %rax
    mov $sockaddr, %rsi
    mov $SA_LEN, %rdx
    syscall

    cmp $0, %rax
    jl _fail_bind

    # listen(listenfd, BACKLOG)
    mov $50, %rax
    mov $BACKLOG, %rsi
    syscall

    cmp $0, %rax
    jl _fail_listen

    ret

# accept connection
# args:
#     rdi: listenfd(modified)
# return: connfd
_accept:
    # connfd accept(listedfd, sockaddr = NULL, *addrlen = NULL)
    mov $43, %rax
    xor %rsi, %rsi
    xor %rdx, %rdx
    syscall

    cmp $0, %rax
    jl _fail_accept

    # store connfd
    push %rax

    # print connected message
    # write(STDOUT, msg_connect, msg_connect_len)
    mov $1, %rax
    mov $STDOUT, %rdi
    mov $msg_connect, %rsi
    mov $msg_connect_len, %rdx
    syscall

    # pop connfd back to rax and return
    pop %rax
    ret

# read from connfd to buffer
# args:
#     rdi: connfd(unmodified)
# return: read_cnt
_read:
    mov $0, %rax
    mov $buffer, %rsi
    mov $buffer_size, %rdx
    syscall
    ret

# write from buffer to connfd
# args:
#     rdi: connfd(unmodified)
#     rsi: read_cnt(modified)
# return: void
_write:
    mov $1, %rax
    mov %rsi, %rdx
    mov $buffer, %rsi
    syscall
    ret

# a simple tcp echo
# loop until an empty line (EOF)
# args:
#     rdi: connfd(unmodified)
# return: void
_echo:
    call _read        # read(connfd)
    mov %rax, %r14    # save read_cnt at r14
    mov %r14, %rsi    # write(connfd, read_cnt)
    call _write

    cmp $0, %r14
    jne _echo
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
    mov $msg_disconnect, %rsi
    mov $msg_disconnect_len, %rdx
    syscall
    ret

# main function
_start:
    call _socket	  # listenfd = _socket()
    mov %rax, %r12    # save listenfd at r12
    mov %r12, %rdi    # listen(listenfd)
    call _listen

.loop:
    mov %r12, %rdi    # accept(listenfd)
    call _accept
    mov %rax, %r13    # save connfd at r13
    mov %r13, %rdi    # echo(connfd)    
    call _echo
    mov %r13, %rdi    # close(connfd) 
    call _close
    jmp .loop

    # exit(0)
    xor %rdi, %rdi
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
    mov $err_socket, %rsi
    mov $err_socket_len, %rdx
    call _fail

# handle bind syscall error
_fail_bind:
    mov $err_bind, %rsi
    mov $err_bind_len, %rdx
    call _fail

# handle listen syscall error
_fail_listen:
    mov $err_listen, %rsi
    mov $err_listen_len, %rdx
    call _fail

# handle accept syscall error
_fail_accept:
    mov $err_accept, %rsi
    mov $err_accept_len, %rdx
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

