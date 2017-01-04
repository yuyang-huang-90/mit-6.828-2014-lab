# lab1 sln

# Ex. 1
Source code implementation finished.

# Ex. 2
Read the relevant Sections of the manual.


# Ex. 3
I set break point at `cprintf` and exam the args in both GDB and QEMU.

* In GDB
``
(gdb) b cprintf
Breakpoint 1 at 0xf0101532: file kern/printf.c, line 26.
(gdb) c
Continuing.
The target architecture is assumed to be i386
=> 0xf0101532 <cprintf>:        push   %ebp

Breakpoint 1, cprintf (fmt=0xf0102480 "6828 decimal is %o octal!\n") at kern/printf.c:26
26      {
(gdb) info locals
cnt = 0x0
(gdb) info args
fmt = 0xf0102480 "6828 decimal is %o octal!\n"
(gdb) x /xw 0xf0102480
0xf0102480:     0x38323836
``

* In QEMU
``
(qemu) info pg
VPN range     Entry         Flags        Physical page
[00000-003ff]  PDE[000]     ----A----P
  [00000-000b7]  PTE[000-0b7] --------WP 00000-000b7
  [000b8-000b8]  PTE[0b8]     ---DA---WP 000b8
  [000b9-000ff]  PTE[0b9-0ff] --------WP 000b9-000ff
  [00100-00102]  PTE[100-102] ----A---WP 00100-00102
  [00103-00110]  PTE[103-110] --------WP 00103-00110
  [00111-00111]  PTE[111]     ---DA---WP 00111
  [00112-00113]  PTE[112-113] --------WP 00112-00113
  [00114-00114]  PTE[114]     ---DA---WP 00114
  [00115-003ff]  PTE[115-3ff] --------WP 00115-003ff
[f0000-f03ff]  PDE[3c0]     ----A---WP
  [f0000-f00b7]  PTE[000-0b7] --------WP 00000-000b7
  [f00b8-f00b8]  PTE[0b8]     ---DA---WP 000b8
  [f00b9-f00ff]  PTE[0b9-0ff] --------WP 000b9-000ff
  [f0100-f0102]  PTE[100-102] ----A---WP 00100-00102
  [f0103-f0110]  PTE[103-110] --------WP 00103-00110
  [f0111-f0111]  PTE[111]     ---DA---WP 00111
  [f0112-f0113]  PTE[112-113] --------WP 00112-00113
  [f0114-f0114]  PTE[114]     ---DA---WP 00114
  [f0115-f03ff]  PTE[115-3ff] --------WP 00115-003ff
(qemu) xp /x 0xf0102480
00000000f0102480: 0x00000000
(qemu) xp /x 0x00102480
0000000000102480: 0x38323836
``

We can see the  virtual address `0xf0102480` and the physical address
`0x00102480` have the same content. Just as the pagetable show us.

## Question 1
The type of x will have to be `uintptr_t`. The reason for this is we try to
write the content at x. When read/write the memory in the kernel, all the
address should be virtual address because the address translation will
automatically handled by the MMU.

#Ex.4
Source code implementation finished.

#Ex.5
Source code implementation finished.
