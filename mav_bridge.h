#ifndef MAV_BRIDGE_H
#define MAV_BRIDGE_H

#include"serialcom.h"

extern Serialcom comm_link;

#define MAVLINK_USE_CONVENIENCE_FUNCTIONS

#include "mavlink/v2.0/mavlink_types.h"

#define MAVLINK_USE_CONVENIENCE_FUNCTIONS

extern mavlink_system_t mavlink_system;


static inline void comm_send_ch(mavlink_channel_t chan, uint8_t ch){

    const char c = (const char)ch;
    if (chan == comm_link.mav_ch){
        comm_link.writeData(&c, 1);
    }
}
#include "mavlink/v2.0/ardupilotmega/mavlink.h"
#include "mavlink/v2.0/common/mavlink.h"

#endif // MAV_BRIDGE_H
