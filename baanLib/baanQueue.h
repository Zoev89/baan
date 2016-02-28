#ifndef _BAANQUEUE_H_
#define _BAANQUEUE_H_

typedef struct _baanQueue_t *pbaanQueue_t;

pbaanQueue_t baanQueueCreate(int diepte, const char *naam);

// queue destroy als er nog een taak op de
// queue actief zijn leidt tot acces violations
void baanQueueDestroy(pbaanQueue_t queue);

int baanQueueSend(pbaanQueue_t queue, int buf[4]);
void baanQueueReceive(pbaanQueue_t queue, int buf[4]);

// return aantal elementen voor non blocking read
int baanQueueCount(pbaanQueue_t queue);

#endif // _BAANQUEUE_H_

