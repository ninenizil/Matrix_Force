/* Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/i2c.h>
#include <linux/i2c/sx150x.h>
#include <linux/i2c/isl9519.h>
#include <linux/gpio.h>
#include <linux/msm_ssbi.h>
#include <linux/regulator/gpio-regulator.h>
#include <linux/mfd/pm8xxx/pm8921.h>
#include <linux/mfd/pm8xxx/pm8xxx-adc.h>
#include <linux/regulator/consumer.h>
#include <linux/spi/spi.h>
#include <linux/slimbus/slimbus.h>
#include <linux/bootmem.h>
#include <linux/msm_kgsl.h>
#ifdef CONFIG_ANDROID_PMEM
#include <linux/android_pmem.h>
#endif
#include <linux/cyttsp.h>
#include <linux/dma-mapping.h>
#include <linux/platform_data/qcom_crypto_device.h>
#include <linux/platform_data/qcom_wcnss_device.h>
#include <linux/leds.h>
#include <linux/leds-pm8xxx.h>
#include <linux/msm_tsens.h>
#include <linux/ks8851.h>
#include <linux/i2c/isa1200.h>
#include <linux/memory.h>
#include <linux/memblock.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/setup.h>
#include <asm/hardware/gic.h>
#include <asm/mach/mmc.h>

#include <mach/board.h>
#include <mach/msm_iomap.h>
#include <mach/msm_spi.h>
#ifdef CONFIG_USB_MSM_OTG_72K
#include <mach/msm_hsusb.h>
#else
#include <linux/usb/msm_hsusb.h>
#endif
#include <linux/usb/android.h>
#include <mach/usbdiag.h>
#include <mach/socinfo.h>
#include <mach/rpm.h>
#ifndef CONFIG_MSM_DSPS
#if defined(CONFIG_MPU_SENSORS_MPU3050) 
#include <linux/mpu.h>
#endif
#endif
#include <mach/gpio.h>
#include <mach/gpiomux.h>
#include <mach/msm_bus_board.h>
#include <mach/msm_memtypes.h>
#include <mach/dma.h>
#include <mach/msm_dsps.h>
#include <mach/msm_xo.h>
#include <mach/restart.h>

#ifdef CONFIG_WCD9310_CODEC
#include <linux/slimbus/slimbus.h>
#include <linux/mfd/wcd9310/core.h>
#include <linux/mfd/wcd9310/pdata.h>
#endif

#include <linux/ion.h>
#include <mach/ion.h>
#include <mach/mdm2.h>
#include <mach/mdm-peripheral.h>
#include <mach/msm_rtb.h>
#include <mach/msm_cache_dump.h>
#include <mach/scm.h>
#include <mach/iommu_domains.h>

#include <linux/fmem.h>

#include "timer.h"
#include "devices.h"
#include "devices-msm8x60.h"
#include "spm.h"
#include "board-8960.h"
#include "pm.h"
#include <mach/cpuidle.h>
#include "rpm_resources.h"
#include <mach/mpm.h>
#include "acpuclock.h"
#include "rpm_log.h"
#include "smd_private.h"
#include "pm-boot.h"
#include "msm_watchdog.h"



#ifdef CONFIG_PN544_NFC
#include <linux/nfc/pn544.h>
#endif

#ifdef CONFIG_SENSORS_AKM8962C 
#include <linux/akm8962_new.h>
#endif
#ifdef CONFIG_SENSORS_ST_LIS3DHTR 
#include <linux/lis3dh.h>
#endif
#ifdef CONFIG_MXC_MMA8452
#include <linux/mma854x.h> 
#endif
#ifdef CONFIG_SENSORS_KXTIK 
#include <linux/kxtik.h> 
#endif

/*
 * ZTE_PLATFORM
 */
#define ZTE_RAM_CONSOLE

/*
 * ZTE_PLATFORM
 */
#define ZTE_FTM

/*
 * ZTE_PLATFORM
 */
#ifdef ZTE_RAM_CONSOLE
#ifdef CONFIG_ANDROID_RAM_CONSOLE
/*
 * Set to the last 1MB region of the first 'System RAM'
 * and the region allocated by '___alloc_bootmem' should be considered
 */
#define MSM_RAM_CONSOLE_PHYS  0x88D00000 /* Refer to 'debug.c' in bootable */
#define MSM_RAM_CONSOLE_SIZE  SZ_1M
#endif
#endif /* ZTE_RAM_CONSOLE */

#ifdef CONFIG_ZTE_SDLOG

#define MSM_SDLOG_PHYS      0x82000000
#define MSM_SDLOG_SIZE      (SZ_1M * 16)

int sdlog_flag = 0;

#endif


#if defined(CONFIG_MACH_KISKA)

#include <linux/gpio_notify.h>
static int pm_gpio_config(struct gpio_notify_entry *entry, int config)
{
	int err = 0;
	struct pm_gpio enable = {
		.direction      = PM_GPIO_DIR_IN,
		.pull           = PM_GPIO_PULL_NO,
		.vin_sel        = PM8058_GPIO_VIN_S3,
		.function       = PM_GPIO_FUNC_NORMAL,
		.inv_int_pol    = 0,
	};
	struct pm_gpio disable = {
		.direction      = PM_GPIO_DIR_IN,
		.pull           = PM_GPIO_PULL_NO,
		.vin_sel        = PM8058_GPIO_VIN_S3,
		.function       = PM_GPIO_FUNC_NORMAL,
		.inv_int_pol    = 0,
	};
	if (NULL == entry) {
		return -1;
	}
	if (config) {
		err = gpio_request(entry->gpio, entry->name);
		if (err < 0) {
			printk(KERN_ERR"(%s:%d)Unable to request gpio %d %s\n",
			       __FUNCTION__, __LINE__, entry->gpio, entry->name);
			return err;
		}
		pm8xxx_gpio_config(entry->gpio, &enable);
		gpio_direction_input(entry->gpio);
	} else {
		pm8xxx_gpio_config(entry->gpio, &disable);
		gpio_free(entry->gpio);
	}
	return 0;
}

struct gpio_notify_entry gpio_notify_array[] = {
	{
		.config = pm_gpio_config,
		.gpio   = PM8921_GPIO_PM_TO_SYS (36),
		.name   = "UIM1_DET_CONN",
		.h      = "DECONN",
		.l      = "CONN",
		.delay  = 0,
	},
};

static struct gpio_notify_platform_data gpio_notify_data = {
	.array  = gpio_notify_array,
	.size = ARRAY_SIZE(gpio_notify_array),
};

static struct platform_device msm_gpio_notify_platform_dev = {
	.name = "msm_gpio_notify",
	.id   = -1,
	.dev            = {
		.platform_data = &gpio_notify_data,
	},
};


#endif

static struct platform_device msm_fm_platform_init = {
	.name = "iris_fm",
	.id   = -1,
};

#define KS8851_RST_GPIO		89
#define KS8851_IRQ_GPIO		90
#define HAP_SHIFT_LVL_OE_GPIO	47

#ifdef CONFIG_USE_BCM4330 

enum {
	//BT_TX = 38,
	//BT_RX = 39,
	//BT_CTS = 40,
	//BT_RFR = 41,
	BT_PCM_DOUT = 63,
	BT_PCM_DIN = 64,
	BT_PCM_SYNC =65,
	BT_PCM_CLK = 66,
	BT_RST = 25,
	//BT_REG_ON = PM8921_GPIO_PM_TO_SYS (21),
	MSM2BT_WAKE = 51,
	BT2MSM_WAKE = 52,
};
static struct platform_device msm_bt_power_device = {
	.name = "bt_power",
};

