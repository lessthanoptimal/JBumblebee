#ifndef __BUMBLEBEEDRIVER_H__
#define __BUMBLEBEEDRIVER_H__

#include <stdint.h>
#include <inttypes.h>


#define true 1
#define false 0

uint64_t BCVBB_camera_timestamp();

uint32_t BCVBB_camera_width();

uint32_t BCVBB_camera_height();

int BCVBB_camera_init( int interpolation , int fps , int maxDelayMilli );

int BCVBB_camera_grab( uint8_t* rgb );

void BCVBB_camera_shutdown();

#endif
