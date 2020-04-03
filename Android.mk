# example: Android Native Library makefile for jsmn
# contributed by Bin Li <bin.li@windriver.com>

LOCAL_PATH := $(call my-dir)
libjsmn_lib_path := .
libjsmn_c_includes := $(LOCAL_PATH)/$(libjsmn_lib_path) \

libjsmn_local_src_c_files := \
	$(libjsmn_lib_path)/jsmn.c \

include $(CLEAR_VARS)
LOCAL_MODULE    := libjsmn
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)
LOCAL_C_INCLUDES:= $(libjsmn_c_includes)
LOCAL_SRC_FILES := $(libjsmn_local_src_c_files)
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libjsmn
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)
LOCAL_C_INCLUDES:= $(libjsmn_c_includes)
LOCAL_SRC_FILES := $(libjsmn_local_src_c_files)
include $(BUILD_SHARED_LIBRARY)