static struct resource bluesleep_resources[] = {
	{
		.name	= "gpio_host_wake",
		.start	= BT2MSM_WAKE,
		.end	= BT2MSM_WAKE,
		.flags	= IORESOURCE_IO,
	},
	{
		.name	= "gpio_ext_wake",
		.start	= MSM2BT_WAKE,
		.end	= MSM2BT_WAKE,
		.flags	= IORESOURCE_IO,
	},
	{
		.name	= "host_wake",
		.start	= MSM_GPIO_TO_INT(BT2MSM_WAKE),
		.end	= MSM_GPIO_TO_INT(BT2MSM_WAKE),
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device msm_bluesleep_device = {
	.name = "bluesleep",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(bluesleep_resources),
	.resource	= bluesleep_resources,
};

static unsigned bt_config_power_on[] = {
	GPIO_CFG(MSM2BT_WAKE, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
	//GPIO_CFG(BT_RFR, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* RFR */
	//GPIO_CFG(BT_CTS, 1, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* CTS */
	//GPIO_CFG(BT_RX, 1, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* Rx */
	//GPIO_CFG(BT_TX, 1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* Tx */
       GPIO_CFG(BT_RST, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),          /*BT_RST*/
    //   GPIO_CFG(BT_REG_ON, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),  /* BT_REG_ON */
	GPIO_CFG(BT2MSM_WAKE, 0, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL, GPIO_CFG_2MA),	/* HOST_WAKE */
};

static unsigned bt_config_power_off[] = {
	GPIO_CFG(MSM2BT_WAKE, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* WAKE */
    	//GPIO_CFG(BT_RFR, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),  /* RFR */
    	//GPIO_CFG(BT_CTS, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),  /* CTS */
    	//GPIO_CFG(BT_RX, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),  /* Rx */
    	//GPIO_CFG(BT_TX, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),  /* Tx */
    	GPIO_CFG(BT_RST, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* reset */
    //	GPIO_CFG(BT_REG_ON, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), /* BT_REG_ON */
	GPIO_CFG(BT2MSM_WAKE, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	/* HOST_WAKE */
};

static int bluetooth_power(int on)
{
	int pin, rc;

	printk(KERN_DEBUG "%s\n", __func__);
	//tpa2028d1_set_speaker_amp(on);
	if (on) {
			//msm_gpiomux_install(bt_uart_cfg,
					//ARRAY_SIZE(bt_uart_cfg));
		for (pin = 0; pin < ARRAY_SIZE(bt_config_power_on); pin++) {
			rc = gpio_tlmm_config(bt_config_power_on[pin],
					      GPIO_CFG_ENABLE);
			if (rc) {
				printk(KERN_ERR
				       "%s: gpio_tlmm_config(%#x)=%d\n",
				       __func__, bt_config_power_on[pin], rc);
				return -EIO;
			}
		}
        printk(KERN_ERR "bt power on");
    //    dump_stack();
     //   rc = gpio_direction_output(BT_REG_ON, 1);  /*bt on :BT_REG_ON -->1*/
     /*   if (rc) 
        {
            printk(KERN_ERR "%s: generation wifi power (%d)\n",
                   __func__, rc);
            return -EIO;
        }
		mdelay(50);*/

        rc = gpio_direction_output(BT_RST, 1);  /*bton:BT_RST -->1*/
		if (rc) {
			printk(KERN_ERR "%s: generation BTS4020 main clock is failed (%d)\n",
			       __func__, rc);
			return -EIO;
		}
		gpio_direction_output(MSM2BT_WAKE, 1);
		
		mdelay(50);
	} else {
        rc = gpio_direction_output(BT_RST, 0);  /*bt off:BT_RST -->0*/
        if (rc) 
        {
            printk(KERN_ERR "%s:  bt power off fail (%d)\n",
                   __func__, rc);
            return -EIO;
        }

 //       rc = gpio_direction_output(BT_REG_ON, 0);  /*bt_reg_on off on :BT_REG_ON -->0*/
   /*     if (rc) 
        {
            printk(KERN_ERR "%s:  bt power off fail (%d)\n",
                   __func__, rc);
            return -EIO;
        }*/

	for (pin = 0; pin < ARRAY_SIZE(bt_config_power_off); pin++) {
			rc = gpio_tlmm_config(bt_config_power_off[pin],
					      GPIO_CFG_ENABLE);
			if (rc) {
				printk(KERN_ERR
				       "%s: gpio_tlmm_config(%#x)=%d\n",
				       __func__, bt_config_power_off[pin], rc);
				return -EIO;
			}
		}
	}
	return 0;
}

static void __init bt_power_init(void)
{
    int pin = 0, rc = 0;
	msm_bt_power_device.dev.platform_data = &bluetooth_power;
    
	if (gpio_request(BT_RST, "BT_POWER"))		
  		  printk("Failed to request gpio 88 for BT_POWER\n");	
    /*
	if (gpio_request(BT_REG_ON, "BT_REG_ON"))		
  		  printk("Failed to request gpio 109 for BT_REG_ON\n");	
*/
	for (pin = 0; pin < ARRAY_SIZE(bt_config_power_on); pin++) {
		rc = gpio_tlmm_config(bt_config_power_on[pin],
				      GPIO_CFG_ENABLE);
		if (rc) {
			printk(KERN_ERR
			       "%s: gpio_tlmm_config(%#x)=%d\n",
			       __func__, bt_config_power_on[pin], rc);
		}
	}

    rc = gpio_direction_output(BT_RST, 0);  /*bt off:BT_RST -->0*/
    if (rc) 
    {
        printk(KERN_ERR "%s:  bt power off fail (%d)\n",
               __func__, rc);
    }

 //   rc = gpio_direction_output(BT_REG_ON, 0);  /*bt_reg_on off on :BT_REG_ON -->0*/
 /*   if (rc) 
    {
        printk(KERN_ERR "%s:  bt power off fail (%d)\n",
               __func__, rc);
    }
*/

	for (pin = 0; pin < ARRAY_SIZE(bt_config_power_off); pin++) {
		rc = gpio_tlmm_config(bt_config_power_off[pin],
				      GPIO_CFG_ENABLE);
		if (rc) {
			printk(KERN_ERR
			       "%s: gpio_tlmm_config(%#x)=%d\n",
			       __func__, bt_config_power_off[pin], rc);
		}
	}
//#endif
//	printk(KERN_ERR"init bluetooth_power ");
//	tpa_power_test();
//	bluetooth_power(1);
}

#endif

#if defined(CONFIG_GPIO_SX150X) || defined(CONFIG_GPIO_SX150X_MODULE)

struct sx150x_platform_data msm8960_sx150x_data[] = {
	[SX150X_CAM] = {
		.gpio_base         = GPIO_CAM_EXPANDER_BASE,
		.oscio_is_gpo      = false,
		.io_pullup_ena     = 0x0,
		.io_pulldn_ena     = 0xc0,
		.io_open_drain_ena = 0x0,
		.irq_summary       = -1,
	},
	[SX150X_LIQUID] = {
		.gpio_base         = GPIO_LIQUID_EXPANDER_BASE,
		.oscio_is_gpo      = false,
		.io_pullup_ena     = 0x0c08,
		.io_pulldn_ena     = 0x4060,
		.io_open_drain_ena = 0x000c,
		.io_polarity       = 0,
		.irq_summary       = -1,
	},
};

#endif

#define MSM_PMEM_ADSP_SIZE         0x7800000 /* Need to be multiple of 64K */
#define MSM_PMEM_AUDIO_SIZE        0x2B4000
#define MSM_PMEM_SIZE 0x2800000 /* 40 Mbytes */
#define MSM_LIQUID_PMEM_SIZE 0x4000000 /* 64 Mbytes */
#define MSM_HDMI_PRIM_PMEM_SIZE 0x4000000 /* 64 Mbytes */

#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
#define MSM_PMEM_KERNEL_EBI1_SIZE  0x280000
#define MSM_ION_SF_SIZE		MSM_PMEM_SIZE
#define MSM_ION_MM_FW_SIZE	0x200000
#define MSM_ION_MM_SIZE		MSM_PMEM_ADSP_SIZE
#define MSM_ION_QSECOM_SIZE	0x600000 /* (6MB) */
#define MSM_ION_MFC_SIZE	SZ_8K
#define MSM_ION_AUDIO_SIZE	MSM_PMEM_AUDIO_SIZE
#define MSM_ION_HEAP_NUM	8
#define MSM_LIQUID_ION_MM_SIZE (MSM_ION_MM_SIZE + 0x600000)
#define MSM_LIQUID_ION_SF_SIZE MSM_LIQUID_PMEM_SIZE
#define MSM_HDMI_PRIM_ION_SF_SIZE MSM_HDMI_PRIM_PMEM_SIZE

#define MSM8960_FIXED_AREA_START 0xb0000000
#define MAX_FIXED_AREA_SIZE	0x10000000
#define MSM_MM_FW_SIZE		0x200000
#define MSM8960_FW_START	(MSM8960_FIXED_AREA_START - MSM_MM_FW_SIZE)

static unsigned msm_ion_sf_size = MSM_ION_SF_SIZE;
#else
#define MSM_PMEM_KERNEL_EBI1_SIZE  0x110C000
#define MSM_ION_HEAP_NUM	1
#endif

/*
 * ZTE_PLATFORM
 */
#ifdef ZTE_BOOT_MODE
int __init bootmode_init(char *mode)
{
    int is_boot_into_ftm = 0;

    if (!strncmp(mode, SOCINFO_CMDLINE_BOOTMODE_NORMAL, strlen(SOCINFO_CMDLINE_BOOTMODE_NORMAL)))
    {
        is_boot_into_ftm = 0;
    }
    else if (!strncmp(mode, SOCINFO_CMDLINE_BOOTMODE_FTM, strlen(SOCINFO_CMDLINE_BOOTMODE_FTM)))
    {
        is_boot_into_ftm = 1;
    }
    else
    {
        is_boot_into_ftm = 0;
    }

    socinfo_set_ftm_flag(is_boot_into_ftm);

    return 1;
}

__setup(SOCINFO_CMDLINE_BOOTMODE, bootmode_init);
#endif


#ifdef PASS_LCD_ID_TO_KERNEL
extern u32 LcdPanleID ;
static int atoi(const char *name)
{
	int val = 0;

	for (;; name++) {
		switch (*name) {
		case '0' ... '9':
			val = 10*val+(*name-'0');
			break;
		default:
			return val;
		}
	}
}
int __init get_lcd_id(char *id)
{
     //char panel = *id;
	printk("\n  LCD ID string %s",id);
	LcdPanleID = atoi(id);
	printk("\n  LcdPanleID = %d",LcdPanleID);
	return 1;
}

__setup(LCD_ID_STRING, get_lcd_id);

char Lcd_display_mode = 0 ;

int __init get_lcd_mode(char *id)
{
	 if (!strncmp(id, "cmd", strlen("cmd")))
		Lcd_display_mode = 1;
	 else
	 	Lcd_display_mode = 0;
	 
	 return 1;
}
__setup(LCD_MODE_STRING, get_lcd_mode);
#endif

/*
 * ZTE_PLATFORM
 */
#if defined (CONFIG_MACH_ELDEN)
typedef enum {
    HW_VER_01      = 0,
    HW_VER_INVALID = 1,
    HW_VER_MAX
} zte_hw_ver_type;

static const char *zte_hw_ver_str[HW_VER_MAX] = {
    [HW_VER_01]     = "N9120.H02",
    [HW_VER_INVALID]= "INVALID"
};

const char* read_zte_hw_ver(void)
{
    return zte_hw_ver_str[HW_VER_01];
}

#elif defined (CONFIG_MACH_GORDON)
typedef enum {
    HW_VER_01      = 0, /* GPIO43:0 GPIO42:0 GPIO13:0 */
    HW_VER_02      = 1, /* GPIO43:0 GPIO42:0 GPIO13:1 */
    HW_VER_INVALID = 2,
    HW_VER_MAX
} zte_hw_ver_type;

static const char *zte_hw_ver_str[HW_VER_MAX] = {
    [HW_VER_01]     = "c7zA",
    [HW_VER_02]     = "c7zB",
    [HW_VER_INVALID]= "INVALID"
};

const char* read_zte_hw_ver(void)
{
    zte_hw_ver_type hw_ver = HW_VER_INVALID;
    int32_t val_gpio_13 = 0, val_gpio_42 = 0, val_gpio_43 = 0;
    int32_t rc = 0;

    /* Get value from GPIO13 */
    rc =gpio_request(13, "gpio13_zte_hw_ver");
	if (!rc) {
        gpio_tlmm_config(GPIO_CFG(13, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA), GPIO_CFG_ENABLE);
        rc = gpio_direction_input(13);
		if (!rc) {
			val_gpio_13 = gpio_get_value_cansleep(13);
		}
		gpio_free(13);
	}

    /* Get value from GPIO42 */
    rc =gpio_request(42, "gpio42_zte_hw_ver");
	if (!rc) {
        gpio_tlmm_config(GPIO_CFG(42, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA), GPIO_CFG_ENABLE);
        rc = gpio_direction_input(42);
		if (!rc) {
			val_gpio_42 = gpio_get_value_cansleep(42);
		}
		gpio_free(42);
	}
	
	/* Get value from GPIO43 */
    rc =gpio_request(43, "gpio43_zte_hw_ver");
	if (!rc) {
        gpio_tlmm_config(GPIO_CFG(43, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_8MA), GPIO_CFG_ENABLE);
        rc = gpio_direction_input(43);
		if (!rc) {
			val_gpio_43 = gpio_get_value_cansleep(43);
		}
		gpio_free(43);
	}

    /* Get HW version from GPIO */
    hw_ver = (zte_hw_ver_type)((val_gpio_43 << 2) | (val_gpio_42 << 1) | val_gpio_13);
    if (hw_ver >= HW_VER_MAX) {
        pr_err("%s: invalid HW version: %d\n", __func__, hw_ver);
        hw_ver = HW_VER_INVALID;
    }

    return zte_hw_ver_str[hw_ver];
}
#elif defined (CONFIG_MACH_HAYES)
typedef enum {
    HW_VER_01      = 0,
    HW_VER_INVALID = 1,
    HW_VER_MAX
} zte_hw_ver_type;

static const char *zte_hw_ver_str[HW_VER_MAX] = {
    [HW_VER_01]     = "c7wA",
    [HW_VER_INVALID]= "INVALID"
};

const char* read_zte_hw_ver(void)
{
    return zte_hw_ver_str[HW_VER_01];
}
#else
typedef enum {
    HW_VER_INVALID = 0,
    HW_VER_MAX
} zte_hw_ver_type;

static const char *zte_hw_ver_str[HW_VER_MAX] = {
    [HW_VER_INVALID]= "INVALID"
};

const char* read_zte_hw_ver(void)
{
    return zte_hw_ver_str[HW_VER_INVALID];
}
#endif /* CONFIG_MACH_ELDEN */

#ifdef CONFIG_KERNEL_PMEM_EBI_REGION
static unsigned pmem_kernel_ebi1_size = MSM_PMEM_KERNEL_EBI1_SIZE;
static int __init pmem_kernel_ebi1_size_setup(char *p)
{
	pmem_kernel_ebi1_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_kernel_ebi1_size", pmem_kernel_ebi1_size_setup);
#endif

#ifdef CONFIG_ANDROID_PMEM
static unsigned pmem_size = MSM_PMEM_SIZE;
static unsigned pmem_param_set;
static int __init pmem_size_setup(char *p)
{
	pmem_size = memparse(p, NULL);
	pmem_param_set = 1;
	return 0;
}
early_param("pmem_size", pmem_size_setup);

static unsigned pmem_adsp_size = MSM_PMEM_ADSP_SIZE;

static int __init pmem_adsp_size_setup(char *p)
{
	pmem_adsp_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_adsp_size", pmem_adsp_size_setup);

static unsigned pmem_audio_size = MSM_PMEM_AUDIO_SIZE;

static int __init pmem_audio_size_setup(char *p)
{
	pmem_audio_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_audio_size", pmem_audio_size_setup);
#endif

#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
static struct android_pmem_platform_data android_pmem_pdata = {
	.name = "pmem",
	.allocator_type = PMEM_ALLOCATORTYPE_ALLORNOTHING,
	.cached = 1,
	.memory_type = MEMTYPE_EBI1,
};

static struct platform_device android_pmem_device = {
	.name = "android_pmem",
	.id = 0,
	.dev = {.platform_data = &android_pmem_pdata},
};

static struct android_pmem_platform_data android_pmem_adsp_pdata = {
	.name = "pmem_adsp",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
	.memory_type = MEMTYPE_EBI1,
};
static struct platform_device android_pmem_adsp_device = {
	.name = "android_pmem",
	.id = 2,
	.dev = { .platform_data = &android_pmem_adsp_pdata },
};
#endif

static struct android_pmem_platform_data android_pmem_audio_pdata = {
	.name = "pmem_audio",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
	.memory_type = MEMTYPE_EBI1,
};

static struct platform_device android_pmem_audio_device = {
	.name = "android_pmem",
	.id = 4,
	.dev = { .platform_data = &android_pmem_audio_pdata },
};
#endif

struct fmem_platform_data fmem_pdata = {
};

#define DSP_RAM_BASE_8960 0x8da00000
#define DSP_RAM_SIZE_8960 0x1800000
static int dspcrashd_pdata_8960 = 0xDEADDEAD;

static struct resource resources_dspcrashd_8960[] = {
	{
		.name   = "msm_dspcrashd",
		.start  = DSP_RAM_BASE_8960,
		.end    = DSP_RAM_BASE_8960 + DSP_RAM_SIZE_8960,
		.flags  = IORESOURCE_DMA,
	},
};

static struct platform_device msm_device_dspcrashd_8960 = {
	.name           = "msm_dspcrashd",
	.num_resources  = ARRAY_SIZE(resources_dspcrashd_8960),
	.resource       = resources_dspcrashd_8960,
	.dev = { .platform_data = &dspcrashd_pdata_8960 },
};

static struct memtype_reserve msm8960_reserve_table[] __initdata = {
	[MEMTYPE_SMI] = {
	},
	[MEMTYPE_EBI0] = {
		.flags	=	MEMTYPE_FLAGS_1M_ALIGN,
	},
	[MEMTYPE_EBI1] = {
		.flags	=	MEMTYPE_FLAGS_1M_ALIGN,
	},
};

#if defined(CONFIG_MSM_RTB)
static struct msm_rtb_platform_data msm_rtb_pdata = {
	.size = SZ_1M,
};

static int __init msm_rtb_set_buffer_size(char *p)
{
	int s;

	s = memparse(p, NULL);
	msm_rtb_pdata.size = ALIGN(s, SZ_4K);
	return 0;
}
early_param("msm_rtb_size", msm_rtb_set_buffer_size);


static struct platform_device msm_rtb_device = {
	.name           = "msm_rtb",
	.id             = -1,
	.dev            = {
		.platform_data = &msm_rtb_pdata,
	},
};
#endif

static void __init reserve_rtb_memory(void)
{
#if defined(CONFIG_MSM_RTB)
	msm8960_reserve_table[MEMTYPE_EBI1].size += msm_rtb_pdata.size;
#endif
}

static void __init size_pmem_devices(void)
{
#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
	android_pmem_adsp_pdata.size = pmem_adsp_size;

	if (!pmem_param_set) {
		if (machine_is_msm8960_liquid())
			pmem_size = MSM_LIQUID_PMEM_SIZE;
		if (hdmi_is_primary)
			pmem_size = MSM_HDMI_PRIM_PMEM_SIZE;
	}

	android_pmem_pdata.size = pmem_size;
#endif
	android_pmem_audio_pdata.size = MSM_PMEM_AUDIO_SIZE;
#endif
}

static void __init reserve_memory_for(struct android_pmem_platform_data *p)
{
	msm8960_reserve_table[p->memory_type].size += p->size;
}

static void __init reserve_pmem_memory(void)
{
#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
	reserve_memory_for(&android_pmem_adsp_pdata);
	reserve_memory_for(&android_pmem_pdata);
#endif
	reserve_memory_for(&android_pmem_audio_pdata);
	msm8960_reserve_table[MEMTYPE_EBI1].size += pmem_kernel_ebi1_size;
#endif
}

static int msm8960_paddr_to_memtype(unsigned int paddr)
{
	return MEMTYPE_EBI1;
}

#define FMEM_ENABLED 1

#ifdef CONFIG_ION_MSM
#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
static struct ion_cp_heap_pdata cp_mm_ion_pdata = {
	.permission_type = IPT_TYPE_MM_CARVEOUT,
	.align = SZ_64K,
	.reusable = FMEM_ENABLED,
	.mem_is_fmem = FMEM_ENABLED,
	.fixed_position = FIXED_MIDDLE,
	.iommu_map_all = 1,
	.iommu_2x_map_domain = VIDEO_DOMAIN,
};

static struct ion_cp_heap_pdata cp_mfc_ion_pdata = {
	.permission_type = IPT_TYPE_MFC_SHAREDMEM,
	.align = PAGE_SIZE,
	.reusable = 0,
	.mem_is_fmem = FMEM_ENABLED,
	.fixed_position = FIXED_HIGH,
};

static struct ion_co_heap_pdata co_ion_pdata = {
	.adjacent_mem_id = INVALID_HEAP_ID,
	.align = PAGE_SIZE,
	.mem_is_fmem = 0,
};

static struct ion_co_heap_pdata fw_co_ion_pdata = {
	.adjacent_mem_id = ION_CP_MM_HEAP_ID,
	.align = SZ_128K,
	.mem_is_fmem = FMEM_ENABLED,
	.fixed_position = FIXED_LOW,
};
#endif

/**
 * These heaps are listed in the order they will be allocated. Due to
 * video hardware restrictions and content protection the FW heap has to
 * be allocated adjacent (below) the MM heap and the MFC heap has to be
 * allocated after the MM heap to ensure MFC heap is not more than 256MB
 * away from the base address of the FW heap.
 * However, the order of FW heap and MM heap doesn't matter since these
 * two heaps are taken care of by separate code to ensure they are adjacent
 * to each other.
 * Don't swap the order unless you know what you are doing!
 */
static struct ion_platform_data ion_pdata = {
	.nr = MSM_ION_HEAP_NUM,
	.heaps = {
		{
			.id	= ION_SYSTEM_HEAP_ID,
			.type	= ION_HEAP_TYPE_SYSTEM,
			.name	= ION_VMALLOC_HEAP_NAME,
		},
#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
		{
			.id	= ION_CP_MM_HEAP_ID,
			.type	= ION_HEAP_TYPE_CP,
			.name	= ION_MM_HEAP_NAME,
			.size	= MSM_ION_MM_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *) &cp_mm_ion_pdata,
		},
		{
			.id	= ION_MM_FIRMWARE_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_MM_FIRMWARE_HEAP_NAME,
			.size	= MSM_ION_MM_FW_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *) &fw_co_ion_pdata,
		},
		{
			.id	= ION_CP_MFC_HEAP_ID,
			.type	= ION_HEAP_TYPE_CP,
			.name	= ION_MFC_HEAP_NAME,
			.size	= MSM_ION_MFC_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *) &cp_mfc_ion_pdata,
		},
		{
			.id	= ION_SF_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_SF_HEAP_NAME,
			.size	= MSM_ION_SF_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *) &co_ion_pdata,
		},
		{
			.id	= ION_IOMMU_HEAP_ID,
			.type	= ION_HEAP_TYPE_IOMMU,
			.name	= ION_IOMMU_HEAP_NAME,
		},
		{
			.id	= ION_QSECOM_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_QSECOM_HEAP_NAME,
			.size	= MSM_ION_QSECOM_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *) &co_ion_pdata,
		},
		{
			.id	= ION_AUDIO_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_AUDIO_HEAP_NAME,
			.size	= MSM_ION_AUDIO_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *) &co_ion_pdata,
		},
#endif
	}
};

static struct platform_device ion_dev = {
	.name = "ion-msm",
	.id = 1,
	.dev = { .platform_data = &ion_pdata },
};
#endif

struct platform_device fmem_device = {
	.name = "fmem",
	.id = 1,
	.dev = { .platform_data = &fmem_pdata },
};

static void __init adjust_mem_for_liquid(void)
{
	unsigned int i;

	if (!pmem_param_set) {
		if (machine_is_msm8960_liquid())
			msm_ion_sf_size = MSM_LIQUID_ION_SF_SIZE;

		if (hdmi_is_primary)
			msm_ion_sf_size = MSM_HDMI_PRIM_ION_SF_SIZE;

		if (machine_is_msm8960_liquid() || hdmi_is_primary) {
			for (i = 0; i < ion_pdata.nr; i++) {
				if (ion_pdata.heaps[i].id == ION_SF_HEAP_ID) {
					ion_pdata.heaps[i].size =
						msm_ion_sf_size;
					pr_debug("msm_ion_sf_size 0x%x\n",
						msm_ion_sf_size);
					break;
				}
			}
		}
	}
}

static void __init reserve_mem_for_ion(enum ion_memory_types mem_type,
				      unsigned long size)
{
	msm8960_reserve_table[mem_type].size += size;
}

static void __init msm8960_reserve_fixed_area(unsigned long fixed_area_size)
{
#if defined(CONFIG_ION_MSM) && defined(CONFIG_MSM_MULTIMEDIA_USE_ION)
	int ret;

	if (fixed_area_size > MAX_FIXED_AREA_SIZE)
		panic("fixed area size is larger than %dM\n",
			MAX_FIXED_AREA_SIZE >> 20);

	reserve_info->fixed_area_size = fixed_area_size;
	reserve_info->fixed_area_start = MSM8960_FW_START;

	ret = memblock_remove(reserve_info->fixed_area_start,
		reserve_info->fixed_area_size);
	BUG_ON(ret);
#endif
}

/**
 * Reserve memory for ION and calculate amount of reusable memory for fmem.
 * We only reserve memory for heaps that are not reusable. However, we only
 * support one reusable heap at the moment so we ignore the reusable flag for
 * other than the first heap with reusable flag set. Also handle special case
 * for video heaps (MM,FW, and MFC). Video requires heaps MM and MFC to be
 * at a higher address than FW in addition to not more than 256MB away from the
 * base address of the firmware. This means that if MM is reusable the other
 * two heaps must be allocated in the same region as FW. This is handled by the
 * mem_is_fmem flag in the platform data. In addition the MM heap must be
 * adjacent to the FW heap for content protection purposes.
 */
static void __init reserve_ion_memory(void)
{
#if defined(CONFIG_ION_MSM) && defined(CONFIG_MSM_MULTIMEDIA_USE_ION)
	unsigned int i;
	unsigned int reusable_count = 0;
	unsigned int fixed_size = 0;
	unsigned int fixed_low_size, fixed_middle_size, fixed_high_size;
	unsigned long fixed_low_start, fixed_middle_start, fixed_high_start;

	adjust_mem_for_liquid();
	fmem_pdata.size = 0;
	fmem_pdata.reserved_size_low = 0;
	fmem_pdata.reserved_size_high = 0;
	fmem_pdata.align = PAGE_SIZE;
	fixed_low_size = 0;
	fixed_middle_size = 0;
	fixed_high_size = 0;

	/* We only support 1 reusable heap. Check if more than one heap
	 * is specified as reusable and set as non-reusable if found.
	 */
	for (i = 0; i < ion_pdata.nr; ++i) {
		const struct ion_platform_heap *heap = &(ion_pdata.heaps[i]);

		if (heap->type == ION_HEAP_TYPE_CP && heap->extra_data) {
			struct ion_cp_heap_pdata *data = heap->extra_data;

			reusable_count += (data->reusable) ? 1 : 0;

			if (data->reusable && reusable_count > 1) {
				pr_err("%s: Too many heaps specified as "
					"reusable. Heap %s was not configured "
					"as reusable.\n", __func__, heap->name);
				data->reusable = 0;
			}
		}
	}

	for (i = 0; i < ion_pdata.nr; ++i) {
		struct ion_platform_heap *heap =
						&(ion_pdata.heaps[i]);
		int align = SZ_4K;
		int iommu_map_all = 0;
		int adjacent_mem_id = INVALID_HEAP_ID;

		if (heap->extra_data) {
			int fixed_position = NOT_FIXED;
			int mem_is_fmem = 0;

			switch (heap->type) {
			case ION_HEAP_TYPE_CP:
				mem_is_fmem = ((struct ion_cp_heap_pdata *)
					heap->extra_data)->mem_is_fmem;
				fixed_position = ((struct ion_cp_heap_pdata *)
					heap->extra_data)->fixed_position;
				align = ((struct ion_cp_heap_pdata *)
						heap->extra_data)->align;
				iommu_map_all =
					((struct ion_cp_heap_pdata *)
					heap->extra_data)->iommu_map_all;
				break;
			case ION_HEAP_TYPE_CARVEOUT:
				mem_is_fmem = ((struct ion_co_heap_pdata *)
					heap->extra_data)->mem_is_fmem;
				fixed_position = ((struct ion_co_heap_pdata *)
					heap->extra_data)->fixed_position;
				adjacent_mem_id = ((struct ion_co_heap_pdata *)
					heap->extra_data)->adjacent_mem_id;
				break;
			default:
				break;
			}

			if (iommu_map_all) {
				if (heap->size & (SZ_64K-1)) {
					heap->size = ALIGN(heap->size, SZ_64K);
					pr_info("Heap %s not aligned to 64K. Adjusting size to %x\n",
						heap->name, heap->size);
				}
			}

			if (mem_is_fmem && adjacent_mem_id != INVALID_HEAP_ID)
				fmem_pdata.align = align;

			if (fixed_position != NOT_FIXED)
				fixed_size += heap->size;
			else
				reserve_mem_for_ion(MEMTYPE_EBI1, heap->size);

			if (fixed_position == FIXED_LOW)
				fixed_low_size += heap->size;
			else if (fixed_position == FIXED_MIDDLE)
				fixed_middle_size += heap->size;
			else if (fixed_position == FIXED_HIGH)
				fixed_high_size += heap->size;

			if (mem_is_fmem)
				fmem_pdata.size += heap->size;
		}
	}

	if (!fixed_size)
		return;

	if (fmem_pdata.size) {
		fmem_pdata.reserved_size_low = fixed_low_size;
		fmem_pdata.reserved_size_high = fixed_high_size;
	}

	msm8960_reserve_fixed_area(fixed_size + MSM_MM_FW_SIZE);

	fixed_low_start = MSM8960_FIXED_AREA_START;
	fixed_middle_start = fixed_low_start + fixed_low_size;
	fixed_high_start = fixed_middle_start + fixed_middle_size;

	for (i = 0; i < ion_pdata.nr; ++i) {
		struct ion_platform_heap *heap = &(ion_pdata.heaps[i]);

		if (heap->extra_data) {
			int fixed_position = NOT_FIXED;

			switch (heap->type) {
			case ION_HEAP_TYPE_CP:
				fixed_position = ((struct ion_cp_heap_pdata *)
					heap->extra_data)->fixed_position;
				break;
			case ION_HEAP_TYPE_CARVEOUT:
				fixed_position = ((struct ion_co_heap_pdata *)
					heap->extra_data)->fixed_position;
				break;
			default:
				break;
			}

			switch (fixed_position) {
			case FIXED_LOW:
				heap->base = fixed_low_start;
				break;
			case FIXED_MIDDLE:
				heap->base = fixed_middle_start;
				break;
			case FIXED_HIGH:
				heap->base = fixed_high_start;
				break;
			default:
				break;
			}
		}
	}
#endif
}

static void __init reserve_mdp_memory(void)
{
	msm8960_mdp_writeback(msm8960_reserve_table);
}

#if defined(CONFIG_MSM_CACHE_DUMP)
static struct msm_cache_dump_platform_data msm_cache_dump_pdata = {
	.l2_size = L2_BUFFER_SIZE,
};

static struct platform_device msm_cache_dump_device = {
	.name           = "msm_cache_dump",
	.id             = -1,
	.dev            = {
		.platform_data = &msm_cache_dump_pdata,
	},
};

#endif

static void reserve_cache_dump_memory(void)
{
#ifdef CONFIG_MSM_CACHE_DUMP
	unsigned int spare;
	unsigned int l1_size;
	unsigned int l2_size;
	unsigned int total;
	int ret;

	ret = scm_call(L1C_SERVICE_ID, L1C_BUFFER_GET_SIZE_COMMAND_ID, &spare,
		sizeof(spare), &l1_size, sizeof(l1_size));

	if (ret)
		/* Fall back to something reasonable here */
		l1_size = L1_BUFFER_SIZE;

	ret = scm_call(L1C_SERVICE_ID, L2C_BUFFER_GET_SIZE_COMMAND_ID, &spare,
		sizeof(spare), &l2_size, sizeof(l2_size));

	if (ret)
		/* Fall back to something reasonable here */
		l2_size = L2_BUFFER_SIZE;

	total = l1_size + l2_size;

	msm8960_reserve_table[MEMTYPE_EBI1].size += total;
	msm_cache_dump_pdata.l1_size = l1_size;
	msm_cache_dump_pdata.l2_size = l2_size;
#endif
}

static void __init msm8960_calculate_reserve_sizes(void)
{
	size_pmem_devices();
	reserve_pmem_memory();
	reserve_ion_memory();
	reserve_mdp_memory();
	reserve_rtb_memory();
	reserve_cache_dump_memory();
}

static struct reserve_info msm8960_reserve_info __initdata = {
	.memtype_reserve_table = msm8960_reserve_table,
	.calculate_reserve_sizes = msm8960_calculate_reserve_sizes,
	.reserve_fixed_area = msm8960_reserve_fixed_area,
	.paddr_to_memtype = msm8960_paddr_to_memtype,
};

static int msm8960_memory_bank_size(void)
{
	return 1<<29;
}

static void __init locate_unstable_memory(void)
{
	struct membank *mb = &meminfo.bank[meminfo.nr_banks - 1];
	unsigned long bank_size;
	unsigned long low, high;

	bank_size = msm8960_memory_bank_size();
	low = meminfo.bank[0].start;
	high = mb->start + mb->size;

	/* Check if 32 bit overflow occured */
	if (high < mb->start)
		high = ~0UL;

	low &= ~(bank_size - 1);

	if (high - low <= bank_size)
		return;

	msm8960_reserve_info.bank_size = bank_size;
#ifdef CONFIG_ENABLE_DMM
	msm8960_reserve_info.low_unstable_address = mb->start -
					MIN_MEMORY_BLOCK_SIZE + mb->size;
	msm8960_reserve_info.max_unstable_size = MIN_MEMORY_BLOCK_SIZE;
	pr_info("low unstable address %lx max size %lx bank size %lx\n",
		msm8960_reserve_info.low_unstable_address,
		msm8960_reserve_info.max_unstable_size,
		msm8960_reserve_info.bank_size);
#else
	msm8960_reserve_info.low_unstable_address = 0;
	msm8960_reserve_info.max_unstable_size = 0;
#endif
}

static void __init place_movable_zone(void)
{
#ifdef CONFIG_ENABLE_DMM
	movable_reserved_start = msm8960_reserve_info.low_unstable_address;
	movable_reserved_size = msm8960_reserve_info.max_unstable_size;
	pr_info("movable zone start %lx size %lx\n",
		movable_reserved_start, movable_reserved_size);
#endif
}

static void __init msm8960_early_memory(void)
{
	reserve_info = &msm8960_reserve_info;
	locate_unstable_memory();
	place_movable_zone();
}

static char prim_panel_name[PANEL_NAME_MAX_LEN];
static char ext_panel_name[PANEL_NAME_MAX_LEN];
static int __init prim_display_setup(char *param)
{
	if (strnlen(param, PANEL_NAME_MAX_LEN))
		strlcpy(prim_panel_name, param, PANEL_NAME_MAX_LEN);
	return 0;
}
early_param("prim_display", prim_display_setup);

static int __init ext_display_setup(char *param)
{
	if (strnlen(param, PANEL_NAME_MAX_LEN))
		strlcpy(ext_panel_name, param, PANEL_NAME_MAX_LEN);
	return 0;
}
early_param("ext_display", ext_display_setup);

static void __init msm8960_reserve(void)
{
	msm8960_set_display_params(prim_panel_name, ext_panel_name);
	msm_reserve();
	if (fmem_pdata.size) {
#if defined(CONFIG_ION_MSM) && defined(CONFIG_MSM_MULTIMEDIA_USE_ION)
		fmem_pdata.phys = reserve_info->fixed_area_start +
			MSM_MM_FW_SIZE;
		pr_info("mm fw at %lx (fixed) size %x\n",
			reserve_info->fixed_area_start, MSM_MM_FW_SIZE);
		pr_info("fmem start %lx (fixed) size %lx\n",
			fmem_pdata.phys, fmem_pdata.size);
#else
		fmem_pdata.phys = reserve_memory_for_fmem(fmem_pdata.size, fmem_pdata.align);
#endif
	}
}

static int msm8960_change_memory_power(u64 start, u64 size,
	int change_type)
{
	return soc_change_memory_power(start, size, change_type);
}


#ifdef CONFIG_ZTE_SDLOG

static bool msm8960_is_sdlog_enable(void)
{

    if (sdlog_flag != 0)
    {
        return true;
    }

    return false;
}



void __init msm8960_allocate_sdlog_region(void)
{
    
	unsigned long size;
	size = MSM_SDLOG_SIZE;

    //if sdlog is disable, do not need to reserve the memory.
    if (!msm8960_is_sdlog_enable())
    {
        pr_info("sdlog is disable, do not allocat mem\n");
        return;
    }

    //reserve 16M memory for sdlog use
    reserve_bootmem(MSM_SDLOG_PHYS, size, BOOTMEM_DEFAULT);
    pr_info("allocating 12m bytes at (0x82000000 physical) for sdlog\n");

}

#endif

static void __init msm8960_allocate_memory_regions(void)
{
#ifdef CONFIG_ZTE_SDLOG
	msm8960_allocate_sdlog_region();
#endif

	msm8960_allocate_fb_region();
}

#ifdef CONFIG_WCD9310_CODEC

#define TABLA_INTERRUPT_BASE (NR_MSM_IRQS + NR_GPIO_IRQS + NR_PM8921_IRQS)

/* Micbias setting is based on 8660 CDP/MTP/FLUID requirement
 * 4 micbiases are used to power various analog and digital
 * microphones operating at 1800 mV. Technically, all micbiases
 * can source from single cfilter since all microphones operate
 * at the same voltage level. The arrangement below is to make
 * sure all cfilters are exercised. LDO_H regulator ouput level
 * does not need to be as high as 2.85V. It is choosen for
 * microphone sensitivity purpose.
 */
static struct tabla_pdata tabla_platform_data = {
	.slimbus_slave_device = {
		.name = "tabla-slave",
		.e_addr = {0, 0, 0x10, 0, 0x17, 2},
	},
	.irq = MSM_GPIO_TO_INT(62),
	.irq_base = TABLA_INTERRUPT_BASE,
	.num_irqs = NR_TABLA_IRQS,
	.reset_gpio = PM8921_GPIO_PM_TO_SYS(34),
	.micbias = {
		.ldoh_v = TABLA_LDOH_2P85_V,
		.cfilt1_mv = 1800,
		.cfilt2_mv = 2700,
		.cfilt3_mv = 1800,
		.bias1_cfilt_sel = TABLA_CFILT1_SEL,
		.bias2_cfilt_sel = TABLA_CFILT2_SEL,
		.bias3_cfilt_sel = TABLA_CFILT3_SEL,
		.bias4_cfilt_sel = TABLA_CFILT3_SEL,
	},
	.regulator = {
	{
		.name = "CDC_VDD_CP",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_CDC_VDDA_CP_CUR_MAX,
	},
	{
		.name = "CDC_VDDA_RX",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_CDC_VDDA_RX_CUR_MAX,
	},
	{
		.name = "CDC_VDDA_TX",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_CDC_VDDA_TX_CUR_MAX,
	},
	{
		.name = "VDDIO_CDC",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_VDDIO_CDC_CUR_MAX,
	},
	{
		.name = "VDDD_CDC_D",
		.min_uV = 1225000,
		.max_uV = 1225000,
		.optimum_uA = WCD9XXX_VDDD_CDC_D_CUR_MAX,
	},
	{
		.name = "CDC_VDDA_A_1P2V",
		.min_uV = 1225000,
		.max_uV = 1225000,
		.optimum_uA = WCD9XXX_VDDD_CDC_A_CUR_MAX,
	},
	},
};

static struct slim_device msm_slim_tabla = {
	.name = "tabla-slim",
	.e_addr = {0, 1, 0x10, 0, 0x17, 2},
	.dev = {
		.platform_data = &tabla_platform_data,
	},
};

static struct tabla_pdata tabla20_platform_data = {
	.slimbus_slave_device = {
		.name = "tabla-slave",
		.e_addr = {0, 0, 0x60, 0, 0x17, 2},
	},
	.irq = MSM_GPIO_TO_INT(62),
	.irq_base = TABLA_INTERRUPT_BASE,
	.num_irqs = NR_TABLA_IRQS,
	.reset_gpio = PM8921_GPIO_PM_TO_SYS(34),
	.micbias = {
		.ldoh_v = TABLA_LDOH_2P85_V,
		.cfilt1_mv = 1800,
		.cfilt2_mv = 2700,
		.cfilt3_mv = 1800,
		.bias1_cfilt_sel = TABLA_CFILT1_SEL,
		.bias2_cfilt_sel = TABLA_CFILT2_SEL,
		.bias3_cfilt_sel = TABLA_CFILT3_SEL,
		.bias4_cfilt_sel = TABLA_CFILT3_SEL,
	},
	.regulator = {
	{
		.name = "CDC_VDD_CP",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_CDC_VDDA_CP_CUR_MAX,
	},
	{
		.name = "CDC_VDDA_RX",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_CDC_VDDA_RX_CUR_MAX,
	},
	{
		.name = "CDC_VDDA_TX",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_CDC_VDDA_TX_CUR_MAX,
	},
	{
		.name = "VDDIO_CDC",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_VDDIO_CDC_CUR_MAX,
	},
	{
		.name = "VDDD_CDC_D",
		.min_uV = 1225000,
		.max_uV = 1225000,
		.optimum_uA = WCD9XXX_VDDD_CDC_D_CUR_MAX,
	},
	{
		.name = "CDC_VDDA_A_1P2V",
		.min_uV = 1225000,
		.max_uV = 1225000,
		.optimum_uA = WCD9XXX_VDDD_CDC_A_CUR_MAX,
	},
	},
};

static struct slim_device msm_slim_tabla20 = {
	.name = "tabla2x-slim",
	.e_addr = {0, 1, 0x60, 0, 0x17, 2},
	.dev = {
		.platform_data = &tabla20_platform_data,
	},
};
#endif

static struct slim_boardinfo msm_slim_devices[] = {
#ifdef CONFIG_WCD9310_CODEC
	{
		.bus_num = 1,
		.slim_slave = &msm_slim_tabla,
	},
	{
		.bus_num = 1,
		.slim_slave = &msm_slim_tabla20,
	},
#endif
	/* add more slimbus slaves as needed */
};

#define MSM_WCNSS_PHYS	0x03000000
#define MSM_WCNSS_SIZE	0x280000

static struct resource resources_wcnss_wlan[] = {
	{
		.start	= RIVA_APPS_WLAN_RX_DATA_AVAIL_IRQ,
		.end	= RIVA_APPS_WLAN_RX_DATA_AVAIL_IRQ,
		.name	= "wcnss_wlanrx_irq",
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= RIVA_APPS_WLAN_DATA_XFER_DONE_IRQ,
		.end	= RIVA_APPS_WLAN_DATA_XFER_DONE_IRQ,
		.name	= "wcnss_wlantx_irq",
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= MSM_WCNSS_PHYS,
		.end	= MSM_WCNSS_PHYS + MSM_WCNSS_SIZE - 1,
		.name	= "wcnss_mmio",
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= 84,
		.end	= 88,
		.name	= "wcnss_gpios_5wire",
		.flags	= IORESOURCE_IO,
	},
};

static struct qcom_wcnss_opts qcom_wcnss_pdata = {
	.has_48mhz_xo	= 1,
};

static struct platform_device msm_device_wcnss_wlan = {
	.name		= "wcnss_wlan",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(resources_wcnss_wlan),
	.resource	= resources_wcnss_wlan,
	.dev		= {.platform_data = &qcom_wcnss_pdata},
};

#if defined(CONFIG_CRYPTO_DEV_QCRYPTO) || \
		defined(CONFIG_CRYPTO_DEV_QCRYPTO_MODULE) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV_MODULE)

#define QCE_SIZE		0x10000
#define QCE_0_BASE		0x18500000

#define QCE_HW_KEY_SUPPORT	0
#define QCE_SHA_HMAC_SUPPORT	1
#define QCE_SHARE_CE_RESOURCE	1
#define QCE_CE_SHARED		0

/* Begin Bus scaling definitions */
static struct msm_bus_vectors crypto_hw_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_ADM_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
	{
		.src = MSM_BUS_MASTER_ADM_PORT1,
		.dst = MSM_BUS_SLAVE_GSBI1_UART,
		.ab = 0,
		.ib = 0,
	},
};

static struct msm_bus_vectors crypto_hw_active_vectors[] = {
	{
		.src = MSM_BUS_MASTER_ADM_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 70000000UL,
		.ib = 70000000UL,
	},
	{
		.src = MSM_BUS_MASTER_ADM_PORT1,
		.dst = MSM_BUS_SLAVE_GSBI1_UART,
		.ab = 2480000000UL,
		.ib = 2480000000UL,
	},
};

static struct msm_bus_paths crypto_hw_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(crypto_hw_init_vectors),
		crypto_hw_init_vectors,
	},
	{
		ARRAY_SIZE(crypto_hw_active_vectors),
		crypto_hw_active_vectors,
	},
};

static struct msm_bus_scale_pdata crypto_hw_bus_scale_pdata = {
		crypto_hw_bus_scale_usecases,
		ARRAY_SIZE(crypto_hw_bus_scale_usecases),
		.name = "cryptohw",
};
/* End Bus Scaling Definitions*/

static struct resource qcrypto_resources[] = {
	[0] = {
		.start = QCE_0_BASE,
		.end = QCE_0_BASE + QCE_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.name = "crypto_channels",
		.start = DMOV_CE_IN_CHAN,
		.end = DMOV_CE_OUT_CHAN,
		.flags = IORESOURCE_DMA,
	},
	[2] = {
		.name = "crypto_crci_in",
		.start = DMOV_CE_IN_CRCI,
		.end = DMOV_CE_IN_CRCI,
		.flags = IORESOURCE_DMA,
	},
	[3] = {
		.name = "crypto_crci_out",
		.start = DMOV_CE_OUT_CRCI,
		.end = DMOV_CE_OUT_CRCI,
		.flags = IORESOURCE_DMA,
	},
};

static struct resource qcedev_resources[] = {
	[0] = {
		.start = QCE_0_BASE,
		.end = QCE_0_BASE + QCE_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.name = "crypto_channels",
		.start = DMOV_CE_IN_CHAN,
		.end = DMOV_CE_OUT_CHAN,
		.flags = IORESOURCE_DMA,
	},
	[2] = {
		.name = "crypto_crci_in",
		.start = DMOV_CE_IN_CRCI,
		.end = DMOV_CE_IN_CRCI,
		.flags = IORESOURCE_DMA,
	},
	[3] = {
		.name = "crypto_crci_out",
		.start = DMOV_CE_OUT_CRCI,
		.end = DMOV_CE_OUT_CRCI,
		.flags = IORESOURCE_DMA,
	},
};

#endif

#if defined(CONFIG_CRYPTO_DEV_QCRYPTO) || \
		defined(CONFIG_CRYPTO_DEV_QCRYPTO_MODULE)

static struct msm_ce_hw_support qcrypto_ce_hw_suppport = {
	.ce_shared = QCE_CE_SHARED,
	.shared_ce_resource = QCE_SHARE_CE_RESOURCE,
	.hw_key_support = QCE_HW_KEY_SUPPORT,
	.sha_hmac = QCE_SHA_HMAC_SUPPORT,
	.bus_scale_table = &crypto_hw_bus_scale_pdata,
};

static struct platform_device qcrypto_device = {
	.name		= "qcrypto",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(qcrypto_resources),
	.resource	= qcrypto_resources,
	.dev		= {
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.platform_data = &qcrypto_ce_hw_suppport,
	},
};
#endif

#if defined(CONFIG_CRYPTO_DEV_QCEDEV) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV_MODULE)

static struct msm_ce_hw_support qcedev_ce_hw_suppport = {
	.ce_shared = QCE_CE_SHARED,
	.shared_ce_resource = QCE_SHARE_CE_RESOURCE,
	.hw_key_support = QCE_HW_KEY_SUPPORT,
	.sha_hmac = QCE_SHA_HMAC_SUPPORT,
	.bus_scale_table = &crypto_hw_bus_scale_pdata,
};

static struct platform_device qcedev_device = {
	.name		= "qce",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(qcedev_resources),
	.resource	= qcedev_resources,
	.dev		= {
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.platform_data = &qcedev_ce_hw_suppport,
	},
};
#endif

#define MDM2AP_ERRFATAL			70
#define AP2MDM_ERRFATAL			95
#define MDM2AP_STATUS			69
#define AP2MDM_STATUS			94
#define AP2MDM_PMIC_RESET_N		80
#define AP2MDM_KPDPWR_N			81

static struct resource mdm_resources[] = {
	{
		.start	= MDM2AP_ERRFATAL,
		.end	= MDM2AP_ERRFATAL,
		.name	= "MDM2AP_ERRFATAL",
		.flags	= IORESOURCE_IO,
	},
	{
		.start	= AP2MDM_ERRFATAL,
		.end	= AP2MDM_ERRFATAL,
		.name	= "AP2MDM_ERRFATAL",
		.flags	= IORESOURCE_IO,
	},
	{
		.start	= MDM2AP_STATUS,
		.end	= MDM2AP_STATUS,
		.name	= "MDM2AP_STATUS",
		.flags	= IORESOURCE_IO,
	},
	{
		.start	= AP2MDM_STATUS,
		.end	= AP2MDM_STATUS,
		.name	= "AP2MDM_STATUS",
		.flags	= IORESOURCE_IO,
	},
	{
		.start	= AP2MDM_PMIC_RESET_N,
		.end	= AP2MDM_PMIC_RESET_N,
		.name	= "AP2MDM_PMIC_RESET_N",
		.flags	= IORESOURCE_IO,
	},
	{
		.start	= AP2MDM_KPDPWR_N,
		.end	= AP2MDM_KPDPWR_N,
		.name	= "AP2MDM_KPDPWR_N",
		.flags	= IORESOURCE_IO,
	},
};

static struct mdm_platform_data mdm_platform_data = {
	.mdm_version = "2.5",
};

static struct platform_device mdm_device = {
	.name		= "mdm2_modem",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(mdm_resources),
	.resource	= mdm_resources,
	.dev		= {
		.platform_data = &mdm_platform_data,
	},
};

static struct platform_device *mdm_devices[] __initdata = {
	&mdm_device,
};

#define MSM_SHARED_RAM_PHYS 0x80000000

static void __init msm8960_map_io(void)
{
	msm_shared_ram_phys = MSM_SHARED_RAM_PHYS;
	msm_map_msm8960_io();

	if (socinfo_init() < 0)
		pr_err("socinfo_init() failed!\n");
}

static void __init msm8960_init_irq(void)
{
	msm_mpm_irq_extn_init();
	gic_init(0, GIC_PPI_START, MSM_QGIC_DIST_BASE,
						(void *)MSM_QGIC_CPU_BASE);
}

