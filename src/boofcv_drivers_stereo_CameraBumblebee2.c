#include "boofcv_drivers_stereo_CameraBumblebee2.h"
#include "BumblebeeDriver.h"

#include <stdio.h>

int sizeRGB = 0;

/*
 * Class:     boofcv_drivers_stereo_CameraBumblebee2
 * Method:    shutdown
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_boofcv_drivers_stereo_CameraBumblebee2_shutdown
  (JNIEnv *env, jobject obj) {
    BCVBB_camera_shutdown();
}

/*
 * Class:     boofcv_drivers_stereo_CameraBumblebee2
 * Method:    getWidth
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_boofcv_drivers_stereo_CameraBumblebee2__1getWidth
  (JNIEnv *env, jobject obj) {
    return (jint)BCVBB_camera_width();
}

/*
 * Class:     boofcv_drivers_stereo_CameraBumblebee2
 * Method:    getHeight
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_boofcv_drivers_stereo_CameraBumblebee2__1getHeight
  (JNIEnv *env, jobject obj) {
    return (jint)BCVBB_camera_height()*2;
}

/*
 * Class:     boofcv_drivers_stereo_CameraBumblebee2
 * Method:    grabRGB
 * Signature: ([B)Z
 */
JNIEXPORT jboolean JNICALL Java_boofcv_drivers_stereo_CameraBumblebee2_grabRGB
  (JNIEnv *env, jobject obj, jbyteArray array ) {
    if( array == NULL ) {
        fprintf(stderr,"Array is null.\n");
        return false;
    }

    jsize size = (*env)->GetArrayLength(env, array);
    if( size < sizeRGB ) {
        fprintf(stderr,"Requested array is too small to capture RGB image\n");
        return false;
    }

    jbyte* rgb = (*env)->GetByteArrayElements(env,array,NULL);

    int ret = BCVBB_camera_grab((uint8_t*)rgb);

    if( ret ) {
        (*env)->ReleaseByteArrayElements(env, array, rgb, 0);
    }
    return ret;
}

/*
 * Class:     boofcv_drivers_stereo_CameraBumblebee2
 * Method:    _init
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_boofcv_drivers_stereo_CameraBumblebee2__1init
  (JNIEnv *env, jobject obj, jint interpType , jint fps , jint maxLagMilli ) {

    if( BCVBB_camera_init((int)interpType,(int)fps,(int)maxLagMilli) == true ) {
        sizeRGB = (int)(BCVBB_camera_width()*BCVBB_camera_height()*2*3);
        return true;
    }
   return false;
}

/*
 * Class:     boofcv_drivers_stereo_CameraBumblebee2
 * Method:    getTimeStamp
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_boofcv_drivers_stereo_CameraBumblebee2_getTimeStamp
  (JNIEnv *env, jobject obj) {
    return (jlong)BCVBB_camera_timestamp();
}
