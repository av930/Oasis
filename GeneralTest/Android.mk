LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := eng
LOCAL_CERTIFICATE := platform
LOCAL_SRC_FILES := $(call all-java-files-under,src)
LOCAL_PACKAGE_NAME := generaltest
LOCAL_CERTIFICATE := platform
LOCAL_STATIC_JAVA_LIBRARIES := ApplicationLib
include $(BUILD_PACKAGE)
