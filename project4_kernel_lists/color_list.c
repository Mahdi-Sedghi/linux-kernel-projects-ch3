/**
 * color_list.c - Demonstrates Linux kernel circular doubly linked list
 * insertion, traversal, and safe deletion using struct color.
 *
 * Operating System Concepts - Chapter 3, Project 4 (Part I)
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/slab.h>

struct color {
    int red;
    int blue;
    int green;
    struct list_head list;
};

/* Declare and initialize the head of the linked list */
static LIST_HEAD(color_list);

static int add_color(int r, int b, int g)
{
    struct color *c = kmalloc(sizeof(*c), GFP_KERNEL);
    if (!c) {
        return -ENOMEM;
    }
    c->red = r;
    c->blue = b;
    c->green = g;
    INIT_LIST_HEAD(&c->list);
    list_add_tail(&c->list, &color_list);
    return 0;
}

static int __init color_list_init(void)
{
    struct color *ptr;

    printk(KERN_INFO "Loading color_list module: creating 4 colors...\n");

    /* Insert four colors into the linked list */
    add_color(138, 43, 226);  /* Violet */
    add_color(255, 215, 0);   /* Gold   */
    add_color(220, 20, 60);   /* Crimson */
    add_color(0, 128, 128);   /* Teal   */

    /* Traverse the linked list and output contents */
    list_for_each_entry(ptr, &color_list, list) {
        printk(KERN_INFO "Color -> Red: %3d, Blue: %3d, Green: %3d\n",
               ptr->red, ptr->blue, ptr->green);
    }

    return 0;
}

static void __exit color_list_exit(void)
{
    struct color *ptr, *next;

    printk(KERN_INFO "Unloading color_list module: deleting elements...\n");

    /* Safely traverse and remove each element, returning memory via kfree */
    list_for_each_entry_safe(ptr, next, &color_list, list) {
        printk(KERN_INFO "Deleting Color -> Red: %3d, Blue: %3d, Green: %3d\n",
               ptr->red, ptr->blue, ptr->green);
        list_del(&ptr->list);
        kfree(ptr);
    }

    printk(KERN_INFO "All color_list elements deleted and memory freed.\n");
}

module_init(color_list_init);
module_exit(color_list_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Linux Kernel Linked List Module (Part I - Colors)");
MODULE_AUTHOR("Mahdi");