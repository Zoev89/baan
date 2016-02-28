#include <stdio.h>

#include "errorPrint.h"

static FILE *errorFile;

ErrorPrint::ErrorPrint(int &tickTimer) :
    mTickTimer(tickTimer)
{

}

void ErrorPrint::errorPrint (const char *msg, ...)
{
  va_list arg_ptr;
  char string[50];

  if (errorFile == NULL)
    {
      errorFile = fopen ("baanLog.txt", "w");
    }

  va_start (arg_ptr, msg);
  sprintf (string, "[%dm %2ds %3dms] ", mTickTimer / (10 * 60),
           (mTickTimer / 10) % 60, mTickTimer % (10) * 100);
  printf ("%s", string);
  vprintf (msg, arg_ptr);
  if (errorFile)
    {
      fprintf (errorFile, "%s", string);
      vfprintf (errorFile, msg, arg_ptr);
    }
  va_end (arg_ptr);

}

void ErrorPrint::errorPrintClose (void)
{
  if (errorFile)
    {
      fclose (errorFile);
      errorFile = NULL;
    }
}
