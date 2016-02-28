#ifndef USBINOUT_H_
#define USBINOUT_H_

int openUsbPort(void);
void closeUsbPort(void);

// zorg dat bufferOnt multiple size heeft van 64
int zendUsbBuffer (unsigned char *buffer, unsigned char *bufferOnt, int lengte, int *stopBit);


#endif /*USBINOUT_H_*/
