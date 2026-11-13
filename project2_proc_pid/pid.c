/**
 * pid.c - Kernel module that communicates with /proc/pid to report
 * task command name, PID, and state given a process identifier.
 *
 * Operating System Concepts - Chapter 3, Project 2
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/pid.h>

#define BUFFER_SIZE 128
#define PROC_NAME "pid"

/* Stores the current target PID written to /proc/pid */
static long l_pid = -1;

/**
 * proc_read - Called when /proc/pid is read (e.g., `cat /proc/pid`).
 */
static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
    int rv = 0;
    char buffer[BUFFER_SIZE];
    static int completed = 0;
    struct task_struct *tsk = NULL;
    long task_state = 0;

    if (completed) {
        completed = 0;
        return 0;
    }

    /* Locate the task_struct associated with l_pid */
    tsk = pid_task(find_vpid(l_pid), PIDTYPE_PID);
    if (tsk == NULL) {
        printk(KERN_WARNING "/proc/%s: No task found for pid = %ld\n", PROC_NAME, l_pid);
        return 0;
    }

    completed = 1;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 14, 0)
    task_state = READ_ONCE(tsk->__state);
#else
    task_state = tsk->state;
#endif

    rv = snprintf(buffer, BUFFER_SIZE,
                  "command = [%s] pid = [%d] state = [%ld]\n",
                  tsk->comm, tsk->pid, task_state);

    /* Copy the formatted string to user-space buffer */
    if (copy_to_user(usr_buf, buffer, rv)) {
        return -EFAULT;
    }

    return rv;
}

/**
 * proc_write - Called when data is written to /proc/pid (e.g., `echo "1395" > /proc/pid`).
 */
static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos)
{
    char *k_mem;

    if (count == 0 || count > BUFFER_SIZE) {
        return -EINVAL;
    }

    /* Allocate kernel memory (+1 for null-terminator safety) */
    k_mem = kmalloc(count + 1, GFP_KERNEL);
    if (!k_mem) {
        return -ENOMEM;
    }

    /* Copy string from user space to kernel space */
    if (copy_from_user(k_mem, usr_buf, count)) {
        kfree(k_mem);
        return -EFAULT;
    }

    k_mem[count] = '\0';

    /* Convert string to long integer (base 10) */
    if (kstrtol(k_mem, 10, &l_pid) != 0) {
        printk(KERN_ERR "/proc/%s: Failed to parse PID from input\n", PROC_NAME);
        kfree(k_mem);
        return -EINVAL;
    }

    printk(KERN_INFO "/proc/%s: Target PID updated to %ld\n", PROC_NAME, l_pid);

    /* Return kernel memory */
    kfree(k_mem);

    return count;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops proc_fops = {
    .proc_read  = proc_read,
    .proc_write = proc_write,
};
#else
static const struct file_operations proc_fops = {
    .owner = THIS_MODULE,
    .read  = proc_read,
    .write = proc_write,
};
#endif

static int __init proc_init(void)
{
    /* Create /proc/pid with read/write permissions (0666) */
    if (!proc_create(PROC_NAME, 0666, NULL, &proc_fops)) {
        printk(KERN_ERR "Failed to create /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
    return 0;
}

static void __exit proc_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Task Information Kernel Module via /proc/pid");
MODULE_AUTHOR("Mahdi");