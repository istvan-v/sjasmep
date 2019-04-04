# sjasmep
sjasm 0.39g6 with bug fixes and minor improvements

     OOO     O   O    OOO  O   O
    O   O       O O  O   O OO OO
    O       OO O   O O     O O O
     OOO     O O   O  OOO  O O O
        O    O OOOOO     O O   O
    O   O    O O   O O   O O   O
     OOO  O  O O   O  OOO  O   O
           OO

SjASM Z80 Assembler v0.39g6
Copyright 2006 Sjoerd Mastijn

www.xl2s.tk - sjasm@xl2s.tk

Introduction
============
SjASM version 0.3 is just yet another Z80 cross assembler.
Thanks to Eli-Jean Leyssens there is also a Linux version of SjASM.

DISCLAIMER
----------
If SjASM breaks anything - I didn't do it.

Changes from 0.2
----------------
- SjASM v0.3x assumes Z80-mode as there are no other cpus supported.
- All calculations are 32 bits.

Changes from 0.30
-----------------
- '#' Can be used now to indicate a hexadecimal value.
- Local 'number' labels should really work now.
- Multiple error messages per source line possible.
- Things like ld a,(4)+1 work again (=> ld a,5). Somehow I broke this in v0.3.
- Filenames don't need surrounding quotes anymore.
- Macro's can be nested once again.
- Better define-handling.
- 'Textarea' detects forward references.
- Include within include searches relative to that include.
- 'Unlocked' some directives (assert, output).
- '#' Can be used in labels.
- No space needed between label and '=' in statements like 'label=value'.
- The 'export' directive now exports 'label: EQU value' instead of 'label = value'.
- Labelnames starting with a register name (like HL_kip) shouldn't confuse SjASM anymore.
- RLC, RRC, RL, RR, SLA, SRA, SLL (SLI), RES and SET undocumented instructions added.
- "ex af,af" and "ex af" are possible now.
- Added defb, defw and defs.
- Locallabels now also work correctly when used in macros.
- Added // and limited /* */ comments.
- SjASM now checks the label values between passes.
- '>>>' Operator added.
- Sources included.
- '>>>' And '>>' operators now work the way they should (I think).
- Removed the 'data/text/pool'-bug. (Together with the data/text/pool directives. ~8)
- Added endmodule and endmod to end a module.
- STRUCT directive.
- REPT, DWORD, DD, DEFD directives.
- More freedom with character constants; in some cases it's possible to use double quotes...
- It's now possible to specify include paths at the command line.
- Angle brackets are used now to include commas in macroarguments. So check your macro's.
- Fixed a structure initialization bug.
- It's not possible to use CALLs or jumps without arguments anymore.
- DZ and ABYTEZ directives to define null terminated strings.
- SjASM now checks for lines that are too long.
- Added '16 bit' LD, LDI and LDD 'instructions'. (See end of this text:)
- PUSH and POP accept a list of registers.
- Added '16 bit SUB' instruction.
- Unreferenced labels are indicated with an 'X' in the label listing.
- Unknown escapecodes in strings result in just one error (instead of more).
- Labelnameparts now have a maximum of 70 characters.
- Improved IX and IY offset checking.
- Maximum, minimum, and, or, mod, xor, not, shl, shr, low and high operators added.
- Logical operations result in -1 or 0 instead of 1 or 0. Of course, -1 for true and 0 for false ;)
- Fixed the 'ifdef <illegal identifier>' bug. (:

New in 0.39g:
--------------
- ENDMAP directive.
- DEFM and DM synonyms for BYTE.
- Some bug fixes:
 - file size is reset when a new output file is opened.
 - 'bytes lost' warning fixed.
And thanks to Konami Man:
- PHASE and DEPHASE directives as synonyms of TEXTAREA and ENDT.
- FPOS directive.
- Expanded OUTPUT directive.
- The possibility to generate a symbol file.

New in 0.39g1:
- SjASM now allows spaces in filenames.

New in 0.39g6:
- : Operator.
- MULUB and MULUW work.
- Labels starting with an underscore are listed.
- Can't remember the other changes.

Known bugs:
-----------
- The listfile doesn't always look that good.
- Commas in strings in macroarguments are not ignored.
- Sourcelines are not further processed after an ELSE, ENDwhatever and the like.
- Using INCLUDE instead of INCBIN isn't detected, and will crash SjASM quite nicely.
- The combination of comments, strings and the af' register can be very confusing for SjASM :)
