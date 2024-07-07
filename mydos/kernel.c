/*
 *    SPDX-FileCopyrightText: 2021 Monaco F. J. <monaco@usp.br>
 *    SPDX-FileCopyrightText: 2024 Fellip15 <fellipalves15@gmail.com>
 *   
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *  This file is a derivative work from SYSeg (https://gitlab.com/monaco/syseg)
 *  and contains modifications carried out by the following author(s):
 *  Fellip15 <fellipalves15@gmail.com>
 */

/* This source file implements the kernel entry function 'kmain' called
   by the bootloader, and the command-line interpreter. Other kernel functions
   were implemented separately in another source file for legibility. */

#include "bios1.h"		/* For kwrite() etc.            */
#include "bios2.h"		/* For kread() etc.             */
#include "kernel.h"		/* Essential kernel functions.  */
#include "kaux.h"		/* Auxiliary kernel functions.  */

#define CMD_LINE_LEN 1024	  /* Max length of the command line.          */
#define MAX_ARGS 32		  /* Max number of arguments in command line. */
#define DIR_ENTRY_LEN 32 	  /* Max file name length in bytes.           */
#define SECTOR_SIZE 512 	  /* Max file name length in bytes.           */

/* In order to allow for the media to be bootable by BIOS, the file system 
   signature starts with a jump instruction that leaps over the header data, 
   and lands at the bootstrap program right next to it. In the present example,
   the signature is the instruction 'jump 0xe', follwed by the character
   sequence 'ty' (we thus jump 14 bytes). */

#define FS_SIGNATURE "\xeb\xety"  /* File system signature.                   */
#define FS_SIGLEN 4               /* Signature length.   

/* Kernel's entry function. */
struct fs_header_t
{
  unsigned char  signature[FS_SIGLEN];    /* The file system signature.              */
  unsigned short total_number_of_sectors; /* Number of 512-byte disk blocks.         */
  unsigned short number_of_boot_sectors;  /* Sectors reserved for boot code.         */
  unsigned short number_of_file_entries;  /* Maximum number of files in the disk.    */
  unsigned short max_file_size;		  /* Maximum size of a file in blocks.       */
  unsigned int unused_space;              /* Remaining space less than max_file_size.*/
} __attribute__((packed));      /* Disable alignment to preserve offsets.  */

void kmain(void)
{
  int i, j;
  
  register_syscall_handler();	/* Register syscall handler at int 0x21.*/

  splash();			/* Uncessary spash screen.              */

  shell();			/* Invoke the command-line interpreter. */
  
  halt();			/* On exit, halt.                       */
  
}

/* Tiny Shell (command-line interpreter). */

char buffer[BUFF_SIZE];
int go_on = 1;

void shell()
{
  int i;
  clear();
  kwrite ("TigerOS 1.7\n Welcome!\n");

  while (go_on)
    {

      /* Read the user input. 
	 Commands are single-word ASCII tokens with no blanks. */
      do
	{
	  kwrite(PROMPT);
	  kread (buffer);
	}
      while (!buffer[0]);

      /* Check for matching built-in commands */
      
      i=0;
      while (cmds[i].funct)
	{
	  if (!strcmp(buffer, cmds[i].name))
	    {
	      cmds[i].funct();
	      break;
	    }
	  i++;
	}

      /* If the user input does not match any built-in command name, just
	 ignore and read the next command. If we were to execute external
	 programs, on the other hand, this is where we would search for a 
	 corresponding file with a matching name in the storage device, 
	 load it and transfer it the execution. Left as exercise. */
      
      if (!cmds[i].funct)
	kwrite ("Command not found\n");
    }
}


/* Array with built-in command names and respective function pointers. 
   Function prototypes are in kernel.h. */

struct cmd_t cmds[] =
  {
    {"help",    f_help},     /* Print a help message.       */
    {"quit",    f_quit},     /* Exit TyDOS.                 */
    {"ls",      f_list},     /* List file names.            */
    {"exec",    f_exec},     /* Execute an example program. */
    {0, 0}
  };


/* Build-in shell command: help. */

void f_help()
{
  kwrite ("...me, Obi-Wan, you're my only hope!\n\n");
  kwrite ("   But we can try also some commands:\n");
  kwrite ("      exec    (to execute an user program example\n");
  kwrite ("      ls      (to list files names\n");
  kwrite ("      quit    (to exit TyDOS)\n");
}

void f_list()
{
    struct fs_header_t *fs_header = (struct fs_header_t *)0x7c00;
    int total_file_names_size = fs_header->number_of_file_entries * DIR_ENTRY_LEN;

    int files_beginning_sector = 1 + fs_header->number_of_boot_sectors;
    int sectors_amount = total_file_names_size / SECTOR_SIZE;

    char file_names[total_file_names_size];
    for (int i = 0; i < total_file_names_size; i++) 
      file_names[i] = 0;

    asm volatile (
        "mov $0x02, %%ah\n"                          // Function 02h - Read Sectors From Drive
        "mov boot_drive, %%dl\n"                   // Boot drive number
        "mov $0x00, %%dh\n"                        // Head number (0-based)
        "mov $0x00, %%ch\n"                       // Cylinder number (0-based)
        "mov %[sectors_amount], %%al\n"          // Number of sectors to read (byte)
        "mov %[files_beginning_sector], %%cl\n" // Sector number (1-based)
        "mov %[file_names], %%bx\n"            // Output buffer      
        "int $0x13\n"                         // Call BIOS interrupt 13h
        :
        : [sectors_amount] "g" (sectors_amount),
          [files_beginning_sector] "g" (files_beginning_sector),
          [file_names] "g" (file_names)
        : "%ah", "%dl", "%dh", "%ch", "%al", "%cl", "%bx"
    );

    kwrite("Listing file names:\n");
    for (int file_pos = 0; file_pos < total_file_names_size; file_pos += DIR_ENTRY_LEN) {
      if (file_names[file_pos] != 0) {
        kwrite(file_names + file_pos);
        kwrite("\n");
      }
    }
}

void f_quit()
{
  kwrite ("Program halted. Bye.");
  go_on = 0;
}

/* Built-in shell command: example.

   Execute an example user program which invokes a syscall.

   The example program (built from the source 'prog.c') is statically linked
   to the kernel by the linker script (tydos.ld). In order to extend the
   example, and load and external C program, edit 'f_exec' and 'prog.c' choosing
   a different name for the entry function, such that it does not conflict with
   the 'main' function of the external program.  Even better: remove 'f_exec'
   entirely, and suppress the 'example_program' section from the tydos.ld, and
   edit the Makefile not to include 'prog.o' and 'libtydos.o' from 'tydos.bin'.

  */

extern int main();
void f_exec()
{
  main();			/* Call the user program's 'main' function. */
}

