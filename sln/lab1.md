# lab1 sln

## EX.1:Familiarize yourself with the assembly language materials available on the 6.828 reference page. You don't have to read them now, but you'll almost certainly want to refer to some of this material when reading and writing x86 assembly.

Finished reading the doc.

## EX.2:Use GDB's si (Step Instruction) command to trace into the ROM BIOS for a few more instructions, and try to guess what it might be doing. You might want to look at Phil Storrs I/O Ports Description, as well as other materials on the 6.828 reference materials page. No need to figure out all the details - just the general idea of what the BIOS is doing first.

``
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
``

## Exercise 3: Exam bootloader code


The bootloader setup register, switch from 16-bits model to 32-bits  model.
Then call the bootmain. In the bootmain, the bootloader load the kernel image
into `0x100000`, after that, jump to the `0x10000c` to execute the kernel entry
code. In the `kernel.asm` the kernel start from `0xf010000c`, this address is
the virtual address.


## Exercise 4. Reading K&R

Finished.

## Exercise 5. Trace through the first few instructions of the boot loader again and identify the first instruction that would "break" or otherwise do the wrong thing if you were to get the boot loader's link address wrong.

The first instruction that would "break" is the `ljump` instruction which will
change the address mode from 16 bits to 32 bits.

## Exercise 6.  Examine the 8 words of memory at 0x00100000 at the point the BIOS enters the boot loader,

Before the copying the kernel code to that memory address. It only contains
trash. As a result, every time we restart the qemu, the containt at that
address could be trash.


## Exercise 7. Use QEMU and GDB to trace into the JOS kernel and stop at the movl %eax, %cr0. Examine memory at 0x00100000 and at 0xf0100000. Now, single step over that instruction using the stepi GDB command. Again, examine memory at 0x00100000 and at 0xf0100000. Make sure you understand what just happened. What is the first instruction after the new mapping is established that would fail to work properly if the mapping weren't in place? Comment out the movl %eax, %cr0 in kern/entry.S, trace into it, and see if you were right.

Before `movl %eax, %cr0` the memory at `0x100000` contains the kernel code and
the memory at `0xf0100000` contains the trash (`0x00`). After step over `movl
%eax, %cr0` the paging is turning on and the memory at `0xf0100000` have the
same content as `0x100000`.

The first istruction that will goes wrong is the instruction just after

``
mov $relocated, %eax  # relocated = 0xf010002f
jmp *%eax             # FAIL
``

Because the paging is not turned on. The content at `0xf010002f` is noting but
trash.


## Exercise 8. Format printing console exercise.

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
   ``
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
  ``
  The `va_arg()` is called 3 times before `cons_puts('1')`, `cons_puts('3')` and `cons_puts('4')`.
  Each time after `va_arg()` is called, the `ap` is shift by 4 bytes to point
  to next args.

4. 

5. The value will be printed is the 4 bytes value on the stack. Because the
  `vprintfmt` will interpret the 4 byptes value as integer. 
   
