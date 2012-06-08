LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# Source
LOCAL_SRC_FILES := 	\
			Wca3.cpp \
			android_wica_jni.cpp \
			debug_utils.c	\
			wci_frame.c \
			wci_stripe_decoder.c \
			wci_encoder.c \
			wci_codec.c \
			wci_profile.c \
			compress/wci_huff_decompress.c \
			compress/wci_qsp_header.c \
			compress/wci_rle.c \
			compress/wci_vlc.c \
			compress/wci_qsp_compress.c \
			compress/wci_huff_comperss.c \
			compress/wci_qsp_stripe_decompress.c \
			compress/wci_huff_tabs.c \
			compress/wci_qsp.c \
			image/wci_image_component.c \
			image/wci_color_space_in.c \
			image/wci_color_space_out.c \
			image/wci_color_space_table.c \
			image/wci_image_convert.c \
			image/wci_image.c \
			utils/wci_emms.c \
			utils/wci_mem_align.c \
			wav_codec/wci_lpc.c \
			wav_codec/wci_wavelet.c \
			wav_codec/wci_adpcm.c \
			wav_codec/wci_wav_stripe_quant.c \
			wav_codec/wci_wav_stripe_trans.c \
			wav_codec/wci_wav_codec.c \
			wav_codec/wci_wav_block_dequant.c \
			wav_codec/wci_wav_block_quant.c \
			wav_codec/wci_wav_quant.c \
			wav_codec/wci_wav_stripe.c \
			wav_codec/wci_wav_layer.c \

LOCAL_MODULE_TAGS := optional

# Include path
LOCAL_C_INCLUDES := \
		$(call include-path-for, system-core)/cutils	\
		$(JNI_H_INCLUDE)

# Flags
LOCAL_CFLAGS := -DARCH_IS_32BIT -DARCH_IS_IA32 -DENCODER_SUPPORT#

# Output
LOCAL_MODULE := libwica

LOCAL_SHARED_LIBRARIES := libcutils

LOCAL_PRELINK_MODULE := false
# Build type
include $(BUILD_SHARED_LIBRARY)


