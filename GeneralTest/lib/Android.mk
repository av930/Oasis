LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# Source
LOCAL_SRC_FILES := android_memorytestnative_jni.cpp

LOCAL_MODULE_TAGS := eng
LOCAL_CERTIFICATE := platform
# Include path
LOCAL_C_INCLUDES := \
		$(JNI_H_INCLUDE)

# Output
LOCAL_MODULE := libmemorytestnative

#LOCAL_SHARED_LIBRARIES := libcutils

LOCAL_PRELINK_MODULE := false
# Build type
include $(BUILD_SHARED_LIBRARY)


