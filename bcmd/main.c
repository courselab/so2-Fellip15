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

#include "bios.h"
#include "opt.h"

#define PROMPT "$ "		/* Prompt sign.      */
#define SIZE 20			/* Read buffer size. */

char buffer[SIZE];		/* Read buffer.      */

void copy() {
  readln(buffer);
  char same[SIZE];
  for (int i = 0; i < SIZE; i++) {
    same[i] = buffer[i];
  }

  println(same);
}

int main()
{
  clear();
  
  println  ("Boot Command 1.0");

  while (1)
    {
      print(PROMPT);		/* Show prompt.               */
      readln(buffer);		/* Read use input.            */

      if (buffer[0])		/* Execute built-in command.  */
	{
    if (!strcmp(buffer,"copy"))
      copy();
	  else 
	    println("Unkown command.");
	}
    }
  
  return 0;

}

