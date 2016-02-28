// Baan: A railway model controlling program
// Copyright (C) 1998 - 2001 Eric Kathmann
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston,
// MA  02111-1307, USA.

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "leesdata.h"

// ik support 20 open files tegelijkertijd
#define AANTAL_FILES 20
static FILE *files[AANTAL_FILES];
static int lineCount[AANTAL_FILES];
static int laatsteIndex;


void
zoekFile (FILE * file)
{
  if (files[laatsteIndex] != file)
    {
      // is een nieuwe file of onbekend eerst uitvinden
      // of we hem al hebben
      for (laatsteIndex = 0; laatsteIndex < AANTAL_FILES; laatsteIndex++)
        {
          if (files[laatsteIndex] == file)
            // gevonden
            break;
        }
      if (laatsteIndex >= AANTAL_FILES)
        {
          // niet gevonden zoek naar een lege plek
          for (laatsteIndex = 0; laatsteIndex < AANTAL_FILES; laatsteIndex++)
            {
              if (NULL == files[laatsteIndex])
                {
                  files[laatsteIndex] = file;
                  lineCount[laatsteIndex] = 0;
                  break;
                }
            }
          if (laatsteIndex >= AANTAL_FILES)
            {
              // tja de tabel is vol nu initializeer ik maar wat
              laatsteIndex = 0;
              files[laatsteIndex] = file;
              lineCount[laatsteIndex] = 0;
            }
        }
    }
}

int
EricFgetsGetLineCount (FILE * file)
{
  zoekFile (file);
  return lineCount[laatsteIndex];
}

int
EricFgetsGetLastLineCount ()
{
  return lineCount[laatsteIndex];
}

void
EricFgetsSetLineCount (FILE * file, int count)
{
  zoekFile (file);
  lineCount[laatsteIndex] = count;
}

void
EricFgetsClose (FILE * file)
{
  zoekFile (file);
  lineCount[laatsteIndex] = 0;
}

char *
EricFgets (char *String, int MaxSize, FILE * file)
{
  char *ret;
  unsigned int i;

  // Eerst even de laatsteIndex updaten
  zoekFile (file);
  do
    {
      ret = fgets (String, MaxSize, file);
      if (ret == NULL)
        return NULL;

      lineCount[laatsteIndex] += 1;
      // zoek het eerste non spatie
      for (i = 0; i < strlen (String); i++)
        {
          if ((String[i] != ' ') && (String[i] != '\t'))
            {
              break;
            }
        }
      if (i)
        memcpy (String, &String[i], strlen (String) - i);
    }
  while ((*String == '#') || ((iscntrl (*String) && (*String != '\t'))));

  /*
   ** Haal de \n uit de string zodat we hem goed 
   ** gebruiken kunnen voor b.v. filenamen
   **/
  for (i = 0; i < strlen (String); i++)
    {
      if (iscntrl (String[i]) && (String[i] != '\t'))
        {
          String[i] = 0;
          break;
        }
    }
//  printf("--%s--\n",String);
//  printf("--%d--\n",String[0]);
  return String;
}
