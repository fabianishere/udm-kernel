/*
 * ledtrig-ext.c - LED trigger based on external events
 *
 * This LED trigger registers up to 10 instances, which can be 
 * assigned to different LEDs and triggered outside of ledcore component.
 *
 * An API named ledtrig_external is exported for any user, who wants to trigger 
 * particular LED from its code.
 *
 * Copyright 2017 Ubiquiti Networks, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/leds.h>

#define MAX_EXT_TRIGGERS       10
#define MAX_NAME_LEN           16

#define EXT_NAME_PATTERN       "external%d"

struct led_trigger_ext {
    char name[MAX_NAME_LEN];
    struct led_trigger trigger;
};

static struct led_trigger_ext *ext_triggers[MAX_EXT_TRIGGERS];

/**
 * ledtrig_external - emit an external event as a trigger
 * @index: index of external LED trigger to be emitted.
 */
int ledtrig_external(int index, int on)
{
    if (index < 0 || index >= MAX_EXT_TRIGGERS)
        return -EINVAL;
    
    led_trigger_event(&ext_triggers[index]->trigger, (on) ? LED_FULL : LED_OFF);
    return 0;
}
EXPORT_SYMBOL(ledtrig_external);

static void ledtrig_ext_activate(struct led_classdev *led_cdev) {
    led_cdev->activated = true;
}

static void ledtrig_ext_deactivate(struct led_classdev *led_cdev) {
    led_cdev->activated = false;
}

static int __init ledtrig_ext_init(void)
{
    int i, ret = 0;

    for(i = 0; i < MAX_EXT_TRIGGERS; i++) {
        ext_triggers[i] = kzalloc(sizeof(struct led_trigger_ext), GFP_KERNEL);
        if (!ext_triggers[i])
            return -ENOMEM;

        snprintf(ext_triggers[i]->name, MAX_NAME_LEN, EXT_NAME_PATTERN, i);
        /* Fill led_trigger struct and register trigger */
        ext_triggers[i]->trigger.name = ext_triggers[i]->name;
        ext_triggers[i]->trigger.activate = ledtrig_ext_activate;
        ext_triggers[i]->trigger.deactivate = ledtrig_ext_deactivate;
        
        ret = led_trigger_register(&ext_triggers[i]->trigger);
        if (ret != 0)
            return ret;
    }
     
    return ret;
}
module_init(ledtrig_ext_init);
 
static void __exit ledtrig_ext_exit(void)
{
    int i;
    
    for(i = 0; i < MAX_EXT_TRIGGERS; i++) {
        if (ext_triggers[i]) {
            led_trigger_unregister(&ext_triggers[i]->trigger);
            kfree(ext_triggers[i]);
        }
    }
}
module_exit(ledtrig_ext_exit);


MODULE_AUTHOR("Ubiquiti Networks, Inc.");
MODULE_DESCRIPTION("External LED trigger");
MODULE_LICENSE("GPL");

