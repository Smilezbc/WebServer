#ifndef WEBSERVER_CALLBACKS_H
#define WEBSERVER_CALLBACKS_H

#include "TimeStamp.h"
namespace webServer
{
    typedef void (*EventCallback)();
    typedef void (*ReadEventCallback)(TimeStamp time);
}
#endif