static void __init msm8960_init_buses(void)
{
#ifdef CONFIG_MSM_BUS_SCALING
	msm_bus_rpm_set_mt_mask();
	msm_bus_8960_apps_fabric_pdata.rpm_enabled = 1;
	msm_bus_8960_sys_fabric_pdata.rpm_enabled = 1;
	msm_bus_8960_mm_fabric_pdata.rpm_enabled = 1;
	msm_bus_apps_fabric.dev.platform_data =
		&msm_bus_8960_apps_fabric_pdata;
	msm_bus_sys_fabric.dev.platform_data = &msm_bus_8960_sys_fabric_pdata;
	msm_bus_mm_fabric.dev.platform_data = &msm_bus_8960_mm_fabric_pdata;
	msm_bus_sys_fpb.dev.platform_data = &msm_bus_8960_sys_fpb_pdata;
	msm_bus_cpss_fpb.dev.platform_data = &msm_bus_8960_cpss_fpb_pdata;
#endif
}

static struct msm_spi_platform_data msm8960_qup_spi_gsbi1_pdata = {
	.max_clock_speed = 15060000,
	.infinite_mode   = 1	
};

#ifdef CONFIG_USB_MSM_OTG_72K
static struct msm_otg_platform_data msm_otg_pdata;
#else
static int wr_phy_init_seq[] = {
	0x44, 0x80, /* set VBUS valid threshold
			and disconnect valid threshold */
	0x38, 0x81, /* update DC voltage level */
	0x14, 0x82, /* set preemphasis and rise/fall time */
	0x13, 0x83, /* set source impedance adjusment */
	-1};

static int liquid_v1_phy_init_seq[] = {
	0x44, 0x80,/* set VBUS valid threshold
			and disconnect valid threshold */
	0x3C, 0x81,/* update DC voltage level */
	0x18, 0x82,/* set preemphasis and rise/fall time */
	0x23, 0x83,/* set source impedance sdjusment */
	-1};

static int zte_phy_init_seq_override[] = {
#ifdef CONFIG_MACH_ELDEN	
	0x34, 0x81,
	0x39, 0x82,
	0x33, 0x83,
#elif defined(CONFIG_MACH_GORDON)
	0x37, 0x81,
	0x39, 0x82,
#elif defined(CONFIG_MACH_DANA)
	0x38, 0x81,
	0x1b, 0x82,	
#elif defined(CONFIG_MACH_FROSTY)	
	0x37, 0x81,
	0x2b, 0x82,	
#elif defined(CONFIG_MACH_KISKA)
	0x44, 0x80,
	0x3f, 0x81,	
	0x3A, 0x82,
	0x13, 0x83,
#elif defined(CONFIG_MACH_HAYES)
	0x34, 0x81,
	0x39, 0x82,
	0x33, 0x83,
#endif	
	-1};/*for usb eye diagram test*/

#ifdef CONFIG_MSM_BUS_SCALING
/* Bandwidth requests (zero) if no vote placed */
static struct msm_bus_vectors usb_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_SPS,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};

/* Bus bandwidth requests in Bytes/sec */
static struct msm_bus_vectors usb_max_vectors[] = {
	{
		.src = MSM_BUS_MASTER_SPS,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 60000000,		/* At least 480Mbps on bus. */
		.ib = 960000000,	/* MAX bursts rate */
	},
};

static struct msm_bus_paths usb_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(usb_init_vectors),
		usb_init_vectors,
	},
	{
		ARRAY_SIZE(usb_max_vectors),
		usb_max_vectors,
	},
};

static struct msm_bus_scale_pdata usb_bus_scale_pdata = {
	usb_bus_scale_usecases,
	ARRAY_SIZE(usb_bus_scale_usecases),
	.name = "usb",
};
#endif

static struct msm_otg_platform_data msm_otg_pdata = {
	.mode			= USB_PERIPHERAL, //USB_OTG
	.otg_control		= OTG_PMIC_CONTROL,
	.phy_type		= SNPS_28NM_INTEGRATED_PHY,
	.pmic_id_irq		= PM8921_USB_ID_IN_IRQ(PM8921_IRQ_BASE),
	.power_budget		= 750,
#ifdef CONFIG_MSM_BUS_SCALING
	.bus_scale_table	= &usb_bus_scale_pdata,
#endif
};
#endif

#ifdef CONFIG_USB_EHCI_MSM_HSIC
#define HSIC_HUB_RESET_GPIO	91
static struct msm_hsic_host_platform_data msm_hsic_pdata = {
	.strobe		= 150,
	.data		= 151,
};
#else
static struct msm_hsic_host_platform_data msm_hsic_pdata;
#endif

#define PID_MAGIC_ID		0x71432909
#define SERIAL_NUM_MAGIC_ID	0x61945374
#define SERIAL_NUMBER_LENGTH	127
#define DLOAD_USB_BASE_ADD	0x2A03F0C8

struct magic_num_struct {
	uint32_t pid;
	uint32_t serial_num;
};

struct dload_struct {
	uint32_t	reserved1;
	uint32_t	reserved2;
	uint32_t	reserved3;
	uint16_t	reserved4;
	uint16_t	pid;
	char		serial_number[SERIAL_NUMBER_LENGTH];
	uint16_t	reserved5;
	struct magic_num_struct magic_struct;
};

static int usb_diag_update_pid_and_serial_num(uint32_t pid, const char *snum)
{
	struct dload_struct __iomem *dload = 0;

	dload = ioremap(DLOAD_USB_BASE_ADD, sizeof(*dload));
	if (!dload) {
		pr_err("%s: cannot remap I/O memory region: %08x\n",
					__func__, DLOAD_USB_BASE_ADD);
		return -ENXIO;
	}

	pr_debug("%s: dload:%p pid:%x serial_num:%s\n",
				__func__, dload, pid, snum);
	/* update pid */
	dload->magic_struct.pid = PID_MAGIC_ID;
	dload->pid = pid;

	/* update serial number */
	dload->magic_struct.serial_num = 0;
	if (!snum) {
		memset(dload->serial_number, 0, SERIAL_NUMBER_LENGTH);
		goto out;
	}

	dload->magic_struct.serial_num = SERIAL_NUM_MAGIC_ID;
	strlcpy(dload->serial_number, snum, SERIAL_NUMBER_LENGTH);
out:
	iounmap(dload);
	return 0;
}

static struct android_usb_platform_data android_usb_pdata = {
	.update_pid_and_serial_num = usb_diag_update_pid_and_serial_num,
};

static struct platform_device android_usb_device = {
	.name	= "android_usb",
	.id	= -1,
	.dev	= {
		.platform_data = &android_usb_pdata,
	},
};

static uint8_t spm_wfi_cmd_sequence[] __initdata = {
			0x03, 0x0f,
};

static uint8_t spm_power_collapse_without_rpm[] __initdata = {
			0x00, 0x24, 0x54, 0x10,
			0x09, 0x03, 0x01,
			0x10, 0x54, 0x30, 0x0C,
			0x24, 0x30, 0x0f,
};

static uint8_t spm_power_collapse_with_rpm[] __initdata = {
			0x00, 0x24, 0x54, 0x10,
			0x09, 0x07, 0x01, 0x0B,
			0x10, 0x54, 0x30, 0x0C,
			0x24, 0x30, 0x0f,
};

static struct msm_spm_seq_entry msm_spm_seq_list[] __initdata = {
	[0] = {
		.mode = MSM_SPM_MODE_CLOCK_GATING,
		.notify_rpm = false,
		.cmd = spm_wfi_cmd_sequence,
	},
	[1] = {
		.mode = MSM_SPM_MODE_POWER_COLLAPSE,
		.notify_rpm = false,
		.cmd = spm_power_collapse_without_rpm,
	},
	[2] = {
		.mode = MSM_SPM_MODE_POWER_COLLAPSE,
		.notify_rpm = true,
		.cmd = spm_power_collapse_with_rpm,
	},
};

static struct msm_spm_platform_data msm_spm_data[] __initdata = {
	[0] = {
		.reg_base_addr = MSM_SAW0_BASE,
		.reg_init_values[MSM_SPM_REG_SAW2_CFG] = 0x1F,
#if defined(CONFIG_MSM_AVS_HW)
		.reg_init_values[MSM_SPM_REG_SAW2_AVS_CTL] = 0x00,
		.reg_init_values[MSM_SPM_REG_SAW2_AVS_HYSTERESIS] = 0x00,
#endif
		.reg_init_values[MSM_SPM_REG_SAW2_SPM_CTL] = 0x01,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DLY] = 0x02020204,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_0] = 0x0060009C,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_1] = 0x0000001C,
		.vctl_timeout_us = 50,
		.num_modes = ARRAY_SIZE(msm_spm_seq_list),
		.modes = msm_spm_seq_list,
	},
	[1] = {
		.reg_base_addr = MSM_SAW1_BASE,
		.reg_init_values[MSM_SPM_REG_SAW2_CFG] = 0x1F,
#if defined(CONFIG_MSM_AVS_HW)
		.reg_init_values[MSM_SPM_REG_SAW2_AVS_CTL] = 0x00,
		.reg_init_values[MSM_SPM_REG_SAW2_AVS_HYSTERESIS] = 0x00,
#endif
		.reg_init_values[MSM_SPM_REG_SAW2_SPM_CTL] = 0x01,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DLY] = 0x02020204,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_0] = 0x0060009C,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_1] = 0x0000001C,
		.vctl_timeout_us = 50,
		.num_modes = ARRAY_SIZE(msm_spm_seq_list),
		.modes = msm_spm_seq_list,
	},
};

static uint8_t l2_spm_wfi_cmd_sequence[] __initdata = {
			0x00, 0x20, 0x03, 0x20,
			0x00, 0x0f,
};

static uint8_t l2_spm_gdhs_cmd_sequence[] __initdata = {
			0x00, 0x20, 0x34, 0x64,
			0x48, 0x07, 0x48, 0x20,
			0x50, 0x64, 0x04, 0x34,
			0x50, 0x0f,
};
static uint8_t l2_spm_power_off_cmd_sequence[] __initdata = {
			0x00, 0x10, 0x34, 0x64,
			0x48, 0x07, 0x48, 0x10,
			0x50, 0x64, 0x04, 0x34,
			0x50, 0x0F,
};

static struct msm_spm_seq_entry msm_spm_l2_seq_list[] __initdata = {
	[0] = {
		.mode = MSM_SPM_L2_MODE_RETENTION,
		.notify_rpm = false,
		.cmd = l2_spm_wfi_cmd_sequence,
	},
	[1] = {
		.mode = MSM_SPM_L2_MODE_GDHS,
		.notify_rpm = true,
		.cmd = l2_spm_gdhs_cmd_sequence,
	},
	[2] = {
		.mode = MSM_SPM_L2_MODE_POWER_COLLAPSE,
		.notify_rpm = true,
		.cmd = l2_spm_power_off_cmd_sequence,
	},
};

static struct msm_spm_platform_data msm_spm_l2_data[] __initdata = {
	[0] = {
		.reg_base_addr = MSM_SAW_L2_BASE,
		.reg_init_values[MSM_SPM_REG_SAW2_SPM_CTL] = 0x00,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DLY] = 0x02020204,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_0] = 0x00A000AE,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_1] = 0x00A00020,
		.modes = msm_spm_l2_seq_list,
		.num_modes = ARRAY_SIZE(msm_spm_l2_seq_list),
	},
};

#define PM_HAP_EN_GPIO		PM8921_GPIO_PM_TO_SYS(33)
#define PM_HAP_LEN_GPIO		PM8921_GPIO_PM_TO_SYS(20)

static struct msm_xo_voter *xo_handle_d1;

static int isa1200_power(int on)
{
	int rc = 0;

	gpio_set_value(HAP_SHIFT_LVL_OE_GPIO, !!on);

	rc = on ? msm_xo_mode_vote(xo_handle_d1, MSM_XO_MODE_ON) :
			msm_xo_mode_vote(xo_handle_d1, MSM_XO_MODE_OFF);
	if (rc < 0) {
		pr_err("%s: failed to %svote for TCXO D1 buffer%d\n",
				__func__, on ? "" : "de-", rc);
		goto err_xo_vote;
	}

	return 0;

err_xo_vote:
	gpio_set_value(HAP_SHIFT_LVL_OE_GPIO, !on);
	return rc;
}

static int isa1200_dev_setup(bool enable)
{
	int rc = 0;

	struct pm_gpio hap_gpio_config = {
		.direction      = PM_GPIO_DIR_OUT,
		.pull           = PM_GPIO_PULL_NO,
		.out_strength   = PM_GPIO_STRENGTH_HIGH,
		.function       = PM_GPIO_FUNC_NORMAL,
		.inv_int_pol    = 0,
		.vin_sel        = 2,
		.output_buffer  = PM_GPIO_OUT_BUF_CMOS,
		.output_value   = 0,
	};

	if (enable == true) {
		rc = pm8xxx_gpio_config(PM_HAP_EN_GPIO, &hap_gpio_config);
		if (rc) {
			pr_err("%s: pm8921 gpio %d config failed(%d)\n",
					__func__, PM_HAP_EN_GPIO, rc);
			return rc;
		}

		rc = pm8xxx_gpio_config(PM_HAP_LEN_GPIO, &hap_gpio_config);
		if (rc) {
			pr_err("%s: pm8921 gpio %d config failed(%d)\n",
					__func__, PM_HAP_LEN_GPIO, rc);
			return rc;
		}

		rc = gpio_request(HAP_SHIFT_LVL_OE_GPIO, "hap_shft_lvl_oe");
		if (rc) {
			pr_err("%s: unable to request gpio %d (%d)\n",
					__func__, HAP_SHIFT_LVL_OE_GPIO, rc);
			return rc;
		}

		rc = gpio_direction_output(HAP_SHIFT_LVL_OE_GPIO, 0);
		if (rc) {
			pr_err("%s: Unable to set direction\n", __func__);
			goto free_gpio;
		}

		xo_handle_d1 = msm_xo_get(MSM_XO_TCXO_D1, "isa1200");
		if (IS_ERR(xo_handle_d1)) {
			rc = PTR_ERR(xo_handle_d1);
			pr_err("%s: failed to get the handle for D1(%d)\n",
							__func__, rc);
			goto gpio_set_dir;
		}
	} else {
		gpio_free(HAP_SHIFT_LVL_OE_GPIO);

		msm_xo_put(xo_handle_d1);
	}

	return 0;

gpio_set_dir:
	gpio_set_value(HAP_SHIFT_LVL_OE_GPIO, 0);
free_gpio:
	gpio_free(HAP_SHIFT_LVL_OE_GPIO);
	return rc;
}

static struct isa1200_regulator isa1200_reg_data[] = {
	{
		.name = "vcc_i2c",
		.min_uV = ISA_I2C_VTG_MIN_UV,
		.max_uV = ISA_I2C_VTG_MAX_UV,
		.load_uA = ISA_I2C_CURR_UA,
	},
};

static struct isa1200_platform_data isa1200_1_pdata = {
	.name = "vibrator",
	.dev_setup = isa1200_dev_setup,
	.power_on = isa1200_power,
	.hap_en_gpio = PM_HAP_EN_GPIO,
	.hap_len_gpio = PM_HAP_LEN_GPIO,
	.max_timeout = 15000,
	.mode_ctrl = PWM_GEN_MODE,
	.pwm_fd = {
		.pwm_div = 256,
	},
	.is_erm = false,
	.smart_en = true,
	.ext_clk_en = true,
	.chip_en = 1,
	.regulator_info = isa1200_reg_data,
	.num_regulators = ARRAY_SIZE(isa1200_reg_data),
};

static struct i2c_board_info msm_isa1200_board_info[] __initdata = {
	{
		I2C_BOARD_INFO("isa1200_1", 0x90>>1),
	},
};


#if 0 
static void gsbi_qup_i2c_touch_gpio_config(int adap_id, int config_type)
{
  int rc = 0;
	rc = gpio_request(16, "scl_data");/* GSBI3 I2C QUP SDA */
	if (rc) {
		pr_err("%s: unable to request GSBI3 I2C QUP SDA gpio [%d]\n",
				__func__, 16);
	}
	rc = gpio_request(17, "scl_clk");/* GSBI3 I2C QUP SCL */
	if (rc) {
		pr_err("%s: unable to request GSBI3 I2C QUP SCL gpio [%d]\n",
				__func__, 17);
	}	
}
#endif

static struct i2c_board_info sii_device_info[] __initdata = {
	{
		I2C_BOARD_INFO("Sil-9244", 0x39),
		.flags = I2C_CLIENT_WAKE,
		.irq = MSM_GPIO_TO_INT(15),
	},
};

#if 0
#ifndef CONFIG_MSM_DSPS
static void gsbi_qup_i2c_sensors_gpio_config(int adap_id, int config_type)
{
  int rc = 0;
	rc = gpio_request(44, "scl_data");/* GSBI12 I2C QUP SDA */
	if (rc) {
		pr_err("%s: unable to request GSBI12 I2C QUP SDA gpio [%d]\n",
				__func__, 44);
	}
	rc = gpio_request(45, "scl_clk");/* GSBI12 I2C QUP SCL */
	if (rc) {
		pr_err("%s: unable to request GSBI12 I2C QUP SCL gpio [%d]\n",
				__func__, 45);
	}	
}
#endif
#endif /* #if 0 */

#ifdef CONFIG_MHL_Sii8334
static void gsbi_qup_i2c_mhl_gpio_config(int adap_id, int config_type)
{
  int rc = 0;
	rc = gpio_request(24, "scl_data");/* GSBI10 I2C QUP SDA */
	if (rc) {
		printk("%s: unable to request GSBI10 I2C QUP SDA gpio [%d]\n",
				__func__, 24);
	}
	else
	{
	        printk("%s: ok to request GSBI10 I2C QUP SDA gpio [%d]\n",
				__func__, 24);
	 }
	 
//	 gpio_direction_output(24, 1);

	rc = gpio_request(25, "scl_clk");/* GSBI10 I2C QUP SCL */
	if (rc) {
		printk("%s: unable to request GSBI10 I2C QUP SCL gpio [%d]\n",
				__func__, 25);
	}
	else
	{
               printk("%s: ok to request GSBI10 I2C QUP SCL gpio [%d]\n",
				__func__, 25);
	 }
	
//	 gpio_direction_output(25, 1);
}
#endif

static struct msm_i2c_platform_data msm8960_i2c_qup_gsbi4_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
};

#ifdef CONFIG_ISPCAM 
static void gsbi_qup_i2c_isp_gpio_config(int adap_id, int config_type)
{

  int rc = 0;
	rc = gpio_request(32, "scl_data");/* GSBI7 I2C QUP DATA */
	if (rc) {
		printk("%s: unable to request GSBI7 I2C QUP DATA gpio [%d]\n",
				__func__, 32);
	}
	else
	{
	        printk("%s: ok to request GSBI7 I2C QUP DATA gpio [%d]\n",
				__func__, 32);
	 }
	 
	rc = gpio_request(33, "scl_clk");/* GSBI7 I2C QUP CLK */
	if (rc) {
		printk("%s: unable to request GSBI7 I2C QUP CLK gpio [%d]\n",
				__func__, 33);
	}
	else
	{
               printk("%s: ok to request GSBI7 I2C QUP CLK gpio [%d]\n",
				__func__, 33);
	 }
	 

}
#endif
#ifdef CONFIG_FLSH_ADP1650
static void gsbi_qup_i2c_isp_gpio_config(int adap_id, int config_type)
{

  int rc = 0;
	rc = gpio_request(32, "scl_data");/* GSBI7 I2C QUP DATA */
	if (rc) {
		printk("%s: unable to request GSBI7 I2C QUP DATA gpio [%d]\n",
				__func__, 32);
	}
	else
	{
	        printk("%s: ok to request GSBI7 I2C QUP DATA gpio [%d]\n",
				__func__, 32);
	 }
	 
	rc = gpio_request(33, "scl_clk");/* GSBI7 I2C QUP CLK */
	if (rc) {
		printk("%s: unable to request GSBI7 I2C QUP CLK gpio [%d]\n",
				__func__, 33);
	}
	else
	{
               printk("%s: ok to request GSBI7 I2C QUP CLK gpio [%d]\n",
				__func__, 33);
	 }
	 

}
#endif
static struct msm_i2c_platform_data msm8960_i2c_qup_gsbi3_pdata = {
	.clk_freq = 300000,
	.src_clk_rate = 24000000,
};

#ifdef CONFIG_MHL_Sii8334
static struct msm_i2c_platform_data msm8960_i2c_qup_gsbi5_pdata = {
	.clk_freq = 300000,
	.src_clk_rate = 24000000,
	.msm_i2c_config_gpio = gsbi_qup_i2c_mhl_gpio_config,
	.use_gsbi_shared_mode = 1,
};
#endif
#ifdef CONFIG_ISPCAM 
static struct msm_i2c_platform_data msm8960_i2c_qup_gsbi7_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
	.use_gsbi_shared_mode = 1,
	.msm_i2c_config_gpio = gsbi_qup_i2c_isp_gpio_config,
};
#endif

#ifdef CONFIG_PN544_NFC
static struct msm_i2c_platform_data msm8960_i2c_qup_gsbi10_pdata = {
	.clk_freq = 300000,
	.src_clk_rate = 24000000,
};
#else
static struct msm_i2c_platform_data msm8960_i2c_qup_gsbi10_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
};
#endif /* CONFIG_PN544_NFC */
#ifdef CONFIG_FLSH_ADP1650
static struct msm_i2c_platform_data msm8960_i2c_qup_gsbi7_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
	.use_gsbi_shared_mode = 1,
	.msm_i2c_config_gpio = gsbi_qup_i2c_isp_gpio_config,
};
#endif /* CONFIG_PN544_NFC */
static struct msm_i2c_platform_data msm8960_i2c_qup_gsbi12_pdata = {
	.clk_freq = 400000,
	.src_clk_rate = 24000000,
};

static struct msm_rpm_platform_data msm_rpm_data = {
	.reg_base_addrs = {
		[MSM_RPM_PAGE_STATUS] = MSM_RPM_BASE,
		[MSM_RPM_PAGE_CTRL] = MSM_RPM_BASE + 0x400,
		[MSM_RPM_PAGE_REQ] = MSM_RPM_BASE + 0x600,
		[MSM_RPM_PAGE_ACK] = MSM_RPM_BASE + 0xa00,
	},

	.irq_ack = RPM_APCC_CPU0_GP_HIGH_IRQ,
	.irq_err = RPM_APCC_CPU0_GP_LOW_IRQ,
	.irq_vmpm = RPM_APCC_CPU0_GP_MEDIUM_IRQ,
	.msm_apps_ipc_rpm_reg = MSM_APCS_GCC_BASE + 0x008,
	.msm_apps_ipc_rpm_val = 4,
};

static struct msm_pm_sleep_status_data msm_pm_slp_sts_data = {
	.base_addr = MSM_ACC0_BASE + 0x08,
	.cpu_offset = MSM_ACC1_BASE - MSM_ACC0_BASE,
	.mask = 1UL << 13,
};

static struct ks8851_pdata spi_eth_pdata = {
	.irq_gpio = KS8851_IRQ_GPIO,
	.rst_gpio = KS8851_RST_GPIO,
};

static struct spi_board_info spi_board_info[] __initdata = {
	{
		.modalias               = "ks8851",
		.irq                    = MSM_GPIO_TO_INT(KS8851_IRQ_GPIO),
		.max_speed_hz           = 19200000,
		.bus_num                = 0,
		.chip_select            = 0,
		.mode                   = SPI_MODE_0,
		.platform_data		= &spi_eth_pdata
	},
	{
		.modalias               = "dsi_novatek_3d_panel_spi",
		.max_speed_hz           = 10800000,
		.bus_num                = 0,
		.chip_select            = 1,
		.mode                   = SPI_MODE_0,
	},
};

static struct platform_device msm_device_saw_core0 = {
	.name          = "saw-regulator",
	.id            = 0,
	.dev	= {
		.platform_data = &msm_saw_regulator_pdata_s5,
	},
};

static struct platform_device msm_device_saw_core1 = {
	.name          = "saw-regulator",
	.id            = 1,
	.dev	= {
		.platform_data = &msm_saw_regulator_pdata_s6,
	},
};

static struct tsens_platform_data msm_tsens_pdata  = {
		.slope			= {910, 910, 910, 910, 910},
		.tsens_factor		= 1000,
		.hw_type		= MSM_8960,
		.tsens_num_sensor	= 5,
};

#ifdef CONFIG_MSM_FAKE_BATTERY
static struct platform_device fish_battery_device = {
	.name = "fish_battery",
};
#endif

static struct platform_device msm8960_device_ext_5v_vreg __devinitdata = {
	.name	= GPIO_REGULATOR_DEV_NAME,
	.id	= PM8921_MPP_PM_TO_SYS(7),
	.dev	= {
		.platform_data = &msm_gpio_regulator_pdata[GPIO_VREG_ID_EXT_5V],
	},
};

static struct platform_device msm8960_device_ext_l2_vreg __devinitdata = {
	.name	= GPIO_REGULATOR_DEV_NAME,
	.id	= 91,
	.dev	= {
		.platform_data = &msm_gpio_regulator_pdata[GPIO_VREG_ID_EXT_L2],
	},
};

static struct platform_device msm8960_device_ext_3p3v_vreg __devinitdata = {
	.name	= GPIO_REGULATOR_DEV_NAME,
	.id	= PM8921_GPIO_PM_TO_SYS(17),
	.dev	= {
		.platform_data =
			&msm_gpio_regulator_pdata[GPIO_VREG_ID_EXT_3P3V],
	},
};

static struct platform_device msm8960_device_ext_otg_sw_vreg __devinitdata = {
	.name	= GPIO_REGULATOR_DEV_NAME,
	.id	= PM8921_GPIO_PM_TO_SYS(42),
	.dev	= {
		.platform_data =
			&msm_gpio_regulator_pdata[GPIO_VREG_ID_EXT_OTG_SW],
	},
};

static struct platform_device msm8960_device_rpm_regulator __devinitdata = {
	.name	= "rpm-regulator",
	.id	= -1,
	.dev	= {
		.platform_data = &msm_rpm_regulator_pdata,
	},
};

static struct msm_rpm_log_platform_data msm_rpm_log_pdata = {
	.phys_addr_base = 0x0010C000,
	.reg_offsets = {
		[MSM_RPM_LOG_PAGE_INDICES] = 0x00000080,
		[MSM_RPM_LOG_PAGE_BUFFER]  = 0x000000A0,
	},
	.phys_size = SZ_8K,
	.log_len = 4096,		  /* log's buffer length in bytes */
	.log_len_mask = (4096 >> 2) - 1,  /* length mask in units of u32 */
};

static struct platform_device msm_rpm_log_device = {
	.name	= "msm_rpm_log",
	.id	= -1,
	.dev	= {
		.platform_data = &msm_rpm_log_pdata,
	},
};

/*
 * ZTE_PLATFORM
 */
#ifdef ZTE_RAM_CONSOLE
#ifdef CONFIG_ANDROID_RAM_CONSOLE
static struct resource ram_console_resource[] = {
    {
        .start  = MSM_RAM_CONSOLE_PHYS,
        .end    = MSM_RAM_CONSOLE_PHYS + MSM_RAM_CONSOLE_SIZE - 1,
        .flags	= IORESOURCE_MEM,
    },
};

static struct platform_device ram_console_device = {
    .name = "ram_console",
    .id = -1,
    .num_resources  = ARRAY_SIZE(ram_console_resource),
    .resource       = ram_console_resource,
};
#endif
#endif /* ZTE_RAM_CONSOLE */

/*
 * ZTE_PLATFORM
 */
#ifdef ZTE_FTM
static struct platform_device zte_ftm_device = {
    .name = "zte_ftm",
    .id = 0,
};
#endif

#ifdef CONFIG_LEDS_GPIO
static struct gpio_led android_led_list[] = {
#ifdef CONFIG_ZTE_SPOTLIGHTS_LEDS_GPIO	
	{
		//.name = "spotlight",flashlight
		.name = "flashlight",
		.gpio = 2,
	},
#endif
};

static struct gpio_led_platform_data android_leds_data = {
	.num_leds	= ARRAY_SIZE(android_led_list),
	.leds		= android_led_list,
};

static struct platform_device android_leds = {
	.name		= "leds-gpio",
	.id		= -1,
	.dev		= {
		.platform_data = &android_leds_data,
	},
};
#endif


static struct platform_device *common_devices[] __initdata = {
	&msm8960_device_dmov,
	&msm_device_smd,
	//&msm8960_device_uart_gsbi5,
	
#ifdef CONFIG_USE_BCM4330
	&msm_device_uart_dm11,
	&msm_bt_power_device, //adb for bt
	&msm_bluesleep_device,//adb for bt
#endif

