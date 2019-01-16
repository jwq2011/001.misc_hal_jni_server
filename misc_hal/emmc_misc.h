#include <hardware/hardware.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <math.h>
#include <time.h>
#include <semaphore.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/inotify.h>
#include <poll.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <sys/reboot.h>
#include <cutils/log.h>
#include <cutils/sockets.h>
#include <cutils/properties.h>

struct misc_module_t
{
	struct hw_module_t hw_module;
};
struct misc_control_device_t
{
	struct hw_device_t hw_device;
    int (*get_boot_config)(struct misc_control_device_t *dev);
    int (*set_boot_config)(struct misc_control_device_t *dev, u_int32_t value);
    int (*set_boot_lights)(struct misc_control_device_t *dev, u_int32_t value);
    int (*set_boot_camera)(struct misc_control_device_t *dev, u_int32_t value);
};

#define MISC_HARDWARE_MODULE_ID "misc_hal"
