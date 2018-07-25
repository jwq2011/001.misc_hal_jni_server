/* linux/drivers/char/usb.c

   National Semiconductor SCx200 GPIO driver.  Allows a user space
   process to play with the GPIO pins.

   Copyright (c) 2001,2002 Christer Weinigel <wingel@nano-system.com> */
#include <linux/gpio/consumer.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/fb.h>
#include <linux/backlight.h>
#include <linux/err.h>
#include <linux/pwm.h>
#include <linux/pwm_backlight.h>
#include <linux/regulator/consumer.h>
#include <linux/slab.h>

static struct platform_device *pdev;

MODULE_AUTHOR("Christer Weinigel <wingel@nano-system.com>");
MODULE_DESCRIPTION("NatSemi/AMD SCx200 GPIO Pin Driver");
MODULE_LICENSE("GPL");

static int major = 0;		/* default to dynamic major */
module_param(major, int, 0);
MODULE_PARM_DESC(major, "Major device number");
static struct kobject *tpa6140_kobj;
#define MAX_PINS 32		/* 64 later, when known ok */
 static int usbsys;

unsigned char SdcardInsertFlag=0;
unsigned char StartupSdcardInsertFlag=0;

unsigned char UdiskInsertFlag=0;
unsigned char StartupUdiskInsertFlag=0;

unsigned char StartupTimes=0;

static struct timer_list SystemStartupTimer;

#define READ_FROM_MMC_REGISTER 1

#define ADD_SAVE_BL_CONFIG 1
#ifdef ADD_SAVE_BL_CONFIG
#include <linux/device.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/time.h>
#include <linux/rtc.h>
#include <linux/slab.h>
//#include <linux/interrupt.h>
#include <linux/ioctl.h>
#include <linux/completion.h>
//#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <linux/types.h>
#include <linux/cdev.h>

#define IOCTL_ANDROID_GET_BOOTCONFIG    0x80000001
#define IOCTL_ANDROID_SET_BOOTCONFIG    0x80000002

#define IOCTL_ANDROID_SET_BOOT_LIGHTS   0x80000003
#define IOCTL_ANDROID_SET_BOOT_CAMERA   0x80000004

static DEFINE_MUTEX(nvram_mutex);


unsigned char SystemBrightness = -1;
unsigned char GetSystemBlFlag = 0;

#define READ_FROM_CPU_REGISTER 0

#if  READ_FROM_CPU_REGISTER
#define ANDROID_CONFIG_BOOT	(~((1 << 7) | (1 << 8)))
static long get_backlight_config(void)
{
	u32 reg, value;
//	reg = readl(0x20CC068);
	reg = readl(0x20CC068);

	value = reg & ANDROID_CONFIG_BOOT;

	printk(KERN_ERR "%d %s: reg = %u, value = %u\n", __LINE__, __func__, reg, value);

	return value;
}

static int set_backlight_config(char *mBuffer)
{
	u32 reg, Brightness;
	reg = __raw_readl(0x20CC068);
	reg &= 0x7F;

	if (sscanf(mBuffer, "%d", &Brightness) == 1 && Brightness >= 0) {
		//writel(Brightness|reg, 0x20CC068);
		__raw_writel(0x20CC068, Brightness|reg);
	}
}

static int __init bl_open_setup(char *line)
{
	//strlcpy(saved_root_name, line, sizeof(saved_root_name));
	printk("\n=====%d== %s====, line = %s\n",__LINE__,__FUNCTION__, line);
	return 1;
}

__setup("init=", bl_open_setup);

//extern char *bl_lightness;

static int bl_open(struct inode *inode, struct file *file)
{
//	printk("\n=====%d== %s==== bl_lightness = %s\n",__LINE__,__FUNCTION__, bl_lightness);
//	printk(KERN_ERR "%s %d saved_command_line = %s\n", __func__, __LINE__, saved_command_line);
	return 0;
}

static ssize_t
bl_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos)
{
/*
#ifdef OLD_CHANGE
	unsigned char tmp[2];
        tmp[0] =  1;
        if(SystemEmmcUsbStatus())
        {
            tmp[0] = 0;
        }
	copy_to_user(buf, tmp, 1);
#else
	get_backlight_config();
#endif*/
	u32 reg, value;
//	reg = readl(0x20CC068);
	reg = readl(0x20CC068);

	value = reg & ANDROID_CONFIG_BOOT;
	printk(KERN_ERR "%d %s: reg = %u, value = %u\n", __LINE__, __func__, reg, value);
	return nbytes;	
}

