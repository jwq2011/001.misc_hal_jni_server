LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_SHARED_LIBRARIES := liblog libcutils
LOCAL_SRC_FILES := misc_hal.c
LOCAL_MODULE := misc_hal.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_TAGS := eng  
include $(BUILD_SHARED_LIBRARY)
     

 

    
  


   

  


