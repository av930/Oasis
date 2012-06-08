#include <jni.h>
#include <stdio.h>
#include <Wca3.h>
#include "debug_utils.h"

static const char *classPathName = "com/lge/wica/Wica";

static CWca3* sWca3 = new CWca3();

static jint create_jni(JNIEnv *env, jobject thiz, jstring filename);
static jintArray decode_jni(JNIEnv *env, jobject thiz, jint nHandle, jint nFrame);
static jint getFrameNum_jni(JNIEnv *env, jobject thiz, jint nHandle);
static jint getWidth_jni(JNIEnv *env, jobject thiz, jint nHandle);
static jint getHeight_jni(JNIEnv *env, jobject thiz, jint nHandle);
static jboolean setAlpha_jni(JNIEnv *env, jobject thiz, jint nHandle, jint nAlpha);
static jboolean destroy_jni(JNIEnv *env, jobject thiz, jint nHandle);

static JNINativeMethod methods[] = {
  {"create_jni", "(Ljava/lang/String;)I", (void*)create_jni },
  {"decode_jni", "(II)[I", (void*)decode_jni },
  {"getFrameNum_jni", "(I)I", (void*)getFrameNum_jni },
  {"getWidth_jni", "(I)I", (void*)getWidth_jni },
  {"getHeight_jni", "(I)I", (void*)getHeight_jni },
  {"setAlpha_jni", "(II)Z", (void*)setAlpha_jni },
  {"destroy_jni", "(I)Z", (void*)destroy_jni },
};

static jint create_jni(JNIEnv *env, jobject thiz, jstring filename) {
    CAPP_TRACE("create_jni wica");
    CWca3* pWca3 = new CWca3();
	pWca3->Load(env, (char*)env->GetStringUTFChars(filename, NULL));
	return (int)pWca3;
}

static jintArray decode_jni(JNIEnv *env, jobject thiz, jint nHandle, jint nFrame) {
	CWca3* pWca3 = (CWca3*)nHandle;
    CAPP_TRACE("decode_jni wica");
	pWca3->Decode(nFrame);
	return (jintArray)pWca3->GetImage();
}

static jint getFrameNum_jni(JNIEnv *env, jobject thiz, jint nHandle) {
	CWca3* pWca3 = (CWca3*)nHandle;
    CAPP_TRACE("getFrameNum_jni wica");
	return pWca3->GetFrameNum();
}

static jint getWidth_jni(JNIEnv *env, jobject thiz, jint nHandle) {
	CWca3* pWca3 = (CWca3*)nHandle;
    CAPP_TRACE("getWidth_jni wica");
	return pWca3->GetWidth();
}

static jint getHeight_jni(JNIEnv *env, jobject thiz, jint nHandle) {
	CWca3* pWca3 = (CWca3*)nHandle;
    CAPP_TRACE("getHeight_jni wica");
	return pWca3->GetHeight();
}

static jboolean setAlpha_jni(JNIEnv *env, jobject thiz, jint nHandle, jint nAlpha) {
	CWca3* pWca3 = (CWca3*)nHandle;
    CAPP_TRACE("setAlpha_jni wica");
	pWca3->SetAlpha(nAlpha);
    return TRUE;
}

static jboolean destroy_jni(JNIEnv *env, jobject thiz, jint nHandle) {
	CWca3* pWca3 = (CWca3*)nHandle;
    CAPP_TRACE("destroy_jni wica");
    if(pWca3)
        delete pWca3;
	return TRUE;
}


/*
 * Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;

    clazz = env->FindClass(className);
    if (clazz == NULL) {
        fprintf(stderr, "Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        fprintf(stderr, "RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

/*
 * Register native methods for all classes we know about.
 */
static int registerNatives(JNIEnv* env)
{
  if (!registerNativeMethods(env, classPathName,
                 methods, sizeof(methods) / sizeof(methods[0]))) {
    return JNI_FALSE;
  }

  return JNI_TRUE;
}

/*
 * Set some test stuff up.
 *
 * Returns the JNI version on success, -1 on failure.
 */

typedef union {
    JNIEnv* env;
    void* venv;
} UnionJNIEnvToVoid;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    jint result = -1;
    JNIEnv* env = NULL;
    
    printf("JNI_OnLoad");

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
        fprintf(stderr, "GetEnv failed");
        goto bail;
    }
    env = uenv.env;

    if (!registerNatives(env)) {
        fprintf(stderr, "registerNatives failed");
    }
    
    result = JNI_VERSION_1_4;
    
bail:
    return result;
}
