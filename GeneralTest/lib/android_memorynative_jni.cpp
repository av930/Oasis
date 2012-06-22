#include <jni.h>
#include <stdio.h>
#include <string.h>

static const char *classPathName = "com/lge/generaltest/MemoryNative";

static void test1_jni(JNIEnv *env, jobject thiz);
static void test2_jni(JNIEnv *env, jobject thiz);

static JNINativeMethod methods[] = {
  {"test1_jni", "()V", (void*)test1_jni },
  {"test2_jni", "()V", (void*)test2_jni }
};
char s_source[10];
char* s_target ="0123456789012";
static void test1_jni(JNIEnv *env, jobject thiz) {
	memcpy(s_source,s_target, strlen(s_target));
}

static void test2_jni(JNIEnv *env, jobject thiz) {
	char source[10];
	char* target ="0123456789012";
	memcpy(source,target, strlen(target));	
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
