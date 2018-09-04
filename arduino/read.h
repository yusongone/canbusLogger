
#include <Canbus.h>
#include <defaults.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>
#include <global.h>

#include <QueueArray.h>
#ifndef CANQUEUE
#define CANQUEUE


typedef void (*receiveHandler)(tCAN *message);

class CanQueue{
  public:
    void request(unsigned char pid);
    void receiveData(receiveHandler f);
  private:
};

extern CanQueue CQ;

#endif


