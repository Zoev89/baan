#ifndef _BAANSEMAPHORE_H_
#define _BAANSEMAPHORE_H_

typedef struct _baanSemaphore_t *pbaanSemaphore_t;

pbaanSemaphore_t baanSemaphoreCreate(int count);
void baanSemaphoreDestroy(pbaanSemaphore_t sema);
void baanSemaphoreUp(pbaanSemaphore_t sema);
void baanSemaphoreDown(pbaanSemaphore_t sema);

#endif // _BAANSEMAPHORE_H_

