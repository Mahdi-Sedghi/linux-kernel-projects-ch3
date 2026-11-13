/**
 * collatz.c - Generates the Collatz sequence starting from a module
 * parameter `start` (default = 25) and stores it in a kernel linked list.
 *
 * Operating System Concepts - Chapter 3, Project 4 (Part II)
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/slab.h>

static int start = 25;
module_param(start, int, 0);
MODULE_PARM_DESC(start, "Starting integer for the Collatz sequence (default = 25)");

struct collatz_node {
    int value;
    struct list_head list;
};

static LIST_HEAD(collatz_list);

static int add_collatz_node(int val)
{
    struct collatz_node *node = kmalloc(sizeof(*node), GFP_KERNEL);
    if (!node) {
        return -ENOMEM;
    }
    node->value = val;
    INIT_LIST_HEAD(&node->list);
    list_add_tail(&node->list, &collatz_list);
    return 0;
}

static int __init collatz_init(void)
{
    int current_val = start;
    struct collatz_node *ptr;

    if (current_val <= 0) {
        printk(KERN_ERR "collatz: 'start' parameter must be a positive integer (got %d)\n", start);
        return -EINVAL;
    }

    printk(KERN_INFO "Loading collatz module with start = %d\n", start);

    /* Generate Collatz sequence and store in linked list */
    while (current_val > 1) {
        if (add_collatz_node(current_val) != 0) {
            return -ENOMEM;
        }
        if (current_val % 2 == 0) {
            current_val /= 2;
        } else {
            current_val = 3 * current_val + 1;
        }
    }
    /* Add the final 1 */
    add_collatz_node(1);

    /* Traverse list and log the generated sequence */
    printk(KERN_INFO "Collatz sequence for start = %d:\n", start);
    list_for_each_entry(ptr, &collatz_list, list) {
        printk(KERN_INFO "  collatz value: %d\n", ptr->value);
    }

    return 0;
}

static void __exit collatz_exit(void)
{
    struct collatz_node *ptr, *next;

    printk(KERN_INFO "Unloading collatz module: freeing sequence list...\n");

    list_for_each_entry_safe(ptr, next, &collatz_list, list) {
        list_del(&ptr->list);
        kfree(ptr);
    }

    printk(KERN_INFO "Collatz linked list removed and memory returned to kernel.\n");
}

module_init(collatz_init);
module_exit(collatz_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Collatz Sequence Kernel Linked List Module (Part II)");
MODULE_AUTHOR("Mahdi");