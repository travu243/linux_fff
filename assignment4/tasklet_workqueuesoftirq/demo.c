#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GPT");
MODULE_DESCRIPTION("Demo Tasklet vs Workqueue");

// tasklet
static void tasklet_fn(struct tasklet_struct *t);
DECLARE_TASKLET(my_tasklet, tasklet_fn);

// workqueue
static struct workqueue_struct *my_wq;
static DECLARE_WORK(my_work, NULL);

static void work_fn(struct work_struct *work)
{
    pr_info("workqueue is running in %s context (PID: %d)\n",
            in_interrupt() ? "interrupt" : "process", current->pid);
}

static void tasklet_fn(struct tasklet_struct *t)
{
    pr_info("tasklet is running in %s context\n", in_interrupt() ? "interrupt" : "process");
    if (my_wq)
        queue_work(my_wq, &my_work);
}

static int __init mod_init(void)
{
    pr_info("module loaded\n");

    my_wq = create_singlethread_workqueue("my_wq");
    if (!my_wq)
        return -ENOMEM;

    INIT_WORK(&my_work, work_fn);

    tasklet_schedule(&my_tasklet);  // tasklet run first

    return 0;
}

static void __exit mod_exit(void)
{
    if (my_wq)
        destroy_workqueue(my_wq);

    tasklet_kill(&my_tasklet);

    pr_info("module unloaded\n");
}

module_init(mod_init);
module_exit(mod_exit);
