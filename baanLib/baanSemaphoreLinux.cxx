#include <stdio.h>
#include "baanSemaphore.h"
#include <semaphore.h>

struct _baanSemaphore_t
{
  sem_t sema;
};

pbaanSemaphore_t
baanSemaphoreCreate (int count)
{

  pbaanSemaphore_t psema;
  int ret;
  psema = new _baanSemaphore_t;
  if (psema == NULL)
    return psema;

  // Initialize a semaphore NOPSHARED with COUNT.
  ret = sem_init (&psema->sema, 0, count);
  if (-1 == ret)
    {
      delete psema;
      return NULL;
    }
  return psema;
}

void
baanSemaphoreDestroy (pbaanSemaphore_t sema)
{
  if (sema)
    {
      /* Close descriptor for named semaphore SEM.  */
      sem_close (&sema->sema);
      delete sema;
    }
}

void
baanSemaphoreUp (pbaanSemaphore_t sema)
{
  /* Post SEM.  */
  sem_post (&sema->sema);
}

void
baanSemaphoreDown (pbaanSemaphore_t sema)
{
  // Wait for SEM being posted.
  sem_wait (&sema->sema);
}