static ssize_t
bl_write (struct file *file, const char __user *buf, size_t ret, loff_t *ppos)
{

	char mBuffer[10];

	memset(mBuffer, 0, 10);
	copy_from_user(mBuffer, buf, 10);
	printk(KERN_ERR "%d %s: buf = %s\n", __LINE__, __func__, buf);
	//	__get_user(mBuffer, (char *)arg);
#if 0
	if(memcmp(mBuffer,"BL", 2) == 0)
		set_backlight_config(mBuffer + 2, 3);
#else

	set_backlight_config(mBuffer);
#endif
}

#elif ( READ_FROM_MMC_REGISTER)
static unsigned int get_boot_config(void)
{
    struct file *fd;
//    size_t ret;
    int ret;
    unsigned char buff[10];
	unsigned int value;

    loff_t off;

    mm_segment_t old_fs;

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    fd = filp_open("/dev/block/mmcblk3", O_RDONLY, 0);
    off = 0x7FFE00;

    ret = vfs_read(fd,buff, sizeof(buff), &off);
	if (IS_ERR(ret)) {
		printk(KERN_ERR "%s vfs_read fail!\n", __func__);
		goto CLOSE_FILP;
	}

    ret = vfs_fsync(fd, 0);
	if (IS_ERR(ret)) {
		printk(KERN_ERR "%s vfs_fsync fail!\n", __func__);
		goto CLOSE_FILP;
	}

	if((buff[0]==0xaa) || (buff[1]==0x11))
	{
//		GetSystemBlFlag = 1;
//		SystemBrightness = (char)(buff[2]- '0')*1000 + (char)(buff[3] - '0')*100 + (char)(buff[4] - '0');
//        sscanf();

		printk("vfs_read BL%d %x %x %x %x %x..\n", SystemBrightness, buff[2], buff[3], buff[4]);
	}

//	printk("vfs_read %s %s %s %s %s %s %s %s..\n",buff[2],buff[3],buff[4],buff[5],buff[6],buff[7],buff[8],buff[9]);
    printk(KERN_ERR "vfs_read %s\n", buff+2);

CLOSE_FILP:
	filp_close(fd, NULL);

//	return SystemBrightness;
	sscanf(buff+2, "%x", &value);
	printk(KERN_ERR "value = %d\n", value);
    return value;
}

static int set_boot_config(unsigned char *buf,unsigned char len)
{
    struct file *fd;
    int ret;
    unsigned char buff[513];

    loff_t off;

    mm_segment_t old_fs;

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    fd = filp_open("/dev/block/mmcblk3", O_RDWR, 0);
	if (IS_ERR(fd)) {
		printk(KERN_ERR "%s open fail!\n", __func__);
		goto CLOSE_FILP;
	}
#ifdef OLD_CHANGE
    off = 0x3ffa00;	// 4MB - 0x600
#else
	off = 0x7FFE00;	// 6MB - 0x600
#endif

    buff[0]=0xaa;
    buff[1]=0x11;
	memcpy(buff+2, buf, len);
    ret = vfs_write(fd, buff, 2+len, &off);
	if (IS_ERR(ret)) {
		printk(KERN_ERR "%s vfs_write fail!\n", __func__);
		goto CLOSE_FILP;
	}

    ret = vfs_fsync(fd, 0);
	if (IS_ERR(ret)) {
		printk(KERN_ERR "%s vfs_fsync fail!\n", __func__);
		goto CLOSE_FILP;
	}

//	printk(KERN_ERR "vfs_write %s %s %s %s %s %s %s %s ..\n",buff[2],buff[3],buff[4],buff[5],buff[6],buff[7],buff[8],buff[9]);
	printk(KERN_ERR "vfs_write %s \n", &buff[2]);

CLOSE_FILP:
	filp_close(fd, NULL);

	return 0;
}

#if 0
static int get_backlight_config(void)
{
    struct file *fd;
    size_t ret;
    unsigned char buff[513];

    loff_t off;

    mm_segment_t old_fs;

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    fd = filp_open("/dev/block/mmcblk3p8", O_RDONLY, 0);
    off = 0x5ffa00;

    ret = vfs_read(fd,buff, 7, &off);
	if (IS_ERR(ret)) {
		printk(KERN_ERR "%s vfs_read fail!\n", __func__);
		goto CLOSE_FILP;
	}

    ret = vfs_fsync(fd, 0);
	if (IS_ERR(ret)) {
		printk(KERN_ERR "%s vfs_fsync fail!\n", __func__);
		goto CLOSE_FILP;
	}

	if((buff[0]==0xaa) || (buff[1]==0x11))
	{
		GetSystemBlFlag = 1;
		SystemBrightness = (char)(buff[2 ]- '0')*100 + (char)(buff[3] - '0')*10 + (char)(buff[4] - '0');

		printk("vfs_read BL%d %x %x %x %x %x..\n", SystemBrightness, buff[2], buff[3], buff[4]);
	}

	printk("vfs_read %x %x %x %x %x..\n",buff[2], buff[3], buff[4], buff[5], buff[6]);

CLOSE_FILP:
	filp_close(fd, NULL);

	return ret;
}

