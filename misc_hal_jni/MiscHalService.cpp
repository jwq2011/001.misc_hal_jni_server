#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <jni.h>
#include <misc_hal.h>

#define LOG_NDEBUG 0

struct misc_control_device_t *misc_hal_device = NULL;

static jlong GetBootConfig(JNIEnv* env, jobject thiz){
    ALOGI("Misc HAL JNI: GetBootConfig() is invoked.");
    if (misc_hal_device == NULL)	{
        ALOGI("Misc HAL JNI: led_hal_device was not fetched correctly.");
        return -1;
    }
    else
    {
        return misc_hal_device->get_boot_config(misc_hal_device);
    }
}

static jboolean SetBootConfig(JNIEnv* env, jobject thiz, jlong value){
    ALOGI("Misc HAL JNI: GetBootConfig() is invoked.");
    if (misc_hal_device == NULL)	{
        ALOGI("Misc HAL JNI: led_hal_device was not fetched correctly.");
        return -1;
    }
    else
    {
        return misc_hal_device->set_boot_config(misc_hal_device, value);
    }
}

static jboolean SetBootLights(JNIEnv* env, jobject thiz, jlong value){
    ALOGI("Misc HAL JNI: SetBootLights() is invoked.");
    if (misc_hal_device == NULL)	{
        ALOGI("Misc HAL JNI: led_hal_device was not fetched correctly.");
        return -1;
    }
    else
    {
        return misc_hal_device->set_boot_lights(misc_hal_device, value);
    }
}

static jboolean SetBootCamera(JNIEnv* env, jobject thiz, jlong value){
    ALOGI("Misc HAL JNI: SetBootCamera() is invoked.");
    if (misc_hal_device == NULL)	{
        ALOGI("Misc HAL JNI: led_hal_device was not fetched correctly.");
        return -1;
    }
    else
    {
        return misc_hal_device->set_boot_camera(misc_hal_device, value);
    }
}

static inline int misc_control_open(const struct hw_module_t* module,
        struct misc_control_device_t** device)
{
	return module->methods->open(module, MISC_HARDWARE_MODULE_ID,
	        (struct hw_device_t**) device);
}

static jboolean BootConfigInit(JNIEnv *env, jclass clazz)
{
	misc_module_t* module;

	ALOGE("**********start find hal *********");
	ALOGE(MISC_HARDWARE_MODULE_ID);

	if (hw_get_module(MISC_HARDWARE_MODULE_ID, (const hw_module_t**) &module)
	        == 0)
	{
		ALOGI("MiscService JNI: MISC Stub found.");
		if (misc_control_open(&module->hw_module, &misc_hal_device) == 0)
		{
			ALOGI("MiscService JNI: Get Stub operations.");
			return 0;
		}
	}

	ALOGE("MiscService JNI: Get Stub operations failed.");
	return -1;
}

#if 0
static jboolean SysSetBootConfig(JNIEnv *env, jobject clazz, jlong data)
{
    int fd;
    int res;
    jboolean ret;
//  int uRet;
    fd = open(SYSTEMDRIVENAME, O_RDWR);
    if(fd < 0) {
        ALOGW("Unable to open system driver: %s\n", strerror(errno));
        return false;
    }
    ALOGW("jni IOCTL_ANDROID_SETBOOTCONFIG ..\n");
    res= ioctl(fd, IOCTL_ANDROID_SETBOOTCONFIG, &data);
    close(fd);
    return true;
}

static jlong SysGetBootConfig(JNIEnv *env, jobject clazz)
{
    int fd;
    int res;
    jboolean ret;
    long uRet;
    fd = open(SYSTEMDRIVENAME, O_RDWR);
    if(fd < 0) {
        ALOGW("Unable to open system driver: %s\n", strerror(errno));
        return false;
    }
    ALOGW("jni IOCTL_ANDROID_GETBOOTCONFIG ..\n");
    res= ioctl(fd, IOCTL_ANDROID_GETBOOTCONFIG, &uRet);
    close(fd);
    ALOGW("jni IOCTL_ANDROID_GETBOOTCONFIG %u ...\n",uRet);
    return uRet;
}

static jboolean SysSetBootConfigLight(JNIEnv *env, jobject clazz, jlong data)
{
    int fd;
    int res;
    jboolean ret;
//  int uRet;
    fd = open(SYSTEMDRIVENAME, O_RDWR);
    if(fd < 0) {
        ALOGW("Unable to open system driver: %s\n", strerror(errno));
        return false;
    }
    ALOGW("jni IOCTL_ANDROID_SETBOOTCONFIG_BRIGHTNESS ..\n");
    res= ioctl(fd, IOCTL_ANDROID_SETBOOTCONFIG_BRIGHTNESS, &data);
    close(fd);
    return true;
}

static jboolean SysSetBootConfigCamera(JNIEnv *env, jobject clazz, jlong data)
{
    int fd;
    int res;
    jboolean ret;
//  int uRet;
    fd = open(SYSTEMDRIVENAME, O_RDWR);
    if(fd < 0) {
        ALOGW("Unable to open system driver: %s\n", strerror(errno));
        return false;
    }
    ALOGW("jni IOCTL_ANDROID_SETBOOTCONFIG_CAMERA_FLAG ..\n");
    res= ioctl(fd, IOCTL_ANDROID_SETBOOTCONFIG_CAMERA_FLAG, &data);
    close(fd);
    return true;
}

#endif

static jboolean BootConfigRelease(JNIEnv *env, jclass clazz)
{
	ALOGE("BootConfigRelease Release success.");
	return 0;
}

static const JNINativeMethod methods[] =
{
    { "nativeInit",			"()Z",  (void *) BootConfigInit },
    { "nativeGetEmmcData",	"()J",  (void *) GetBootConfig },
    { "nativeSetEmmcData",	"(J)Z", (void *) SetBootConfig },
//  { "_setBootLights", "(J)Z", (void *) SetBootLights },
//  { "_setBootCamera", "(J)Z", (void *) SetBootCamera },
    { "nativeUnInit",		"()Z",  (void *) BootConfigRelease },
};

int register_misc_hal_jni(JNIEnv* env)
{
	static const char* const kClassName = "com/roadrover/services/jni/EMMC";

	jclass clazz;

	clazz = env->FindClass(kClassName);
	if (clazz == NULL)
	{
		ALOGE("Can't find class %s\n", kClassName);
		return -1;
	}
	if (env->RegisterNatives(clazz, methods,
	        sizeof(methods) / sizeof(methods[0])) != JNI_OK)
	{
		ALOGE("Failed registering methods for %s\n", kClassName);
		return -1;
	}
	ALOGE("register hal jni OK!\n");

	return 0;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;
	jint result = -1;

	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
	{
		ALOGE("GetEnv failed!");
		return result;
	}

	register_misc_hal_jni(env);

	return JNI_VERSION_1_4;
}

