LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := ApplicationLib
LOCAL_MODULE_TAGS := eng
LOCAL_CERTIFICATE := platform
LOCAL_SDK_VERSION := 8
LOCAL_SRC_FILES := 	$(call all-java-files-under,src)
include $(BUILD_STATIC_JAVA_LIBRARY)

ifeq ($(TARGET_BUILD_APPS),)
include $(call all-makefiles-under, $(LOCAL_PATH))
endif
