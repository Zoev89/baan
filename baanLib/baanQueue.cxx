#include "baanQueue.h"
#include "baanSemaphore.h"
#include "errorPrint.h"

#include <stdio.h>


struct _baanQueue_t
{
  pbaanSemaphore_t accesSema;
  pbaanSemaphore_t blockSema;
  int *buffer;
  int read;
  int write;
  int count;                    // aantal elementen in de queue ready for non blocking read
  int depth;
  const char *naam;
};


pbaanQueue_t
baanQueueCreate (int diepte, const char *naam)
{
  int *buf;
  pbaanQueue_t queue;

  if (NULL == naam)
    return NULL;
  buf = new int[diepte * 4];
  if (buf == NULL)
    return NULL;
  queue = new _baanQueue_t;
  if (queue == NULL)
    return NULL;
  queue->buffer = buf;
  queue->read = 0;
  queue->write = 0;
  queue->count = 0;
  queue->accesSema = baanSemaphoreCreate (1);
  if (queue->accesSema == NULL)
    return NULL;
  queue->blockSema = baanSemaphoreCreate (0);
  if (queue->blockSema == NULL)
    return NULL;
  queue->depth = diepte;
  queue->naam = naam;
  return queue;

}

void
baanQueueDestroy (pbaanQueue_t queue)
{
  // als er nog taken zitten te wachten dan kan deze
  // destroy verkeerd gaan
  baanSemaphoreDestroy (queue->accesSema);
  baanSemaphoreDestroy (queue->blockSema);
  delete queue->buffer;
  delete queue;
}

// queue destroy tijdens processing wordt niet opgevangen!
int
baanQueueSend (pbaanQueue_t queue, int buf[4])
{
  int write, writeNext, ret;

  baanSemaphoreDown (queue->accesSema);
  write = queue->write;
  writeNext = queue->write + 1;
  if (writeNext >= queue->depth)
    writeNext = 0;
  if (writeNext == queue->read)
    {
      // queue is vol
      ret = 1;
      printf ("Queue %s VOL\n", queue->naam);
    }
  else
    {
      // zet item in de queue
      ret = 0;
      write = write * 4;
      queue->buffer[write++] = buf[0];
      queue->buffer[write++] = buf[1];
      queue->buffer[write++] = buf[2];
      queue->buffer[write++] = buf[3];
      queue->write = writeNext;
      queue->count++;
      baanSemaphoreUp (queue->blockSema);
    }
  baanSemaphoreUp (queue->accesSema);
  return ret;

}

void
baanQueueReceive (pbaanQueue_t queue, int buf[4])
{
  int read, readNext;

  baanSemaphoreDown (queue->blockSema);
  // er is een item in de queue
  baanSemaphoreDown (queue->accesSema);
  read = queue->read;
  if (read == queue->write)
    {
      printf ("Queue %s inconsistency %d!\n", queue->naam, read);
      buf[0] = 0;
      buf[1] = 0;
      buf[2] = 0;
      buf[3] = 0;
      baanSemaphoreUp (queue->accesSema);
      return;
    }
  readNext = read + 1;
  if (readNext >= queue->depth)
    readNext = 0;
  read = read * 4;
  buf[0] = queue->buffer[read++];
  buf[1] = queue->buffer[read++];
  buf[2] = queue->buffer[read++];
  buf[3] = queue->buffer[read++];
  queue->read = readNext;
  queue->count--;
  baanSemaphoreUp (queue->accesSema);
}

int
baanQueueCount (pbaanQueue_t queue)
{
  return queue->count;
}
