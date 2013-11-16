/* drivers/misc/lowmemorykiller.c
 *
 * The lowmemorykiller driver lets user-space specify a set of memory thresholds
 * where processes with a range of oom_score_adj values will get killed. Specify
 * the minimum oom_score_adj values in
 * /sys/module/lowmemorykiller/parameters/adj and the number of free pages in
 * /sys/module/lowmemorykiller/parameters/minfree. Both files take a comma
 * separated list of numbers in ascending order.
 *
 * For example, write "0,8" to /sys/module/lowmemorykiller/parameters/adj and
 * "1024,4096" to /sys/module/lowmemorykiller/parameters/minfree to kill
 * processes with a oom_score_adj value of 8 or higher when the free memory
 * drops below 4096 pages and kill processes with a oom_score_adj value of 0 or
 * higher when the free memory drops below 1024 pages.
 *
 * The driver considers memory used for caches to be free, but if a large
 * percentage of the cached memory is locked this can be very inaccurate
 * and processes may not get killed until the normal oom killer is triggered.
 *
 * Copyright (C) 2007-2008 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/oom.h>
#include <linux/sched.h>
#include <linux/rcupdate.h>
#include <linux/profile.h>
#include <linux/notifier.h>
<<<<<<< HEAD

static uint32_t lowmem_debug_level = 0;
static short lowmem_adj[6] = {
=======
#include <linux/swap.h>
#ifdef CONFIG_MEMORY_HOTPLUG
#include <linux/memory.h>
#include <linux/memory_hotplug.h>
#endif

#ifdef CONFIG_ZSWAP
#include <linux/fs.h>
#endif

#ifdef CONFIG_ZRAM_FOR_ANDROID
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/mm_inline.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <linux/cpu.h>
#include <asm/atomic.h>

#if defined(CONFIG_SMP)
#define NR_TO_RECLAIM_PAGES 		(1024*2) /* 8MB*cpu_core, include file pages */
#define MIN_FREESWAP_PAGES 		(NR_TO_RECLAIM_PAGES*2*NR_CPUS)
#define MIN_RECLAIM_PAGES 		(NR_TO_RECLAIM_PAGES/8)
#define MIN_CSWAP_INTERVAL 		(10*HZ) /* 10 senconds */
#else /* CONFIG_SMP */
#define NR_TO_RECLAIM_PAGES 		1024 /* 4MB, include file pages */
#define MIN_FREESWAP_PAGES 		(NR_TO_RECLAIM_PAGES*2)  /* 4MB*2 */
#define MIN_RECLAIM_PAGES 		(NR_TO_RECLAIM_PAGES/8)
#define MIN_CSWAP_INTERVAL 		(10*HZ) /* 10 senconds */
#endif

struct soft_reclaim {
	atomic_t kcompcached_running;
	atomic_t need_to_reclaim;
	atomic_t lmk_running;
	struct task_struct *kcompcached;
};

static struct soft_reclaim s_reclaim;
extern atomic_t kswapd_thread_on;
static unsigned long prev_jiffy;
static uint32_t number_of_reclaim_pages = NR_TO_RECLAIM_PAGES;
static uint32_t minimum_freeswap_pages = MIN_FREESWAP_PAGES;
static uint32_t minimum_reclaim_pages = MIN_RECLAIM_PAGES;
static uint32_t minimum_interval_time = MIN_CSWAP_INTERVAL;
#endif /* CONFIG_ZRAM_FOR_ANDROID */

#define ENHANCED_LMK_ROUTINE
#define LMK_COUNT_READ

#ifdef ENHANCED_LMK_ROUTINE
#define LOWMEM_DEATHPENDING_DEPTH 3
#endif

static uint32_t lowmem_debug_level = 1;

#ifdef LMK_COUNT_READ
static uint32_t lmk_count = 0;
#endif