	&msm_device_uart_dm6,
	&msm_device_saw_core0,
	&msm_device_saw_core1,
	&msm8960_device_ext_5v_vreg,
	&msm8960_device_ssbi_pmic,
	&msm8960_device_ext_otg_sw_vreg,
#if !defined(CONFIG_MACH_KISKA)
	&msm8960_device_qup_spi_gsbi1,
#endif
	&msm8960_device_qup_i2c_gsbi3,
	&msm8960_device_qup_i2c_gsbi4,

#ifdef CONFIG_MHL_Sii8334
	&msm8960_device_qup_i2c_gsbi5,
#endif	
#ifdef CONFIG_FLSH_ADP1650
		&msm8960_device_qup_i2c_gsbi7,		
#endif
#ifdef CONFIG_ISPCAM
		&msm8960_device_qup_i2c_gsbi7,
#endif

/*
 * Modified to fix failure of startup
 */
#if 1
	&msm8960_device_qup_i2c_gsbi10,
#endif

#ifndef CONFIG_MSM_DSPS
	&msm8960_device_qup_i2c_gsbi12,
#endif
#ifdef CONFIG_LEDS_GPIO
       &android_leds,
#endif
	&msm_slim_ctrl,
	&msm_device_wcnss_wlan,
#if defined(CONFIG_CRYPTO_DEV_QCRYPTO) || \
		defined(CONFIG_CRYPTO_DEV_QCRYPTO_MODULE)
	&qcrypto_device,
#endif

#if defined(CONFIG_CRYPTO_DEV_QCEDEV) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV_MODULE)
	&qcedev_device,
#endif
#ifdef CONFIG_MSM_ROTATOR
	&msm_rotator_device,
#endif
	&msm_device_sps,
#ifdef CONFIG_MSM_FAKE_BATTERY
	&fish_battery_device,
#endif
	&fmem_device,
#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
	&android_pmem_device,
	&android_pmem_adsp_device,
#endif
	&android_pmem_audio_device,
#endif
	&msm_device_vidc,
	&msm_device_bam_dmux,
	&msm_fm_platform_init,
#ifdef CONFIG_MACH_KISKA
	&msm_gpio_notify_platform_dev,
#endif

#if defined(CONFIG_TSIF) || defined(CONFIG_TSIF_MODULE)
#ifdef CONFIG_MSM_USE_TSIF1
	&msm_device_tsif[1],
#else
	&msm_device_tsif[0],
#endif
#endif

#ifdef CONFIG_HW_RANDOM_MSM
	&msm_device_rng,
#endif
	&msm_rpm_device,
#ifdef CONFIG_ION_MSM
	&ion_dev,
#endif
	&msm_rpm_log_device,
	&msm_rpm_stat_device,
	&msm_device_tz_log,

#ifdef CONFIG_MSM_QDSS
	&msm_etb_device,
	&msm_tpiu_device,
	&msm_funnel_device,
	&msm_etm_device,
#endif
	&msm_device_dspcrashd_8960,
	&msm8960_device_watchdog,
#ifdef CONFIG_MSM_RTB
	&msm_rtb_device,
#endif
	&msm8960_device_cache_erp,
#ifdef CONFIG_MSM_CACHE_DUMP
	&msm_cache_dump_device,
#endif
	&msm8960_cpu_idle_device,
	&msm8960_msm_gov_device,
};

static struct platform_device *sim_devices[] __initdata = {
	&msm8960_device_otg,
	&msm8960_device_gadget_peripheral,
	&msm_device_hsusb_host,
	&msm_device_hsic_host,
	&android_usb_device,
	&msm_device_vidc,
	&msm_bus_apps_fabric,
	&msm_bus_sys_fabric,
	&msm_bus_mm_fabric,
	&msm_bus_sys_fpb,
	&msm_bus_cpss_fpb,
	&msm_pcm,
	&msm_multi_ch_pcm,
	&msm_pcm_routing,
	&msm_cpudai0,
	&msm_cpudai1,
	&msm_cpudai_hdmi_rx,
	&msm_cpudai_bt_rx,
	&msm_cpudai_bt_tx,
	&msm_cpudai_fm_rx,
	&msm_cpudai_fm_tx,
	&msm_cpudai_auxpcm_rx,
	&msm_cpudai_auxpcm_tx,
	&msm_cpu_fe,
	&msm_stub_codec,
	&msm_voice,
	&msm_voip,
	&msm_lpa_pcm,
	&msm_compr_dsp,
	&msm_cpudai_incall_music_rx,
	&msm_cpudai_incall_record_rx,
	&msm_cpudai_incall_record_tx,

#if defined(CONFIG_CRYPTO_DEV_QCRYPTO) || \
		defined(CONFIG_CRYPTO_DEV_QCRYPTO_MODULE)
	&qcrypto_device,
#endif

#if defined(CONFIG_CRYPTO_DEV_QCEDEV) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV_MODULE)
	&qcedev_device,
#endif
};

static struct platform_device *rumi3_devices[] __initdata = {
	&msm_kgsl_3d0,
	&msm_kgsl_2d0,
	&msm_kgsl_2d1,
#ifdef CONFIG_MSM_GEMINI
	&msm8960_gemini_device,
#endif
};

static struct platform_device *cdp_devices[] __initdata = {
	&msm_8960_q6_lpass,
	&msm_8960_q6_mss_fw,
	&msm_8960_q6_mss_sw,
	&msm_8960_riva,
	&msm_pil_tzapps,
	&msm_pil_vidc,
	&msm8960_device_otg,
	&msm8960_device_gadget_peripheral,
	&msm_device_hsusb_host,
	&android_usb_device,
	&msm_pcm,
	&msm_multi_ch_pcm,
	&msm_pcm_routing,
	&msm_cpudai0,
	&msm_cpudai1,
	&msm_cpudai_hdmi_rx,
	&msm_cpudai_bt_rx,
	&msm_cpudai_bt_tx,
	&msm_cpudai_fm_rx,
	&msm_cpudai_fm_tx,
	&msm_cpudai_auxpcm_rx,
	&msm_cpudai_auxpcm_tx,
	&msm_cpu_fe,
	&msm_stub_codec,
	&msm_kgsl_3d0,
#ifdef CONFIG_MSM_KGSL_2D
	&msm_kgsl_2d0,
	&msm_kgsl_2d1,
#endif
#ifdef CONFIG_MSM_GEMINI
	&msm8960_gemini_device,
#endif
	&msm_voice,
	&msm_voip,
	&msm_lpa_pcm,
	&msm_cpudai_afe_01_rx,
	&msm_cpudai_afe_01_tx,
	&msm_cpudai_afe_02_rx,
	&msm_cpudai_afe_02_tx,
	&msm_pcm_afe,
	&msm_compr_dsp,
	&msm_cpudai_incall_music_rx,
	&msm_cpudai_incall_record_rx,
	&msm_cpudai_incall_record_tx,
	&msm_pcm_hostless,
	&msm_bus_apps_fabric,
	&msm_bus_sys_fabric,
	&msm_bus_mm_fabric,
	&msm_bus_sys_fpb,
	&msm_bus_cpss_fpb,
	
/*
 * ZTE_PLATFORM
 */
#ifdef ZTE_RAM_CONSOLE
#ifdef CONFIG_ANDROID_RAM_CONSOLE
    &ram_console_device,
#endif
#endif /* ZTE_RAM_CONSOLE */

/*
 * ZTE_PLATFORM
 */
#ifdef ZTE_FTM
    &zte_ftm_device,
#endif

};

static void __init msm8960_i2c_init(void)
{
	msm8960_device_qup_i2c_gsbi4.dev.platform_data =
					&msm8960_i2c_qup_gsbi4_pdata;

	msm8960_device_qup_i2c_gsbi3.dev.platform_data =
					&msm8960_i2c_qup_gsbi3_pdata;
				
#ifdef CONFIG_MHL_Sii8334
	msm8960_device_qup_i2c_gsbi5.dev.platform_data =
					&msm8960_i2c_qup_gsbi5_pdata;
#endif 

#ifdef CONFIG_ISPCAM
		msm8960_device_qup_i2c_gsbi7.dev.platform_data =
						&msm8960_i2c_qup_gsbi7_pdata;
#endif
#ifdef CONFIG_FLSH_ADP1650
		msm8960_device_qup_i2c_gsbi7.dev.platform_data =
						&msm8960_i2c_qup_gsbi7_pdata;
#endif
	msm8960_device_qup_i2c_gsbi10.dev.platform_data =
					&msm8960_i2c_qup_gsbi10_pdata;

	msm8960_device_qup_i2c_gsbi12.dev.platform_data =
					&msm8960_i2c_qup_gsbi12_pdata;
}

static void __init msm8960_gfx_init(void)
{
	uint32_t soc_platform_version = socinfo_get_version();
	if (SOCINFO_VERSION_MAJOR(soc_platform_version) == 1) {
		struct kgsl_device_platform_data *kgsl_3d0_pdata =
				msm_kgsl_3d0.dev.platform_data;
		kgsl_3d0_pdata->pwrlevel[0].gpu_freq = 400000000;
		kgsl_3d0_pdata->pwrlevel[1].gpu_freq = 325000000;
		kgsl_3d0_pdata->pwrlevel[2].gpu_freq = 300000000;
		kgsl_3d0_pdata->pwrlevel[3].gpu_freq = 266667000;
		kgsl_3d0_pdata->pwrlevel[4].gpu_freq = 128000000;
	}
}

static struct msm_cpuidle_state msm_cstates[] __initdata = {
	{0, 0, "C0", "WFI",
		MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT},

	{0, 1, "C2", "POWER_COLLAPSE",
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE},

	{1, 0, "C0", "WFI",
		MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT},
};

static struct msm_pm_platform_data msm_pm_data[MSM_PM_SLEEP_MODE_NR * 2] = {
	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_POWER_COLLAPSE)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 0,
		.suspend_enabled = 0,
	},

	[MSM_PM_MODE(0, MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT)] = {
		.idle_supported = 1,
		.suspend_supported = 1,
		.idle_enabled = 1,
		.suspend_enabled = 1,
	},

	[MSM_PM_MODE(1, MSM_PM_SLEEP_MODE_POWER_COLLAPSE)] = {
		.idle_supported = 0,
		.suspend_supported = 1,
		.idle_enabled = 0,
		.suspend_enabled = 1,  //tcd change from 0 -> 1
	},

	[MSM_PM_MODE(1, MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT)] = {
		.idle_supported = 1,
		.suspend_supported = 0,
		.idle_enabled = 1,
		.suspend_enabled = 0,
	},
};

static struct msm_rpmrs_level msm_rpmrs_levels[] = {
	{
		MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT,
		MSM_RPMRS_LIMITS(ON, ACTIVE, MAX, ACTIVE),
		true,
		100, 650, 801, 200,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(ON, GDHS, MAX, ACTIVE),
		false,
		8500, 51, 1122000, 8500,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(ON, HSFS_OPEN, MAX, ACTIVE),
		false,
		9000, 51, 1130300, 9000,
	},
	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(ON, HSFS_OPEN, ACTIVE, RET_HIGH),
		false,
		10000, 51, 1130300, 10000,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(OFF, GDHS, MAX, ACTIVE),
		false,
		12000, 14, 2205900, 12000,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(OFF, HSFS_OPEN, MAX, ACTIVE),
		false,
		18000, 12, 2364250, 18000,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(OFF, HSFS_OPEN, ACTIVE, RET_HIGH),
		false,
		23500, 10, 2667000, 23500,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(OFF, HSFS_OPEN, RET_HIGH, RET_LOW),
		false,
		29700, 5, 2867000, 30000,
	},
};

static struct msm_pm_boot_platform_data msm_pm_boot_pdata __initdata = {
	.mode = MSM_PM_BOOT_CONFIG_TZ,
};

uint32_t msm_rpm_get_swfi_latency(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(msm_rpmrs_levels); i++) {
		if (msm_rpmrs_levels[i].sleep_mode ==
			MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT)
			return msm_rpmrs_levels[i].latency_us;
	}

	return 0;
}

#ifdef CONFIG_I2C
#define I2C_SURF 1
#define I2C_FFA  (1 << 1)
#define I2C_RUMI (1 << 2)
#define I2C_SIM  (1 << 3)
#define I2C_FLUID (1 << 4)
#define I2C_LIQUID (1 << 5)

struct i2c_registry {
	u8                     machs;
	int                    bus;
	struct i2c_board_info *info;
	int                    len;
};

/* Sensors DSPS platform data */
#ifdef CONFIG_MSM_DSPS
#define DSPS_PIL_GENERIC_NAME		"dsps"
#endif /* CONFIG_MSM_DSPS */

static void __init msm8960_init_dsps(void)
{
#ifdef CONFIG_MSM_DSPS
	struct msm_dsps_platform_data *pdata =
		msm_dsps_device.dev.platform_data;
	pdata->pil_name = DSPS_PIL_GENERIC_NAME;
	pdata->gpios = NULL;
	pdata->gpios_num = 0;

	platform_device_register(&msm_dsps_device);
#endif /* CONFIG_MSM_DSPS */
}

static int hsic_peripheral_status = 1;
static DEFINE_MUTEX(hsic_status_lock);

void peripheral_connect()
{
	mutex_lock(&hsic_status_lock);
	if (hsic_peripheral_status)
		goto out;
	platform_device_add(&msm_device_hsic_host);
	hsic_peripheral_status = 1;
out:
	mutex_unlock(&hsic_status_lock);
}
EXPORT_SYMBOL(peripheral_connect);

void peripheral_disconnect()
{
	mutex_lock(&hsic_status_lock);
	if (!hsic_peripheral_status)
		goto out;
	platform_device_del(&msm_device_hsic_host);
	hsic_peripheral_status = 0;
out:
	mutex_unlock(&hsic_status_lock);
}
EXPORT_SYMBOL(peripheral_disconnect);

static void __init msm8960_init_hsic(void)
{
#ifdef CONFIG_USB_EHCI_MSM_HSIC
	uint32_t version = socinfo_get_version();

	if (SOCINFO_VERSION_MAJOR(version) == 1)
		return;

	if (PLATFORM_IS_CHARM25() || machine_is_msm8960_liquid())
		platform_device_register(&msm_device_hsic_host);
#endif
}

#ifdef CONFIG_ISL9519_CHARGER
static struct isl_platform_data isl_data __initdata = {
	.valid_n_gpio		= 0,	/* Not required when notify-by-pmic */
	.chg_detection_config	= NULL,	/* Not required when notify-by-pmic */
	.max_system_voltage	= 4200,
	.min_system_voltage	= 3200,
	.chgcurrent		= 1000, /* 1900, */
	.term_current		= 400,	/* Need fine tuning */
	.input_current		= 2048,
};

static struct i2c_board_info isl_charger_i2c_info[] __initdata = {
	{
		I2C_BOARD_INFO("isl9519q", 0x9),
		.irq		= 0,	/* Not required when notify-by-pmic */
		.platform_data	= &isl_data,
	},
};
#endif /* CONFIG_ISL9519_CHARGER */

#ifdef CONFIG_SENSORS_AKM8962C 
struct akm8962_platform_data akm_platform_data_8962 ={
	.layout			 = 5,
	.gpio_DRDY		 = 18,
};
#endif

#ifdef CONFIG_MXC_MMA8452
struct mma8452_platform_data mma8452_pdata = {
   .axis_map_cordination = 5,
};
#endif
#ifdef CONFIG_SENSORS_KXTIK 

#if defined(CONFIG_MACH_ELDEN)||defined(CONFIG_MACH_GORDON)||defined(CONFIG_MACH_HAYES) || defined(CONFIG_MACH_ILIAMNA)
#define KXTIK_DEVICE_MAP  3
#else
#define KXTIK_DEVICE_MAP  5
#endif

#define KXTIK_MAP_X (KXTIK_DEVICE_MAP-1)%2 
#define KXTIK_MAP_Y KXTIK_DEVICE_MAP%2 
//#define KXTIK_NEG_X (KXTIK_DEVICE_MAP/2)%2 
//#define KXTIK_NEG_Y (KXTIK_DEVICE_MAP+1)/4 
#define KXTIK_NEG_X (((KXTIK_DEVICE_MAP+2)/2)%2)
#define KXTIK_NEG_Y (((KXTIK_DEVICE_MAP+5)/4)%2)
#define KXTIK_NEG_Z (KXTIK_DEVICE_MAP-1)/4

struct kxtik_platform_data kxtik_pdata = { 
  .min_interval = 10, 
  .poll_interval  = 200,
 
  .axis_map_x  = KXTIK_MAP_X, 
  .axis_map_y  = KXTIK_MAP_Y, 
  .axis_map_z  = 2, 
 
  .negate_x  = KXTIK_NEG_X, 
  .negate_y  = KXTIK_NEG_Y, 
  .negate_z  = KXTIK_NEG_Z, 
 
  .res_12bit  = RES_12BIT, 
  .g_range  = KXTIK_G_2G, 
 
  //.data_odr_init    = ODR12_5F, 
}; 
#endif /* CONFIG_SENSORS_KXTIK */ 

#ifdef CONFIG_SENSORS_ST_LIS3DHTR
struct lis3dh_acc_platform_data lis3dh_acc_plt_dat = {
        .poll_interval = 50,            //Driver polling interval as 50ms
        .min_interval = 10,             //Driver polling interval minimum 10ms
        .g_range = LIS3DH_ACC_G_2G,      //Full Scale of LSM303DLH Accelerometer
        .axis_map_x = 0,                //x = x
        .axis_map_y = 1,                //y = y
        .axis_map_z = 2,                //z = z
        .negate_x = 0,                  //x = +x
        .negate_y = 0,                  //y = +y
        .negate_z = 0,                  //z = +z
        .gpio_int1 = -EINVAL,
	      .gpio_int2 = -EINVAL,
}; 
#endif

#ifndef CONFIG_MSM_DSPS

#if defined(CONFIG_MPU_SENSORS_MPU3050) 

#ifdef CONFIG_MACH_CRATER
static struct mpu_platform_data mpu3050_data = {
	.int_config  = 0x10,
	.orientation = {  0,  1,  0, 
			   -1,  0,  0, 
			   0,  0, -1 },
};

/* accel */
static struct ext_slave_platform_data kxtf9 = {
		 .bus         = EXT_SLAVE_BUS_SECONDARY,
		 .orientation = {  1,  0,  0, 
				    0,  -1,  0, 
				    0,  0, -1 },
};

/* compass */
static struct ext_slave_platform_data inv_mpu_ak8975_data = {
		 .bus         = EXT_SLAVE_BUS_PRIMARY,
		 .orientation = { 0, 1, 0, 
				  -1, 0, 0, 
				  0, 0, 1 },
};
#elif defined(CONFIG_MACH_DANA)||defined(CONFIG_MACH_KISKA)
static struct mpu_platform_data mpu3050_data = {
	.int_config  = 0x10,
	.orientation = {  -1,  0,  0, 
			   0,  -1,  0, 
			   0,  0, 1 },
};

/* accel */
static struct ext_slave_platform_data kxtf9 = {
		 .bus         = EXT_SLAVE_BUS_SECONDARY,
		 .orientation = {  1,  0,  0, 
				    0,  1,  0, 
				    0,  0, 1 },
};

/* compass */
static struct ext_slave_platform_data inv_mpu_ak8975_data = {
		 .bus         = EXT_SLAVE_BUS_PRIMARY,
		 .orientation = { -1, 0, 0, 
				  0, 1, 0, 
				  0, 0, -1 },
};
#elif defined(CONFIG_MACH_JARVIS)
static struct mpu_platform_data mpu3050_data = {
	.int_config  = 0x10,
	.orientation = {  0,  1,  0, 
			   1,  0,  0, 
			   0,  0, -1 },
};

/* accel */
static struct ext_slave_platform_data kxtf9 = {
		 .bus         = EXT_SLAVE_BUS_SECONDARY,
		 .orientation = {  -1,  0,  0, 
				    0,  1,  0, 
				    0,  0, -1 },
};

/* compass */
static struct ext_slave_platform_data inv_mpu_ak8975_data = {
		 .bus         = EXT_SLAVE_BUS_PRIMARY,
		 .orientation = { -1, 0, 0, 
				  0, 1, 0, 
				  0, 0, -1 },
};
#else
static struct mpu_platform_data mpu3050_data = {
	.int_config  = 0x10,
	.orientation = {  -1,  0,  0, 
			   0,  1,  0, 
			   0,  0, -1 },
};

/* accel */
static struct ext_slave_platform_data kxtf9 = {
		 .bus         = EXT_SLAVE_BUS_SECONDARY,
		 .orientation = {  -1,  0,  0, 
				    0,  1,  0, 
				    0,  0, -1 },
};

/* compass */
static struct ext_slave_platform_data inv_mpu_ak8975_data = {
		 .bus         = EXT_SLAVE_BUS_PRIMARY,
		 .orientation = { -1, 0, 0, 
				  0, 1, 0, 
				  0, 0, -1 },
};
#endif /* CONFIG_MACH_CRATER */

#endif

#define SENSOR_TAOS_I2C_SLAVE_ADDR	0x39

#define WM2K_I2C_SLAVE_ADDR 0x3A

static struct i2c_board_info msm_i2c_gsbi12_sensors_info[] = {
	{
		I2C_BOARD_INFO("taos", SENSOR_TAOS_I2C_SLAVE_ADDR),
	},
#ifdef CONFIG_MACH_GORDON
	{
		I2C_BOARD_INFO("wm2000", WM2K_I2C_SLAVE_ADDR),
	},
#endif

#ifdef CONFIG_SENSORS_AKM8962C
	{
		.type = "akm8962",
#if defined(CONFIG_MACH_ELDEN)||defined(CONFIG_MACH_GORDON)||defined(CONFIG_MACH_HAYES) || defined(CONFIG_MACH_ILIAMNA)
		.addr = 0x0E,
#else
		.addr = 0x0c,
#endif		
		//.flag = I2C_CLIENT_WAKE,
		.platform_data = &akm_platform_data_8962,
		.irq = MSM_GPIO_TO_INT(70),
	},
#endif
#ifdef CONFIG_MXC_MMA8452
    { 
    	.type = "mma8452", 
	    .platform_data = &mma8452_pdata,  
    	.addr = 0x1C,     // MMA8452 i2c slave address 
    }, 
#endif
#ifdef CONFIG_SENSORS_KXTIK
	{ 
		I2C_BOARD_INFO("kxtik", KXTIK_I2C_ADDR), 
    	.platform_data = &kxtik_pdata,  
    	.irq = MSM_GPIO_TO_INT(10), // Replace with appropriate GPIO setup 
  },
#endif
#ifdef CONFIG_SENSORS_ST_LIS3DHTR
	{ 
    I2C_BOARD_INFO("lis3dh_acc", 0x18),
    .platform_data = &lis3dh_acc_plt_dat,
  },
#endif
#if defined(CONFIG_MPU_SENSORS_MPU3050) 
	{
		I2C_BOARD_INFO("mpu3050", 0x68),
		.irq = MSM_GPIO_TO_INT(69),
		.platform_data = &mpu3050_data,
	},

	{
		//I2C_BOARD_INFO("mma845x", 0x1C),
			I2C_BOARD_INFO("kxtf9", 0x0F),
		//.irq = (IH_GPIO_BASE + ACCEL_IRQ_GPIO),
		.platform_data = &kxtf9
	},
	{
	#if defined(CONFIG_MACH_DANA)||defined(CONFIG_MACH_KISKA)
		I2C_BOARD_INFO("ak8975", 0x0E),
	#else 	
		I2C_BOARD_INFO("ak8975", 0x0C),
    #endif 	
		//.irq = (IH_GPIO_BASE + COMPASS_IRQ_GPIO),
		.platform_data = &inv_mpu_ak8975_data,
	},
#endif
};
#endif

static struct i2c_board_info liquid_io_expander_i2c_info[] __initdata = {
	{
		I2C_BOARD_INFO("sx1508q", 0x20),
		.platform_data = &msm8960_sx150x_data[SX150X_LIQUID]
	},
};
#ifdef CONFIG_PN544_NFC
struct pn544_i2c_platform_data pn544_data={
       .irq_gpio=106,
       .ven_gpio=PM8921_GPIO_PM_TO_SYS(7),
       .firm_gpio=55,  //zte_hw_bug
       .dcdc_gpio=0,
       .clock_gpio=0,
       .int_active_low=0,
};

static struct i2c_board_info msm_i2c_gsbi10_nfc_info[] = {
	{ 
                I2C_BOARD_INFO("pn544", 0x50>>1 ), 
		   .platform_data = &pn544_data,			
                .irq = MSM_GPIO_TO_INT(106),   
        },
};
#endif
#ifdef CONFIG_FLSH_ADP1650
static struct i2c_board_info msm_i2c_gsbi7_adp1650_info[] = {
	{ 
                I2C_BOARD_INFO("adp1650", 0x30 ), 
        },
};
#endif
#ifdef CONFIG_MHL_Sii8334
#define MHL_RESET 8
static void Sii8334_reset(void)
{	
	static int bFirst=1;
	int rc;
	static struct regulator *reg_8921_l12, *reg_8921_s4;
	
	if(bFirst)
	{
			/* TBD: PM8921 regulator instead of 8901 */
			if (!reg_8921_l12) {
				reg_8921_l12 = regulator_get(NULL, "8921_l12");
				if (IS_ERR(reg_8921_l12)) {
					pr_err("could not get reg_8921_l12, rc = %ld\n",
						PTR_ERR(reg_8921_l12));
				}
				rc = regulator_set_voltage(reg_8921_l12, 1200000, 1200000);
				if (rc) {
					pr_err("set_voltage failed for 8921_l12, rc=%d\n", rc);
				}
			}
			
			
			if (!reg_8921_s4) {
			reg_8921_s4 = regulator_get(NULL, "8921_s4");
			if (IS_ERR(reg_8921_s4)) {
				pr_err("could not get reg_8921_s4, rc = %ld\n",
					PTR_ERR(reg_8921_s4));
			}
			rc = regulator_set_voltage(reg_8921_s4, 1800000, 1800000);
			if (rc) {
				pr_err("set_voltage failed for 8921_s4, rc=%d\n", rc);
			}
		}
		
		if (1){

		printk("%s,power on!\n", __func__);


		rc = regulator_enable(reg_8921_l12);
		if (rc) {
			pr_err("%s: regulator_enable of reg_8921_l12 failed(%d)\n",
				__func__, rc);
		}


		rc = regulator_enable(reg_8921_s4);
		if (rc) {
			pr_err("%s: regulator_enable of reg_8921_s4 failed(%d)\n",
				__func__, rc);
		}


	}
		rc = gpio_request(MHL_RESET, "MHL_RESET");
		bFirst = 0;
		if (rc) {
			pr_err("'%s'(%d) gpio_request failed, rc=%d\n",
				"MHL_RESET", MHL_RESET, rc);
		}
	 
	}


	
	msleep(1000);
	gpio_direction_output(MHL_RESET, 1);
	msleep(200);
	gpio_direction_output(MHL_RESET, 0);
	msleep(200);
	gpio_direction_output(MHL_RESET, 1);
}

struct MHL_platform_data {
	void (*reset) (void);
};

static struct MHL_platform_data Sii8334_data = {
	.reset = Sii8334_reset,
};

	//#define CI2CA true
	#ifdef CI2CA 
	#define SII8334_plus 0x02  //Define sii8334's I2c Address of all pages by the status of CI2CA.
	#else
	#define SII8334_plus 0x00  //Define sii8334's I2c Address of all pages by the status of CI2CA.
	#endif

static struct i2c_board_info hdmi_mhl_boardinfo[] __initdata = {
	{
	 .type = "sii8334_PAGE_TPI",
	 .addr = 0x39 + SII8334_plus, //0x39
	 .irq = MSM_GPIO_TO_INT(7),// 7,  //define the interrupt signal input pin
	 .platform_data = &Sii8334_data,
	},
	/*
	{
	 .type = "sii8334_PAGE_TX_L0",
	 .addr = 0x39 + SII8334_plus, //0x39
	},
	*/
	{
	 .type = "sii8334_PAGE_TX_L1",
	 .addr = 0x3D + SII8334_plus, //0x3d
	},
	{
	 .type = "sii8334_PAGE_TX_2",
	 .addr = 0x49 + SII8334_plus, //0x49
	},
	{
	 .type = "sii8334_PAGE_TX_3",
	 .addr = 0x4D + SII8334_plus, //0x4d
	},
	{
	 .type = "sii8334_PAGE_CBUS",
	 .addr = 0x64 + SII8334_plus, //0x64
	},
};
#endif 

static struct i2c_registry msm8960_i2c_devices[] __initdata = {
#ifdef CONFIG_MHL_Sii8334
	{
		I2C_SURF | I2C_FFA | I2C_FLUID | I2C_LIQUID | I2C_RUMI,
		MSM_8960_GSBI5_QUP_I2C_BUS_ID,
		hdmi_mhl_boardinfo,
		ARRAY_SIZE(hdmi_mhl_boardinfo),
	},
#endif 
#ifdef CONFIG_ISL9519_CHARGER
	{
		I2C_LIQUID,
		MSM_8960_GSBI10_QUP_I2C_BUS_ID,
		isl_charger_i2c_info,
		ARRAY_SIZE(isl_charger_i2c_info),
	},
#endif /* CONFIG_ISL9519_CHARGER */
	{
		I2C_FFA | I2C_LIQUID,
		MSM_8960_GSBI10_QUP_I2C_BUS_ID,
		sii_device_info,
		ARRAY_SIZE(sii_device_info),
	},
	{
		I2C_LIQUID,
		MSM_8960_GSBI10_QUP_I2C_BUS_ID,
		msm_isa1200_board_info,
		ARRAY_SIZE(msm_isa1200_board_info),
	},
	{
		I2C_LIQUID,
		MSM_8960_GSBI10_QUP_I2C_BUS_ID,
		liquid_io_expander_i2c_info,
		ARRAY_SIZE(liquid_io_expander_i2c_info),
	},
#ifndef CONFIG_MSM_DSPS
	{
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_8960_GSBI12_QUP_I2C_BUS_ID,
		msm_i2c_gsbi12_sensors_info,
		ARRAY_SIZE(msm_i2c_gsbi12_sensors_info),
	},
#endif
#ifdef CONFIG_PN544_NFC
{
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_8960_GSBI10_QUP_I2C_BUS_ID,
		msm_i2c_gsbi10_nfc_info,
		ARRAY_SIZE(msm_i2c_gsbi10_nfc_info),
	},
#endif
#ifdef CONFIG_FLSH_ADP1650
{
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_8960_GSBI7_QUP_I2C_BUS_ID,
		msm_i2c_gsbi7_adp1650_info,
		ARRAY_SIZE(msm_i2c_gsbi7_adp1650_info),
	},
#endif
};
#endif /* CONFIG_I2C */

