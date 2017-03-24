## Q. 1
The purpose of having a separate handle function for each exception/interrupt
is to maintain privilege separation. For example, if we have only one interrupt
handler, it is hard to only allow the user code to invoke `T_BRKPT` and
`T_SYSCALL` but not allow the user code to invoke other handler. One to one
mapping, is the best way to serve this purpose.

## Q. 2
There is not extra work need to be done to make `softint` work. The `int $14`
is the page fault exception. But this exception can only handled by the kernel
code (CPL=0). As the result, the `int $13`: general protection fault is  generated.

## Q.3
In order to make the code generate the breakpoint  exception. We need correctly
setup the IDT with the correct permission(DPL=3). Otherwise, the breakpoint
exception will trigger the general protection fault.

## Q.4
This mechanisms ensured the privilege separation, which means the user space
can issue system call and set breakpoints, but cannot manipulate virtual
memory (softint, int $14).
