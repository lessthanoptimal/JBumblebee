#include "BumblebeeDriver.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    printf("Before init\n");

    if( !BCVBB_camera_init(0,0,1000) ) {
        printf("Init failed!\n");
        return 0;
    }

    int w = (int)BCVBB_camera_width();
    int h = (int)BCVBB_camera_height();

    printf("width  = %d\n",w);
    printf("height = %d\n",h);

    uint8_t *rgb = malloc(w*h*3*2);
    BCVBB_camera_grab(rgb);

    BCVBB_camera_shutdown();

    FILE* imagefile=fopen("Part.ppm","wb");
    fprintf(imagefile,"P6\n%u %u\n255\n", w, h*2);
    fwrite(rgb, 1, 2*h*w*3, imagefile);
    fclose(imagefile);
    printf("wrote: Part.ppm (%d image bytes)\n",2*h*w*3);

    printf("DONE! Test passed\n");
    return 0;
}
