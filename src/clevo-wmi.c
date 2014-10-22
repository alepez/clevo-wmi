/*
 *  clevo-wmi.c
 *
 *  Copyright (C) 2013  Ash Hughes <ashley.hughes@blueyonder.co.uk>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * ---------------------------------------------------------------------
 *
 *  Based on code by:
 *
 *  * Michael WJ Wang @ Clevo Co, <weijoe.wang AT gmail.com>
 *  * Peter Wu, <lekensteyn@gmail.com>
 *  * Steven David Seeger, <steven.seeger@flightsystems.net>
 *  * Christoph Jaeger, <christophjaeger@linux.com>
 */

#define CLEVO_WMI_VER "0.1"
#define CLEVO_WMI_NAME KBUILD_MODNAME

#include <linux/acpi.h>
#include <linux/delay.h>
#include <linux/dmi.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/leds.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/workqueue.h>

MODULE_AUTHOR("Ash Hughes <ashley.hughes@blueyonder.co.uk>");
MODULE_DESCRIPTION("Clevo WMI Driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(CLEVO_WMI_VER);

struct clevo_wmi
{
	struct workqueue_struct *led_workqueue;
};

static struct workqueue_struct *led_workqueue;
struct platform_device *clevo_platform_device;

static int clevo_wmi_probe(struct platform_device *dev)
{
	return 0;
}

static int clevo_wmi_remove(struct platform_device *dev)
{
	return 0;
}

static int clevo_wmi_resume(struct platform_device *dev)
{
	return 0;
}

static struct platform_driver clevo_platform_driver = {
	.remove = clevo_wmi_remove,
	.resume = clevo_wmi_resume,
	.driver = {
		.name  = CLEVO_WMI_NAME,
		.owner = THIS_MODULE,
	},
};

static struct _led_work {
	struct work_struct work;
	int wk;
} led_work;

static void airplane_led_update(struct work_struct *work)
{
	u8 byte;
	struct _led_work *w;

	w = container_of(work, struct _led_work, work);

	ec_read(0xD9, &byte);
	ec_write(0xD9, w->wk ? byte | 0x40 : byte & ~0x40);
}

static enum led_brightness airplane_led_get(struct led_classdev *led_cdev)
{
	u8 byte;

	ec_read(0xD9, &byte);
	return byte & 0x40 ? LED_FULL : LED_OFF;
}

/* must not sleep */
static void airplane_led_set(struct led_classdev *led_cdev,
                             enum led_brightness value)
{
	led_work.wk = value;
	queue_work(led_workqueue, &led_work.work);
}

static struct led_classdev airplane_led = {
	.name = "clevo::airplane",
	.brightness_get = airplane_led_get,
	.brightness_set = airplane_led_set,
	.max_brightness = 1,
};

static int __init clevo_led_init(void)
{
	int err;

	led_workqueue = create_singlethread_workqueue("led_workqueue");
	if (unlikely(!led_workqueue))
		return -ENOMEM;

	INIT_WORK(&led_work.work, airplane_led_update);

	err = led_classdev_register(&clevo_platform_device->dev, &airplane_led);
	if (err)
		goto err_destroy_workqueue;

	return 0;

err_destroy_workqueue:
	destroy_workqueue(led_workqueue);
	led_workqueue = NULL;

	return err;
}

static void clevo_led_exit(void)
{
	if (!IS_ERR_OR_NULL(airplane_led.dev))
		led_classdev_unregister(&airplane_led);
	if (led_workqueue)
		destroy_workqueue(led_workqueue);
}


static int __init clevo_wmi_init(void)
{
	int err;

	clevo_platform_device =
		platform_create_bundle(&clevo_platform_driver,
		                       clevo_wmi_probe, NULL, 0, NULL, 0);

	if (unlikely(IS_ERR(clevo_platform_device)))
		return PTR_RET(clevo_platform_device);

	err = clevo_led_init();
	if (unlikely(err))
		pr_err(CLEVO_WMI_NAME ": Could not register LED device\n");
	return 0;
}

static void __exit clevo_wmi_exit(void)
{
	clevo_led_exit();
}

module_init(clevo_wmi_init);
module_exit(clevo_wmi_exit);

