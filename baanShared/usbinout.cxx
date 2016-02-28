#include <stdio.h>
#include <string.h>
#include <usb.h>
#include "usbinout.h"

// for usb command structures
// avr069.h for the commands I redefined them again here
/*****************[ General command constants ]**************************/

#define CMD_SIGN_ON                         0x01
#define CMD_SET_PARAMETER                   0x02
#define CMD_GET_PARAMETER                   0x03
#define CMD_OSCCAL                          0x05
#define CMD_LOAD_ADDRESS                    0x06
#define CMD_FIRMWARE_UPGRADE                0x07
#define CMD_RESET_PROTECTION                0x0A

/*****************[ ISP command constants ]******************************/

#define CMD_ENTER_PROGMODE_ISP              0x10
#define CMD_LEAVE_PROGMODE_ISP              0x11
#define CMD_CHIP_ERASE_ISP                  0x12
#define CMD_PROGRAM_FLASH_ISP               0x13
#define CMD_READ_FLASH_ISP                  0x14
#define CMD_PROGRAM_EEPROM_ISP              0x15
#define CMD_READ_EEPROM_ISP                 0x16
#define CMD_PROGRAM_FUSE_ISP                0x17
#define CMD_READ_FUSE_ISP                   0x18
#define CMD_PROGRAM_LOCK_ISP                0x19
#define CMD_READ_LOCK_ISP                   0x1A
#define CMD_READ_SIGNATURE_ISP              0x1B
#define CMD_READ_OSCCAL_ISP                 0x1C
#define CMD_SPI_MULTI                       0x1D

/*****************[ Treinbaan command constants ]************************/
#define CMD_RXTX_DATA 0x20
// First byte must be the command, the second unsigned byte the amount of
// bytes that will be transmitted (max 246 56+64+64+64). So the first usb block will
// have 56 bytes in it and the following usb blocks will have 64 in them.
// The return command has the answer structure: is first byte this command.
// Second byte the status (OK) and the third byte the amount of bytes 
// you will recieve. The first recieved buffer will only have maximal
// 56 bytes of data at offset 8

#define CMD_UART_ONOFF 0x21
// The first byte will specify if the uart should be on (1)  or off (0)


#define TREIN_OFFSET 8

/*****************[ status constants ]***************************/

// Success
#define STATUS_CMD_OK                       0x00


//
// end avr096 include
//


static usb_dev_handle *usbHandle = NULL;
static struct usb_device *usbDev = NULL;
static int usb_interface = -1;
static int ep;                  // usb endpoint for read interface
static int usbActive;


int
uartOnOff (int onOff)
{
  int lengteRet = 0;
  int len;
  char string[64];

  string[0] = CMD_UART_ONOFF;
  string[1] = onOff;
  len = usb_bulk_write (usbHandle, 0x2, string, 2, 5000);
  if (len != 2)
    {
      return 1;
    }
  len = usb_bulk_read (usbHandle, ep, string, 64, 5000);
  if ((len < 2) || (string[0] != CMD_UART_ONOFF))
    {
      return 2;
    }
  return 0;
}

// retoneerd het aantal ontvangen bytes
int
zendUsbBuffer (unsigned char *buffer, unsigned char *bufferOnt, int lengte,
               int *stopBit)
{
  int lengteRet = 0;
  // we kunnen maximaal 246 bytes sturen
  // (8 bit aantal)
  if (lengte <= 246)
    {
      char string[64];
      int i, l, len, n, c;

      // Het eerste bericht heeft het commando
      // en lengte. Daarna is er nog 56 bytes over
      // om meteen me te sturen

      l = n = lengte;
      if (l > 56)
        {
          l = 56;
        }
      string[0] = CMD_RXTX_DATA;
      string[1] = lengte;
      string[2] = 143;  // default getal voor 50ms timing jehebt zeker 10 bytes idle nodig tussen aantal verzonden en terug koment
      for (i = 0; i < l; i++)
        {
          string[i + 8] = buffer[i];
        }
      usb_bulk_write (usbHandle, 0x2, string, l + 8, 1000);
      n -= l;
      len = usb_bulk_read (usbHandle, ep, string, 64, 1000);
      if ((len < 8) || (string[0] != CMD_RXTX_DATA))
        {
          // error in usb communicatie
          return -1;
        }
      lengteRet = (int) ((unsigned char) string[2]);
      *stopBit = (string[3] == 0);
      if (string[4] & 0x8)
          printf("OverRun %x\n",string[4]&0xff);
      memcpy ((char *) bufferOnt, &string[8], 56);
      c = 56;
      while (n > 0)
        {
          // Nu gaan we blokjes van 64 bytes sturen
          // waarbij n nog het aantal overige bytes is
          // l de maximale lengte tot 64 en c de index
          // in de buffer
          l = n;
          if (l > 64)
            l = 64;
          usb_bulk_write (usbHandle, 0x2, (char *) &buffer[c], l, 1000);
          len =
            usb_bulk_read (usbHandle, ep, (char *) &bufferOnt[c], 64, 1000);
          if ((len <= 0) || (len > 64))
            {
              // error in usb communicatie
              return -2;
            }
          c += 64;
          n -= 64;
        }
    }
  else
    {
      return -3;
    }
  return lengteRet;
}

