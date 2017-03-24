# EX.2

```
# this is the long jump to 0xfe05b
0xffff0:     ljmp   $0xf000,$0xe05b
# test cs:0x6574 is zero otherwise exit
0xfe05b:     cmpl   $0x0,%cs:0x6574
0xfe062:     jne    0xfd2b6


#zero out ax and set %esp=0x7000
0xfe066:     xor    %ax,%ax
0xfe068:     mov    %ax,%ss
0xfe06a:     mov    $0x7000,%esp

# set %edx to an memeroy are
0xfe070:     mov    $0xf3c24,%edx
#jump to 0xfd124
0xfe076:     jmp    0xfd124

#clear %ecx
0xfd124:     mov    %eax,%ecx

#disable interupt
0xfd127:     cli    
#clear direction flag set low to high read mode
0xfd128:     cld    
# eax = 0x8f
0xfd129:     mov    $0x8f,%eax
# NMI enable
0xfd12f:     out    %al,$0x70
# read time into %al   
0xfd131:     in     $0x71,%al
# read system config into al
0xfd133:     in     $0x92,%al
#enable system device
0xfd135:     or     $0x2,%al
0xfd137:     out    %al,$0x92
# load interrupt descriptor table. datastruct used to process interrupts
0xfd139:     lidtw  %cs:0x6690
# load global descriptor table, data structure used to hold information about different memeroy segments and code can access
0xfd13f:     lgdtw  %cs:0x6650

$cr0 ctrl cr0 = 0 protect mode
0xfd145:     mov    %cr0,%eax
0xfd148:     or     $0x1,%eax
0xfd14c:     mov    %eax,%cr0
#ljump
0xfd14f:     ljmpl  $0x8,$0xfd157

# set ds, es, ss, fs, gs to 0x10 and then clear eax
0xfd157:     mov    $0x10,%eax
0xfd15c:     mov    %eax,%ds
0xfd15e:     mov    %eax,%es
0xfd160:     mov    %eax,%ss
0xfd162:     mov    %eax,%fs
0xfd164:     mov    %eax,%gns
0xfd166:     mov    %ecx,%eax
```

# Ex. 3

The bootloader setup register, switch from 16-bits model to 32-bits  model.
Then call the bootmain. In the bootmain, the bootloader load the kernel image
into `0x100000`, after that, jump to the `0x10000c` to execute the kernel entry
code. In the `kernel.asm` the kernel start from `0xf010000c`, this address is
the virtual address.



# Ex. 5

The first instruction that would "break" is the `ljump` instruction which will
change the address mode from 16 bits to 32 bits.

# Ex. 6

Before the copying the kernel code to that memory address. It only contains
trash. As a result, every time we restart the qemu, the containt at that
address could be trash.


# Ex. 7

Before `movl %eax, %cr0` the memory at `0x100000` contains the kernel code and
the memory at `0xf0100000` contains the trash (`0x00`). After step over `movl
%eax, %cr0` the paging is turning on and the memory at `0xf0100000` have the
same content as `0x100000`.

The first istruction that will goes wrong is the instruction just after

```
mov $relocated, %eax  # relocated = 0xf010002f
jmp *%eax             # FAIL
```

Because the paging is not turned on. The content at `0xf010002f` is noting but
trash.


# Ex. 8

The place to add the `"%o"` support is at  `printfmt.c:207`. The why to add
base8 output support is the same as base16.

1. `printf.c` and `console.c` interface via `cputchar()` the `printf.c` warps
   the `cputchar()` inside the `punch()` in order to count how many characters
   have been outputted.

2. `crt_pos` is the position of the cursor. `CRT_SIZE` is the size of the display.
    the functionality of those code is when the `crt_pos` is going out of
    screen, shifting one line up in the screen and fill the last line as blank.

3. `fmt` points to the format string `x %d, y %x, z %d\n`,  the `ap` points to
   the argument list in the memory `ap, ap+4, ap+8` points to `1,3,4`
   respectively.
   The calls of `cons_putc()`is as follows:
  ```
    cons_putc ('x')
    cons_putc (' ')
    cons_putc ('1')
    cons_putc (',')
    cons_putc (' ')
    cons_putc ('y')
    cons_putc (' ')
    cons_putc ('3')
    cons_putc (',')
    cons_putc (' ')
    cons_putc ('z')
    cons_putc (' ')
    cons_putc ('4')
    cons_putc ('\n')
  ```

  The `va_arg()` is called 3 times before `cons_puts('1')`, `cons_puts('3')` and `cons_puts('4')`.
  Each time after `va_arg()` is called, the `ap` is shift by 4 bytes to point
  to next args.

4. The output is `He110 World`. `57616` could be represented in hex as `e110`. The following sequence of bytes: `0x72 0x6c 0x64 0x00`(little-endin) is `rld\0` in ascii.
   In big-endian system, the `i` should be `0x726c6400`. The value `57616` need
   not to be changed.

5. The value will be printed is the 4 bytes value on the stack. Because the
  `vprintfmt` will interpret the 4 bytes value as integer.

6. The `cprintf()` need not to be changed. The things need to be changed is
   `va_start`, `va_arg` and `va_end` in order to make sure we fetch the
   argument in right order.

# Ex. 9

The kernel stack is setup in the `entry.S` as follows:

```
movl $0x0, %ebp
movl $(bootstacktop), %esp
```

`bootstacktop` is a label in `entry.S` with the size of `KSTACKSIZE` which is
16 KB. The space is reserved by `.space` command. `$esp` is pointed to the
highest address `$(bootstacktop)`.


# Ex. 10

Each time the function is called `$ebp`, `$ebx`, `x`, `$0xf0101760` and `$eip` is pushed on the stack.

# Ex. 11

Finished, see the source code for more detail.


# Ex. 12

In `kernel.ld`, we can find the configuration for allocation the debug
information in section `.stab` and `.stabstr`.

