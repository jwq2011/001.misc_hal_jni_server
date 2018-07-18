# Android.mk
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE:= misc_hal_jni

LOCAL_MODULE_PATH := /home/roadrover/WorkLib/01.SourceCode/001.mic_hal/misc_hal_jni

LOCAL_SRC_FILES:= MiscHalService.cpp

LOCAL_SHARED_LIBRARIES := \
	libandroid_runtime \
	libcutils \
	libhardware \
	libhardware_legacy \
	libnativehelper \
#	libsystem_server \
	libutils \
	libui \
	libsurfaceflinger_client
       
  
LOCAL_C_INCLUDES += \
	$(JNI_H_INCLUDE) \
	hardware/imx/mx6/misc_hal

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)
