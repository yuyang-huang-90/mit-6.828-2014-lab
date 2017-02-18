# Ex.1
Code is implemented.

# Ex.2
Code is implemented.

## Q.1
The bootloader doesn't need `MPBOOTPHYS` like macro is because it both loaded
and linked at low address. However, the `mpentry.S` is load at low address and
linked at high address. Without such macro, it cannot translate the high
address to addresses relative to where the code actually gets loaded.

# Ex.3
Code is implemented.

# Ex.4
Code is implemented.

# Ex.5
Code is implemented.

## Q.2
If two CPU shares same kstack, images a situation: CPU 1 enters the kernel and
start executing some code, CPU 2 enters the kernel and start executing some
code, CPU1 returns to user mode. At this time, the CPU1 will pop of CPU2's
frames. It should cause some problem.


## Q.3
The pointer of variable `e` is pointing to someware in the `UENVS`, this part
of pages is set up also in env's virtual memeory `env_setup_vm()`. So the
process can access the same part of memory before and after the addressing
switching.

## Q.4
Because the registers may hold some tmp value for user environment just
before the user environment call the `yield` function. In order to resume the
environment seamlessly we need to save the register values before switching
environment. The register value is saved as a part of trap frame in `alltraps`
code. To restore the state of the process, `env_pop_tf()` is been called.

# Ex.6
Code is implemented.

# Ex.7
Code is implemented.

# Ex.8
Code is implemented.

# Ex.9
Code is implemented.

# Ex.10 & Ex.12 & Ex.13
Code is implemented. (The IDT init parts have already been done in lab3.)

# Ex.14 & Ex.15
Code is implemented.