static void __init register_i2c_devices(void)
{
#ifdef CONFIG_I2C
	u8 mach_mask = 0;
	int i;
#ifdef CONFIG_MSM_CAMERA
	struct i2c_registry msm8960_camera_i2c_devices = {
		I2C_SURF | I2C_FFA | I2C_FLUID | I2C_LIQUID | I2C_RUMI,
		MSM_8960_GSBI4_QUP_I2C_BUS_ID,
		msm8960_camera_board_info.board_info,
		msm8960_camera_board_info.num_i2c_board_info,
	};
#ifdef CONFIG_ISPCAM
struct i2c_registry msm8960_isp_camera_i2c_devices = {
	I2C_SURF | I2C_FFA | I2C_FLUID | I2C_LIQUID | I2C_RUMI,
	MSM_8960_GSBI7_QUP_I2C_BUS_ID,
	msm8960_isp_camera_board_info.board_info,
	msm8960_isp_camera_board_info.num_i2c_board_info,
};
#endif
#endif

	/* Build the matching 'supported_machs' bitmask */
	if (machine_is_msm8960_cdp())
		mach_mask = I2C_SURF;
	else if (machine_is_msm8960_rumi3())
		mach_mask = I2C_RUMI;
	else if (machine_is_msm8960_sim())
		mach_mask = I2C_SIM;
	else if (machine_is_msm8960_fluid())
		mach_mask = I2C_FLUID;
	else if (machine_is_msm8960_liquid())
		mach_mask = I2C_LIQUID;
	else if (machine_is_msm8960_mtp())
		mach_mask = I2C_FFA;

#ifdef CONFIG_MACH_ADAMS
    else if (machine_is_adams())
        mach_mask = I2C_FFA;
#elif defined(CONFIG_MACH_BAKER)
	else if (machine_is_baker())
		mach_mask = I2C_FFA;
#elif defined(CONFIG_MACH_CRATER)
	else if (machine_is_crater())
		mach_mask = I2C_FFA;
#elif defined(CONFIG_MACH_DANA)
	else if (machine_is_dana())
        mach_mask = I2C_FFA;
#elif defined(CONFIG_MACH_ELDEN)
	else if (machine_is_elden())
		mach_mask = I2C_FFA;
#elif defined(CONFIG_MACH_FROSTY)
	else if (machine_is_frosty())
		mach_mask = I2C_FFA;
#elif defined(CONFIG_MACH_GORDON)
	else if (machine_is_gordon())
		mach_mask = I2C_FFA;
#elif defined(CONFIG_MACH_HAYES)
	else if (machine_is_hayes())
		mach_mask = I2C_FFA;
#elif defined(CONFIG_MACH_ILIAMNA)
	else if (machine_is_iliamna())
		mach_mask = I2C_FFA;
#elif defined(CONFIG_MACH_JARVIS)
	else if (machine_is_jarvis())
		mach_mask = I2C_FFA;
#elif defined(CONFIG_MACH_KISKA)
	else if (machine_is_kiska())
		mach_mask = I2C_FFA;
#else
#endif /* CONFIG_MACH_ADAMS */

	else
		pr_err("unmatched machine ID in register_i2c_devices\n");

	/* Run the array and install devices as appropriate */
	for (i = 0; i < ARRAY_SIZE(msm8960_i2c_devices); ++i) {
		if (msm8960_i2c_devices[i].machs & mach_mask)
			i2c_register_board_info(msm8960_i2c_devices[i].bus,
						msm8960_i2c_devices[i].info,
						msm8960_i2c_devices[i].len);
	}
#ifdef CONFIG_MSM_CAMERA
	if (msm8960_camera_i2c_devices.machs & mach_mask)
		i2c_register_board_info(msm8960_camera_i2c_devices.bus,
			msm8960_camera_i2c_devices.info,
			msm8960_camera_i2c_devices.len);
#ifdef CONFIG_ISPCAM

	if (msm8960_isp_camera_i2c_devices.machs & mach_mask)
		i2c_register_board_info(msm8960_isp_camera_i2c_devices.bus,
			msm8960_isp_camera_i2c_devices.info,
			msm8960_isp_camera_i2c_devices.len);
#endif
#endif

#ifdef CONFIG_INPUT_TOUCHSCREEN
	msm8960_ts_init(MSM_8960_GSBI3_QUP_I2C_BUS_ID);	
#endif

#endif
}

static void __init msm8960_sim_init(void)
{
	struct msm_watchdog_pdata *wdog_pdata = (struct msm_watchdog_pdata *)
		&msm8960_device_watchdog.dev.platform_data;

	wdog_pdata->bark_time = 15000;
	msm_tsens_early_init(&msm_tsens_pdata);
	BUG_ON(msm_rpm_init(&msm_rpm_data));
	BUG_ON(msm_rpmrs_levels_init(msm_rpmrs_levels,
				ARRAY_SIZE(msm_rpmrs_levels)));
	regulator_suppress_info_printing();
	platform_device_register(&msm8960_device_rpm_regulator);
	msm_clock_init(&msm8960_clock_init_data);
	msm8960_init_pmic();

	msm8960_device_otg.dev.platform_data = &msm_otg_pdata;
	msm8960_init_gpiomux();
	msm8960_i2c_init();
	msm_spm_init(msm_spm_data, ARRAY_SIZE(msm_spm_data));
	msm_spm_l2_init(msm_spm_l2_data);
	msm8960_init_buses();
	platform_add_devices(common_devices, ARRAY_SIZE(common_devices));
	msm8960_pm8921_gpio_mpp_init();
	platform_add_devices(sim_devices, ARRAY_SIZE(sim_devices));
	acpuclk_init(&acpuclk_8960_soc_data);

	msm8960_device_qup_spi_gsbi1.dev.platform_data =
				&msm8960_qup_spi_gsbi1_pdata;
	spi_register_board_info(spi_board_info, ARRAY_SIZE(spi_board_info));

	msm8960_init_mmc();
	msm8960_init_fb();
	slim_register_board_info(msm_slim_devices,
		ARRAY_SIZE(msm_slim_devices));
#ifdef CONFIG_USE_BCM4330 	
	bt_power_init();
#endif
	msm_pm_set_platform_data(msm_pm_data, ARRAY_SIZE(msm_pm_data));
	msm_pm_set_rpm_wakeup_irq(RPM_APCC_CPU0_WAKE_UP_IRQ);
	msm_cpuidle_set_states(msm_cstates, ARRAY_SIZE(msm_cstates),
				msm_pm_data);
	BUG_ON(msm_pm_boot_init(&msm_pm_boot_pdata));
	msm_pm_init_sleep_status_data(&msm_pm_slp_sts_data);
}

static void __init msm8960_rumi3_init(void)
{
	msm_tsens_early_init(&msm_tsens_pdata);
	BUG_ON(msm_rpm_init(&msm_rpm_data));
	BUG_ON(msm_rpmrs_levels_init(msm_rpmrs_levels,
				ARRAY_SIZE(msm_rpmrs_levels)));
	regulator_suppress_info_printing();
	platform_device_register(&msm8960_device_rpm_regulator);
	msm_clock_init(&msm8960_dummy_clock_init_data);
	msm8960_init_gpiomux();
	msm8960_init_pmic();
	msm8960_device_qup_spi_gsbi1.dev.platform_data =
				&msm8960_qup_spi_gsbi1_pdata;
	spi_register_board_info(spi_board_info, ARRAY_SIZE(spi_board_info));
	msm8960_i2c_init();
	msm_spm_init(msm_spm_data, ARRAY_SIZE(msm_spm_data));
	msm_spm_l2_init(msm_spm_l2_data);
	platform_add_devices(common_devices, ARRAY_SIZE(common_devices));
	msm8960_pm8921_gpio_mpp_init();
	platform_add_devices(rumi3_devices, ARRAY_SIZE(rumi3_devices));
	msm8960_init_mmc();
	register_i2c_devices();
	msm8960_init_fb();
	slim_register_board_info(msm_slim_devices,
		ARRAY_SIZE(msm_slim_devices));
	msm_pm_set_platform_data(msm_pm_data, ARRAY_SIZE(msm_pm_data));
#ifdef CONFIG_USE_BCM4330 	
	bt_power_init();
#endif
	msm_pm_set_rpm_wakeup_irq(RPM_APCC_CPU0_WAKE_UP_IRQ);
	msm_cpuidle_set_states(msm_cstates, ARRAY_SIZE(msm_cstates),
				msm_pm_data);
	BUG_ON(msm_pm_boot_init(&msm_pm_boot_pdata));
	msm_pm_init_sleep_status_data(&msm_pm_slp_sts_data);
}

static void __init msm8960_cdp_init(void)
{
/*
 * ZTE_PLATFORM
 */
#if 1
    const char *hw_ver = NULL;
#endif

	if (meminfo_init(SYS_MEMORY, SZ_256M) < 0)
		pr_err("meminfo_init() failed!\n");

	msm_tsens_early_init(&msm_tsens_pdata);
	BUG_ON(msm_rpm_init(&msm_rpm_data));
	BUG_ON(msm_rpmrs_levels_init(msm_rpmrs_levels,
				ARRAY_SIZE(msm_rpmrs_levels)));

	regulator_suppress_info_printing();
	if (msm_xo_init())
		pr_err("Failed to initialize XO votes\n");
	platform_device_register(&msm8960_device_rpm_regulator);
	msm_clock_init(&msm8960_clock_init_data);
	if (machine_is_msm8960_liquid())
		msm_otg_pdata.mhl_enable = true;
	msm8960_device_otg.dev.platform_data = &msm_otg_pdata;
	if (machine_is_msm8960_mtp() || machine_is_msm8960_fluid() ||
		machine_is_msm8960_cdp()) {
		msm_otg_pdata.phy_init_seq = wr_phy_init_seq;
	} else if (machine_is_msm8960_liquid()) {
			msm_otg_pdata.phy_init_seq =
				liquid_v1_phy_init_seq;
	}

	msm_otg_pdata.phy_init_seq_override =
				zte_phy_init_seq_override;/*for usb eye diagram test*/
		
	msm_otg_pdata.swfi_latency =
		msm_rpmrs_levels[0].latency_us;
#ifdef CONFIG_USB_EHCI_MSM_HSIC
	if (machine_is_msm8960_liquid()) {
		if (SOCINFO_VERSION_MAJOR(socinfo_get_version()) >= 2)
			msm_hsic_pdata.hub_reset = HSIC_HUB_RESET_GPIO;
	}
#endif
	msm_device_hsic_host.dev.platform_data = &msm_hsic_pdata;
	msm8960_init_gpiomux();
	msm8960_device_qup_spi_gsbi1.dev.platform_data =
				&msm8960_qup_spi_gsbi1_pdata;
	spi_register_board_info(spi_board_info, ARRAY_SIZE(spi_board_info));

	msm8960_init_pmic();
	if ((SOCINFO_VERSION_MAJOR(socinfo_get_version()) >= 2 &&
		(machine_is_msm8960_mtp())) || machine_is_msm8960_liquid())
		msm_isa1200_board_info[0].platform_data = &isa1200_1_pdata;
	msm8960_i2c_init();
	msm8960_gfx_init();
	msm_spm_init(msm_spm_data, ARRAY_SIZE(msm_spm_data));
	msm_spm_l2_init(msm_spm_l2_data);
	msm8960_init_buses();
	platform_add_devices(msm_footswitch_devices,
		msm_num_footswitch_devices);
	if (machine_is_msm8960_liquid())
		platform_device_register(&msm8960_device_ext_3p3v_vreg);
	if (machine_is_msm8960_cdp())
		platform_device_register(&msm8960_device_ext_l2_vreg);
	platform_add_devices(common_devices, ARRAY_SIZE(common_devices));
	msm8960_pm8921_gpio_mpp_init();
	platform_add_devices(cdp_devices, ARRAY_SIZE(cdp_devices));
	msm8960_init_hsic();
	#ifdef CONFIG_MSM_CAMERA
	msm8960_init_cam();
	#endif
	msm8960_init_mmc();
	acpuclk_init(&acpuclk_8960_soc_data);
/*
#if defined (CONFIG_TOUCHSCREEN_ATMEL_MXT)	
	if (machine_is_msm8960_liquid())
		mxt_init_hw_liquid();
#endif
*/
	register_i2c_devices();
	msm8960_init_fb();
	slim_register_board_info(msm_slim_devices,
		ARRAY_SIZE(msm_slim_devices));
	msm8960_init_dsps();
	msm_pm_set_platform_data(msm_pm_data, ARRAY_SIZE(msm_pm_data));
	msm_pm_set_rpm_wakeup_irq(RPM_APCC_CPU0_WAKE_UP_IRQ);
	msm_cpuidle_set_states(msm_cstates, ARRAY_SIZE(msm_cstates),
				msm_pm_data);
	change_memory_power = &msm8960_change_memory_power;
	BUG_ON(msm_pm_boot_init(&msm_pm_boot_pdata));
	msm_pm_init_sleep_status_data(&msm_pm_slp_sts_data);
	if (PLATFORM_IS_CHARM25())
		platform_add_devices(mdm_devices, ARRAY_SIZE(mdm_devices));

/*
 * ZTE_PLATFORM
 */
#if 1
    hw_ver = read_zte_hw_ver();
    socinfo_sync_sysfs_zte_hw_ver(hw_ver);
#endif
}

MACHINE_START(MSM8960_SIM, "QCT MSM8960 SIMULATOR")
	.map_io = msm8960_map_io,
	.reserve = msm8960_reserve,
	.init_irq = msm8960_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8960_sim_init,
	.init_early = msm8960_allocate_memory_regions,
	.init_very_early = msm8960_early_memory,
MACHINE_END

MACHINE_START(MSM8960_RUMI3, "QCT MSM8960 RUMI3")
	.map_io = msm8960_map_io,
	.reserve = msm8960_reserve,
	.init_irq = msm8960_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8960_rumi3_init,
	.init_early = msm8960_allocate_memory_regions,
	.init_very_early = msm8960_early_memory,
MACHINE_END

MACHINE_START(MSM8960_CDP, "QCT MSM8960 CDP")
	.map_io = msm8960_map_io,
	.reserve = msm8960_reserve,
	.init_irq = msm8960_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8960_cdp_init,
	.init_early = msm8960_allocate_memory_regions,
	.init_very_early = msm8960_early_memory,
MACHINE_END

MACHINE_START(MSM8960_MTP, "QCT MSM8960 MTP")
	.map_io = msm8960_map_io,
	.reserve = msm8960_reserve,
	.init_irq = msm8960_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8960_cdp_init,
	.init_early = msm8960_allocate_memory_regions,
	.init_very_early = msm8960_early_memory,
MACHINE_END

MACHINE_START(MSM8960_FLUID, "QCT MSM8960 FLUID")
	.map_io = msm8960_map_io,
	.reserve = msm8960_reserve,
	.init_irq = msm8960_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8960_cdp_init,
	.init_early = msm8960_allocate_memory_regions,
	.init_very_early = msm8960_early_memory,
MACHINE_END

MACHINE_START(MSM8960_LIQUID, "QCT MSM8960 LIQUID")
	.map_io = msm8960_map_io,
	.reserve = msm8960_reserve,
	.init_irq = msm8960_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8960_cdp_init,
	.init_early = msm8960_allocate_memory_regions,
	.init_very_early = msm8960_early_memory,
MACHINE_END

#ifdef CONFIG_MACH_ADAMS

MACHINE_START(ADAMS, "ZTE MSM8960 ADAMS")
	.map_io = msm8960_map_io,
	.reserve = msm8960_reserve,
	.init_irq = msm8960_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8960_cdp_init,
	.init_early = msm8960_allocate_memory_regions,
	.init_very_early = msm8960_early_memory,
MACHINE_END

#elif defined(CONFIG_MACH_BAKER)

MACHINE_START(BAKER, "ZTE MSM8960 BAKER")
	.map_io = msm8960_map_io,
	.reserve = msm8960_reserve,
	.init_irq = msm8960_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8960_cdp_init,
	.init_early = msm8960_allocate_memory_regions,
	.init_very_early = msm8960_early_memory,	
MACHINE_END

#elif defined(CONFIG_MACH_CRATER)

MACHINE_START(CRATER, "ZTE MSM8960 CRATER")
	.map_io = msm8960_map_io,
	.reserve = msm8960_reserve,
	.init_irq = msm8960_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8960_cdp_init,
	.init_early = msm8960_allocate_memory_regions,
	.init_very_early = msm8960_early_memory,
MACHINE_END

#elif defined(CONFIG_MACH_DANA)

MACHINE_START(DANA, "ZTE MSM8960 DANA")
	.map_io = msm8960_map_io,
	.reserve = msm8960_reserve,
	.init_irq = msm8960_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8960_cdp_init,
	.init_early = msm8960_allocate_memory_regions,
	.init_very_early = msm8960_early_memory,
	
MACHINE_END

#elif defined(CONFIG_MACH_ELDEN)

MACHINE_START(ELDEN, "ZTE MSM8960 ELDEN")
	.map_io = msm8960_map_io,
	.reserve = msm8960_reserve,
	.init_irq = msm8960_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8960_cdp_init,
	.init_early = msm8960_allocate_memory_regions,
	.init_very_early = msm8960_early_memory,
MACHINE_END

#elif defined(CONFIG_MACH_FROSTY)

MACHINE_START(FROSTY, "ZTE MSM8960 FROSTY")
	.map_io = msm8960_map_io,
	.reserve = msm8960_reserve,
	.init_irq = msm8960_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8960_cdp_init,
	.init_early = msm8960_allocate_memory_regions,
	.init_very_early = msm8960_early_memory,
MACHINE_END

#elif defined(CONFIG_MACH_GORDON)

MACHINE_START(GORDON, "ZTE MSM8960 GORDON")
	.map_io = msm8960_map_io,
	.reserve = msm8960_reserve,
	.init_irq = msm8960_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8960_cdp_init,
	.init_early = msm8960_allocate_memory_regions,
	.init_very_early = msm8960_early_memory,
MACHINE_END

#elif defined(CONFIG_MACH_HAYES)

MACHINE_START(HAYES, "ZTE MSM8960 HAYES")
	.map_io = msm8960_map_io,
	.reserve = msm8960_reserve,
	.init_irq = msm8960_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8960_cdp_init,
	.init_early = msm8960_allocate_memory_regions,
	.init_very_early = msm8960_early_memory,
MACHINE_END

#elif defined(CONFIG_MACH_ILIAMNA)

MACHINE_START(ILIAMNA, "ZTE MSM8960 ILIAMNA")
	.map_io = msm8960_map_io,
	.reserve = msm8960_reserve,
	.init_irq = msm8960_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8960_cdp_init,
	.init_early = msm8960_allocate_memory_regions,
	.init_very_early = msm8960_early_memory,
MACHINE_END

#elif defined(CONFIG_MACH_JARVIS)

MACHINE_START(JARVIS, "ZTE MSM8960 JARVIS")
	.map_io = msm8960_map_io,
	.reserve = msm8960_reserve,
	.init_irq = msm8960_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8960_cdp_init,
	.init_early = msm8960_allocate_memory_regions,
	.init_very_early = msm8960_early_memory,
MACHINE_END

#elif defined(CONFIG_MACH_KISKA)

MACHINE_START(KISKA, "ZTE MSM8960 KISKA")
	.map_io = msm8960_map_io,
	.reserve = msm8960_reserve,
	.init_irq = msm8960_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8960_cdp_init,
	.init_early = msm8960_allocate_memory_regions,
	.init_very_early = msm8960_early_memory,
MACHINE_END

#endif /* CONFIG_MACH_ADAMS */

                                                                                                                                                                                                                                                                                                     files/Kconfig                                                                                       0000664 0001756 0001756 00000165346 12214722135 014257  0                                                                                                    ustar   rbheromax                       rbheromax                                                                                                                                                                                                              if ARCH_MSM

menu "MSM SoC Type"

config ARCH_MSM7X01A
	bool "MSM7x00A / MSM7x01A"
	select ARCH_MSM_ARM11
	select MSM_VIC
	select CPU_V6
	select MSM_REMOTE_SPINLOCK_SWP
	select MSM_PM if PM

config ARCH_MSM7X25
	bool "MSM7x25"
	select ARCH_MSM_ARM11
	select MSM_VIC
	select CPU_V6
	select MSM_REMOTE_SPINLOCK_SWP
	select MULTI_IRQ_HANDLER
	select MSM_PM if PM

config ARCH_MSM7X27
	bool "MSM7x27"
	select ARCH_MSM_ARM11 if MSM_SOC_REV_NONE
	select ARCH_HAS_BARRIERS if MSM_SOC_REV_NONE
	select ARCH_MSM_CORTEX_A5 if MSM_SOC_REV_A
	select MSM_VIC
	select CPU_V6 if MSM_SOC_REV_NONE
	select CPU_V7 if MSM_SOC_REV_A
	select MSM_REMOTE_SPINLOCK_SWP if MSM_SOC_REV_NONE
	select MSM_GPIOMUX
	select REGULATOR
	select MULTI_IRQ_HANDLER
	select MSM_PROC_COMM_REGULATOR
	select CLEANCACHE
	select QCACHE
	select MSM_PM2 if PM

config ARCH_MSM7X30
	bool "MSM7x30"
	select ARCH_MSM_SCORPION
	select MSM_VIC
	select CPU_V7
	select MSM_REMOTE_SPINLOCK_DEKKERS
	select ARCH_SPARSEMEM_ENABLE
	select ARCH_HAS_HOLES_MEMORYMODEL
	select MEMORY_HOTPLUG
	select MEMORY_HOTREMOVE
	select ARCH_ENABLE_MEMORY_HOTPLUG
	select ARCH_ENABLE_MEMORY_HOTREMOVE
	select MIGRATION
	select ARCH_MEMORY_PROBE
	select ARCH_MEMORY_REMOVE
	select MSM_GPIOMUX
	select RESERVE_FIRST_PAGE
	select MSM_DALRPC
	select MSM_SPM_V1
	select REGULATOR
	select MSM_PROC_COMM_REGULATOR
	select MULTI_IRQ_HANDLER
	select MSM_PM2 if PM

config ARCH_QSD8X50
	bool "QSD8X50"
	select ARCH_MSM_SCORPION
	select MSM_VIC
	select CPU_V7
	select MSM_REMOTE_SPINLOCK_LDREX
	select CPU_USE_DOMAINS
	select EMULATE_DOMAIN_MANAGER_V7
	select MSM_GPIOMUX
	select MSM_DALRPC
	select MSM_PM2 if PM

config ARCH_MSM8X60
	bool "MSM8X60"
	select ARCH_MSM_SCORPIONMP
	select SMP_PARALLEL_START if SMP
	select ARM_GIC
	select CPU_V7
	select MSM_REMOTE_SPINLOCK_LDREX
	select ARCH_REQUIRE_GPIOLIB
	select MSM_ADM3
	select REGULATOR
	select MSM_RPM_REGULATOR
	select MSM_V2_TLMM
	select MSM_PIL
	select ARCH_HAS_CPU_IDLE_WAIT
	select MSM_DIRECT_SCLK_ACCESS
	select MSM_RPM
	select MSM_XO
	select MSM_GPIOMUX
	select MSM_BUS_SCALING
	select MSM_SECURE_IO
	select MSM_DALRPC
	select MSM_QDSP6_APR
	select MSM_NATIVE_RESTART
	select ARCH_INLINE_SPIN_TRYLOCK
	select ARCH_INLINE_SPIN_TRYLOCK_BH
	select ARCH_INLINE_SPIN_LOCK
	select ARCH_INLINE_SPIN_LOCK_BH
	select ARCH_INLINE_SPIN_LOCK_IRQ
	select ARCH_INLINE_SPIN_LOCK_IRQSAVE
	select ARCH_INLINE_SPIN_UNLOCK
	select ARCH_INLINE_SPIN_UNLOCK_BH
	select ARCH_INLINE_SPIN_UNLOCK_IRQ
	select ARCH_INLINE_SPIN_UNLOCK_IRQRESTORE
	select ARCH_INLINE_READ_TRYLOCK
	select ARCH_INLINE_READ_LOCK
	select ARCH_INLINE_READ_LOCK_BH
	select ARCH_INLINE_READ_LOCK_IRQ
	select ARCH_INLINE_READ_LOCK_IRQSAVE
	select ARCH_INLINE_READ_UNLOCK
	select ARCH_INLINE_READ_UNLOCK_BH
	select ARCH_INLINE_READ_UNLOCK_IRQ
	select ARCH_INLINE_READ_UNLOCK_IRQRESTORE
	select ARCH_INLINE_WRITE_TRYLOCK
	select ARCH_INLINE_WRITE_LOCK
	select ARCH_INLINE_WRITE_LOCK_BH
	select ARCH_INLINE_WRITE_LOCK_IRQ
	select ARCH_INLINE_WRITE_LOCK_IRQSAVE
	select ARCH_INLINE_WRITE_UNLOCK
	select ARCH_INLINE_WRITE_UNLOCK_BH
	select ARCH_INLINE_WRITE_UNLOCK_IRQRESTORE
	select CPU_HAS_L2_PMU
	select MSM_SPM_V1
	select MSM_SCM if SMP
	select MULTI_IRQ_HANDLER
	select MSM_MULTIMEDIA_USE_ION
	select MSM_PM8X60 if PM

config ARCH_MSM8960
	bool "MSM8960"
	select ARCH_MSM_KRAITMP
	select ARM_GIC
	select CPU_V7
	select MSM_V2_TLMM
	select MSM_GPIOMUX
	select MSM_SCM if SMP
	select MSM_DIRECT_SCLK_ACCESS
	select REGULATOR
	select MSM_RPM_REGULATOR
	select MSM_RPM
	select MSM_XO
	select MSM_QDSP6_APR
	select MSM_PIL
	select MSM_AUDIO_QDSP6 if SND_SOC
	select CPU_HAS_L2_PMU
	select MSM_SPM_V2
	select MSM_L2_SPM
	select MSM_NATIVE_RESTART
	select DONT_MAP_HOLE_AFTER_MEMBANK0
	select MSM_REMOTE_SPINLOCK_SFPB
	select ARCH_POPULATES_NODE_MAP
	select ARCH_SPARSEMEM_ENABLE
	select ARCH_HAS_HOLES_MEMORYMODEL
	select MEMORY_HOTPLUG if ENABLE_DMM
	select MEMORY_HOTREMOVE if ENABLE_DMM
	select ARCH_ENABLE_MEMORY_HOTPLUG if ENABLE_DMM
	select ARCH_ENABLE_MEMORY_HOTREMOVE if ENABLE_DMM
	select MIGRATION if ENABLE_DMM
	select ARCH_MEMORY_PROBE if ENABLE_DMM
	select ARCH_MEMORY_REMOVE if ENABLE_DMM
	select FIX_MOVABLE_ZONE if ENABLE_DMM
	select CLEANCACHE
	select QCACHE
	select MSM_MULTIMEDIA_USE_ION
	select MULTI_IRQ_HANDLER
	select MSM_PM8X60 if PM
	select HOLES_IN_ZONE if SPARSEMEM
	select MSM_KRAIT_WFE_FIXUP

config ARCH_MSM8930
	bool "MSM8930"
	select ARCH_MSM_KRAITMP
	select ARM_GIC
	select CPU_V7
	select MSM_V2_TLMM
	select MSM_GPIOMUX
	select MSM_SCM if SMP
	select MSM_DIRECT_SCLK_ACCESS
	select REGULATOR
	select MSM_RPM
	select MSM_XO
	select MSM_QDSP6_APR
	select MSM_PIL
	select MSM_AUDIO_QDSP6 if SND_SOC
	select CPU_HAS_L2_PMU
	select MSM_SPM_V2
	select MSM_L2_SPM
	select MSM_NATIVE_RESTART
	select DONT_MAP_HOLE_AFTER_MEMBANK0
	select MSM_REMOTE_SPINLOCK_SFPB
	select ARCH_POPULATES_NODE_MAP
	select ARCH_SPARSEMEM_ENABLE
	select ARCH_HAS_HOLES_MEMORYMODEL
	select MEMORY_HOTPLUG if ENABLE_DMM
	select MEMORY_HOTREMOVE if ENABLE_DMM
	select ARCH_ENABLE_MEMORY_HOTPLUG if ENABLE_DMM
	select ARCH_ENABLE_MEMORY_HOTREMOVE if ENABLE_DMM
	select MIGRATION if ENABLE_DMM
	select ARCH_MEMORY_PROBE if ENABLE_DMM
	select ARCH_MEMORY_REMOVE if ENABLE_DMM
	select FIX_MOVABLE_ZONE if ENABLE_DMM
	select MSM_ULTRASOUND
	select MULTI_IRQ_HANDLER
	select MSM_PM8X60 if PM
	select HOLES_IN_ZONE if SPARSEMEM
	select MSM_KRAIT_WFE_FIXUP

config ARCH_APQ8064
	bool "APQ8064"
	select ARCH_MSM_KRAITMP
	select MACH_APQ8064_SIM if !MACH_APQ8064_RUMI3
	select MSM_V2_TLMM
	select ARM_GIC
	select CPU_V7
	select MSM_SCM if SMP
	select MSM_GPIOMUX
	select MSM_REMOTE_SPINLOCK_SFPB
	select MSM_PIL
	select MSM_QDSP6_APR
	select MSM_AUDIO_QDSP6 if SND_SOC
	select MULTI_IRQ_HANDLER
	select MSM_PM8X60 if PM
	select HOLES_IN_ZONE if SPARSEMEM
	select MSM_KRAIT_WFE_FIXUP

config ARCH_MSMCOPPER
	bool "MSM Copper"
	select ARCH_MSM_KRAITMP
	select MSM_V2_TLMM
	select ARM_GIC
	select CPU_V7
	select MSM_SCM if SMP
	select MSM_GPIOMUX
	select MULTI_IRQ_HANDLER
	select MSM_MULTIMEDIA_USE_ION

config ARCH_FSM9XXX
	bool "FSM9XXX"
	select ARCH_MSM_SCORPION
	select MSM_VIC
	select CPU_V7
	select MSM_REMOTE_SPINLOCK_LDREX
	select FSM9XXX_TLMM
	select MULTI_IRQ_HANDLER
	select MSM_DALRPC