static int lowmem_adj[6] = {
>>>>>>> fc9b728... update12
	0,
	1,
	6,
	12,
	13,
	15
};
static int lowmem_adj_size = 6;
static int lowmem_minfree[6] = {
	3 * 512,	/* 6MB */
	2 * 1024,	/* 8MB */
	4 * 1024,	/* 16MB */
	16 * 1024,	/* 64MB */
	20 * 1024,  /* 80MB */
    28 * 1024,  /* 112MB */
};
static int lowmem_minfree_size = 6;

static unsigned long lowmem_deathpending_timeout;

#define lowmem_print(level, x...)			\
	do {						\
		if (lowmem_debug_level >= (level))	\
			printk(x);			\
	} while (0)

static int lowmem_shrink(struct shrinker *s, struct shrink_control *sc)
{
	struct task_struct *tsk;
	struct task_struct *selected = NULL;
	int rem = 0;
	int tasksize;
	int i;
	short min_score_adj = OOM_SCORE_ADJ_MAX + 1;
	int selected_tasksize = 0;
	short selected_oom_score_adj;
	int array_size = ARRAY_SIZE(lowmem_adj);
	int other_free = global_page_state(NR_FREE_PAGES);
	int other_file = global_page_state(NR_FILE_PAGES) -
						global_page_state(NR_SHMEM);
<<<<<<< HEAD
=======
#ifdef CONFIG_MEMORY_HOTPLUG
	struct zone *zone;

	if (offlining) {
		/* Discount all free space in the section being offlined */
		for_each_zone(zone) {
			 if (zone_idx(zone) == ZONE_MOVABLE) {
				other_free -= zone_page_state(zone,
						NR_FREE_PAGES);
				lowmem_print(4, "lowmem_shrink discounted "
					"%lu pages in movable zone\n",
					zone_page_state(zone, NR_FREE_PAGES));
			}
		}
	}
#endif

#if defined(CONFIG_ZRAM_FOR_ANDROID) || defined(CONFIG_ZSWAP)
	other_file -= total_swapcache_pages;
#endif /* CONFIG_ZRAM_FOR_ANDROID */

	/*
	 * If we already have a death outstanding, then
	 * bail out right away; indicating to vmscan
	 * that we have nothing further to offer on
	 * this pass.
	 *
	 */
#ifdef ENHANCED_LMK_ROUTINE
	for (i = 0; i < LOWMEM_DEATHPENDING_DEPTH; i++) {
		if (lowmem_deathpending[i] &&
			time_before_eq(jiffies, lowmem_deathpending_timeout))
			return 0;
	}
#else
	if (lowmem_deathpending &&
	    time_before_eq(jiffies, lowmem_deathpending_timeout))
		return 0;
#endif
>>>>>>> fc9b728... update12

	if (lowmem_adj_size < array_size)
		array_size = lowmem_adj_size;
	if (lowmem_minfree_size < array_size)
		array_size = lowmem_minfree_size;
	for (i = 0; i < array_size; i++) {
		if (other_free < lowmem_minfree[i] &&
		    other_file < lowmem_minfree[i]) {
			min_score_adj = lowmem_adj[i];
			break;
		}
	}
	if (sc->nr_to_scan > 0)
		lowmem_print(3, "lowmem_shrink %lu, %x, ofree %d %d, ma %hd\n",
				sc->nr_to_scan, sc->gfp_mask, other_free,
				other_file, min_score_adj);
	rem = global_page_state(NR_ACTIVE_ANON) +
		global_page_state(NR_ACTIVE_FILE) +
		global_page_state(NR_INACTIVE_ANON) +
		global_page_state(NR_INACTIVE_FILE);
	if (sc->nr_to_scan <= 0 || min_score_adj == OOM_SCORE_ADJ_MAX + 1) {
		lowmem_print(5, "lowmem_shrink %lu, %x, return %d\n",
			     sc->nr_to_scan, sc->gfp_mask, rem);
		return rem;
	}
	selected_oom_score_adj = min_score_adj;

<<<<<<< HEAD
	rcu_read_lock();
=======
#ifdef CONFIG_ZRAM_FOR_ANDROID
	atomic_set(&s_reclaim.lmk_running, 1);
#endif /* CONFIG_ZRAM_FOR_ANDROID */
	read_lock(&tasklist_lock);
>>>>>>> fc9b728... update12
	for_each_process(tsk) {
		struct task_struct *p;
		short oom_score_adj;

		if (tsk->flags & PF_KTHREAD)
			continue;

		p = find_lock_task_mm(tsk);
		if (!p)
			continue;

		if (test_tsk_thread_flag(p, TIF_MEMDIE) &&
		    time_before_eq(jiffies, lowmem_deathpending_timeout)) {
			task_unlock(p);
			rcu_read_unlock();
			return 0;
		}
		oom_score_adj = p->signal->oom_score_adj;
		if (oom_score_adj < min_score_adj) {
			task_unlock(p);
			continue;
		}
		tasksize = get_mm_rss(p->mm);
		task_unlock(p);
		if (tasksize <= 0)
			continue;
		if (selected) {
			if (oom_score_adj < selected_oom_score_adj)
				continue;
			if (oom_score_adj == selected_oom_score_adj &&
			    tasksize <= selected_tasksize)
				continue;
		}
		selected = p;
		selected_tasksize = tasksize;
		selected_oom_score_adj = oom_score_adj;
		lowmem_print(2, "select %d (%s), adj %hd, size %d, to kill\n",
			     p->pid, p->comm, oom_score_adj, tasksize);
	}
<<<<<<< HEAD
=======
#ifdef ENHANCED_LMK_ROUTINE
	for (i = 0; i < LOWMEM_DEATHPENDING_DEPTH; i++) {
		if (selected[i]) {
			lowmem_print(1, "send sigkill to %d (%s), adj %d, size %d\n",

				selected[i]->pid, selected[i]->comm,
				selected_oom_score_adj[i], selected_tasksize[i]);

			lowmem_deathpending[i] = selected[i];
			lowmem_deathpending_timeout = jiffies + HZ;
			send_sig(SIGKILL, selected[i], 0);
			rem -= selected_tasksize[i];
#ifdef LMK_COUNT_READ
			lmk_count++;
#endif
		}
	}
#else
>>>>>>> fc9b728... update12
	if (selected) {
		lowmem_print(1, "send sigkill to %d (%s), adj %hd, size %d\n",
			     selected->pid, selected->comm,
			     selected_oom_score_adj, selected_tasksize);
		lowmem_deathpending_timeout = jiffies + HZ;
		send_sig(SIGKILL, selected, 0);
		set_tsk_thread_flag(selected, TIF_MEMDIE);
		rem -= selected_tasksize;
#ifdef LMK_COUNT_READ
		lmk_count++;
#endif
	}
	lowmem_print(4, "lowmem_shrink %lu, %x, return %d\n",
		     sc->nr_to_scan, sc->gfp_mask, rem);
<<<<<<< HEAD
	rcu_read_unlock();
=======
	read_unlock(&tasklist_lock);

#ifdef CONFIG_ZRAM_FOR_ANDROID
	atomic_set(&s_reclaim.lmk_running, 0);
#endif /* CONFIG_ZRAM_FOR_ANDROID */

>>>>>>> fc9b728... update12
	return rem;
}

