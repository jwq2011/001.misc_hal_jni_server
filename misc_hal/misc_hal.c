#include "misc_hal.h"

#define LOG_NDEBUG 0
#define LOG_TAG "MISC"

#define SYSTEMDRIVENAME "/dev/SysStauts"
#define IOCTL_ANDROID_GET_BOOTCONFIG     0x8000001
#define IOCTL_ANDROID_SET_BOOTCONFIG     0x8000002

#define IOCTL_ANDROID_SET_BOOT_LIGHTS    0x8000003
#define IOCTL_ANDROID_SET_BOOT_CAMERA    0x8000004

static int dev_file = 0;
unsigned int boot_data;

static int misc_init_gpm(void)
{
    int res;

//	dev_file = open(SYSTEMDRIVENAME, O_RDWR);
    if(dev_file < 0) {
    	ALOGW("Unable to open system driver ..%s\n", SYSTEMDRIVENAME);
    	return -1;
    }
#if 1
    res= ioctl(dev_file, IOCTL_ANDROID_GET_BOOTCONFIG, &boot_data);
#else
	char buff[8];
	read(dev_file, buff, sizeof(buff));
	sscanf(buff, "%d", &boot_data);
#endif
    ALOGW("jni IOCTL_ANDROID_GET_BOOTCONFIG %d..\n", boot_data);
//	close(dev_file);
    return 0;
}

int misc_device_close(struct hw_device_t* device)
{
	struct misc_control_device_t* ctx = (struct misc_control_device_t*) device;
	if (ctx)
	{
		free(ctx);
	}
	close(dev_file);
    boot_data = 0;

	return 0;
}

static int get_boot_args(struct misc_control_device_t *dev)
{
    int fd;
    int res;
    bool ret;
    int uRet;

//	dev_file = open(SYSTEMDRIVENAME, O_RDWR);
    if(dev_file < 0) {
        ALOGW("Unable to get_boot_args system driver ..\n");
        return 0;
    }
	return 0;
#if 1
    res= ioctl(dev_file, IOCTL_ANDROID_GET_BOOTCONFIG, &uRet);
#else
	char buff[8];
	read(dev_file, buff, sizeof(buff));
	sscanf(buff, "%d", &boot_data);
#endif
    ALOGW("jni IOCTL_ANDROID_GET_BOOTCONFIG %d ...\n", uRet);
//	close(dev_file);
    return uRet;
}

static int set_boot_args(struct misc_control_device_t *dev, u_int32_t value)
{
    int reg_data = value;

//  misc_init_gpm();
//  reg_data = boot_data | value;

	ALOGW("hal set_boot_args %u ..\n", value);
//	dev_file = open(SYSTEMDRIVENAME, O_RDWR);

    if(dev_file < 0) {
        ALOGW("Unable to set_boot_args system driver ..\n");
        return false;
    }
    ALOGW("jni IOCTL_ANDROID_SETBOOTCONFIG %d ..\n", reg_data);
    ioctl(dev_file, IOCTL_ANDROID_SET_BOOTCONFIG, &reg_data);
//	close(dev_file);
    return true;
}

static int set_lights(struct misc_control_device_t *dev, u_int32_t value)
{
    unsigned int reg_data;

    misc_init_gpm();
    reg_data = boot_data | value;

    if(dev_file < 0) {
        ALOGW("Unable to set_lights system driver ..%s\n");
        return false;
    }
    ALOGW("jni IOCTL_ANDROID_SET_BOOT_LIGHTS %s ..\n", reg_data);
    ioctl(dev_file, IOCTL_ANDROID_SET_BOOTCONFIG, &reg_data);

    return true;
}

static int set_camera(struct misc_control_device_t *dev, u_int32_t value)
{
    unsigned int reg_data;

    misc_init_gpm();
    reg_data = boot_data || value;

    if(dev_file < 0) {
        ALOGW("Unable to set_camera system driver ..%s\n");
        return false;
    }
    ALOGW("jni IOCTL_ANDROID_SET_BOOT_CAMERA ..\n");
    ioctl(dev_file, IOCTL_ANDROID_SET_BOOTCONFIG, &value);

    return true;
}

static int misc_device_open(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device)
{
	struct misc_control_device_t *dev;
	dev = (struct misc_control_device_t *) malloc(sizeof(*dev));
	memset(dev, 0, sizeof(*dev));

	dev->hw_device.tag = HARDWARE_DEVICE_TAG;
	dev->hw_device.version = 0;
	dev->hw_device.module = (struct hw_module_t*) module;
	dev->hw_device.close = misc_device_close;

    dev->get_boot_config = get_boot_args;
    dev->set_boot_config = set_boot_args;
    dev->set_boot_lights = set_lights;
    dev->set_boot_camera = set_camera;

	//*device = &dev->hw_device;
	*device = (hw_device_t*)dev;

	dev_file = open(SYSTEMDRIVENAME, O_RDWR);
#if 0
	if (dev_file < 0)
	{
		LOGI("Misc Stub: open %s fail.", SYSTEMDRIVENAME);
	}
	else
	{
		LOGI("Misc Stub: open %s success .", SYSTEMDRIVENAME);
	}
#else
    ALOGI("Misc Stub: open %s %s.", SYSTEMDRIVENAME, (dev_file < 0)?"fail":"success");
#endif

	return misc_init_gpm();
}

static struct hw_module_methods_t misc_module_methods =
{
	.open           = misc_device_open
};

struct hw_module_t HAL_MODULE_INFO_SYM =
{
    .tag            = HARDWARE_MODULE_TAG,
    .version_major  = 1,
    .version_minor  = 0,
    .id             = MISC_HARDWARE_MODULE_ID,
    .name           = "Sample MISC HAL Stub",
    .author         = "roadrover",
    .methods        = &misc_module_methods,
};