config ARCH_MSM9615
	bool "MSM9615"
	select ARM_GIC
	select ARCH_MSM_CORTEX_A5
	select CPU_V7
	select MSM_V2_TLMM
	select MSM_GPIOMUX
	select MSM_RPM
	select MSM_SPM_V2
	select MSM_NATIVE_RESTART
	select REGULATOR
	select MSM_RPM_REGULATOR
	select MULTI_IRQ_HANDLER
	select MSM_PM8X60 if PM
	select MSM_XO
	select MSM_QDSP6_APR
	select MSM_AUDIO_QDSP6 if SND_SOC
	select FIQ

endmenu

choice
	prompt "MSM SoC Revision"
	default MSM_SOC_REV_NONE
config MSM_SOC_REV_NONE
	bool "N/A"
	select EMULATE_DOMAIN_MANAGER_V7 if ARCH_QSD8X50
	select VERIFY_PERMISSION_FAULT if ARCH_QSD8X50
config MSM_SOC_REV_A
	bool "Rev. A"
	select ARCH_MSM7X27A if ARCH_MSM7X27
endchoice

config MSM_KRAIT_TBB_ABORT_HANDLER
	bool "Krait TBB/TBH data abort handler"
	depends on ARCH_MSM_KRAIT
	depends on ARM_THUMB
	help
	  Certain early samples of the Krait processor may generate data
	  aborts for TBB / TBH instructions that fail their condition code
	  checks. Enabling this option will ignore these erroneous data aborts,
	  at the expense of a very small performance penalty.

	  If unsure, say N.

config  ARCH_MSM_ARM11
	bool

config  ARCH_MSM_SCORPION
	bool

config  ARCH_MSM_KRAIT
	bool
	select ARM_L1_CACHE_SHIFT_6

config  MSM_SMP
	bool

config  ARCH_MSM_SCORPIONMP
	select ARCH_MSM_SCORPION
	select MSM_SMP
	bool

config  ARCH_MSM_KRAITMP
	select ARCH_MSM_KRAIT
	select MSM_SMP
	bool

config  MSM_KRAIT_WFE_FIXUP
	bool

config  ARCH_MSM_CORTEX_A5
	bool
	select HAVE_HW_BRKPT_RESERVED_RW_ACCESS

config ARCH_MSM7X27A
	bool
	select MSM_DALRPC
	select MSM_PROC_COMM_REGULATOR
	select MULTI_IRQ_HANDLER

config  MSM_VIC
	bool

config MSM_RPM
	bool "Resource Power Manager"
	select MSM_MPM

config MSM_MPM
	bool "Modem Power Manager"

config MSM_XO
	bool

config MSM_REMOTE_SPINLOCK_DEKKERS
	bool
config MSM_REMOTE_SPINLOCK_SWP
	bool
config MSM_REMOTE_SPINLOCK_LDREX
	bool
config MSM_REMOTE_SPINLOCK_SFPB
	bool
config MSM_ADM3
	bool

menu "MSM Board Selection"

config MACH_HALIBUT
	depends on ARCH_MSM7X01A
	depends on MSM_STACKED_MEMORY
	default y
	bool "Halibut Board (QCT SURF7201A)"
	help
	  Support for the Qualcomm SURF7201A eval board.

config MACH_MSM7201A_SURF
	depends on ARCH_MSM7X01A
	depends on MSM_STACKED_MEMORY
	default y
	bool "MSM7201A SURF"
	help
	  Support for the Qualcomm MSM7201A SURF eval board.

config MACH_MSM7201A_FFA
	depends on ARCH_MSM7X01A
	depends on MSM_STACKED_MEMORY
	default y
	bool "MSM7201A FFA"
	help
	  Support for the Qualcomm MSM7201A FFA eval board.

config MACH_TROUT
	depends on ARCH_MSM7X01A
	depends on MSM_STACKED_MEMORY
	default y
	bool "Trout"

config MACH_MSM7X27_SURF
	depends on ARCH_MSM7X27
	depends on !MSM_STACKED_MEMORY
	default y
	bool "MSM7x27 SURF"
	help
	  Support for the Qualcomm MSM7x27 SURF eval board.

config MACH_MSM7X27_FFA
	depends on ARCH_MSM7X27
	depends on !MSM_STACKED_MEMORY
	default y
	bool "MSM7x27 FFA"
	help
	  Support for the Qualcomm MSM7x27 FFA eval board.

config MACH_MSM7X27A_RUMI3
        depends on ARCH_MSM7X27A
        depends on !MSM_STACKED_MEMORY
        default y
        bool "MSM7x27A RUMI3"
        help
          Support for the Qualcomm MSM7x27A RUMI3 Emulation Platform.

config MACH_MSM7X27A_SURF
        depends on ARCH_MSM7X27A
        depends on !MSM_STACKED_MEMORY
        default y
        bool "MSM7x27A SURF"
        help
          Support for the Qualcomm MSM7x27A SURF.

config MACH_MSM7X27A_FFA
        depends on ARCH_MSM7X27A
        depends on !MSM_STACKED_MEMORY
        default y
        bool "MSM7x27A FFA"
        help
          Support for the Qualcomm MSM7x27A FFA.

config MACH_MSM7625A_SURF
        depends on ARCH_MSM7X27A
        depends on !MSM_STACKED_MEMORY
        default y
        bool "MSM7625A SURF"
        help
          Support for the Qualcomm MSM7625A SURF.

config MACH_MSM7625A_FFA
        depends on ARCH_MSM7X27A
        depends on !MSM_STACKED_MEMORY
        default y
        bool "MSM7625A FFA"
        help
          Support for the Qualcomm MSM7625A FFA.

config MACH_MSM7627A_QRD1
        depends on ARCH_MSM7X27A
        depends on !MSM_STACKED_MEMORY
        default y
        bool "MSM7627A QRD1"
        help
          Support for the Qualcomm MSM7627A Refrence Design.

config MACH_MSM7X30_SURF
       depends on ARCH_MSM7X30
       depends on !MSM_STACKED_MEMORY
       default y
       bool "MSM7x30 SURF"
       help
         Support for the Qualcomm MSM7x30 SURF eval board.

config MACH_MSM7X30_FFA
       depends on ARCH_MSM7X30
       depends on !MSM_STACKED_MEMORY
       default y
       bool "MSM7x30 FFA"
       help
         Support for the Qualcomm MSM7x30 FFA eval board.

config MACH_MSM7X30_FLUID
       depends on ARCH_MSM7X30
       depends on !MSM_STACKED_MEMORY
       default y
       bool "MSM7x30 FLUID"
       help
         Support for the Qualcomm MSM7x30 FLUID eval board.

config MACH_SAPPHIRE
	depends on ARCH_MSM7X01A
	default n
	bool "Sapphire"

config MACH_QSD8X50_SURF
	depends on ARCH_QSD8X50
	depends on MSM_SOC_REV_NONE
	depends on MSM_STACKED_MEMORY
	default y
	bool "QSD8x50 SURF"
	help
	  Support for the Qualcomm QSD8x50 SURF eval board.

config MACH_QSD8X50_FFA
	depends on ARCH_QSD8X50
	depends on MSM_SOC_REV_NONE
	depends on MSM_STACKED_MEMORY
	default y
	bool "QSD8x50 FFA"
	help
	  Support for the Qualcomm QSD8x50 FFA eval board.

config MACH_MSM7X25_SURF
	depends on ARCH_MSM7X25
	depends on !MSM_STACKED_MEMORY
	default y
	bool "MSM7x25 SURF"
	help
	  Support for the Qualcomm MSM7x25 SURF eval board.

config MACH_MSM7X25_FFA
	depends on ARCH_MSM7X25
	depends on !MSM_STACKED_MEMORY
	default y
	bool "MSM7x25 FFA"
	help
	  Support for the Qualcomm MSM7x25 FFA eval board.

config MACH_MSM8X55_SURF
       depends on ARCH_MSM7X30
       depends on !MSM_STACKED_MEMORY
       default y
       bool "MSM8X55 SURF"
       help
         Support for the Qualcomm MSM8x55 SURF eval board.

config MACH_MSM8X55_FFA
       depends on ARCH_MSM7X30
       depends on !MSM_STACKED_MEMORY
       default y
       bool "MSM8X55 FFA"
       help
         Support for the Qualcomm MSM8x55 FFA eval board.

config MACH_MSM8X55_SVLTE_FFA
       depends on ARCH_MSM7X30
       depends on !MSM_STACKED_MEMORY
       default y
       bool "MSM8X55 SVLTE FFA"
       help
         Support for the Qualcomm MSM8x55 SVLTE FFA eval board.

config MACH_MSM8X55_SVLTE_SURF
       depends on ARCH_MSM7X30
       depends on !MSM_STACKED_MEMORY
       default y
       bool "MSM8X55 SVLTE SURF"
       help
         Support for the Qualcomm MSM8x55 SVLTE SURF eval board.

config MACH_MSM8X60_RUMI3
	depends on ARCH_MSM8X60
	default n
	bool "MSM8x60 RUMI3"
	help
	  Support for the Qualcomm MSM8x60 RUMI3 emulator.

config MACH_MSM8X60_SIM
	depends on ARCH_MSM8X60
	default n
	bool "MSM8x60 Simulator"
	help
	  Support for the Qualcomm MSM8x60 simulator.

config MACH_MSM8X60_SURF
	depends on ARCH_MSM8X60
	default n
	bool "MSM8x60 SURF"
	help
	  Support for the Qualcomm MSM8x60 SURF eval board.

config MACH_MSM8X60_FFA
	depends on ARCH_MSM8X60
	default n
	bool "MSM8x60 FFA"
	help
	  Support for the Qualcomm MSM8x60 FFA eval board.

config MACH_MSM8X60_FLUID
	depends on ARCH_MSM8X60
	default n
	bool "MSM8x60 FLUID"
	help
	  Support for the Qualcomm MSM8x60 FLUID platform. The FLUID is an
	  8x60 target which has a form factor that is much closer to that
	  of a phone than other targets. It also has a new display and
	  touchscreen controller.

config MACH_MSM8X60_FUSION
	depends on ARCH_MSM8X60
	default n
	bool "MSM8x60 FUSION"
	help
	  Support for the Qualcomm MSM8x60 Fusion SURF device.

config MACH_MSM8X60_FUSN_FFA
	depends on ARCH_MSM8X60
	default n
	bool "MSM8x60 FUSN FFA"
	help
	  Support for the Qualcomm MSM8x60 Fusion FFA device.

config MACH_MSM8X60_DRAGON
	depends on ARCH_MSM8X60
	default n
	bool "MSM8x60 DRAGON"
	help
	  Support for the Qualcomm MSM8x60 Dragon board.

config MACH_MSM8960_SIM
	depends on ARCH_MSM8960
	bool "MSM8960 Simulator"
	help
	  Support for the Qualcomm MSM8960 simulator.

config MACH_MSM8960_RUMI3
	depends on ARCH_MSM8960
	bool "MSM8960 RUMI3"
	help
	  Support for the Qualcomm MSM8960 RUMI3 emulator.

config MACH_MSM8960_CDP
	depends on ARCH_MSM8960
	bool "MSM8960 CDP"
	help
	  Support for the Qualcomm MSM8960 CDP device.

config MACH_MSM8960_MTP
	depends on ARCH_MSM8960
	bool "MSM8960 MTP"
	help
	  Support for the Qualcomm MSM8960 MTP device.

config MACH_MSM8960_FLUID
	depends on ARCH_MSM8960
	bool "MSM8960 FLUID"
	help
	  Support for the Qualcomm MSM8960 FLUID device.

config MACH_MSM8960_LIQUID
	depends on ARCH_MSM8960
	bool "MSM8960 LIQUID"
	help
	  Support for the Qualcomm MSM8960 LIQUID device.

config MACH_ADAMS
	depends on ARCH_MSM8960
	bool "ZTE ADAMS"
	help
	  Support for the ZTE MSM8960 Adams device.

config MACH_BAKER
	depends on ARCH_MSM8960
	bool "ZTE BAKER"
	help
	  Support for the ZTE MSM8960 Baker device.

config MACH_CRATER
	depends on ARCH_MSM8960
	bool "ZTE CRATER"
	help
	  Support for the ZTE MSM8960 Crater device.

config MACH_DANA
	depends on ARCH_MSM8960
	bool "ZTE DANA"
	help
	  Support for the ZTE MSM8960 Dana device.

config MACH_ELDEN
	depends on ARCH_MSM8960
	bool "ZTE ELDEN"
	help
	  Support for the ZTE MSM8960 Elden device.

config MACH_FROSTY
	depends on ARCH_MSM8960
	bool "ZTE FROSTY"
	help
	  Support for the ZTE MSM8960 Frosty device.

config MACH_GORDON
	depends on ARCH_MSM8960
	bool "ZTE GORDON"
	help
	  Support for the ZTE MSM8960 Gordon device.

config MACH_HAYES
	depends on ARCH_MSM8960
	bool "ZTE HAYES"
	help
	  Support for the ZTE MSM8960 Hayes device.

config MACH_ILIAMNA
	depends on ARCH_MSM8960
	bool "ZTE ILIAMNA"
	help
	  Support for the ZTE MSM8960 Iliamna device.

config MACH_JARVIS
	depends on ARCH_MSM8960
	bool "ZTE JARVIS"
	help
	  Support for the ZTE MSM8960 Jarvis device.

config MACH_KISKA
	depends on ARCH_MSM8960
	bool "ZTE KISKA"
	help
	  Support for the ZTE MSM8960 Kiska device.

config ZTE_SDLOG
        depends on ARCH_MSM
        depends on !MSM_STACKED_MEMORY
        default y
        bool "sdLog"
        help
          Support for sdlog.	  


config MACH_MSM8930_CDP
	depends on ARCH_MSM8930
	bool "MSM8930 CDP"
	help
	  Support for the Qualcomm MSM8930 CDP device.

config MACH_MSM8930_MTP
	depends on ARCH_MSM8930
	bool "MSM8930 MTP"
	help
	  Support for the Qualcomm MSM8930 MTP device.

config MACH_MSM8930_FLUID
	depends on ARCH_MSM8930
	bool "MSM8930 FLUID"
	help
	  Support for the Qualcomm MSM8930 FLUID device.

config MACH_MSM8627_CDP
	depends on ARCH_MSM8930
	bool "MSM8627 CDP"
	help
	  Support for the Qualcomm MSM8627 CDP device.

config MACH_MSM8627_MTP
	depends on ARCH_MSM8930
	bool "MSM8627 MTP"
	help
	  Support for the Qualcomm MSM8627 MTP device.

config MACH_MSM9615_CDP
	depends on ARCH_MSM9615
	bool "MSM9615 CDP"
	help
	  Support for the Qualcomm MSM9615 CDP device.

config MACH_MSM9615_MTP
	depends on ARCH_MSM9615
	bool "MSM9615 MTP"
	help
	  Support for the Qualcomm MSM9615 MTP device.

config MSM_USE_TSIF1
	depends on ARCH_MSM8X60
	bool "MSM8x60 use TSIF1"
	help
	  Selects TSIF1 core to be used rather than TSIF0.
	  The two TSIF cores share the same DM configuration
	  so they cannot be used simultaneously.

config MACH_APQ8064_SIM
	depends on ARCH_APQ8064
	bool "APQ8064 Simulator"
	help
	  Support for the Qualcomm APQ8064 simulator.

config MACH_APQ8064_RUMI3
	depends on ARCH_APQ8064
	bool "APQ8064 RUMI3"
	help
	  Support for the Qualcomm APQ8064 RUMI3 emulator.

config MACH_FSM9XXX_SURF
	depends on ARCH_FSM9XXX
	depends on !MSM_STACKED_MEMORY
	default y
	bool "FSM9XXX SURF"
	help
	  Support for the Qualcomm FSM9xxx femtocell
	  chipset based SURF evaluation board and
	  FFA board.

endmenu

config MSM_STACKED_MEMORY
	bool "Stacked Memory"
	default y
	help
	  This option is used to indicate the presence of on-die stacked
	  memory.  When present this memory bank is used for a high speed
	  shared memory interface.  When not present regular RAM is used.

config PHYS_OFFSET
	hex
	default "0x40800000" if ARCH_MSM9615
	default "0x80200000" if ARCH_APQ8064
	default "0x80200000" if ARCH_MSM8960
	default "0x80200000" if ARCH_MSM8930
	default "0x80200000" if ARCH_MSMCOPPER
	default "0x10000000" if ARCH_FSM9XXX
	default "0x00200000" if !MSM_STACKED_MEMORY
	default "0x00000000" if ARCH_QSD8X50 && MSM_SOC_REV_A
	default "0x20000000" if ARCH_QSD8X50
	default "0x40200000" if ARCH_MSM8X60
	default "0x10000000"

config KERNEL_PMEM_EBI_REGION
	bool "Enable in-kernel PMEM region for EBI"
	default y if ARCH_MSM8X60
	depends on ANDROID_PMEM && (ARCH_MSM8X60 || ARCH_MSM8960 || ARCH_MSMCOPPER)
	help
	   Enable the in-kernel PMEM allocator to use EBI memory.

config KERNEL_PMEM_SMI_REGION
	bool "Enable in-kernel PMEM region for SMI"
	default y if ARCH_MSM8X60
	depends on ANDROID_PMEM && ((ARCH_QSD8X50 && !PMEM_GPU0) || (ARCH_MSM8X60 && !VCM))
	help
	   Enable the in-kernel PMEM allocator to use SMI memory.

config PMEM_GPU0
	bool "Enable PMEM GPU0 region"
	default y
	depends on ARCH_QSD8X50 && ANDROID_PMEM
	help
	  Enable the PMEM GPU0 device on SMI Memory.

config MSM_AMSS_VERSION
	int
	default 6210 if MSM_AMSS_VERSION_6210
	default 6220 if MSM_AMSS_VERSION_6220
	default 6225 if MSM_AMSS_VERSION_6225

choice
	prompt "AMSS modem firmware version"

	default MSM_AMSS_VERSION_6225

	config MSM_AMSS_VERSION_6210
		bool "6.2.10"

	config MSM_AMSS_VERSION_6220
		bool "6.2.20"

	config MSM_AMSS_VERSION_6225
		bool "6.2.20 + New ADSP"
endchoice

config MSM_HAS_DEBUG_UART_HS
	bool
	help
	  Say Y here if high speed MSM UART is present.

config MSM_HAS_DEBUG_UART_HS_V14
	bool
	select MSM_HAS_DEBUG_UART_HS
	help
	  Say Y here if high speed MSM UART v1.4 is present.

config DEBUG_MSM8930_UART
	bool "Kernel low-level debugging messages via MSM 8930 UART"
	depends on ARCH_MSM8930 && DEBUG_LL
	select MSM_HAS_DEBUG_UART_HS
	help
	  Say Y here if you want the debug print routines to direct
	  their output to the serial port on MSM 8930 devices.

choice
	prompt "Debug UART"
	depends on DEBUG_LL

	config DEBUG_MSM_UART1
		bool "Kernel low-level debugging messages via MSM UART1"
		depends on ARCH_MSM7X00A || ARCH_MSM7X30 || ARCH_QSD8X50
		help
		  Say Y here if you want the debug print routines to direct
		  their output to the first serial port on MSM devices.

	config DEBUG_MSM_UART2
		bool "Kernel low-level debugging messages via MSM UART2"
		depends on ARCH_MSM7X00A || ARCH_MSM7X30 || ARCH_QSD8X50
		help
		  Say Y here if you want the debug print routines to direct
		  their output to the second serial port on MSM devices.

	config DEBUG_MSM_UART3
		bool "Kernel low-level debugging messages via MSM UART3"
		depends on ARCH_MSM7X00A || ARCH_MSM7X30 || ARCH_QSD8X50
		help
		  Say Y here if you want the debug print routines to direct
		  their output to the third serial port on MSM devices.

	config DEBUG_MSM8660_UART
		bool "Kernel low-level debugging messages via MSM 8660 UART"
		depends on ARCH_MSM8X60
		select MSM_HAS_DEBUG_UART_HS
		help
		  Say Y here if you want the debug print routines to direct
		  their output to the serial port on MSM 8660 devices.

	config DEBUG_MSM8960_UART
		bool "Kernel low-level debugging messages via MSM 8960 UART"
		depends on ARCH_MSM8960
		select DEBUG_MSM8930_UART
		select MSM_HAS_DEBUG_UART_HS
		help
		  Say Y here if you want the debug print routines to direct
		  their output to the serial port on MSM 8960 devices.

	config DEBUG_MSMCOPPER_UART
		bool "Kernel low-level debugging messages via MSM Copper UART"
		depends on ARCH_MSMCOPPER
		select MSM_HAS_DEBUG_UART_HS_V14
		help
		  Say Y here if you want the debug print routines to direct
		  their output to the serial port on MSM Copper devices.
endchoice

choice
	prompt "Default Timer"
	default MSM7X00A_USE_GP_TIMER

	config MSM7X00A_USE_GP_TIMER
		bool "GP Timer"
	help
	  Low resolution timer that allows power collapse from idle.

	config MSM7X00A_USE_DG_TIMER
		bool "DG Timer"
	help
	  High resolution timer.
endchoice

choice
	prompt "Suspend sleep mode"
	default MSM7X00A_SLEEP_MODE_POWER_COLLAPSE_SUSPEND
	help
	  Allows overriding the sleep mode used. Leave at power
	  collapse suspend unless the arm9 image has problems.

	config MSM7X00A_SLEEP_MODE_POWER_COLLAPSE_SUSPEND
		bool "Power collapse suspend"
	help
	  Lowest sleep state. Returns through reset vector.

	config MSM7X00A_SLEEP_MODE_POWER_COLLAPSE
		bool "Power collapse"
	help
	  Sleep state that returns through reset vector.

	config MSM7X00A_SLEEP_MODE_APPS_SLEEP
		bool "Apps Sleep"

	config MSM7X00A_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT
		bool "Ramp down cpu clock and wait for interrupt"

	config MSM7X00A_SLEEP_WAIT_FOR_INTERRUPT
		bool "Wait for interrupt"
endchoice

config MSM7X00A_SLEEP_MODE
	int
	default 0 if MSM7X00A_SLEEP_MODE_POWER_COLLAPSE_SUSPEND
	default 1 if MSM7X00A_SLEEP_MODE_POWER_COLLAPSE
	default 2 if MSM7X00A_SLEEP_MODE_APPS_SLEEP
	default 3 if MSM7X00A_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT
	default 4 if MSM7X00A_SLEEP_WAIT_FOR_INTERRUPT

choice
	prompt "Idle sleep mode"
	default MSM7X00A_IDLE_SLEEP_MODE_POWER_COLLAPSE
	help
	  Allows overriding the sleep mode used from idle. Leave at power
	  collapse suspend unless the arm9 image has problems.

	config MSM7X00A_IDLE_SLEEP_MODE_POWER_COLLAPSE_SUSPEND
		bool "Power collapse suspend"
	help
	  Lowest sleep state. Returns through reset vector.

	config MSM7X00A_IDLE_SLEEP_MODE_POWER_COLLAPSE
		bool "Power collapse"
	help
	  Sleep state that returns through reset vector.

	config MSM7X00A_IDLE_SLEEP_MODE_APPS_SLEEP
		bool "Apps Sleep"

	config MSM7X00A_IDLE_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT
		bool "Ramp down cpu clock and wait for interrupt"

	config MSM7X00A_IDLE_SLEEP_WAIT_FOR_INTERRUPT
		bool "Wait for interrupt"
endchoice

config MSM7X00A_IDLE_SLEEP_MODE
	int
	default 0 if MSM7X00A_IDLE_SLEEP_MODE_POWER_COLLAPSE_SUSPEND
	default 1 if MSM7X00A_IDLE_SLEEP_MODE_POWER_COLLAPSE
	default 2 if MSM7X00A_IDLE_SLEEP_MODE_APPS_SLEEP
	default 3 if MSM7X00A_IDLE_SLEEP_MODE_RAMP_DOWN_AND_WAIT_FOR_INTERRUPT
	default 4 if MSM7X00A_IDLE_SLEEP_WAIT_FOR_INTERRUPT

config MSM7X00A_IDLE_SLEEP_MIN_TIME
	int "Minimum idle time before sleep"
	default 20000000
	help
	  Minimum idle time in nanoseconds before entering low power mode.

config MSM7X00A_IDLE_SPIN_TIME
	int "Idle spin time before cpu ramp down"
	default 80000
	help
	  Spin time in nanoseconds before ramping down cpu clock and entering
	  any low power state.

menuconfig MSM_IDLE_STATS
	bool "Collect idle statistics"
	default y
	help
	  Collect idle statistics and export them in proc/msm_pm_stats.

if MSM_IDLE_STATS

config MSM_IDLE_STATS_FIRST_BUCKET
	int "First bucket time"
	default 62500
	help
	  Upper time limit in nanoseconds of first bucket.

config MSM_IDLE_STATS_BUCKET_SHIFT
	int "Bucket shift"
	default 2

config MSM_IDLE_STATS_BUCKET_COUNT
	int "Bucket count"
	default 10

config MSM_SUSPEND_STATS_FIRST_BUCKET
	int "First bucket time for suspend"
	default 1000000000
	help
	  Upper time limit in nanoseconds of first bucket of the
	  histogram.  This is for collecting statistics on suspend.

endif # MSM_IDLE_STATS

config CPU_HAS_L2_PMU
	bool "L2CC PMU Support"
	help
	 Select this if the L2 cache controller has a Performance Monitoring Unit.

config HTC_HEADSET
	tristate "HTC 2 Wire detection driver"
	default n
	help
	 Provides support for detecting HTC 2 wire devices, such as wired
	 headset, on the trout platform. Can be used with the msm serial
	 debugger, but not with serial console.

config TROUT_BATTCHG
	depends on MACH_TROUT && POWER_SUPPLY
	default y
	bool "Trout battery / charger driver"

config HTC_PWRSINK
	depends on MSM_SMD
	default n
	bool "HTC Power Sink Driver"

config QSD_SVS
	bool "QSD Static Voltage Scaling"
	depends on (MACH_QSD8X50_SURF || MACH_QSD8X50_FFA)
	default y
	select TPS65023
	help
	  Enables static voltage scaling using the TPS65023 PMIC.

config QSD_PMIC_DEFAULT_DCDC1
	int "PMIC default output voltage"
	depends on (MACH_QSD8X50_SURF || MACH_QSD8X50_FFA)
	default 1250
	help
	  This is the PMIC voltage at Linux kernel boot.

config MSM_FIQ_SUPPORT
	default y
	bool "Enable installation of an FIQ handler."

config MSM_SERIAL_DEBUGGER
	select MSM_FIQ_SUPPORT
	select KERNEL_DEBUGGER_CORE
	default n
	bool "FIQ Mode Serial Debugger"
	help
	  The FIQ serial debugger can accept commands even when the
	  kernel is unresponsive due to being stuck with interrupts
	  disabled.  Depends on the kernel debugger core in drivers/misc.

config MSM_SERIAL_DEBUGGER_CONSOLE
	depends on MSM_SERIAL_DEBUGGER
	default n
	bool "Console on FIQ Serial Debugger port"
	help
	  Enables a console so that printk messages are displayed on
	  the debugger serial port as the occur.

config MSM_PROC_COMM
	default y
	bool "Proc-Comm RPC Interface"
	help
	  Enables a lightweight communications interface to the
	  baseband processor.

config MSM_SMD
	bool "MSM Shared Memory Driver (SMD)"
	help
	  Support for the shared memory interface between the apps
	  processor and the baseband processor.  Provides access to
	  the "shared heap", as well as virtual serial channels
	  used to communicate with various services on the baseband
	  processor.

choice
	prompt "MSM Shared memory interface version"
	depends on MSM_SMD
	default MSM_SMD_PKG3 if ARCH_MSM_ARM11
	default MSM_SMD_PKG4 if ARCH_MSM_SCORPION

	config MSM_SMD_PKG3
	  bool
	    prompt "Package 3"

	config MSM_SMD_PKG4
	  bool
	    prompt "Package 4"
endchoice

config MSM_RPC_SDIO_XPRT
	depends on MSM_SDIO_AL
	default y
	bool "MSM SDIO XPRT Layer"
	help
	  SDIO Transport Layer for RPC Rouer

config MSM_RPC_SDIO_DEBUG
	depends on MSM_RPC_SDIO_XPRT
	default y
	bool "MSM SDIO XPRT debug support"
	help
	  Support for debugging SDIO XPRT

config MSM_SMD_DEBUG
	depends on MSM_SMD
	default y
	bool "MSM SMD debug support"
	help
	  Support for debugging the SMD for communication
	  between the ARM9 and ARM11

config MSM_SDIO_AL
	depends on ((ARCH_MSM7X30 || MACH_MSM8X60_FUSN_FFA || MACH_TYPE_MSM8X60_FUSION) && HAS_WAKELOCK)
	default y
	tristate "SDIO-Abstraction-Layer"
	help
	  Support MSM<->MDM Communication over SDIO bus.
	  MDM SDIO-Client should have pipes support.

config MSM_SDIO_DMUX
	bool "SDIO Data Mux Driver"
	depends on MSM_SDIO_AL
	default n
	help
	  Support Muxed Data Channels over SDIO interface.

config MSM_BAM_DMUX
	bool "BAM Data Mux Driver"
	depends on SPS
	default n
	help
	  Support Muxed Data Channels over BAM interface.
	  BAM has a limited number of pipes.  This driver
	  provides a means to support more logical channels
	  via muxing than BAM could without muxing.

config MSM_N_WAY_SMD
	depends on (MSM_SMD && !(ARCH_MSM7X01A))
	default y
	bool "MSM N-WAY SMD support"
	help
	  Supports APPS-QDSP SMD communication along with
	  normal APPS-MODEM SMD communication.

config MSM_N_WAY_SMSM
	depends on (MSM_SMD && !(ARCH_MSM7X01A))
	default y
	bool "MSM N-WAY SMSM support"
	help
	  Supports APPS-QDSP SMSM communication along with
	  normal APPS-MODEM SMSM communication.