#ifdef CONFIG_ZRAM_FOR_ANDROID
void could_cswap(void)
{
	if (atomic_read(&s_reclaim.need_to_reclaim) == 0)
		return;

<<<<<<< HEAD
static int __init lowmem_init(void)
{
	register_shrinker(&lowmem_shrinker);
=======
	if (time_before(jiffies, prev_jiffy + minimum_interval_time))
		return;

	if (atomic_read(&s_reclaim.lmk_running) == 1 || atomic_read(&kswapd_thread_on) == 1) 
		return;

	if (nr_swap_pages < minimum_freeswap_pages)
		return;

	if (idle_cpu(task_cpu(s_reclaim.kcompcached)) && this_cpu_loadx(4) == 0) {
		if (atomic_read(&s_reclaim.kcompcached_running) == 0) {
			wake_up_process(s_reclaim.kcompcached);
			atomic_set(&s_reclaim.kcompcached_running, 1);
			prev_jiffy = jiffies;
		}
	}
}

inline void need_soft_reclaim(void)
{
	atomic_set(&s_reclaim.need_to_reclaim, 1);
}

inline void cancel_soft_reclaim(void)
{
	atomic_set(&s_reclaim.need_to_reclaim, 0);
}

int get_soft_reclaim_status(void)
{
	int kcompcache_running = atomic_read(&s_reclaim.kcompcached_running);
	return kcompcache_running;
}

extern long rtcc_reclaim_pages(long nr_to_reclaim);
static int do_compcache(void * nothing)
{
	int ret;
	set_freezable();

	for ( ; ; ) {
		ret = try_to_freeze();
		if (kthread_should_stop())
			break;

		if (rtcc_reclaim_pages(number_of_reclaim_pages) < minimum_reclaim_pages)
			cancel_soft_reclaim();

		atomic_set(&s_reclaim.kcompcached_running, 0);
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
	}

>>>>>>> fc9b728... update12
	return 0;
}

static ssize_t rtcc_trigger_store(struct class *class, struct class_attribute *attr,
			const char *buf, size_t count)
{
<<<<<<< HEAD
	unregister_shrinker(&lowmem_shrinker);
}

=======
	long val, magic_sign;

	sscanf(buf, "%ld,%ld", &val, &magic_sign);

	if (val < 0 || ((val * val - 1) != magic_sign)) {
		pr_warning("Invalid command.\n");
		goto out;
	}

	need_soft_reclaim();

out:
	return count;
}