int
zoekContact (void)
{
  int len, count;
  char string[64];

  // we doen dit doormiddel van naar het sign on bericht te sturen en te kijken
  // of we dit netjes terug krijgt. Ik gebruik korte timeouts zodat we snel
  // de zaak gevonden hebben en niet eeuwig zitten te wachten op een dood
  // device.
  count = 0;
  do
    {
      string[0] = CMD_SIGN_ON;
      string[1] = 0;
      usb_bulk_write (usbHandle, 0x2, string, 2, 50);

      len = usb_bulk_read (usbHandle, ep, string, 64, 50);
    }
  while ((len <= 0) && (count++ < 5));
  if (count >= 5)
    {
      // contact niet gevonden
      return 1;
    }
  string[len] = 0;
  if (strcmp (&string[3], "AVRISP_MK2") != 0)
    {
      return 2;
    }

  // flush de receiver
  do
    {
      len = usb_bulk_read (usbHandle, ep, string, 64, 50);
    }
  while (len > 0);
  return 0;
}

int
openUsbPort (void)
{
  struct usb_bus *bus;
  struct usb_device *dev;
  int i;
  unsigned int t1, t2;

  if (usbActive == 0)
    {
      usb_init ();
      usb_find_busses ();
      usb_find_devices ();
      for (bus = usb_get_busses (); bus; bus = bus->next)
        {
          for (dev = bus->devices; dev; dev = dev->next)
            {
              if ((dev->descriptor.idVendor == 0x3eb)
                  && (dev->descriptor.idProduct == 0x2104))
                {
                  usbHandle = usb_open (dev);
                  if (!usbHandle)
                    {
                      fprintf (stderr,
                               "Warning: cannot open USB device: %s\n",
                               usb_strerror ());
                    }
                  else
                    {
                      usbDev = dev;
                    }
                }
            }
        }
      if ((usbHandle) && (usbDev))
        {
          char string[256 + 32], stringOnt[256 + 32];
          int len, i, l, x, eerste;

          len = usb_get_string_simple (usbHandle, usbDev->descriptor.iProduct,
                                       string, sizeof (string));
          if (len > 0)
            {
              printf ("Found %s %d\n", string,
                      usbDev->config[0].bConfigurationValue);
            }
          if (usb_set_configuration
              (usbHandle, usbDev->config[0].bConfigurationValue))
            {
              // error
              printf ("usb_set_configuration failed\n");
              return -1;
            }

          usb_interface =
            usbDev->config[0].interface[0].altsetting[0].bInterfaceNumber;
          if (usb_claim_interface (usbHandle, usb_interface))
            {
              // error
              printf ("usb_claim_interface failed\n");
              return -1;
            }

          ep = -1;
          /* Try finding out what our read endpoint is. */
          for (i = 0;
               i < usbDev->config[0].interface[0].altsetting[0].bNumEndpoints;
               i++)
            {
              int possible_ep =
                usbDev->config[0].interface[0].altsetting[0].
                endpoint[i].bEndpointAddress;

              if ((possible_ep & USB_ENDPOINT_DIR_MASK) != 0)
                {
                  ep = possible_ep;
                  break;
                }
            }
          if (ep == -1)
            {
              printf
                ("usbdev_open(): cannot find a read endpoint, using 0x%02x\n",
                 0x82);
              ep = 0x82;
            }

          if (zoekContact ())
            {
              printf ("contact niet kunnen vinden\n");
              return -1;
            }
          if (uartOnOff (1))
            {
              // uart onoff kon geen commando's sturen
              return 3;
            }
          usbActive = 1;
          return 0;
        }
      return 1;
    }
  return 0;
}


void
closeUsbPort ()
{
  if (usbActive)
    {
      uartOnOff (0);
    }
  if (usb_interface > -1)
    {
      usb_release_interface (usbHandle, usb_interface);
      usb_interface = -1;
    }
  if (usbHandle)
    {
      usb_reset (usbHandle);

      usb_close (usbHandle);
      usbHandle = NULL;
    }
  usbActive = 0;
}
