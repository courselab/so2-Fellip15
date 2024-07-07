/*
 *    SPDX-FileCopyrightText: 2021 Monaco F. J. <monaco@usp.br>
 *    SPDX-FileCopyrightText: 2024 Ramon M. M. <ramonmoreiramachado2019@gmail.com>
 *   
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *  This file is a derivative work from SYSeg (https://gitlab.com/monaco/syseg)
 *  and contains modifications carried out by the following author(s):
 *  Ramon M. M. <ramonmoreiramachado2019@gmail.com>
 */

#include "bios.h"
#include "opt.h"

#define MAX_BUFFER_SIZE 10
char buffer[MAX_BUFFER_SIZE];

void concatenate() {
  char buffer1[MAX_BUFFER_SIZE];
  readln(buffer);
  readln(buffer1);
  print(buffer);
  println(buffer1);
}

int main() {
  println("Boot Command 1.0");

  while (1) {
    print("$ ");
    readln(buffer);

    if (!strcmp(buffer, "concat")) {
      concatenate();
    } else {
      println("Unknown command.");
    }
  }

  return 0;
}