static CLASS_ATTR(rtcc_trigger, 0200, NULL, rtcc_trigger_store);
static struct class *kcompcache_class;

static int kcompcache_idle_notifier(struct notifier_block *nb, unsigned long val, void *data)
{
	could_cswap();
	return 0;
}

static struct notifier_block kcompcache_idle_nb = {
	.notifier_call = kcompcache_idle_notifier,
};
#endif /* CONFIG_ZRAM_FOR_ANDROID */


#ifdef CONFIG_ANDROID_LOW_MEMORY_KILLER_AUTODETECT_OOM_ADJ_VALUES
static int lowmem_oom_adj_to_oom_score_adj(int oom_adj)
{
	if (oom_adj == OOM_ADJUST_MAX)
		return OOM_SCORE_ADJ_MAX;
	else
		return (oom_adj * OOM_SCORE_ADJ_MAX) / -OOM_DISABLE;
}

static void lowmem_autodetect_oom_adj_values(void)
{
	int i;
	int oom_adj;
	int oom_score_adj;
	int array_size = ARRAY_SIZE(lowmem_adj);

	if (lowmem_adj_size < array_size)
		array_size = lowmem_adj_size;

	if (array_size <= 0)
		return;

	oom_adj = lowmem_adj[array_size - 1];
	if (oom_adj > OOM_ADJUST_MAX)
		return;

	oom_score_adj = lowmem_oom_adj_to_oom_score_adj(oom_adj);
	if (oom_score_adj <= OOM_ADJUST_MAX)
		return;

	lowmem_print(1, "lowmem_shrink: convert oom_adj to oom_score_adj:\n");
	for (i = 0; i < array_size; i++) {
		oom_adj = lowmem_adj[i];
		oom_score_adj = lowmem_oom_adj_to_oom_score_adj(oom_adj);
		lowmem_adj[i] = oom_score_adj;
		lowmem_print(1, "oom_adj %d => oom_score_adj %d\n",
			     oom_adj, oom_score_adj);
	}
}

static int lowmem_adj_array_set(const char *val, const struct kernel_param *kp)
{
	int ret;

	ret = param_array_ops.set(val, kp);

	/* HACK: Autodetect oom_adj values in lowmem_adj array */
	lowmem_autodetect_oom_adj_values();

	return ret;
}

static int lowmem_adj_array_get(char *buffer, const struct kernel_param *kp)
{
        return param_array_ops.get(buffer, kp);
}

static void lowmem_adj_array_free(void *arg)
{
        param_array_ops.free(arg);
}

