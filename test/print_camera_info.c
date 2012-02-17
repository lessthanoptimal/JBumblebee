/*
 * Get one  image using libdc1394 and store it as portable pix map
 *    (ppm). Based on 'grab_gray_image' from Olaf Ronneberge
 *
 * Written by Damien Douxchamps <ddouxchamps@users.sf.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdint.h>
#include <dc1394/dc1394.h>
#include <stdlib.h>
#include <inttypes.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#define IMAGE_FILE_NAME "image.ppm"

/*-----------------------------------------------------------------------
 *  Releases the cameras and exits
 *-----------------------------------------------------------------------*/
void cleanup_and_exit(dc1394camera_t *camera)
{
    dc1394_video_set_transmission(camera, DC1394_OFF);
    dc1394_capture_stop(camera);
    dc1394_camera_free(camera);
    exit(1);
}

void printVideoMode( int mode ) {
    char * str[]={"DC1394_VIDEO_MODE_160x120_YUV444",
                    "DC1394_VIDEO_MODE_320x240_YUV422",
                    "DC1394_VIDEO_MODE_640x480_YUV411",
                    "DC1394_VIDEO_MODE_640x480_YUV422",
                    "DC1394_VIDEO_MODE_640x480_RGB8",
                    "DC1394_VIDEO_MODE_640x480_MONO8",
                    "DC1394_VIDEO_MODE_640x480_MONO16",
                    "DC1394_VIDEO_MODE_800x600_YUV422",
                    "DC1394_VIDEO_MODE_800x600_RGB8",
                    "DC1394_VIDEO_MODE_800x600_MONO8",
                    "DC1394_VIDEO_MODE_1024x768_YUV422",
                    "DC1394_VIDEO_MODE_1024x768_RGB8",
                    "DC1394_VIDEO_MODE_1024x768_MONO8",
                    "DC1394_VIDEO_MODE_800x600_MONO16",
                    "DC1394_VIDEO_MODE_1024x768_MONO16",
                    "DC1394_VIDEO_MODE_1280x960_YUV422",
                    "DC1394_VIDEO_MODE_1280x960_RGB8",
                    "DC1394_VIDEO_MODE_1280x960_MONO8",
                    "DC1394_VIDEO_MODE_1600x1200_YUV422",
                    "DC1394_VIDEO_MODE_1600x1200_RGB8",
                    "DC1394_VIDEO_MODE_1600x1200_MONO8",
                    "DC1394_VIDEO_MODE_1280x960_MONO16",
                    "DC1394_VIDEO_MODE_1600x1200_MONO16",
                    "DC1394_VIDEO_MODE_EXIF",
                    "DC1394_VIDEO_MODE_FORMAT7_0",
                    "DC1394_VIDEO_MODE_FORMAT7_1",
                    "DC1394_VIDEO_MODE_FORMAT7_2",
                    "DC1394_VIDEO_MODE_FORMAT7_3",
                    "DC1394_VIDEO_MODE_FORMAT7_4",
                    "DC1394_VIDEO_MODE_FORMAT7_5",
                    "DC1394_VIDEO_MODE_FORMAT7_6",
                    "DC1394_VIDEO_MODE_FORMAT7_7"};

    if( mode < DC1394_VIDEO_MODE_160x120_YUV444 || mode > DC1394_VIDEO_MODE_FORMAT7_7 )
        return;

    printf("  Mode [%d] = %s\n",mode,str[mode-DC1394_VIDEO_MODE_160x120_YUV444]);
}

void printFrameRates( int rate )
{
    char *name[]={"DC1394_FRAMERATE_1_875",
              "DC1394_FRAMERATE_3_75",
              "DC1394_FRAMERATE_7_5",
              "DC1394_FRAMERATE_15",
              "DC1394_FRAMERATE_30",
              "DC1394_FRAMERATE_60",
              "DC1394_FRAMERATE_120",
              "DC1394_FRAMERATE_240"};

    if( rate < DC1394_FRAMERATE_1_875 || rate > DC1394_FRAMERATE_240 )
        return;

    printf("    rate[%d] = %s\n",rate,name[rate-DC1394_FRAMERATE_1_875]);
}

void printColorCodings( int colors )
{
    char *name[]={"DC1394_COLOR_CODING_MONO8",
                  "DC1394_COLOR_CODING_YUV411",
                  "DC1394_COLOR_CODING_YUV422",
                  "DC1394_COLOR_CODING_YUV444",
                  "DC1394_COLOR_CODING_RGB8",
                  "DC1394_COLOR_CODING_MONO16",
                  "DC1394_COLOR_CODING_RGB16",
                  "DC1394_COLOR_CODING_MONO16S",
                  "DC1394_COLOR_CODING_RGB16S",
                  "DC1394_COLOR_CODING_RAW8",
                  "DC1394_COLOR_CODING_RAW16"};

    if( colors < DC1394_COLOR_CODING_MONO8 || colors > DC1394_COLOR_CODING_RAW16 )
        return;

    printf("    color[%d] = %s\n",colors,name[colors-DC1394_COLOR_CODING_MONO8]);
}

int main(int argc, char *argv[])
{
    FILE* imagefile;
    dc1394camera_t *camera;
    unsigned int width, height;
    dc1394video_frame_t *frame=NULL;
    //dc1394featureset_t features;
    dc1394_t * d;
    dc1394camera_list_t * list;
    dc1394error_t err;
    int i,j;

    d = dc1394_new ();
    if (!d)
        return 1;
    err=dc1394_camera_enumerate (d, &list);
    DC1394_ERR_RTN(err,"Failed to enumerate cameras");

    if (list->num == 0) {
        dc1394_log_error("No cameras found");
        return 1;
    }

    printf("Total cameras found %d\n",list->num);

    camera = dc1394_camera_new (d, list->ids[0].guid);
    if (!camera) {
        dc1394_log_error("Failed to initialize camera with guid %llx", list->ids[0].guid);
        return 1;
    }
    dc1394_camera_free_list (list);

    printf("Using camera with GUID %"PRIx64"\n", camera->guid);

    dc1394video_modes_t video_modes;
    err=dc1394_video_get_supported_modes(camera,&video_modes);
    DC1394_ERR_RTN(err,"Failed get camera modes");

    printf("Video Modes:\n");
    for( i = 0; i < video_modes.num; i++ ) {
        printVideoMode(video_modes.modes[i]);
        if( video_modes.modes[i] < DC1394_VIDEO_MODE_FORMAT7_0 ) {
           dc1394framerates_t rates;
            err=dc1394_video_get_supported_framerates(camera,video_modes.modes[i],&rates);
            DC1394_ERR_RTN(err,"Failed get mode frame rates");
            for( j = 0; j < rates.num; j++ ) {
                printFrameRates(rates.framerates[j]);
            }
        } else {
            dc1394color_codings_t codings;
            err=dc1394_format7_get_color_codings(camera,video_modes.modes[i],&codings);
            DC1394_ERR_RTN(err,"Failed get format7 color codings");
            for( j = 0; j < codings.num; j++ ) {
                printColorCodings(codings.codings[j]);
            }
            uint32_t width,height;
            err=dc1394_format7_get_max_image_size(camera,video_modes.modes[i],&width,&height);
            DC1394_ERR_RTN(err,"Failed get format7 max size");
            printf("    Max dimension = %d  %d\n",width,height);
        }
    }



    dc1394_camera_free(camera);
    dc1394_free (d);
    return 0;
}
