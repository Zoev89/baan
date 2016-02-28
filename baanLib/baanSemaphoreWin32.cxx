#include "baanSemaphore.h"
#include "windows.h"

struct _baanSemaphore_t
{
   HANDLE sema;
};

pbaanSemaphore_t baanSemaphoreCreate(int count)
{
   pbaanSemaphore_t psema;
   
   psema       = new _baanSemaphore_t;
   if (psema == NULL) return psema;
   psema->sema = CreateSemaphore(NULL,count,1000,NULL);
   return psema;
}

void baanSemaphoreDestroy(pbaanSemaphore_t sema)
{
   if (sema)
   {
   CloseHandle(sema->sema);
   delete sema;
   }
}

void baanSemaphoreUp(pbaanSemaphore_t sema)
{
   SetEvent(sema->sema);
   ReleaseSemaphore(sema->sema,1,NULL);
}

void baanSemaphoreDown(pbaanSemaphore_t sema)
{
    WaitForSingleObject(sema->sema, INFINITE);
}