static struct kernel_param_ops lowmem_adj_array_ops = {
        .set = lowmem_adj_array_set,
        .get = lowmem_adj_array_get,
        .free = lowmem_adj_array_free,
};

static const struct kparam_array __param_arr_adj = {
        .max = ARRAY_SIZE(lowmem_adj),
        .num = &lowmem_adj_size,
        .ops = &param_ops_int,
        .elemsize = sizeof(lowmem_adj[0]),
        .elem = lowmem_adj,
};
#endif

static struct shrinker lowmem_shrinker = {
	.shrink = lowmem_shrink,
	.seeks = DEFAULT_SEEKS * 16
};

static int __init lowmem_init(void)
{
	task_free_register(&task_nb);
	register_shrinker(&lowmem_shrinker);
#ifdef CONFIG_MEMORY_HOTPLUG
	hotplug_memory_notifier(lmk_hotplug_callback, 0);
#endif

#ifdef CONFIG_ZRAM_FOR_ANDROID
	s_reclaim.kcompcached = kthread_run(do_compcache, NULL, "kcompcached");
	if (IS_ERR(s_reclaim.kcompcached)) {
		/* failure at boot is fatal */
		BUG_ON(system_state == SYSTEM_BOOTING);
	}
	set_user_nice(s_reclaim.kcompcached, 0);
	atomic_set(&s_reclaim.need_to_reclaim, 0);
	atomic_set(&s_reclaim.kcompcached_running, 0);
	prev_jiffy = jiffies;

	idle_notifier_register(&kcompcache_idle_nb);

	kcompcache_class = class_create(THIS_MODULE, "kcompcache");
	if (IS_ERR(kcompcache_class)) {
		pr_err("%s: couldn't create kcompcache class.\n", __func__);
		return 0;
	}
	if (class_create_file(kcompcache_class, &class_attr_rtcc_trigger) < 0) {
		pr_err("%s: couldn't create rtcc trigger sysfs file.\n", __func__);
		class_destroy(kcompcache_class);
	}
#endif /* CONFIG_ZRAM_FOR_ANDROID */
	return 0;
}

static void __exit lowmem_exit(void)
{
	unregister_shrinker(&lowmem_shrinker);
	task_free_unregister(&task_nb);

#ifdef CONFIG_ZRAM_FOR_ANDROID
	idle_notifier_unregister(&kcompcache_idle_nb);
	if (s_reclaim.kcompcached) {
		cancel_soft_reclaim();
		kthread_stop(s_reclaim.kcompcached);
		s_reclaim.kcompcached = NULL;
	}

	if (kcompcache_class) {
		class_remove_file(kcompcache_class, &class_attr_rtcc_trigger);
		class_destroy(kcompcache_class);
	}
#endif /* CONFIG_ZRAM_FOR_ANDROID */
}

>>>>>>> fc9b728... update12
module_param_named(cost, lowmem_shrinker.seeks, int, S_IRUGO | S_IWUSR);
module_param_array_named(adj, lowmem_adj, short, &lowmem_adj_size,
			 S_IRUGO | S_IWUSR);
module_param_array_named(minfree, lowmem_minfree, uint, &lowmem_minfree_size,
			 S_IRUGO | S_IWUSR);
module_param_named(debug_level, lowmem_debug_level, uint, S_IRUGO | S_IWUSR);

#ifdef LMK_COUNT_READ
module_param_named(lmkcount, lmk_count, uint, S_IRUGO);
#endif

#ifdef CONFIG_ZRAM_FOR_ANDROID
module_param_named(nr_reclaim, number_of_reclaim_pages, uint, S_IRUSR | S_IWUSR);
module_param_named(min_freeswap, minimum_freeswap_pages, uint, S_IRUSR | S_IWUSR);
module_param_named(min_reclaim, minimum_reclaim_pages, uint, S_IRUSR | S_IWUSR);
module_param_named(min_interval, minimum_interval_time, uint, S_IRUSR | S_IWUSR);
#endif /* CONFIG_ZRAM_FOR_ANDROID */

module_init(lowmem_init);
module_exit(lowmem_exit);

MODULE_LICENSE("GPL");