static int set_backlight_config(unsigned char *buf,unsigned char len)
{
    struct file *fd;
    size_t ret;
    unsigned char buff[513];

    loff_t off;

    mm_segment_t old_fs;

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    fd = filp_open("/dev/block/mmcblk3p8", O_RDWR, 0);
	if (IS_ERR(fd)) {
		printk(KERN_ERR "%s open fail!\n", __func__);
		goto CLOSE_FILP;
	}
#ifdef OLD_CHANGE
    off = 0x3ffa00;	// 4MB - 0x600
#else
	off = 0x5ffa00;	// 6MB - 0x600
#endif

    buff[0]=0xaa;
    buff[1]=0x11;
	memcpy(buff+2, buf, len);
    ret = vfs_write(fd, buff, 2+len, &off);
	if (IS_ERR(ret)) {
		printk(KERN_ERR "%s vfs_write fail!\n", __func__);
		goto CLOSE_FILP;
	}

    ret = vfs_fsync(fd, 0);
	if (IS_ERR(ret)) {
		printk(KERN_ERR "%s vfs_fsync fail!\n", __func__);
		goto CLOSE_FILP;
	}

    printk("vfs_write %x %x %x %x %x ..\n",buff[2],buff[3],buff[4]);

CLOSE_FILP:
	filp_close(fd, NULL);

	return ret;
}
#endif
#endif
//extern char *saved_command_line;
//module_param(saved_command_line, charp, S_IRUGO);


static int __init bl_open_setup(char *line)
{
	//strlcpy(saved_root_name, line, sizeof(saved_root_name));
	printk("\n=====%d== %s====, line = %s\n",__LINE__,__FUNCTION__, line);
	return 1;
}

__setup("init=", bl_open_setup);

//extern char *bl_lightness;

static int bl_open(struct inode *inode, struct file *file)
{
//	printk("\n=====%d== %s==== bl_lightness = %s\n",__LINE__,__FUNCTION__, bl_lightness);
//	printk(KERN_ERR "%s %d saved_command_line = %s\n", __func__, __LINE__, saved_command_line);
	return 0;
}

static ssize_t
bl_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos)
{

#ifdef OLD_CHANGE
	unsigned char tmp[2];
        tmp[0] =  1;
        if(SystemEmmcUsbStatus())
        {
            tmp[0] = 0;
        }
	copy_to_user(buf, tmp, 1);
#else

	struct file *fd;
    size_t ret;
    unsigned char buff[1024];

    loff_t off;

    mm_segment_t old_fs;

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    fd = filp_open("/dev/block/mmcblk3", O_RDONLY, 0);
    off = 0x7FFE00;

    ret = vfs_read(fd,buff, 2+4, &off);
	if (IS_ERR(ret)) {
		printk(KERN_ERR "%s vfs_read fail!\n", __func__);
		goto CLOSE_FILP;
	}

    ret = vfs_fsync(fd, 0);
	if (IS_ERR(ret)) {
		printk(KERN_ERR "%s vfs_fsync fail!\n", __func__);
		goto CLOSE_FILP;
	}

//    get_boot_config();
    unsigned char *mbuff;
//    get_boot_config();
    printk("vfs_read %x %x %x %x %x..\n",buff[2], buff[3], buff[4], buff[5]);

    int i = 0;
    printk("vfs_read :");
    for (i; i<6; i++)
        printk("%x ",buff[i]);
    printk("\n");
    copy_to_user(buf, (void *)buff, 6);

#endif
CLOSE_FILP:

	return nbytes;	
}

