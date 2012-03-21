#include "BumblebeeDriver.h"
#include <stdio.h>
#include <stdlib.h>
#include <dc1394/dc1394.h>
#include <sys/time.h>

static dc1394camera_t *camera = NULL;
static dc1394video_frame_t *frame=NULL;
static dc1394_t * d=NULL;
static dc1394error_t err;

// storage for deinterlaced data
static uint8_t *stereoData=NULL;

// type of interpolation it should use
static int interpType=0;

// frame dimensions
static uint32_t width=0;
static uint32_t height=0;

// time stamp of the most recent image
static uint64_t timestamp=0;

// maximum allowed difference between current time and camera time stamp
static int maxDelay;

void handleErr( char *message ) {
    if( err < 0 ) {
        fprintf(stderr,"%s\n",message);
        exit(0); // return a value here
    }
}

uint64_t BCVBB_camera_timestamp() {
    return timestamp;
}

uint32_t BCVBB_camera_width() {
    return width;
}

uint32_t BCVBB_camera_height() {
    return height;
}

int BCVBB_camera_init( int interpolation , int fps , int maxDelayMilli ) {
    dc1394camera_list_t * list;

    if( width != 0 ) {
        fprintf(stderr,"Bumblebee init called twice!\n");
        return false;
    }

    interpType = interpolation;
    maxDelay = maxDelayMilli;

    d = dc1394_new();
    if (!d) {
        fprintf(stderr,"dc1394_new failed");
        return false;
    }
    err=dc1394_camera_enumerate(d, &list);
    handleErr("Failed to enumerate cameras");

    if (list->num == 0) {
        fprintf(stderr,"No cameras found\n");
        return false;
    }

    camera = dc1394_camera_new (d, list->ids[0].guid);
    if (!camera) {
        dc1394_log_error("Failed to initialize camera with guid %"PRIx64,
                list->ids[0].guid);
        return 1;
    }
    dc1394_camera_free_list (list);

//    printf("Using camera with GUID %"PRIx64"\n", camera->guid);

    /*-----------------------------------------------------------------------
     *  setup capture for format 7
     *-----------------------------------------------------------------------*/
    dc1394video_mode_t mode = DC1394_VIDEO_MODE_FORMAT7_3;
    int bus_period = 125; // valid for ISO 400  see

    dc1394_video_set_iso_speed(camera, DC1394_ISO_SPEED_400);
    dc1394_video_set_mode(camera, mode);

    err = dc1394_format7_set_roi(camera, mode,
                                 DC1394_COLOR_CODING_RAW16,
                                 DC1394_USE_MAX_AVAIL, // use max packet size
                                 0, 0, // left, top
                                 640, 480);  // width, height
    DC1394_ERR_RTN(err,"Unable to set Format7 mode 0.\nEdit the example file manually to fit your camera capabilities");

    err=dc1394_capture_setup(camera, 6, DC1394_CAPTURE_FLAGS_DEFAULT);
    DC1394_ERR_CLN_RTN(err, dc1394_camera_free(camera), "Error capturing");

    // get camera information
    dc1394_get_image_size_from_video_mode(camera, mode, &width, &height);

    // set the framerate by adjusting packet bytes
    // http://damien.douxchamps.net/ieee1394/libdc1394/v2.x/faq/#How_can_I_work_out_the_packet_size_for_a_wanted_frame_rate
    // http://damien.douxchamps.net/ieee1394/libdc1394/v1.x/faq/#How_can_I_work_out_the_packet_size_for_a_wanted_frame_rate
    // Version 1 appears to have more useful information than version 2
    if( fps > 0 ) {
        uint32_t min_bytes,max_bytes,data_depth;
        err=dc1394_format7_get_packet_parameters(camera, mode, &min_bytes, &max_bytes);
        handleErr("dc1394_format7_get_packet_parameters");
        err=dc1394_format7_get_data_depth(camera,mode,&data_depth);
        handleErr("dc1394_format7_get_packet_parameters");

        uint32_t num_packets = (int) (1.0/(bus_period*1e-6*fps) + 0.5);
        if( num_packets <= 0 ) num_packets = 1;
        if( num_packets > 4095 ) num_packets = 4095;

        printf("Setting FPS %d  num_packets = %d  min_bytes %d  max_bytes %d data_depth %d\n",fps,num_packets,min_bytes,max_bytes,data_depth);

        uint32_t denominator = num_packets*8;
        uint32_t packet_size = (width*height*data_depth + denominator - 1)/denominator;
        // enforce that packet_size must be a multiple of min_bytes
        packet_size = (packet_size/min_bytes)*min_bytes;
        if( packet_size < min_bytes ) packet_size = min_bytes;
        if( packet_size > max_bytes ) packet_size = max_bytes;

        printf("num_packets %d packet_size %d  max  %d\n",num_packets,packet_size,max_bytes);

        err=dc1394_format7_set_packet_size(camera, mode, packet_size);
        handleErr("dc1394_format7_set_packet_size");
    }

    // declare stereo data.  will contain bayer image
    stereoData = (uint8_t *)malloc(width*height*2);

    /*-----------------------------------------------------------------------
     *  have the camera start sending us data
     *-----------------------------------------------------------------------*/
    err=dc1394_video_set_transmission(camera,DC1394_ON);
    if (err!=DC1394_SUCCESS) {
        dc1394_log_error("unable to start camera iso transmission");
        dc1394_capture_stop(camera);
        dc1394_camera_free(camera);
        exit(1);
    }

    return true;
}

int BCVBB_camera_grab( uint8_t* rgb ) {
    if( width == 0 ) {
        fprintf(stderr,"Bumblebee init not called!\n");
        return false;
    }

    int i;
    struct timeval time;

    // skip over lagged images
    for( i = 0; i < 10; i++ ) {
        err=dc1394_capture_dequeue(camera, DC1394_CAPTURE_POLICY_WAIT, &frame);
        if (err!=DC1394_SUCCESS) {
            dc1394_log_error("unable to capture");
            dc1394_capture_stop(camera);
            dc1394_camera_free(camera);
            return false;
        }
        gettimeofday(&time,NULL);
        long ms = time.tv_sec*1000 + time.tv_usec/1000;
        if( maxDelay <= 0 || (ms-(frame->timestamp/1000) <= maxDelay) )
            break;
        else
            dc1394_capture_enqueue(camera, frame);
    }
    timestamp = frame->timestamp;


    // stereo crap
    err=dc1394_deinterlace_stereo(frame->image,stereoData,width,height*2);
    DC1394_ERR_RTN(err,"dc1394_deinterlace_stereo error");

    // colorize crap
    err=dc1394_bayer_decoding_8bit(stereoData,rgb,width,height*2,DC1394_COLOR_FILTER_BGGR,interpType);
    DC1394_ERR_RTN(err,"dc1394_bayer_decoding_8bit error");

    dc1394_capture_enqueue(camera, frame);

    return true;
}

void BCVBB_camera_shutdown() {
    if( width == 0 ) {
        fprintf(stderr,"shutdown called before init\n");
        return;
    }

    err=dc1394_video_set_transmission(camera,DC1394_OFF);
    if( err < 0 )
        fprintf(stderr,"Failed to shutdown bumblebee camera\n");

   dc1394_capture_stop(camera);
   dc1394_video_set_transmission(camera, DC1394_OFF);
   dc1394_camera_free(camera);
   dc1394_free (d);
}