config MSM_RESET_MODEM
	tristate "Reset Modem Driver"
	depends on MSM_SMD
	default m
	help
	  Allows the user to reset the modem through a device node.

config MSM_SMD_LOGGING
	depends on MSM_SMD
	default y
	bool "MSM Shared Memory Logger"
	help
	  This option exposes the shared memory logger at /dev/smem_log
	  and a debugfs node named smem_log.

	  If in doubt, say yes.

config MSM_SMD_NMEA
	bool "NMEA GPS Driver"
	depends on MSM_SMD
	default y
	help
	  Enable this to support the NMEA GPS device.

	  If in doubt, say yes.

config MSM_SDIO_TTY
	bool "SDIO TTY Driver"
	depends on MSM_SDIO_AL
	default n
	help
	  Provides a TTY driver SDIO TTY
	  This driver can be used by user space
	  applications for passing data through the
	  SDIO interface.

config MSM_SMD_TTY
	bool "SMD TTY Driver"
	depends on MSM_SMD
	default y
	help
	  Provides TTY interfaces to interact with the modem.

	  If in doubt, say yes.

config MSM_SMD_QMI
	bool "SMD QMI Driver"
	depends on MSM_SMD
	default y
	help
	  Manages network data connections.

	  If in doubt, say yes.

config MSM_SMD_PKT
	bool "SMD Packet Driver"
	depends on MSM_SMD
	default y
	help
	  Provides a binary SMD non-muxed packet port interface.

	  If in doubt, say yes.

config MSM_SDIO_CMUX
	bool "SDIO CMUX Driver"
	depends on MSM_SDIO_AL
	default n
	help
	  Provides a Muxed port interface over SDIO QMI

config MSM_DSPS
	bool "Sensors DSPS driver"
	depends on (MSM_PIL && (ARCH_MSM8X60 || ARCH_MSM8960))
	default n
	help
	  Provides user-space interface to the sensors manager
	  to turn on/off the Sensors Processor system clocks.
	  It is the DSPS responsibility to turn on/off the sensors
	  themself.
	  The number of clocks and their name may vary between targets.
	  It also triggers the PIL to load the DSPS firmware.

config MSM_SDIO_CTL
	bool "SDIO CTL Driver"
	depends on MSM_SDIO_CMUX
	default n
	help
	  Provides a binary SDIO control port interface.

config MSM_ONCRPCROUTER
	depends on MSM_SMD
	default n
	bool "MSM ONCRPC router support"
	help
	  Support for the MSM ONCRPC router for communication between
	  the ARM9 and ARM11

config MSM_IPC_ROUTER
	depends on NET
	default n
	bool "MSM IPC Router support"
	help
	  Support for the MSM IPC Router for communication between
	  the APPs and the MODEM

config MSM_IPC_ROUTER_SMD_XPRT
	depends on MSM_SMD
	depends on MSM_IPC_ROUTER
	default n
	bool "MSM SMD XPRT Layer"
	help
	  SMD Transport Layer for IPC Router

config MSM_ONCRPCROUTER_DEBUG
	depends on MSM_ONCRPCROUTER
	default y
	bool "MSM debug ONCRPC router support"
	help
	  Support for debugging the ONCRPC router for communication
	  between the ARM9 and ARM11

config MSM_RPC_LOOPBACK_XPRT
	depends on MSM_ONCRPCROUTER
	default n
	bool "MSM RPC local routing support"
	help
	  Support for routing RPC messages between APPS clients
	  and APPS servers.  Helps in testing APPS RPC framework.

config MSM_RPCSERVER_TIME_REMOTE
	depends on MSM_ONCRPCROUTER && RTC_HCTOSYS
	default y
	bool "Time remote RPC server"
	help
	  The time remote server receives notification of time bases and
	  reports these events to registered callback functions.

config MSM_RPCSERVER_WATCHDOG
	depends on MSM_ONCRPCROUTER
	default y
	bool "Watchdog RPC server"
	help
	  The dog_keepalive server handles watchdog events.

config MSM_RPC_WATCHDOG
	depends on MSM_ONCRPCROUTER
	default n
	bool "Watchdog RPC client"
	help
	  The dog_keepalive client module.

config MSM_RPC_PING
	depends on MSM_ONCRPCROUTER && DEBUG_FS
	default m
	bool "MSM rpc ping"
	help
	  Implements MSM rpc ping test module.

config MSM_RPC_PROC_COMM_TEST
	depends on DEBUG_FS && MSM_PROC_COMM
	default m
	bool "MSM rpc proc comm test"
	help
	  Implements MSM rpc proc comm test module.

config MSM_RPC_OEM_RAPI
	depends on MSM_ONCRPCROUTER
	default m
	bool "MSM oem rapi"
	help
	  Implements MSM oem rapi client module.

config MSM_RPCSERVER_HANDSET
	depends on MSM_ONCRPCROUTER
	default y
	bool "Handset events RPC server"
	help
	  Support for receiving handset events like headset detect,
	  headset switch and clamshell state.

config MSM_RMT_STORAGE_CLIENT
	depends on (ARCH_MSM && MSM_ONCRPCROUTER)
	default n
	bool "Remote Storage RPC client"
	help
	  Provide RPC mechanism for remote processors to access storage
	  device on apps processor.

config MSM_RMT_STORAGE_CLIENT_STATS
	depends on (MSM_RMT_STORAGE_CLIENT && DEBUG_FS)
	default n
	bool "Remote storage RPC client performance statistics"
	help
	  Collects performance statistics and shows this information
	  through a debugfs file rmt_storage_stats.

config MSM_SDIO_SMEM
        depends on MSM_SDIO_AL
        default n
        bool "SDIO SMEM for remote storage"
        help
          Copies data from remote MDM9K memory to local MSM8x60
	  memory. Used by remote storage client to shadow
	  MDM9K filesystem.

config MSM_DALRPC
	bool "DAL RPC support"
	default n
	help
	  Supports RPC calls to DAL devices on remote processor cores.

config MSM_DALRPC_TEST
	tristate "DAL RPC test module"
	depends on (MSM_DALRPC && DEBUG_FS)
	default m
	help
	  Exercises DAL RPC calls to QDSP6.
	  
config MSM_MPDEC
	bool "Enable kernel based mpdecision"
	depends on MSM_SMP
	default n
	help
	  This enables kernel based multi core control.
	  (up/down hotplug based on load)

config CONFIG_GPU_OC
	bool "GPU OC Freq's"
	default y
	help
	  foobar

if CPU_FREQ_MSM

config MSM_CPU_FREQ_SET_MIN_MAX
	bool "Set Min/Max CPU frequencies."
	default n
	help
	  Allow setting min and max CPU frequencies. Sysfs can be used
	  to override these values.

config MSM_CPU_FREQ_MAX
	int "Max CPU Frequency"
	depends on MSM_CPU_FREQ_SET_MIN_MAX
	default 384000

config MSM_CPU_FREQ_MIN
	int "Min CPU Frequency"
	depends on MSM_CPU_FREQ_SET_MIN_MAX
	default 245760

endif # CPU_FREQ_MSM

config MSM_CPU_AVS
	bool "Enable software controlled Adaptive Voltage Scaling (AVS)"
	depends on (ARCH_MSM_SCORPION && QSD_SVS)
	depends on ARCH_QSD8X50
	default n
	select MSM_AVS_HW
	help
	  This enables the s/w control of Adaptive Voltage Scaling feature
	  in Qualcomm ARMv7 CPUs. It adjusts the voltage for each frequency
	  based on feedback from three ring oscillators in the CPU.

config MSM_AVS_HW
	bool "Enable Adaptive Voltage Scaling (AVS)"
	default n
	help
	  Enable AVS hardware to fine tune voltage at each frequency. The
	  AVS hardware blocks associated with each Qualcomm ARMv7 cores can
	  fine tune the voltages based on the feedback from the ring
	  oscillators.

config MSM_HW3D
	tristate "MSM Hardware 3D Register Driver"
	depends on ANDROID_PMEM
	default y
	help
	  Provides access to registers needed by the userspace OpenGL|ES
	  library.

config MSM_ADSP
	depends on (ARCH_MSM7X01A || ARCH_MSM7X25 || ARCH_MSM7X27)
	tristate "MSM ADSP driver"
	depends on ANDROID_PMEM
	default y
	help
	  Provides access to registers needed by the userspace aDSP library.

config ADSP_RPC_VER
	hex
	default 0x30002 if (ARCH_MSM7X27 || (ARCH_MSM7X25 && AMSS_7X25_VERSION_2009))
	default 0x30001 if (ARCH_MSM7X01A || (ARCH_MSM7X25 && AMSS_7X25_VERSION_2008))
	depends on MSM_ADSP
	help
	 Select proper ADSP RPC version
choice
	prompt "ADSP RPC version"

	default AMSS_7X25_VERSION_2009

	config AMSS_7X25_VERSION_2009
		bool "2.0.09"

	config AMSS_7X25_VERSION_2008
		bool "2.0.08"
endchoice

config MSM7KV2_AUDIO
	bool "MSM7K v2 audio"
	depends on (ARCH_MSM7X30 && ANDROID_PMEM)
	default y
	help
	  Enables QDSP5V2-based audio drivers for audio playbacks and
	  voice call.

config MSM_ADSP_REPORT_EVENTS
	bool "Report modem events from the DSP"
	default y
	depends on (MSM_ADSP || MSM7KV2_AUDIO)
	help
	  Normally, only messages from the aDSP are reported to userspace.
	  With this option, we report events from the aDSP as well.

config MSM_QDSP6
	tristate "QDSP6 support"
	depends on ARCH_QSD8X50 && ANDROID_PMEM
	default y
	help
	  Enable support for qdsp6. This provides audio and video functionality.

config MSM8X60_AUDIO
	tristate "MSM8X60 audio support"
	depends on ARCH_MSM8X60 && ANDROID_PMEM
	default y
	help
	  Enable support for qdsp6v2. This provides audio functionality.

config MSM8X60_FTM_AUDIO_DEVICES
	bool "MSM8X60 audio factory test mode support"
	depends on MSM8X60_AUDIO
	help
	  Enable support audio factory test mode devices.  This is used
	  in a production line environment.

config RTAC
	bool "MSM8K real-time audio calibration support"
	default y
	help
	  Enable support for rtac. This enables calibration during
	  audio operation

config MSM7X27A_AUDIO
	bool "MSM7X27A audio support"
	depends on ARCH_MSM7X27A && MSM_ADSP
	default n
	help
	  Enable support for 7x27a. This provides audio functionality.

config MSM_PROC_COMM_REGULATOR
	bool
	depends on MSM_PROC_COMM && REGULATOR
	help
	  Enable regulator framework support for regulators managed by PMLIB
	  on the modem, and controlled through proccomm calls.

config MSM_VREG_SWITCH_INVERTED
	bool "Reverse vreg switch polarity"
	default n
	help
	  Reverses the enable and disable for vreg switch.

config MSM_DMA_TEST
	tristate "MSM DMA test module"
	default m
	help
	  Intended to be compiled as a module.  Provides a device node
	  and ioctls for testing the MSM dma system.

config WIFI_CONTROL_FUNC
	bool "Enable WiFi control function abstraction"
	help
	  Enables Power/Reset/Carddetect function abstraction

config WIFI_MEM_PREALLOC
	depends on WIFI_CONTROL_FUNC
	bool "Preallocate memory for WiFi buffers"
	help
	  Preallocates memory buffers for WiFi driver

config QSD_AUDIO
	bool "QSD audio"
	depends on ARCH_MSM_SCORPION && MSM_DALRPC && ANDROID_PMEM && !MSM_SMP
	default y
	help
	  Provides PCM, MP3, and AAC audio playback.

config AUDIO_AAC_PLUS
	depends on (MSM_ADSP || QSD_AUDIO || MSM7KV2_AUDIO)
	bool "AAC+ Audio"
	default y
	help
	  Provides AAC+ decoding

config AUDIO_ENHANCED_AAC_PLUS
	depends on AUDIO_AAC_PLUS
	bool "Enhanced AAC+ Audio"
	default y
	help
	  Provides Enhanced AAC+ decoding

config SURF_FFA_GPIO_KEYPAD
	bool "MSM SURF/FFA GPIO keypad"
	depends on INPUT_GPIO = "y"
	default y
	help
	  Select if the GPIO keypad is attached.

config MSM_SLEEP_TIME_OVERRIDE
	bool "Allow overriding suspend/sleep time with PM module parameter"
	default y
	help
	  Enable the module parameter sleep_time_override. Specified
	  in units of seconds, it overwrites the normal sleep time of
	  suspend. The feature is required for automated power management
	  testing.

config MSM_MEMORY_LOW_POWER_MODE
	bool "Control the low power modes of memory"
	default n
	help
	  The application processor controls whether memory should enter
	  which low power mode.

choice
	prompt "Default Memory Low Power Mode during Idle"
	depends on MSM_MEMORY_LOW_POWER_MODE
	default MSM_MEMORY_LOW_POWER_MODE_IDLE_ACTIVE
	help
	  Selects the default low power mode of the memory during idle
	  sleep.

	config MSM_MEMORY_LOW_POWER_MODE_IDLE_ACTIVE
		bool "Memory active"

	config MSM_MEMORY_LOW_POWER_MODE_IDLE_RETENTION
		bool "Memory in retention"

	config MSM_MEMORY_LOW_POWER_MODE_IDLE_DEEP_POWER_DOWN
		bool "Memory in deep power down"
endchoice

choice
	prompt "Default Memory Low Power Mode during Suspend"
	depends on MSM_MEMORY_LOW_POWER_MODE
	default MSM_MEMORY_LOW_POWER_MODE_SUSPEND_ACTIVE
	help
	  Selects the default low power mode of the memory during suspend
	  sleep.

	config MSM_MEMORY_LOW_POWER_MODE_SUSPEND_ACTIVE
		bool "Memory active"

	config MSM_MEMORY_LOW_POWER_MODE_SUSPEND_RETENTION
		bool "Memory in retention"

	config MSM_MEMORY_LOW_POWER_MODE_SUSPEND_DEEP_POWER_DOWN
		bool "Memory in deep power down"
endchoice

choice
	prompt "Power management timeout action"
	default MSM_PM_TIMEOUT_HALT
	help
	  Selects the Application Processor's action when Power Management
	  times out waiting for Modem's handshake.

	config MSM_PM_TIMEOUT_HALT
		bool "Halt the Application Processor"

	config MSM_PM_TIMEOUT_RESET_MODEM
		bool "Reset the Modem Processor"

	config MSM_PM_TIMEOUT_RESET_CHIP
		bool "Reset the entire chip"
endchoice

config MSM_IDLE_WAIT_ON_MODEM
	int "Wait for Modem to become ready for idle power collapse"
	default 0
	help
	  If Modem is not ready to handle Application Processor's request
	  for idle power collapse, wait the number of microseconds in case
	  Modem becomes ready soon.

config MSM_RPM_REGULATOR
	bool "RPM regulator driver"
	depends on MSM_RPM && REGULATOR
	help
	  Compile in support for the RPM regulator driver, used for setting
	  voltages and other parameters of the various power rails supplied
	  by some Qualcomm PMICs.

config MSM_PIL
	bool "Peripheral image loading"
	select FW_LOADER
	depends on (ARCH_MSM8X60 || ARCH_MSM8960)
	default n
	help
	  Some peripherals need to be loaded into memory before they can be
	  brought out of reset.

	  Say yes to support these devices.

config MSM_PIL_MODEM
	tristate "Modem (ARM11) Boot Support"
	depends on MSM_PIL
	help
	  Support for booting and shutting down ARM11 Modem processors.

config MSM_PIL_QDSP6V3
	tristate "QDSP6v3 (Hexagon) Boot Support"
	depends on MSM_PIL
	help
	  Support for booting and shutting down QDSP6v3 processors (hexagon).
	  The QDSP6 is a low power DSP used in audio software applications.

config MSM_PIL_QDSP6V4
	tristate "QDSP6v4 (Hexagon) Boot Support"
	depends on MSM_PIL
	help
	  Support for booting and shutting down QDSP6v4 processors (hexagon).
	  The QDSP6 is a low power DSP used in audio, modem firmware, and modem
	  software applications.

config MSM_PIL_RIVA
	tristate "RIVA (WCNSS) Boot Support"
	depends on MSM_PIL
	help
	  Support for booting and shutting down the RIVA processor (WCNSS).
	  Riva is the wireless subsystem processor used in bluetooth, wireless
	  LAN, and FM software applications.

config MSM_PIL_TZAPPS
	tristate "TZApps Boot Support"
	depends on MSM_PIL
	help
	  Support for booting and shutting down TZApps.

	  TZApps is an image that runs in the secure processor state. It is
	  used to decrypt data and perform secure operations on the behalf of
	  the kernel.

config MSM_PIL_VIDC
	tristate "Video Core Boot Support"
	depends on MSM_PIL
	help
	  Support for authenticating the video core image.

config MSM_SCM
	bool "Secure Channel Manager (SCM) support"
	default n

config MSM_SUBSYSTEM_RESTART
	bool "MSM Subsystem Restart Driver"
	depends on (ARCH_MSM8X60 || ARCH_MSM8960 || ARCH_MSM9615)
	default n
	help
	  This option enables the MSM subsystem restart driver, which provides
	  a framework to handle subsystem crashes.

config MSM_SYSMON_COMM
	bool "MSM System Monitor communication support"
	depends on MSM_SMD && MSM_SUBSYSTEM_RESTART
	default y
	help
	  This option adds support for MSM System Monitor library, which
	  provides an API that may be used for notifying subsystems within
	  the SoC about other subsystems' power-up/down state-changes.

config MSM_MODEM_8960
	bool "MSM 8960 Modem driver"
	depends on (ARCH_MSM8960 || ARCH_MSM9615)
	help
	 This option enables the modem driver for the MSM8960 and MSM9615, which monitors
	 modem hardware watchdog interrupt lines and plugs into the subsystem
	 restart and PIL drivers. For MSM9615, it only supports a full chip reset.

config MSM_LPASS_8960
	tristate "MSM 8960 Lpass driver"
	depends on (ARCH_MSM8960 || ARCH_MSM9615)
	help
	 This option enables the lpass driver for the MSM8960 and MSM9615. This monitors
	 lpass hardware watchdog interrupt lines and plugs into the subsystem
	 restart and PIL drivers. For MSM9615, it only supports a full chip reset.

config MSM_WCNSS_SSR_8960
	tristate "MSM 8960 WCNSS restart module"
	depends on (ARCH_MSM8960)
	help
	 This option enables the WCNSS restart module for MSM8960, which
	 monitors WCNSS hardware watchdog interrupt lines and plugs WCNSS
	 into the subsystem restart framework.

config SCORPION_Uni_45nm_BUG
	bool "Scorpion Uni 45nm(SC45U): Workaround for ICIMVAU and BPIMVA"
	depends on ARCH_MSM7X30 || (ARCH_QSD8X50 && MSM_SOC_REV_A)
	default y
	help
	  Invalidating the Instruction Cache by Modified Virtual Address to PoU and
	  invalidating the Branch Predictor Array by Modified Virtual Address can
	  create invalid entries in the TLB with the wrong ASID values on Scorpion
	  Uniprocessor 45nm (SC45U) cores. This option enables the recommended software
	  workaround for Scorpion Uniprocessor 45nm cores.

	  This bug is not applicable to any ScorpionMP or Scorpion Uni 65nm(SC65U) cores.

config MSM_BUSPM_DEV
	tristate "MSM Bus Performance Monitor Kernel Module"
	depends on (ARCH_MSM8X60 || ARCH_MSM8960)
	default m
	help
	  This kernel module is used to mmap() hardware registers for the
	  performance monitors, counters, etc. The module can also be used to
	  allocate physical memory which is used by bus performance hardware to
	  dump performance data.

config MSM_TZ_LOG
	tristate "MSM Trust Zone (TZ) Log Driver"
	depends on DEBUG_FS
	help
	  This option enables a driver with a debugfs interface for messages
	  produced by the Secure code (Trust zone). These messages provide
	  diagnostic information about TZ operation.

config MSM_RPM_LOG
	tristate "MSM Resource Power Manager Log Driver"
	depends on DEBUG_FS
	depends on MSM_RPM
	default n
	help
	  This option enables a driver which can read from a circular buffer
	  of messages produced by the RPM. These messages provide diagnostic
	  information about RPM operation. The driver outputs the messages
	  via a debugfs node.

config MSM_RPM_STATS_LOG
	tristate "MSM Resource Power Manager Stat Driver"
	depends on DEBUG_FS
	depends on MSM_RPM
	default n
	  help
	  This option enables a driver which reads RPM messages from a shared
	  memory location. These messages provide statistical information about
	  the low power modes that RPM enters. The drivers outputs the message
	  via a debugfs node.

config MSM_IOMMU
	bool "MSM IOMMU Support"
	depends on ARCH_MSM8X60 || ARCH_MSM8960 || ARCH_APQ8064
	select IOMMU_API
	default n
	help
	  Support for the IOMMUs found on certain Qualcomm SOCs.
	  These IOMMUs allow virtualization of the address space used by most
	  cores within the multimedia subsystem.

	  If unsure, say N here.

config IOMMU_PGTABLES_L2
	bool "Allow SMMU page tables in the L2 cache (Experimental)"
	depends on MSM_IOMMU=y
	depends on MMU
	depends on CPU_DCACHE_DISABLE=n
	depends on SMP
	default y
	help
	 Improves TLB miss latency at the expense of potential L2 pollution.
	 However, with large multimedia buffers, the TLB should mostly contain
	 section mappings and TLB misses should be quite infrequent.
	 Most people can probably say Y here.

config MSM_DIRECT_SCLK_ACCESS
	bool "Direct access to the SCLK timer"
	default n

config IOMMU_API
       bool

config MSM_GPIOMUX
	bool

config MSM_V2_TLMM
       bool

config FSM9XXX_TLMM
	bool

config MSM_SECURE_IO
	bool

config MSM_NATIVE_RESTART
	bool

config MSM_PM
	depends on PM
	bool

config MSM_PM2
	depends on PM
	bool

config MSM_PM8X60
	depends on PM
	bool

config MSM_NOPM
	default y if !PM
	bool

config MSM_BUS_SCALING
	bool "Bus scaling driver"
	default n

config MSM_BUS_RPM_MULTI_TIER_ENABLED
	bool "RPM Multi-tiering Configuration"
	depends on MSM_BUS_SCALING

config MSM_WATCHDOG
	bool "MSM Watchdog Support"
	depends on ARCH_MSM8X60 || ARCH_MSM8960 || ARCH_MSM9615
	help
		This enables the watchdog as is present on 8x60. Currently we use
		core 0's watchdog, and reset the entire SoC if it times out. It does
		not run during the bootup process, so it will not catch any early
		lockups.

config MSM_DLOAD_MODE
	bool "Enable download mode on crashes"
	depends on ARCH_MSM8X60 || ARCH_MSM8960 || ARCH_MSM9615
	default n
	help
		This makes the SoC enter download mode when it resets
		due to a kernel panic. Note that this doesn't by itself
		make the kernel reboot on a kernel panic - that must be
		enabled via another mechanism.

config MSM_JTAG
        bool "JTAG debug and trace support"
	help
          Add additional support for JTAG kernel debugging and tracing.

config MSM_ETM
	tristate "Enable MSM ETM and ETB"
	depends on ARCH_MSM8X60
	select MSM_JTAG
	help
	  Enables embedded trace collection on MSM8660

config MSM_QDSS
	bool "Qualcomm Debug Subsystem"
	select MSM_JTAG
	help
	  Enables support for Qualcomm Debug Subsystem.

config MSM_QDSS_ETM_DEFAULT_ENABLE
	bool "Turn on QDSS ETM Tracing by Default"
	depends on MSM_QDSS
	help
	  Turns on QDSS ETM tracing by default. Otherwise, tracing is
	  disabled by default but can be enabled by other means.

config CLOCK_MAP
	bool "Log clk_enable()/clk_disable() calls"
	help
	  Turn on extra debugging for clk_enable()/clk_disable() calls.
	  A bitmap is maintained in non-cached memory and each registered
	  clock gets a unique id. If the bitmap has that bit set the clock
	  is enabled, otherwise the clock is disabled.

config MSM_SLEEP_STATS
	bool "Enable exporting of MSM sleep stats to userspace"
	depends on CPU_IDLE
	default n

config MSM_SLEEP_STATS_DEVICE
	bool "Enable exporting of MSM sleep device stats to userspace"

config MSM_STANDALONE_POWER_COLLAPSE
       bool "Enable standalone power collapse"
       default n

config MSM_GSBI9_UART
	bool "Enable GSBI9 UART device"
	default n
	help
	  This enables GSBI9 configured into UART.

config MSM_SHARED_GPIO_FOR_UART2DM
	bool "Use shared GPIOs into UART mode"
	depends on (ARCH_MSM7X27A && !MMC_MSM_SDC3_8_BIT_SUPPORT && !MMC_MSM_SDC4_SUPPORT)
	help
	  This option configures GPIO muxed with SDC4/MMC3
	  8-bit mode into UART mode. It is used for serial
	  console on UART2DM. Say Y if you want to have
	  serial console on UART2DM.

config MSM_SHOW_RESUME_IRQ
	bool "Enable logging of interrupts that could have caused resume"
	depends on (ARM_GIC || PMIC8058)
	default y if PMIC8058
	default n
	help
		This option logs wake up interrupts that have triggered just before
		the resume loop unrolls. Say Y if you want to debug why the system
		resumed.

config BT_MSM_PINTEST
	tristate "MSM Bluetooth Pin Connectivity Test"
	depends on ((ARCH_MSM8X60 || ARCH_MSM7X27A) && DEBUG_FS)
        default n
	help
	  Bluetooth MSM Pin Connectivity test module.
	  This driver provides support for verifying the MSM to BT pin
	  connectivity.

config MSM_FAKE_BATTERY
		depends on POWER_SUPPLY
		default n
		bool "MSM Fake Battery"
		help
		 Enables MSM fake battery driver.
		 
config ZTE_BATTERY_DANA_4200MV_1650MAH
		default n
		bool "ZTE Battery type1"
		help
		 Enables ZTE BMS driver.		
		 
config ZTE_BATTERY_GORDON_4350MV_1780MAH
		default n
		bool "ZTE Battery type2"
		help
		 Enables ZTE BMS driver.	
		 
config ZTE_BATTERY_ELDEN_4350MV_1735MAH
		default n
		bool "ZTE Battery type3"
		help
		 Enables ZTE BMS driver.
		 
config ZTE_BATTERY_ILIAMNA_4350MV_1735MAH
		default n
		bool "ZTE Battery type4"
		help
		 Enables ZTE BMS driver.	

config ZTE_BATTERY_FROSTY_4200MV_1900MAH
		default n
		bool "ZTE Battery type5"
		help
		 Enables ZTE BMS driver. 

config ZTE_BATTERY_HAYES_4350MV_1735MAH
		default n
		bool "ZTE Battery type5"
		help
		 Enables ZTE BMS driver. 
		 
config ZTE_BATTERY_KISKA_4350MV_1735MAH
		default n
		bool "ZTE Battery type5"
		help
		 Enables ZTE BMS driver. 		 

# CONFIG_ZTE_NON_JEITA_COMPLIANCE
config ZTE_NON_JEITA_COMPLIANCE
		default n
		bool "ZTE non JEITA compliance"
		help
		 non JEITA complianc. 

config MSM_QDSP6_APR
	bool "Audio QDSP6 APR support"
	depends on MSM_SMD
	default n
	help
	  Enable APR IPC protocol support between
	  application processor and QDSP6. APR is
	  used by audio driver to configure QDSP6's
	  ASM, ADM and AFE.


config MSM_AUDIO_QDSP6
        bool "QDSP6 HW Audio support"
        select SND_SOC_MSM_QDSP6_INTF
        default n
        help
          Enable HW audio support in QDSP6.
          QDSP6 can support HW encoder & decoder and audio processing

config MSM_ULTRASOUND
	bool "MSM ultrasound support"
	depends on MSM_AUDIO_QDSP6
	help
	  Enable support for qdsp6/ultrasound.

config MSM_RPC_VIBRATOR
	bool "RPC based MSM Vibrator Support"
	depends on MSM_ONCRPCROUTER
	help
	  Enable the vibrator support on MSM over RPC. The vibrator
	  is connected on the PMIC. Say Y if you want to enable this
	  feature.

config PM8XXX_RPC_VIBRATOR
	bool "RPC based Vibrator on PM8xxx PMICs"
	depends on MSM_RPC_VIBRATOR
	help
	  Enable the vibrator support on MSM over RPC. The vibrator
	  is connected on the PM8XXX PMIC. Say Y if you want to enable
	  this feature.

config MSM_SPM_V1
	bool "Driver support for SPM Version 1"
	help
	  Enables the support for Version 1 of the SPM driver. SPM hardware is
	  used to manage the processor power during sleep. The driver allows
	  configuring SPM to allow different power modes.

config MSM_SPM_V2
	bool "Driver support for SPM Version 2"
	help
	  Enables the support for Version 2 of the SPM driver. SPM hardware is
	  used to manage the processor power during sleep. The driver allows
	  configuring SPM to allow different power modes.

config MSM_L2_SPM
	bool "SPM support for L2 cache"
	depends on MSM_SPM_V2
	help
	  Enable SPM driver support for L2 cache. Some MSM chipsets allow
	  control of L2 cache low power mode with a Subsystem Power manager.
	  Enabling this driver allows configuring L2 SPM for low power modes
	  on supported chipsets.

config MSM_MULTIMEDIA_USE_ION
	bool "Multimedia suport using Ion"
	depends on ION_MSM
	help
	  Enable support for multimedia drivers using Ion for buffer management
	  instead of pmem. Selecting this may also involve userspace
	  dependencies as well.