static ssize_t
bl_write (struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	char mBuffer[10];

	memset(mBuffer, 0, sizeof(mBuffer));
	copy_from_user(mBuffer, buf, count);
    mBuffer[count] = '\0';
	printk(KERN_ERR "%d %s: buf = %s\n", __LINE__, __func__, buf);
	//	__get_user(mBuffer, (char *)arg);
#ifdef OLD_CHANGE
	if(memcmp(mBuffer,"BL", 2) == 0)
		set_backlight_config(mBuffer + 2, 3);
#else

    struct file *fd;
    int ret;
//    unsigned char buff[513];

    loff_t off;

    mm_segment_t old_fs;

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    fd = filp_open("/dev/block/mmcblk3", O_RDWR, 0);
	if (IS_ERR(fd)) {
		printk(KERN_ERR "%s open fail!\n", __func__);
		goto CLOSE_FILP;
	}
#ifdef OLD_CHANGE
    off = 0x3ffa00;	// 4MB - 0x600
#else
	off = 0x7FFE00;	// 6MB - 0x600
#endif

    mBuffer[0]=0xaa;
    mBuffer[1]=0x11;
	memcpy(mBuffer+2, mBuffer, count);
    ret = vfs_write(fd, mBuffer, 2+count, &off);
	if (IS_ERR(ret)) {
		printk(KERN_ERR "%s vfs_write fail!\n", __func__);
		goto CLOSE_FILP;
	}

    ret = vfs_fsync(fd, 0);
	if (IS_ERR(ret)) {
		printk(KERN_ERR "%s vfs_fsync fail!\n", __func__);
		goto CLOSE_FILP;
	}

    printk(KERN_ERR "vfs_write %x %x %x %x %x %x ..\n",mBuffer[0],mBuffer[1],mBuffer[2], mBuffer[3],mBuffer[4],mBuffer[5]);

CLOSE_FILP:
	filp_close(fd, NULL);

//	set_boot_config(mBuffer, sizeof(mBuffer));
    return 0;
#endif
}

static long bl_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ioarg=1;
	unsigned char mBuffer[8];
	unsigned int value;
	mutex_lock(&nvram_mutex);

	printk("usb_ioctl cmd=%u..\n",cmd);
    switch (cmd) 
	{
		case IOCTL_ANDROID_GET_BOOTCONFIG:
//			value = get_backlight_config();
//            mBuffer = get_boot_config();
			value = get_boot_config();
//          memcpy(mBuffer, get_boot_config(), sizeof(mBuffer));
//          sscanf(mBuffer, "%d", &value);
            printk(KERN_ERR "IOCTL_ANDROID_GET_BOOTCONFIG %d\n", value);
			copy_to_user(arg, &value, sizeof(value));
			break;
		case IOCTL_ANDROID_SET_BOOTCONFIG:
			memset(mBuffer,0, sizeof(mBuffer));
			copy_from_user(&value, arg, sizeof(value));
            
			/* switch Dec to Char*/
			sprintf(mBuffer, "%x", value);
			printk(KERN_ERR "IOCTL_ANDROID_SET_BOOTCONFIG %s\n", mBuffer);

//			set_backlight_config(mBuffer);
            set_boot_config(mBuffer, sizeof(mBuffer));
			break;
        case IOCTL_ANDROID_SET_BOOT_LIGHTS:
        case IOCTL_ANDROID_SET_BOOT_CAMERA:
            break;
        #if 0
            case IOCTL_ANDROID_GETBLCONFIG:
            {
                ioarg = GetSystemBlFlag;
                get_backlight_config();
                __put_user(ioarg, (int *)arg);
                break;
            }
            case IOCTL_ANDROID_SETBLCONFIG:
            {
                memset(mBuffer,0,50);
                copy_from_user(mBuffer,(char *)arg,10);
                // __get_user(mBuffer, (char *)arg);
                if(memcmp(mBuffer,"BL",2)==0)
                set_backlight_config(mBuffer+2, 3);
                break;
            }
        #endif
        default:
            break;
	}

	mutex_unlock(&nvram_mutex);

	return 0;
}
#endif

static int bl_release(struct inode *inode, struct file *file)
{
//	printk("====%d== %s====\n",__LINE__,__FUNCTION__ );
	return 0;
}

static const struct file_operations bl_fileops = {
	.owner   = THIS_MODULE,
	.open    = bl_open,
	.read    = bl_read,
	.write   = bl_write,
	.unlocked_ioctl = bl_ioctl,
	.release = bl_release,
	.llseek  = no_llseek,
	///.name   = "aaaaaaaaaaaa",
};

static struct miscdevice bl_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "SysStauts",
	.fops = &bl_fileops,
};

static int __init bl_init(void)
{
	int rc;
	dev_t devid;

	misc_register(&bl_misc);

//	get_backlight_config();

	return 0; /* succeed */
}

static void __exit bl_cleanup(void)
{
     misc_deregister(&bl_init);
}

late_initcall(bl_init);
module_exit(bl_cleanup);

