/*******************************************************************************
Filename    : user.ld
Author      : pry
Date        : 21/08/2018
Licence     : LGPL v3+; see COPYING for details.
Description : The scatter file for Cortex - A9 layout. This file is intended 
              to be used with XC7Z020, and is adapted from the xv6 port.
                     Start_PA    Start_VA
              SRAM1: 0x00000000  0x00000000
              SRAM2: 0x40000000  0x40000000
              DRAM:  0xE0000000  0x60000000
******************************************************************************/

OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)


SECTIONS
{
    . =  0x08004000;

    .text : {
        *u.o(.text .text.*)
        *(.text .text.*)        
    }

    PROVIDE(etext = .); /* Define the 'etext' symbol to this value */

    /* aligned the data to a (4K) page, so it can be assigned
     different protection than the code*/
    . = ALIGN(0x1000);

    .data : {
    *(.data .data.*)
    }
    
    PROVIDE (edata = .);

    . = ALIGN(0x1000);
    PROVIDE (end = .);
    ASSERT(. < 0x80F00000, "Kernel size exceeded the limit, may need to adjust memory configs")
}

/* End Of File ***************************************************************/

/* Copyright (C) Evo - Devo Instrum. All rights reserved ***********************/