config MSM_RTB
	bool "Register tracing"
	help
	  Add support for logging different events to a small uncached
	  region. This is designed to aid in debugging reset cases where the
	  caches may not be flushed before the target resets.

config MSM_RTB_SEPARATE_CPUS
	bool "Separate entries for each cpu"
	depends on MSM_RTB
	help
	  Under some circumstances, it may be beneficial to give dedicated space
	  for each cpu to log accesses. Selecting this option will log each cpu
	  separately. This will guarantee that the last acesses for each cpu
	  will be logged but there will be fewer entries per cpu

config MSM_CACHE_ERP
	bool "Cache / CPU error reporting"
	depends on ARCH_MSM_KRAIT
	help
	  Say 'Y' here to enable reporting of cache and TLB errors to the kernel
	  log. Enabling this feature can be used as a system debugging technique
	  if cache corruption is suspected. Cache error statistics will also be
	  reported in /proc/cpu/msm_cache_erp.

	  For production builds, you should probably say 'N' here.

config MSM_L1_ERR_PANIC
	bool "Panic on L1 cache errors"
	depends on MSM_CACHE_ERP
	help
	To cause the kernel to panic whenever an L1 cache error is detected, say
	'Y' here. This may be useful as a debugging technique if general system
	instability is suspected.

	  For production builds, you should probably say 'N' here.
	  
config MSM_L1_ERR_LOG
	bool "Log CPU ERP events to system memory"
	depends on MSM_CACHE_ERP
	help
	  Enable logging CPU ERP events to an area of memory that will be
	  preserved across a system reset. This may be useful for detecting and
	  troubleshooting ERP-related system crashes in the field.

	  For production builds, you may want to say 'Y' here.
	  

config MSM_L2_ERP_PRINT_ACCESS_ERRORS
	bool "Report L2 master port slave/decode errors in kernel log"
	depends on MSM_CACHE_ERP
	help
	  Master port errors can occur when a memory request is not properly
	  handled by the destination slave. This can occur if the destination
	  register does not exist or is inaccessible due to security
	  restrictions or (in some cases) clock configuration. Enabling this
	  option will cause a backtrace to be printed to the kernel log whenever
	  such an error is encountered. Note that the error is reported as an
	  interrupt rather than as an exception, meaning that the backtrace may
	  have some skid. This option may help with debugging, though production
	  builds should probably say 'N' here.
config MSM_L1_ERR_LOG
	bool "Log CPU ERP events to system memory"
	depends on MSM_CACHE_ERP
	help
	  Enable logging CPU ERP events to an area of memory that will be
	  preserved across a system reset. This may be useful for detecting and
	  troubleshooting ERP-related system crashes in the field.

	  For production builds, you may want to say 'Y' here.

config MSM_L2_ERP_PORT_PANIC
	bool "Panic on L2 master port errors"
	depends on MSM_CACHE_ERP && MSM_L2_ERP_PRINT_ACCESS_ERRORS
	help
	  Master port errors can occur when a memory request is not properly
	  handled by the destination slave. Enable this option to catch drivers
	  which attempt to access bad areas of the address space, or access
	  hardware registers in an improper state (such as certain clocks not
	  being on). This option may help with debugging, though production
	  builds should probably say 'N' here.

config MSM_L2_ERP_1BIT_PANIC
	bool "Panic on recoverable L2 soft errors"
	depends on MSM_CACHE_ERP
	help
	  Enable this option to cause a kernel panic whenever the L2 cache
	  encounters a single-bit (correctable) soft error. This option should
	  only be enabled when doing low-level debugging where cache corruption
	  is suspected.

	  For production builds, you should definitely say 'N' here.

config MSM_L2_ERP_2BIT_PANIC
	bool "Panic on unrecoverable L2 soft errors"
	depends on MSM_CACHE_ERP
	help
	  Enable this option to cause a kernel panic whenever the L2 cache
	  encounters a double-bit (non-correctable) soft error. Debug builds
	  will likely benefit from having this option enabled to catch cache
	  problems as soon as possible.

	  For production builds, it may be acceptable to say 'N' here, since
	  an uncorrectable error might not necessarily cause further problems.
	  
config ZTE_ACPU_OVERCLOCK
	tristate "zte specify a custom voltage and frequency to overclock"
	default m
	help
	  supply a custom voltage and frequency to acpu overclock	  
	  
config ZTE_LONGPOWER_FOR_HW_RESET
	tristate "zte specify long power pressed for hw reset"
	default n
	help
	  supply a specify long power pressed for hw reset	  

config MSM_CACHE_DUMP
	bool "Cache dumping support"
	help
	  Add infrastructure to dump the L1 and L2 caches to an allocated buffer.
	  This allows for analysis of the caches in case cache corruption is
	  suspected.

config MSM_DCVS
	bool "Use MSM DCVS for CPU/GPU Frequency control"
	depends on MSM_SCM
	help
	  Enable support for MSM DCVS to control all CPU and GPU core frequencies.
	  The DCVS manager allows idle driver to feed the idle information to the
	  algorithm and the algorithm returns a frequency for the core which is
	  passed to the frequency change driver.

config MSM_CACHE_DUMP
	bool "Cache dumping support"
	help
	  Add infrastructure to dump the L1 and L2 caches to an allocated buffer.
	  This allows for analysis of the caches in case cache corruption is
	  suspected.

config MSM_CACHE_DUMP_ON_PANIC
	bool "Dump caches on panic"
	depends on MSM_CACHE_DUMP
	help
	  By default, the caches are flushed on panic. This means that trying to
	  look at them in a RAM dump will give useless data. Select this if you
	  want to dump the L1 and L2 caches on panic before any flush occurs.
	  If unsure, say N

endif
                                                                                                                                                                                                                                                                                          files/Makefile                                                                                      0000664 0001756 0001756 00000032636 12214721673 014415  0                                                                                                    ustar   rbheromax                       rbheromax                                                                                                                                                                                                              obj-y += io.o dma.o memory.o
ifndef CONFIG_ARM_ARCH_TIMER
obj-y += timer.o
endif
obj-y += clock.o clock-voter.o clock-dummy.o
obj-y += modem_notifier.o subsystem_map.o
obj-$(CONFIG_CPU_FREQ_MSM) += cpufreq.o
obj-$(CONFIG_MSM_MPDEC) += msm_mpdecision.o
obj-$(CONFIG_DEBUG_FS) += nohlt.o clock-debug.o
obj-$(CONFIG_KEXEC) += msm_kexec.o

obj-$(CONFIG_MSM_PROC_COMM) += proc_comm.o
ifndef CONFIG_ARCH_MSM8X60
	obj-$(CONFIG_MSM_PROC_COMM) += clock-pcom.o
	obj-$(CONFIG_MSM_PROC_COMM) += vreg.o mpp.o
	ifdef CONFIG_MSM_PROC_COMM
ifndef CONFIG_ARCH_FSM9XXX
		obj-$(CONFIG_REGULATOR) += footswitch-pcom.o
endif
		obj-$(CONFIG_DEBUG_FS) += pmic_debugfs.o
	endif
endif

obj-y += acpuclock.o
obj-$(CONFIG_ARCH_MSM7X01A) += acpuclock-7201.o
obj-$(CONFIG_ARCH_MSM7X25) += acpuclock-7201.o
obj-$(CONFIG_ARCH_MSM7X27) += acpuclock-7201.o
obj-$(CONFIG_ARCH_MSM_SCORPION) += pmu.o
obj-$(CONFIG_ARCH_MSM_KRAIT) += msm-krait-l2-accessors.o pmu.o
obj-$(CONFIG_ARCH_MSM7X27A) += pmu.o

ifndef CONFIG_MSM_SMP
obj-$(CONFIG_ARCH_MSM_SCORPION) += msm_fault_handlers.o
endif

obj-$(CONFIG_MSM_VIC) += irq-vic.o

ifdef CONFIG_ARCH_QSD8X50
	obj-$(CONFIG_MSM_SOC_REV_NONE) += acpuclock-8x50.o
endif

obj-$(CONFIG_SMP) += headsmp.o platsmp.o
obj-$(CONFIG_HOTPLUG_CPU) += hotplug.o

obj-$(CONFIG_MSM_CPU_AVS) += avs.o
obj-$(CONFIG_MSM_AVS_HW) += avs_hw.o
obj-$(CONFIG_CPU_V6) += idle-v6.o
obj-$(CONFIG_CPU_V7) += idle-v7.o
obj-$(CONFIG_MSM_JTAG) += jtag.o
obj-$(CONFIG_ZTE_ACPU_OVERCLOCK)		+= zte_krait_oc.o

msm-etm-objs := etm.o
obj-$(CONFIG_MSM_ETM) += msm-etm.o
obj-$(CONFIG_MSM_QDSS) += qdss.o qdss-etb.o qdss-tpiu.o qdss-funnel.o qdss-etm.o

quiet_cmd_mkrpcsym = MKCAP   $@
      cmd_mkrpcsym = $(PERL) $(srctree)/$(src)/mkrpcsym.pl $< $@

target += smd_rpc_sym.c
$(obj)/smd_rpc_sym.c: $(src)/smd_rpc_sym $(src)/mkrpcsym.pl
	$(call if_changed,mkrpcsym)

obj-$(CONFIG_MSM_SCM) += scm.o scm-boot.o
obj-$(CONFIG_MSM_SECURE_IO) += scm-io.o
obj-$(CONFIG_MSM_PIL) += peripheral-loader.o
obj-$(CONFIG_MSM_PIL) += scm-pas.o
ifdef CONFIG_MSM_PIL
obj-$(CONFIG_ARCH_MSM8X60) += peripheral-reset.o
obj-$(CONFIG_ARCH_MSM8960) += peripheral-reset-8960.o
endif
obj-$(CONFIG_MSM_PIL_QDSP6V3) += pil-q6v3.o
obj-$(CONFIG_MSM_PIL_QDSP6V4) += pil-q6v4.o
obj-$(CONFIG_MSM_PIL_RIVA) += pil-riva.o
obj-$(CONFIG_MSM_PIL_TZAPPS) += pil-tzapps.o
obj-$(CONFIG_MSM_PIL_VIDC) += pil-vidc.o
obj-$(CONFIG_MSM_PIL_MODEM) += pil-modem.o
obj-$(CONFIG_ARCH_QSD8X50) += sirc.o
obj-$(CONFIG_ARCH_FSM9XXX) += sirc-fsm9xxx.o
obj-$(CONFIG_MSM_FIQ_SUPPORT) += fiq_glue.o
obj-$(CONFIG_MACH_TROUT) += board-trout-rfkill.o
obj-$(CONFIG_MSM_SDIO_AL) += sdio_al.o
obj-$(CONFIG_MSM_SDIO_AL) += sdio_al_test.o
obj-$(CONFIG_MSM_SDIO_AL) += sdio_al_dloader.o
obj-$(CONFIG_MSM_SDIO_DMUX) += sdio_dmux.o
obj-$(CONFIG_MSM_BAM_DMUX) += bam_dmux.o
obj-$(CONFIG_MSM_SMD_LOGGING) += smem_log.o
obj-$(CONFIG_MSM_SMD) += smd.o smd_debug.o remote_spinlock.o
obj-y += socinfo.o
ifndef CONFIG_ARCH_MSM9615
ifndef CONFIG_ARCH_APQ8064
ifndef CONFIG_ARCH_MSM8960
ifndef CONFIG_ARCH_MSM8X60
	obj-$(CONFIG_MSM_SMD) += pmic.o
	obj-$(CONFIG_MSM_ONCRPCROUTER) += rpc_hsusb.o rpc_pmapp.o rpc_fsusb.o
endif
endif
endif
endif
ifndef CONFIG_ARCH_MSM8960
ifndef CONFIG_ARCH_MSM8X60
ifndef CONFIG_ARCH_APQ8064
ifndef CONFIG_ARCH_MSMCOPPER
	obj-y += nand_partitions.o
endif
endif
endif
endif
obj-$(CONFIG_MSM_SDIO_TTY) += sdio_tty.o
obj-$(CONFIG_MSM_SMD_TTY) += smd_tty.o
obj-$(CONFIG_MSM_SMD_QMI) += smd_qmi.o
obj-$(CONFIG_MSM_SMD_PKT) += smd_pkt.o
obj-$(CONFIG_MSM_SDIO_CMUX) += sdio_cmux.o
obj-$(CONFIG_MSM_DSPS) += msm_dsps.o
obj-$(CONFIG_MSM_SDIO_CTL) += sdio_ctl.o
obj-$(CONFIG_MSM_SMD_NMEA) += smd_nmea.o
obj-$(CONFIG_MSM_RESET_MODEM) += reset_modem.o
obj-$(CONFIG_MSM_IPC_ROUTER_SMD_XPRT) += ipc_router_smd_xprt.o
obj-$(CONFIG_MSM_ONCRPCROUTER) += smd_rpcrouter.o
obj-$(CONFIG_MSM_ONCRPCROUTER) += smd_rpcrouter_device.o
obj-$(CONFIG_MSM_IPC_ROUTER) += ipc_router.o
obj-$(CONFIG_MSM_IPC_ROUTER)+= ipc_socket.o
obj-$(CONFIG_DEBUG_FS) += smd_rpc_sym.o
obj-$(CONFIG_MSM_ONCRPCROUTER) += smd_rpcrouter_servers.o
obj-$(CONFIG_MSM_ONCRPCROUTER) += smd_rpcrouter_clients.o
obj-$(CONFIG_MSM_ONCRPCROUTER) += smd_rpcrouter_xdr.o
obj-$(CONFIG_MSM_ONCRPCROUTER) += rpcrouter_smd_xprt.o
obj-$(CONFIG_MSM_RPC_SDIO_XPRT) += rpcrouter_sdio_xprt.o
obj-$(CONFIG_MSM_RPC_PING) += ping_mdm_rpc_client.o
obj-$(CONFIG_MSM_RPC_PROC_COMM_TEST) += proc_comm_test.o
obj-$(CONFIG_MSM_RPC_PING) += ping_mdm_rpc_client.o ping_apps_server.o
obj-$(CONFIG_MSM_RPC_OEM_RAPI) += oem_rapi_client.o
obj-$(CONFIG_MSM_RPC_WATCHDOG) += rpc_dog_keepalive.o
obj-$(CONFIG_MSM_RPCSERVER_WATCHDOG) += rpc_server_dog_keepalive.o
obj-$(CONFIG_MSM_RPCSERVER_TIME_REMOTE) += rpc_server_time_remote.o
obj-$(CONFIG_MSM_DALRPC) += dal.o
obj-$(CONFIG_MSM_DALRPC_TEST) += dal_remotetest.o
obj-$(CONFIG_ARCH_MSM7X30) += dal_axi.o
obj-$(CONFIG_ARCH_MSM7X27A) += dal_axi.o
obj-$(CONFIG_MSM_ADSP) += qdsp5/
obj-$(CONFIG_MSM7KV2_AUDIO) += qdsp5v2/
obj-$(CONFIG_MSM_RPCSERVER_HANDSET) += rpc_server_handset.o
obj-$(CONFIG_MSM_QDSP6) += qdsp6/
obj-$(CONFIG_MSM8X60_AUDIO) += qdsp6v2/
obj-$(CONFIG_MSM_AUDIO_QDSP6) += qdsp6v2/
obj-$(CONFIG_MSM_HW3D) += hw3d.o
obj-$(CONFIG_PM) += pm-boot.o
obj-$(CONFIG_MSM_PM8X60) += pm-8x60.o
obj-$(CONFIG_MSM_PM2) += pm2.o
obj-$(CONFIG_MSM_PM) += pm.o
obj-$(CONFIG_MSM_NOPM) += no-pm.o

obj-$(CONFIG_MSM_SPM_V1) += spm.o
obj-$(CONFIG_MSM_SPM_V2) += spm-v2.o spm_devices.o

obj-$(CONFIG_MSM_DMA_TEST) += dma_test.o
obj-$(CONFIG_SURF_FFA_GPIO_KEYPAD) += keypad-surf-ffa.o

obj-$(CONFIG_ARCH_MSM7X01A) += board-halibut.o devices-msm7x01a.o clock-pcom-lookup.o
obj-$(CONFIG_MACH_TROUT) += board-trout.o board-trout-gpio.o
obj-$(CONFIG_MACH_TROUT) += board-trout-keypad.o board-trout-panel.o
obj-$(CONFIG_MACH_TROUT) += htc_akm_cal.o htc_wifi_nvs.o htc_acoustic.o
obj-$(CONFIG_MACH_TROUT) += board-trout-mmc.o board-trout-wifi.o
obj-$(CONFIG_ARCH_QSD8X50) += devices-qsd8x50.o clock-pcom-lookup.o
obj-$(CONFIG_MACH_QSD8X50_SURF) += board-qsd8x50.o
obj-$(CONFIG_MACH_QSD8X50_FFA) += board-qsd8x50.o
obj-$(CONFIG_ARCH_MSM8X60) += devices-msm8x60.o clock-local.o clock-8x60.o acpuclock-8x60.o
obj-$(CONFIG_ARCH_MSM8X60) += clock-rpm.o
obj-$(CONFIG_ARCH_MSM8X60) += saw-regulator.o
obj-$(CONFIG_ARCH_MSM8X60) += footswitch-8x60.o

ifdef CONFIG_MSM_RPM_REGULATOR
obj-y += rpm-regulator.o
obj-$(CONFIG_ARCH_MSM8X60) += rpm-regulator-8660.o
obj-$(CONFIG_ARCH_MSM8960) += rpm-regulator-8960.o
obj-$(CONFIG_ARCH_MSM9615) += rpm-regulator-9615.o
endif

ifdef CONFIG_MSM_SUBSYSTEM_RESTART
	obj-y += subsystem_notif.o
	obj-y += subsystem_restart.o
	obj-y += ramdump.o
	obj-$(CONFIG_ARCH_MSM8X60) += modem-8660.o lpass-8660.o
endif
obj-$(CONFIG_MSM_SYSMON_COMM) += sysmon.o
obj-$(CONFIG_MSM_MODEM_8960) += modem-8960.o
obj-$(CONFIG_MSM_LPASS_8960) += lpass-8960.o
obj-$(CONFIG_MSM_WCNSS_SSR_8960) += wcnss-ssr-8960.o
obj-$(CONFIG_MSM_WCNSS_SSR_8960) += board-zte-wifi.o board-zte-ts.o

ifdef CONFIG_CPU_IDLE
	obj-$(CONFIG_ARCH_MSM8960) += cpuidle.o
	obj-$(CONFIG_ARCH_MSM8X60) += cpuidle.o
	obj-$(CONFIG_ARCH_MSM9615) += cpuidle.o
endif

ifdef CONFIG_MSM_CAMERA_V4L2
	obj-$(CONFIG_ARCH_MSM8X60) += board-msm8x60-camera.o
endif
obj-$(CONFIG_ARCH_FSM9XXX) += devices-fsm9xxx.o
obj-$(CONFIG_ARCH_FSM9XXX) += clock-fsm9xxx.o clock-local.o acpuclock-fsm9xxx.o
obj-$(CONFIG_ARCH_FSM9XXX) += dfe-fsm9xxx.o rfic-fsm9xxx.o
obj-$(CONFIG_ARCH_FSM9XXX) += restart-fsm9xxx.o xo-fsm9xxx.o

obj-$(CONFIG_MSM_WATCHDOG) += msm_watchdog.o
obj-$(CONFIG_MSM_WATCHDOG) += msm_watchdog_asm.o
obj-$(CONFIG_MACH_MSM8X60_RUMI3) += board-msm8x60.o
obj-$(CONFIG_MACH_MSM8X60_SIM) += board-msm8x60.o
obj-$(CONFIG_MACH_MSM8X60_SURF) += board-msm8x60.o
obj-$(CONFIG_MACH_MSM8X60_FFA) += board-msm8x60.o
obj-$(CONFIG_MACH_MSM8X60_FLUID) += board-msm8x60.o
obj-$(CONFIG_MACH_MSM8X60_DRAGON) += board-msm8x60.o
obj-$(CONFIG_MACH_TYPE_MSM8X60_FUSION) += board-msm8x60.o mdm.o
obj-$(CONFIG_MACH_MSM8X60_FUSN_FFA) += board-msm8x60.o mdm.o
obj-$(CONFIG_TROUT_H2W) += board-trout-h2w.o
obj-$(CONFIG_TROUT_BATTCHG) += htc_battery.o
obj-$(CONFIG_TROUT_PWRSINK) += htc_pwrsink.o
obj-$(CONFIG_ARCH_MSM7X27) += clock-pcom-lookup.o
obj-$(CONFIG_MACH_MSM7X27_SURF) += board-msm7x27.o devices-msm7x27.o
obj-$(CONFIG_MACH_MSM7X27_FFA) += board-msm7x27.o devices-msm7x27.o
obj-$(CONFIG_ARCH_MSM7X27A) += clock-pcom-lookup.o devices-msm7x27a.o
obj-$(CONFIG_MACH_MSM7X27A_RUMI3) += board-msm7x27a.o board-msm7627a-storage.o board-msm7627a-bt.o board-msm7627a-camera.o
obj-$(CONFIG_MACH_MSM7X27A_SURF) += board-msm7x27a.o board-msm7627a-storage.o board-msm7627a-bt.o board-msm7627a-camera.o
obj-$(CONFIG_MACH_MSM7X27A_FFA) += board-msm7x27a.o board-msm7627a-storage.o board-msm7627a-bt.o board-msm7627a-camera.o
obj-$(CONFIG_MACH_MSM7627A_QRD1) += board-qrd7627a.o board-msm7627a-storage.o board-msm7627a-bt.o board-msm7627a-camera.o
obj-$(CONFIG_ARCH_MSM7X30) += board-msm7x30.o devices-msm7x30.o memory_topology.o
obj-$(CONFIG_ARCH_MSM7X30) += clock-local.o clock-7x30.o acpuclock-7x30.o
obj-$(CONFIG_MACH_MSM7X25_SURF) += board-msm7x27.o devices-msm7x25.o
obj-$(CONFIG_MACH_MSM7X25_FFA) += board-msm7x27.o devices-msm7x25.o
obj-$(CONFIG_ARCH_MSM8960) += clock-local.o clock-dss-8960.o clock-8960.o clock-rpm.o
obj-$(CONFIG_ARCH_MSM8960) += footswitch-8x60.o
obj-$(CONFIG_ARCH_MSM8960) += acpuclock-8960.o
obj-$(CONFIG_ARCH_MSM8960) += memory_topology.o
obj-$(CONFIG_ARCH_MSM8960) += saw-regulator.o
obj-$(CONFIG_ARCH_MSM8960) += devices-8960.o
obj-$(CONFIG_ARCH_APQ8064) += devices-8960.o devices-8064.o

#board-8960-all-objs += board-8960.o board-8960-camera.o board-8960-display.o board-8960-pmic.o board-8960-storage.o board-8960-gpiomux.o
board-8960-all-objs += board-8960-zte.o board-8960-camera.o board-8960-display.o board-8960-pmic.o board-8960-storage.o board-8960-gpiomux.o

board-8930-all-objs += board-8930.o board-8930-camera.o board-8930-display.o board-8930-pmic.o board-8930-storage.o board-8930-gpiomux.o
board-8064-all-objs += board-8064.o board-8064-pmic.o board-8064-storage.o board-8064-gpiomux.o board-8064-camera.o
obj-$(CONFIG_MACH_MSM8960_SIM) += board-8960-all.o board-8960-regulator.o
obj-$(CONFIG_MACH_MSM8960_RUMI3) += board-8960-all.o board-8960-regulator.o
obj-$(CONFIG_MACH_MSM8960_CDP) += board-8960-all.o board-8960-regulator.o
obj-$(CONFIG_MACH_MSM8960_MTP) += board-8960-all.o board-8960-regulator.o
obj-$(CONFIG_MACH_MSM8960_FLUID) += board-8960-all.o board-8960-regulator.o
obj-$(CONFIG_MACH_MSM8930_CDP) += board-8930-all.o board-8930-regulator.o
obj-$(CONFIG_MACH_MSM8930_MTP) += board-8930-all.o board-8930-regulator.o
obj-$(CONFIG_MACH_MSM8930_FLUID) += board-8930-all.o board-8930-regulator.o
obj-$(CONFIG_PM8921_BMS) += bms-batterydata.o bms-batterydata-desay.o
obj-$(CONFIG_MACH_APQ8064_SIM) += board-8064-all.o board-8064-regulator.o
obj-$(CONFIG_MACH_APQ8064_RUMI3) += board-8064-all.o board-8064-regulator.o
obj-$(CONFIG_ARCH_MSM9615) += board-9615.o devices-9615.o board-9615-regulator.o board-9615-gpiomux.o board-9615-storage.o
obj-$(CONFIG_ARCH_MSM9615) += clock-local.o clock-9615.o acpuclock-9615.o clock-rpm.o
obj-$(CONFIG_ARCH_MSMCOPPER) += board-copper.o board-dt.o

obj-$(CONFIG_MACH_SAPPHIRE) += board-sapphire.o board-sapphire-gpio.o
obj-$(CONFIG_MACH_SAPPHIRE) += board-sapphire-keypad.o board-sapphire-panel.o
obj-$(CONFIG_MACH_SAPPHIRE) += board-sapphire-mmc.o board-sapphire-wifi.o
obj-$(CONFIG_MACH_SAPPHIRE) += board-sapphire-rfkill.o msm_vibrator.o

CFLAGS_msm_vibrator.o += -Idrivers/staging/android

obj-$(CONFIG_ARCH_FSM9XXX) += board-fsm9xxx.o

obj-$(CONFIG_TROUT_BATTCHG) += htc_battery.o

obj-$(CONFIG_HTC_PWRSINK) += htc_pwrsink.o
obj-$(CONFIG_HTC_HEADSET) += htc_headset.o
obj-$(CONFIG_MSM_RMT_STORAGE_CLIENT) += rmt_storage_client.o
obj-$(CONFIG_MSM_SDIO_SMEM) += sdio_smem.o
obj-$(CONFIG_MSM_RPM) += rpm.o rpm_resources.o
obj-$(CONFIG_MSM_MPM) += mpm.o
obj-$(CONFIG_MSM_RPM_STATS_LOG) += rpm_stats.o
obj-$(CONFIG_MSM_RPM_LOG) += rpm_log.o
obj-$(CONFIG_MSM_TZ_LOG) += tz_log.o
obj-$(CONFIG_MSM_XO) += msm_xo.o
obj-$(CONFIG_MSM_BUS_SCALING) += msm_bus/
obj-$(CONFIG_MSM_BUSPM_DEV) += msm-buspm-dev.o

obj-$(CONFIG_MSM_IOMMU)		+= iommu.o iommu_dev.o devices-iommu.o iommu_domains.o

ifdef CONFIG_VCM
obj-$(CONFIG_ARCH_MSM8X60) += board-msm8x60-vcm.o
endif

obj-$(CONFIG_ARCH_MSM7X27) += gpiomux-7x27.o gpiomux-v1.o gpiomux.o
obj-$(CONFIG_ARCH_MSM7X30) += gpiomux-7x30.o gpiomux-v1.o gpiomux.o
obj-$(CONFIG_ARCH_QSD8X50) += gpiomux-8x50.o gpiomux-v1.o gpiomux.o
obj-$(CONFIG_ARCH_MSM8X60) += gpiomux-8x60.o gpiomux-v2.o gpiomux.o
obj-$(CONFIG_ARCH_MSM8960) += gpiomux-v2.o gpiomux.o
obj-$(CONFIG_ARCH_APQ8064) += gpiomux-v2.o gpiomux.o
obj-$(CONFIG_ARCH_MSM9615) += gpiomux-v2.o gpiomux.o
obj-$(CONFIG_ARCH_MSMCOPPER) += gpiomux-v2.o gpiomux.o

ifdef CONFIG_FSM9XXX_TLMM
obj-y   += gpio-fsm9xxx.o
else
ifdef CONFIG_MSM_V2_TLMM
obj-y	+= gpio-v2.o
else
obj-y	+= gpio.o
endif
endif

obj-$(CONFIG_MSM_SLEEP_STATS) += msm_rq_stats.o idle_stats.o
obj-$(CONFIG_MSM_SLEEP_STATS_DEVICE) += idle_stats_device.o
obj-$(CONFIG_MSM_DCVS) += msm_dcvs_scm.o msm_dcvs.o msm_dcvs_idle.o
obj-$(CONFIG_MSM_SHOW_RESUME_IRQ) += msm_show_resume_irq.o
obj-$(CONFIG_BT_MSM_PINTEST)  += btpintest.o
obj-$(CONFIG_MSM_FAKE_BATTERY) += fish_battery.o
obj-$(CONFIG_MSM_RPC_VIBRATOR) += msm_vibrator.o
obj-$(CONFIG_MSM_NATIVE_RESTART) += restart.o

obj-$(CONFIG_MSM_PROC_COMM_REGULATOR) += proccomm-regulator.o
ifdef CONFIG_MSM_PROC_COMM_REGULATOR
obj-$(CONFIG_MACH_MSM7X27_SURF) += board-msm7627-regulator.o
obj-$(CONFIG_MACH_MSM7X27_FFA) += board-msm7627-regulator.o
obj-$(CONFIG_ARCH_MSM7X30) += board-msm7x30-regulator.o
obj-$(CONFIG_ARCH_MSM7X27A) += board-msm7x27a-regulator.o
endif

obj-$(CONFIG_ARCH_MSM8960) += mdm2.o mdm_common.o
obj-$(CONFIG_MSM_RTB) += msm_rtb.o
obj-$(CONFIG_MSM_CACHE_ERP) += cache_erp.o
obj-$(CONFIG_MSM_CACHE_DUMP) += msm_cache_dump.o
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
