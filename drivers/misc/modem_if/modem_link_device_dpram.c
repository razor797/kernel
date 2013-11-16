/*
 * Copyright (C) 2010 Samsung Electronics.
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

#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/time.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/wakelock.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>
#include <linux/if_arp.h>
#include <linux/platform_device.h>
#include <linux/kallsyms.h>
#include <linux/suspend.h>
#include <plat/gpio-cfg.h>
#include <mach/gpio.h>

#include <linux/platform_data/modem.h>
#include "modem_prj.h"
#include "modem_utils.h"
#include "modem_link_device_dpram.h"

<<<<<<< HEAD
/*
** Function prototypes for basic DPRAM operations
*/
static inline void clear_intr(struct dpram_link_device *dpld);
static inline u16 recv_intr(struct dpram_link_device *dpld);
static inline void send_intr(struct dpram_link_device *dpld, u16 mask);

static inline u16 get_magic(struct dpram_link_device *dpld);
static inline void set_magic(struct dpram_link_device *dpld, u16 val);
static inline u16 get_access(struct dpram_link_device *dpld);
static inline void set_access(struct dpram_link_device *dpld, u16 val);

static inline u32 get_tx_head(struct dpram_link_device *dpld, int id);
static inline u32 get_tx_tail(struct dpram_link_device *dpld, int id);
static inline void set_tx_head(struct dpram_link_device *dpld, int id, u32 in);
static inline void set_tx_tail(struct dpram_link_device *dpld, int id, u32 out);
static inline u8 *get_tx_buff(struct dpram_link_device *dpld, int id);
static inline u32 get_tx_buff_size(struct dpram_link_device *dpld, int id);

static inline u32 get_rx_head(struct dpram_link_device *dpld, int id);
static inline u32 get_rx_tail(struct dpram_link_device *dpld, int id);
static inline void set_rx_head(struct dpram_link_device *dpld, int id, u32 in);
static inline void set_rx_tail(struct dpram_link_device *dpld, int id, u32 out);
static inline u8 *get_rx_buff(struct dpram_link_device *dpld, int id);
static inline u32 get_rx_buff_size(struct dpram_link_device *dpld, int id);

static inline u16 get_mask_req_ack(struct dpram_link_device *dpld, int id);
static inline u16 get_mask_res_ack(struct dpram_link_device *dpld, int id);
static inline u16 get_mask_send(struct dpram_link_device *dpld, int id);

static inline bool dpram_circ_valid(u32 size, u32 in, u32 out);

static void handle_cp_crash(struct dpram_link_device *dpld);
static int trigger_force_cp_crash(struct dpram_link_device *dpld);
static void dpram_dump_memory(struct link_device *ld, char *buff);

/*
** Functions for debugging
*/
static inline void log_dpram_status(struct dpram_link_device *dpld)
=======
static void trigger_forced_cp_crash(struct dpram_link_device *dpld);

/**
 * set_circ_pointer
 * @dpld: pointer to an instance of dpram_link_device structure
 * @id: IPC device (IPC_FMT, IPC_RAW, etc.)
 * @dir: direction of communication (TX or RX)
 * @ptr: type of the queue pointer (HEAD or TAIL)
 * @addr: address of the queue pointer
 * @val: value to be written to the queue pointer
 *
 * Writes a value to a pointer in a circular queue with verification.
 */
static inline void set_circ_pointer(struct dpram_link_device *dpld, int id,
				int dir, int ptr, void __iomem *addr, u16 val)
>>>>>>> fc9b728... update12
{
	pr_info("mif: %s: {M:0x%X A:%d} {FMT TI:%u TO:%u RI:%u RO:%u} "
		"{RAW TI:%u TO:%u RI:%u RO:%u} {INT:0x%X}\n",
		dpld->ld.mc->name,
		get_magic(dpld), get_access(dpld),
		get_tx_head(dpld, IPC_FMT), get_tx_tail(dpld, IPC_FMT),
		get_rx_head(dpld, IPC_FMT), get_rx_tail(dpld, IPC_FMT),
		get_tx_head(dpld, IPC_RAW), get_tx_tail(dpld, IPC_RAW),
		get_rx_head(dpld, IPC_RAW), get_rx_tail(dpld, IPC_RAW),
		recv_intr(dpld));
}

static void set_dpram_map(struct dpram_link_device *dpld,
			struct mif_irq_map *map)
{
	map->magic = get_magic(dpld);
	map->access = get_access(dpld);

	map->fmt_tx_in = get_tx_head(dpld, IPC_FMT);
	map->fmt_tx_out = get_tx_tail(dpld, IPC_FMT);
	map->fmt_rx_in = get_rx_head(dpld, IPC_FMT);
	map->fmt_rx_out = get_rx_tail(dpld, IPC_FMT);
	map->raw_tx_in = get_tx_head(dpld, IPC_RAW);
	map->raw_tx_out = get_tx_tail(dpld, IPC_RAW);
	map->raw_rx_in = get_rx_head(dpld, IPC_RAW);
	map->raw_rx_out = get_rx_tail(dpld, IPC_RAW);

<<<<<<< HEAD
	map->cp2ap = recv_intr(dpld);
}
=======
		cnt++;
		mif_err("%s: ERR! %s_%s.%s saved(%d) != val(%d), count %d\n",
			ld->name, get_dev_name(id), circ_dir(dir),
			circ_ptr(ptr), saved, val, cnt);
		if (cnt >= MAX_RETRY_CNT) {
			trigger_forced_cp_crash(dpld);
			break;
		}
>>>>>>> fc9b728... update12

/*
** RXB (DPRAM RX buffer) functions
*/
static struct dpram_rxb *rxbq_create_pool(unsigned size, int count)
{
	struct dpram_rxb *rxb;
	u8 *buff;
	int i;

	rxb = kzalloc(sizeof(struct dpram_rxb) * count, GFP_KERNEL);
	if (!rxb) {
		mif_info("ERR! kzalloc rxb fail\n");
		return NULL;
	}

	buff = kzalloc((size * count), GFP_KERNEL|GFP_DMA);
	if (!buff) {
		mif_info("ERR! kzalloc buff fail\n");
		kfree(rxb);
		return NULL;
	}

	for (i = 0; i < count; i++) {
		rxb[i].buff = buff;
		rxb[i].size = size;
		buff += size;
	}

	return rxb;
}

<<<<<<< HEAD
static inline unsigned rxbq_get_page_size(unsigned len)
{
	return ((len + PAGE_SIZE - 1) >> PAGE_SHIFT) << PAGE_SHIFT;
}

static inline bool rxbq_empty(struct dpram_rxb_queue *rxbq)
{
	return (rxbq->in == rxbq->out) ? true : false;
}

static inline int rxbq_free_size(struct dpram_rxb_queue *rxbq)
{
	int in = rxbq->in;
	int out = rxbq->out;
	int qsize = rxbq->size;
	return (in < out) ? (out - in - 1) : (qsize + out - in - 1);
}

static inline struct dpram_rxb *rxbq_get_free_rxb(struct dpram_rxb_queue *rxbq)
{
	struct dpram_rxb *rxb = NULL;

	if (likely(rxbq_free_size(rxbq) > 0)) {
		rxb = &rxbq->rxb[rxbq->in];
		rxbq->in++;
		if (rxbq->in >= rxbq->size)
			rxbq->in -= rxbq->size;
		rxb->data = rxb->buff;
	}

	return rxb;
}

static inline int rxbq_size(struct dpram_rxb_queue *rxbq)
{
	int in = rxbq->in;
	int out = rxbq->out;
	int qsize = rxbq->size;
	return (in >= out) ? (in - out) : (qsize - out + in);
}

static inline struct dpram_rxb *rxbq_get_data_rxb(struct dpram_rxb_queue *rxbq)
{
	struct dpram_rxb *rxb = NULL;

	if (likely(!rxbq_empty(rxbq))) {
		rxb = &rxbq->rxb[rxbq->out];
		rxbq->out++;
		if (rxbq->out >= rxbq->size)
			rxbq->out -= rxbq->size;
	}

	return rxb;
}

static inline u8 *rxb_put(struct dpram_rxb *rxb, unsigned len)
{
	rxb->len = len;
	return rxb->data;
}

static inline void rxb_clear(struct dpram_rxb *rxb)
{
	rxb->data = NULL;
	rxb->len = 0;
}

/*
** DPRAM operations
*/
static int dpram_register_isr(unsigned irq, irqreturn_t (*isr)(int, void*),
				unsigned long flag, const char *name,
				struct dpram_link_device *dpld)
{
	int ret;

	ret = request_irq(irq, isr, flag, name, dpld);
	if (ret) {
		mif_info("%s: ERR! request_irq fail (err %d)\n", name, ret);
		return ret;
	}

	ret = enable_irq_wake(irq);
	if (ret)
		mif_info("%s: ERR! enable_irq_wake fail (err %d)\n", name, ret);

	mif_info("%s (#%d) handler registered\n", name, irq);

	return 0;
}

static inline void clear_intr(struct dpram_link_device *dpld)
{
	if (likely(dpld->dpctl->clear_intr))
		dpld->dpctl->clear_intr();
=======
/**
 * recv_int2ap
 * @dpld: pointer to an instance of dpram_link_device structure
 *
 * Returns the value of the CP-to-AP interrupt register in a DPRAM.
 */
static inline u16 recv_int2ap(struct dpram_link_device *dpld)
{
	return ioread16(dpld->mbx2ap);
>>>>>>> fc9b728... update12
}

static inline u16 recv_intr(struct dpram_link_device *dpld)
{
<<<<<<< HEAD
	if (likely(dpld->dpctl->recv_intr))
		return dpld->dpctl->recv_intr();
	else
		return ioread16(dpld->mbx2ap);
=======
	struct idpram_pm_op *pm_op = dpld->pm_op;

	if (pm_op && pm_op->int2cp_possible) {
		if (!pm_op->int2cp_possible(dpld))
			return;
	}

	iowrite16(mask, dpld->mbx2cp);
>>>>>>> fc9b728... update12
}

static inline void send_intr(struct dpram_link_device *dpld, u16 mask)
{
	if (likely(dpld->dpctl->send_intr))
		dpld->dpctl->send_intr(mask);
	else
		iowrite16(mask, dpld->mbx2cp);
}

static inline u16 get_magic(struct dpram_link_device *dpld)
{
	return ioread16(dpld->magic);
}

static inline void set_magic(struct dpram_link_device *dpld, u16 val)
{
	iowrite16(val, dpld->magic);
}

static inline u16 get_access(struct dpram_link_device *dpld)
{
	return ioread16(dpld->access);
}

static inline void set_access(struct dpram_link_device *dpld, u16 val)
{
	iowrite16(val, dpld->access);
}

static inline u32 get_tx_head(struct dpram_link_device *dpld, int id)
{
	return ioread16(dpld->dev[id]->txq.head);
}

static inline u32 get_tx_tail(struct dpram_link_device *dpld, int id)
{
	return ioread16(dpld->dev[id]->txq.tail);
}

<<<<<<< HEAD
static inline void set_tx_head(struct dpram_link_device *dpld, int id, u32 in)
{
	int cnt = 3;
	u32 val = 0;

	iowrite16((u16)in, dpld->dev[id]->txq.head);

	do {
		/* Check head value written */
		val = ioread16(dpld->dev[id]->txq.head);
		if (likely(val == in))
			return;

		mif_err("ERR: %s txq.head(%d) != in(%d)\n",
			get_dev_name(id), val, in);
		udelay(100);

		/* Write head value again */
		iowrite16((u16)in, dpld->dev[id]->txq.head);
	} while (cnt--);

	trigger_force_cp_crash(dpld);
}

static inline void set_tx_tail(struct dpram_link_device *dpld, int id, u32 out)
{
	int cnt = 3;
	u32 val = 0;

	iowrite16((u16)out, dpld->dev[id]->txq.tail);

	do {
		/* Check tail value written */
		val = ioread16(dpld->dev[id]->txq.tail);
		if (likely(val == out))
			return;

		mif_err("ERR: %s txq.tail(%d) != out(%d)\n",
			get_dev_name(id), val, out);
		udelay(100);

		/* Write tail value again */
		iowrite16((u16)out, dpld->dev[id]->txq.tail);
	} while (cnt--);

	trigger_force_cp_crash(dpld);
}

static inline u8 *get_tx_buff(struct dpram_link_device *dpld, int id)
=======
/**
 * get_txq_buff
 * @dpld: pointer to an instance of dpram_link_device structure
 * @id: IPC device (IPC_FMT, IPC_RAW, etc.)
 *
 * Returns the start address of the buffer in a TXQ.
 */
static inline u8 *get_txq_buff(struct dpram_link_device *dpld, int id)
>>>>>>> fc9b728... update12
{
	return dpld->dev[id]->txq.buff;
}

static inline u32 get_tx_buff_size(struct dpram_link_device *dpld, int id)
{
	return dpld->dev[id]->txq.size;
}

static inline u32 get_rx_head(struct dpram_link_device *dpld, int id)
{
	return ioread16(dpld->dev[id]->rxq.head);
}

static inline u32 get_rx_tail(struct dpram_link_device *dpld, int id)
{
	return ioread16(dpld->dev[id]->rxq.tail);
}

<<<<<<< HEAD
static inline void set_rx_head(struct dpram_link_device *dpld, int id, u32 in)
{
	int cnt = 3;
	u32 val = 0;

	iowrite16((u16)in, dpld->dev[id]->rxq.head);

	do {
		/* Check head value written */
		val = ioread16(dpld->dev[id]->rxq.head);
		if (val == in)
			return;

		mif_err("ERR: %s rxq.head(%d) != in(%d)\n",
			get_dev_name(id), val, in);
		udelay(100);

		/* Write head value again */
		iowrite16((u16)in, dpld->dev[id]->rxq.head);
	} while (cnt--);

	trigger_force_cp_crash(dpld);
}

static inline void set_rx_tail(struct dpram_link_device *dpld, int id, u32 out)
{
	int cnt = 3;
	u32 val = 0;

	iowrite16((u16)out, dpld->dev[id]->rxq.tail);

	do {
		/* Check tail value written */
		val = ioread16(dpld->dev[id]->rxq.tail);
		if (val == out)
			return;

		mif_err("ERR: %s rxq.tail(%d) != out(%d)\n",
			get_dev_name(id), val, out);
		udelay(100);

		/* Write tail value again */
		iowrite16((u16)out, dpld->dev[id]->rxq.tail);
	} while (cnt--);

	trigger_force_cp_crash(dpld);
}

static inline u8 *get_rx_buff(struct dpram_link_device *dpld, int id)
=======
/**
 * get_rxq_buff
 * @dpld: pointer to an instance of dpram_link_device structure
 * @id: IPC device (IPC_FMT, IPC_RAW, etc.)
 *
 * Returns the start address of the buffer in an RXQ.
 */
static inline u8 *get_rxq_buff(struct dpram_link_device *dpld, int id)
{
	return dpld->dev[id]->rxq.buff;
}

/**
 * get_rxq_buff_size
 * @dpld: pointer to an instance of dpram_link_device structure
 * @id: IPC device (IPC_FMT, IPC_RAW, etc.)
 *
 * Returns the size of the buffer in an RXQ.
 */
static inline u32 get_rxq_buff_size(struct dpram_link_device *dpld, int id)
{
	return dpld->dev[id]->rxq.size;
}

/**
 * set_txq_head
 * @dpld: pointer to an instance of dpram_link_device structure
 * @id: IPC device (IPC_FMT, IPC_RAW, etc.)
 * @in: value to be written to the head pointer in a TXQ
 */
static inline void set_txq_head(struct dpram_link_device *dpld, int id, u32 in)
>>>>>>> fc9b728... update12
{
	set_circ_pointer(dpld, id, TX, HEAD, dpld->dev[id]->txq.head, in);
}

<<<<<<< HEAD
static inline u32 get_rx_buff_size(struct dpram_link_device *dpld, int id)
=======
/**
 * set_txq_tail
 * @dpld: pointer to an instance of dpram_link_device structure
 * @id: IPC device (IPC_FMT, IPC_RAW, etc.)
 * @out: value to be written to the tail pointer in a TXQ
 */
static inline void set_txq_tail(struct dpram_link_device *dpld, int id, u32 out)
>>>>>>> fc9b728... update12
{
	set_circ_pointer(dpld, id, TX, TAIL, dpld->dev[id]->txq.tail, out);
}

/**
 * set_rxq_head
 * @dpld: pointer to an instance of dpram_link_device structure
 * @id: IPC device (IPC_FMT, IPC_RAW, etc.)
 * @in: value to be written to the head pointer in an RXQ
 */
static inline void set_rxq_head(struct dpram_link_device *dpld, int id, u32 in)
{
	set_circ_pointer(dpld, id, RX, HEAD, dpld->dev[id]->rxq.head, in);
}

/**
 * set_rxq_tail
 * @dpld: pointer to an instance of dpram_link_device structure
 * @id: IPC device (IPC_FMT, IPC_RAW, etc.)
 * @out: value to be written to the tail pointer in an RXQ
 */
static inline void set_rxq_tail(struct dpram_link_device *dpld, int id, u32 out)
{
	set_circ_pointer(dpld, id, RX, TAIL, dpld->dev[id]->rxq.tail, out);
}

static inline u16 get_mask_req_ack(struct dpram_link_device *dpld, int id)
{
	return dpld->dev[id]->mask_req_ack;
}

static inline u16 get_mask_res_ack(struct dpram_link_device *dpld, int id)
{
	return dpld->dev[id]->mask_res_ack;
}

static inline u16 get_mask_send(struct dpram_link_device *dpld, int id)
{
	return dpld->dev[id]->mask_send;
}

<<<<<<< HEAD
static inline bool dpram_circ_valid(u32 size, u32 in, u32 out)
{
	if (in >= size)
		return false;

	if (out >= size)
		return false;

	return true;
}

/* Get free space in the TXQ as well as in & out pointers */
static inline int dpram_get_txq_space(struct dpram_link_device *dpld, int dev,
		u32 qsize, u32 *in, u32 *out)
{
	struct link_device *ld = &dpld->ld;
	int cnt = 3;
	u32 head;
	u32 tail;
	int space;

	do {
		head = get_tx_head(dpld, dev);
		tail = get_tx_tail(dpld, dev);

		space = (head < tail) ? (tail - head - 1) :
			(qsize + tail - head - 1);
		mif_debug("%s: %s_TXQ qsize[%u] in[%u] out[%u] space[%u]\n",
			ld->name, get_dev_name(dev), qsize, head, tail, space);

		if (dpram_circ_valid(qsize, head, tail)) {
			*in = head;
			*out = tail;
			return space;
		}

		mif_info("%s: CAUTION! <%pf> "
			"%s_TXQ invalid (size:%d in:%d out:%d)\n",
			ld->name, __builtin_return_address(0),
			get_dev_name(dev), qsize, head, tail);

		udelay(100);
	} while (cnt--);

	*in = 0;
	*out = 0;
	return -EINVAL;
}

static void dpram_reset_tx_circ(struct dpram_link_device *dpld, int dev)
{
	set_tx_head(dpld, dev, 0);
	set_tx_tail(dpld, dev, 0);
	if (dev == IPC_FMT)
		trigger_force_cp_crash(dpld);
}

/* Get data size in the RXQ as well as in & out pointers */
static inline int dpram_get_rxq_rcvd(struct dpram_link_device *dpld, int dev,
		u32 qsize, u32 *in, u32 *out)
{
	struct link_device *ld = &dpld->ld;
	int cnt = 3;
	u32 head;
	u32 tail;
	u32 rcvd;

	do {
		head = get_rx_head(dpld, dev);
		tail = get_rx_tail(dpld, dev);
		if (head == tail) {
			*in = head;
			*out = tail;
			return 0;
		}

		rcvd = (head > tail) ? (head - tail) : (qsize - tail + head);
		mif_debug("%s: %s_RXQ qsize[%u] in[%u] out[%u] rcvd[%u]\n",
			ld->name, get_dev_name(dev), qsize, head, tail, rcvd);

		if (dpram_circ_valid(qsize, head, tail)) {
			*in = head;
			*out = tail;
			return rcvd;
		}

		mif_info("%s: CAUTION! <%pf> "
			"%s_RXQ invalid (size:%d in:%d out:%d)\n",
			ld->name, __builtin_return_address(0),
			get_dev_name(dev), qsize, head, tail);

		udelay(100);
	} while (cnt--);

	*in = 0;
	*out = 0;
	return -EINVAL;
}

static void dpram_reset_rx_circ(struct dpram_link_device *dpld, int dev)
{
	set_rx_head(dpld, dev, 0);
	set_rx_tail(dpld, dev, 0);
	if (dev == IPC_FMT)
		trigger_force_cp_crash(dpld);
}

/*
** CAUTION : dpram_allow_sleep() MUST be invoked after dpram_wake_up() success
*/
static int dpram_wake_up(struct dpram_link_device *dpld)
{
	struct link_device *ld = &dpld->ld;

	if (!dpld->dpctl->wakeup)
		return 0;

	if (dpld->dpctl->wakeup() < 0) {
		mif_err("%s: ERR! <%pf> DPRAM wakeup fail once\n",
			ld->name, __builtin_return_address(0));

		if (dpld->dpctl->sleep)
			dpld->dpctl->sleep();
=======
/**
 * reset_txq_circ
 * @dpld: pointer to an instance of dpram_link_device structure
 * @dev: IPC device (IPC_FMT, IPC_RAW, etc.)
 *
 * Empties a TXQ by resetting the head (in) pointer with the value in the tail
 * (out) pointer.
 */
static inline void reset_txq_circ(struct dpram_link_device *dpld, int dev)
{
	struct link_device *ld = &dpld->ld;
	u32 head = get_txq_head(dpld, dev);
	u32 tail = get_txq_tail(dpld, dev);

	mif_info("%s: %s_TXQ: HEAD[%u] <== TAIL[%u]\n",
		ld->name, get_dev_name(dev), head, tail);

	set_txq_head(dpld, dev, tail);
}

/**
 * reset_rxq_circ
 * @dpld: pointer to an instance of dpram_link_device structure
 * @dev: IPC device (IPC_FMT, IPC_RAW, etc.)
 *
 * Empties an RXQ by resetting the tail (out) pointer with the value in the head
 * (in) pointer.
 */
static inline void reset_rxq_circ(struct dpram_link_device *dpld, int dev)
{
	struct link_device *ld = &dpld->ld;
	u32 head = get_rxq_head(dpld, dev);
	u32 tail = get_rxq_tail(dpld, dev);

	mif_info("%s: %s_RXQ: TAIL[%u] <== HEAD[%u]\n",
		ld->name, get_dev_name(dev), tail, head);

	set_rxq_tail(dpld, dev, head);
}

/**
 * check_magic_access
 * @dpld: pointer to an instance of dpram_link_device structure
 *
 * Returns 0 if the "magic code" and "access enable" values are valid, otherwise
 * returns -EACCES.
 */
static int check_magic_access(struct dpram_link_device *dpld)
{
	struct link_device *ld = &dpld->ld;
	int i;
	u16 magic = get_magic(dpld);
	u16 access = get_access(dpld);

	/* Returns 0 if the "magic code" and "access enable" are valid */
	if (likely(magic == DPRAM_MAGIC_CODE && access == 1))
		return 0;

	/* Retry up to 100 times with 100 us delay per each retry */
	for (i = 1; i <= 100; i++) {
		mif_info("%s: magic:%X access:%X -> retry:%d\n",
			ld->name, magic, access, i);
		udelay(100);

		magic = get_magic(dpld);
		access = get_access(dpld);
		if (likely(magic == DPRAM_MAGIC_CODE && access == 1))
			return 0;
	}

	mif_info("%s: !CRISIS! magic:%X access:%X\n", ld->name, magic, access);
	return -EACCES;
}

/**
 * ipc_active
 * @dpld: pointer to an instance of dpram_link_device structure
 *
 * Returns whether or not IPC via the dpram_link_device instance is possible.
 */
static bool ipc_active(struct dpram_link_device *dpld)
{
	struct link_device *ld = &dpld->ld;

	/* Check DPRAM mode */
	if (ld->mode != LINK_MODE_IPC) {
		mif_err("%s: <called by %pf> ERR! ld->mode != LINK_MODE_IPC\n",
			ld->name, CALLER);
		return false;
	}

	/* Check "magic code" and "access enable" values */
	if (check_magic_access(dpld) < 0) {
		mif_err("%s: <called by %pf> ERR! check_magic_access fail\n",
			ld->name, CALLER);
		return false;
	}

	return true;
}

/**
 * get_dpram_status
 * @dpld: pointer to an instance of dpram_link_device structure
 * @dir: direction of communication (TX or RX)
 * @stat: pointer to an instance of mem_status structure
 *
 * Takes a snapshot of the current status of a DPRAM.
 */
static void get_dpram_status(struct dpram_link_device *dpld,
			enum circ_dir_type dir, struct mem_status *stat)
{
#ifdef DEBUG_MODEM_IF
	getnstimeofday(&stat->ts);
#endif

	stat->dir = dir;
	stat->magic = get_magic(dpld);
	stat->access = get_access(dpld);
	stat->head[IPC_FMT][TX] = get_txq_head(dpld, IPC_FMT);
	stat->tail[IPC_FMT][TX] = get_txq_tail(dpld, IPC_FMT);
	stat->head[IPC_FMT][RX] = get_rxq_head(dpld, IPC_FMT);
	stat->tail[IPC_FMT][RX] = get_rxq_tail(dpld, IPC_FMT);
	stat->head[IPC_RAW][TX] = get_txq_head(dpld, IPC_RAW);
	stat->tail[IPC_RAW][TX] = get_txq_tail(dpld, IPC_RAW);
	stat->head[IPC_RAW][RX] = get_rxq_head(dpld, IPC_RAW);
	stat->tail[IPC_RAW][RX] = get_rxq_tail(dpld, IPC_RAW);
	stat->int2ap = recv_int2ap(dpld);
	stat->int2cp = read_int2cp(dpld);
}

#if 0
/**
 * save_ipc_trace_work
 * @work: pointer to an instance of work_struct structure
 *
 * Performs actual file operation for saving RX IPC trace.
 */
static void save_ipc_trace_work(struct work_struct *work)
{
	struct dpram_link_device *dpld;
	struct link_device *ld;
	struct trace_data_queue *trq;
	struct trace_data *trd;
	struct circ_status *stat;
	struct file *fp;
	struct timespec *ts;
	int dev;
	u8 *dump;
	int rcvd;
	u8 *buff;
	char *path;
	struct utc_time utc;
>>>>>>> fc9b728... update12

		udelay(10);

<<<<<<< HEAD
		if (dpld->dpctl->wakeup() < 0) {
			mif_err("%s: ERR! <%pf> DPRAM wakeup fail twice\n",
				ld->name, __builtin_return_address(0));
			return -EACCES;
=======
	buff = kzalloc(dpld->size << 3, GFP_KERNEL);
	if (!buff) {
		while (1) {
			trd = trq_get_data_slot(trq);
			if (!trd)
				break;

			ts = &trd->ts;
			dev = trd->dev;
			stat = &trd->circ_stat;
			dump = trd->data;
			rcvd = trd->size;
			print_ipc_trace(ld, dev, stat, ts, dump, rcvd);

			kfree(dump);
>>>>>>> fc9b728... update12
		}
	}

	atomic_inc(&dpld->accessing);
	return 0;
}

<<<<<<< HEAD
static void dpram_allow_sleep(struct dpram_link_device *dpld)
{
	struct link_device *ld = &dpld->ld;

	if (!dpld->dpctl->sleep)
		return;

	if (atomic_dec_return(&dpld->accessing) <= 0) {
		dpld->dpctl->sleep();
		atomic_set(&dpld->accessing, 0);
		mif_debug("%s: DPRAM sleep possible\n", ld->name);
	}
}

static int dpram_check_access(struct dpram_link_device *dpld)
{
	struct link_device *ld = &dpld->ld;
	int i;
	u16 magic = get_magic(dpld);
	u16 access = get_access(dpld);

	if (likely(magic == DPRAM_MAGIC_CODE && access == 1))
		return 0;

	for (i = 1; i <= 100; i++) {
		mif_info("%s: ERR! magic:%X access:%X -> retry:%d\n",
			ld->name, magic, access, i);
		udelay(100);

		magic = get_magic(dpld);
		access = get_access(dpld);
		if (likely(magic == DPRAM_MAGIC_CODE && access == 1))
			return 0;
	}

	mif_info("%s: !CRISIS! magic:%X access:%X\n", ld->name, magic, access);
	return -EACCES;
}

static bool dpram_ipc_active(struct dpram_link_device *dpld)
{
	struct link_device *ld = &dpld->ld;

	/* Check DPRAM mode */
	if (ld->mode != LINK_MODE_IPC) {
		mif_info("%s: <%pf> ld->mode != LINK_MODE_IPC\n",
			ld->name, __builtin_return_address(0));
		return false;
	}

	if (dpram_check_access(dpld) < 0) {
		mif_info("%s: ERR! <%pf> dpram_check_access fail\n",
			ld->name, __builtin_return_address(0));
		return false;
=======
		ts = &trd->ts;
		dev = trd->dev;
		stat = &trd->circ_stat;
		dump = trd->data;
		rcvd = trd->size;

		ts2utc(ts, &utc);
		snprintf(path, MIF_MAX_PATH_LEN,
			"%s/%s_%s_%d%02d%02d-%02d%02d%02d.lst",
			MIF_LOG_DIR, ld->name, get_dev_name(dev),
			utc.year, utc.mon, utc.day, utc.hour, utc.min, utc.sec);

		fp = mif_open_file(path);
		if (fp) {
			int len;

			snprintf(buff, MIF_MAX_PATH_LEN,
				"[%d-%02d-%02d %02d:%02d:%02d.%03d] "
				"%s %s_RXQ {IN:%u OUT:%u LEN:%d}\n",
				utc.year, utc.mon, utc.day, utc.hour, utc.min,
				utc.sec, utc.msec, ld->name, get_dev_name(dev),
				stat->in, stat->out, stat->size);
			len = strlen(buff);
			mif_dump2format4(dump, rcvd, (buff + len), NULL);
			strcat(buff, "\n");
			len = strlen(buff);

			mif_save_file(fp, buff, len);

			memset(buff, 0, len);
			mif_close_file(fp);
		} else {
			mif_err("%s: %s open fail\n", ld->name, path);
			print_ipc_trace(ld, dev, stat, ts, dump, rcvd);
		}

		kfree(dump);
>>>>>>> fc9b728... update12
	}

	kfree(buff);
}
#endif

<<<<<<< HEAD
static void dpram_ipc_write(struct dpram_link_device *dpld, int dev,
		u32 qsize, u32 in, u32 out, struct sk_buff *skb)
=======
/**
 * set_dpram_map
 * @dpld: pointer to an instance of dpram_link_device structure
 * @map: pointer to an instance of mif_irq_map structure
 *
 * Sets variables in an mif_irq_map instance as current DPRAM status for IPC
 * logging.
 */
static void set_dpram_map(struct dpram_link_device *dpld,
			struct mif_irq_map *map)
{
	map->magic = get_magic(dpld);
	map->access = get_access(dpld);

	map->fmt_tx_in = get_txq_head(dpld, IPC_FMT);
	map->fmt_tx_out = get_txq_tail(dpld, IPC_FMT);
	map->fmt_rx_in = get_rxq_head(dpld, IPC_FMT);
	map->fmt_rx_out = get_rxq_tail(dpld, IPC_FMT);
	map->raw_tx_in = get_txq_head(dpld, IPC_RAW);
	map->raw_tx_out = get_txq_tail(dpld, IPC_RAW);
	map->raw_rx_in = get_rxq_head(dpld, IPC_RAW);
	map->raw_rx_out = get_rxq_tail(dpld, IPC_RAW);

	map->cp2ap = recv_int2ap(dpld);
}

/**
 * dpram_wake_up
 * @dpld: pointer to an instance of dpram_link_device structure
 *
 * Wakes up a DPRAM if it can sleep and increases the "accessing" counter in the
 * dpram_link_device instance.
 *
 * CAUTION!!! dpram_allow_sleep() MUST be invoked after dpram_wake_up() success
 * to decrease the "accessing" counter.
 */
static int dpram_wake_up(struct dpram_link_device *dpld)
>>>>>>> fc9b728... update12
{
	struct link_device *ld = &dpld->ld;
	u8 __iomem *buff = get_tx_buff(dpld, dev);
	u8 *src = skb->data;
	u32 len = skb->len;
	u32 inp;
	struct mif_irq_map map;

<<<<<<< HEAD
	if (in < out) {
		/* +++++++++ in ---------- out ++++++++++ */
		memcpy((buff + in), src, len);
	} else {
		/* ------ out +++++++++++ in ------------ */
		u32 space = qsize - in;

		/* 1) in -> buffer end */
		memcpy((buff + in), src, ((len > space) ? space : len));

		/* 2) buffer start -> out */
		if (len > space)
			memcpy(buff, (src + space), (len - space));
=======
	if (unlikely(!dpld->need_wake_up))
		return 0;

	if (dpld->ext_op->wakeup(dpld) < 0) {
		mif_err("%s: <called by %pf> ERR! wakeup fail\n",
			ld->name, CALLER);
		return -EACCES;
>>>>>>> fc9b728... update12
	}

	/* update new in pointer */
	inp = in + len;
	if (inp >= qsize)
		inp -= qsize;
	set_tx_head(dpld, dev, inp);

	if (dev == IPC_FMT) {
		set_dpram_map(dpld, &map);
		mif_irq_log(ld->mc->msd, map, "ipc_write", sizeof("ipc_write"));
		mif_ipc_log(MIF_IPC_AP2CP, ld->mc->msd, skb->data, skb->len);
	}

	if (ld->aligned && memcmp16_to_io((buff + in), src, 4)) {
		mif_err("%s: memcmp16_to_io fail\n", ld->name);
		trigger_force_cp_crash(dpld);
	}
}

static int dpram_try_ipc_tx(struct dpram_link_device *dpld, int dev)
{
	struct link_device *ld = &dpld->ld;
	struct sk_buff_head *txq = ld->skb_txq[dev];
	struct sk_buff *skb;
	unsigned long int flags;
	u32 qsize = get_tx_buff_size(dpld, dev);
	u32 in;
	u32 out;
	int space;
	int copied = 0;

<<<<<<< HEAD
	spin_lock_irqsave(&dpld->tx_lock[dev], flags);
=======
	if (unlikely(!dpld->need_wake_up))
		return;
>>>>>>> fc9b728... update12

	while (1) {
		space = dpram_get_txq_space(dpld, dev, qsize, &in, &out);
		if (unlikely(space < 0)) {
			spin_unlock_irqrestore(&dpld->tx_lock[dev], flags);
			dpram_reset_tx_circ(dpld, dev);
			return space;
		}

		skb = skb_dequeue(txq);
		if (unlikely(!skb))
			break;

		if (unlikely(space < skb->len)) {
			atomic_set(&dpld->res_required[dev], 1);
			skb_queue_head(txq, skb);
			spin_unlock_irqrestore(&dpld->tx_lock[dev], flags);
			mif_info("%s: %s "
				"qsize[%u] in[%u] out[%u] free[%u] < len[%u]\n",
				ld->name, get_dev_name(dev),
				qsize, in, out, space, skb->len);
			return -ENOSPC;
		}

		/* TX if there is enough room in the queue */
		dpram_ipc_write(dpld, dev, qsize, in, out, skb);
		copied += skb->len;
		dev_kfree_skb_any(skb);
	}

	spin_unlock_irqrestore(&dpld->tx_lock[dev], flags);

	return copied;
}

<<<<<<< HEAD
static void dpram_ipc_rx_task(unsigned long data)
=======
static int capture_dpram_snapshot(struct link_device *ld, struct io_device *iod)
{
	struct dpram_link_device *dpld = to_dpram_link_device(ld);
	struct sk_buff *skb;
	u32 size = dpld->size;
	u32 copied = 0;
	u8 *dump;

	dpram_wake_up(dpld);
	dump = capture_mem_dump(ld, dpld->base, dpld->size);
	dpram_allow_sleep(dpld);

	if (!dump)
		return -ENOMEM;

	while (copied < size) {
		skb = alloc_skb(MAX_DUMP_SKB_SIZE, GFP_ATOMIC);
		if (!skb) {
			mif_err("ERR! alloc_skb fail\n");
			kfree(dump);
			return -ENOMEM;
		}

		skb_put(skb, MAX_DUMP_SKB_SIZE);
		memcpy(skb->data, (dump + copied), MAX_DUMP_SKB_SIZE);
		copied += MAX_DUMP_SKB_SIZE;

		skb_queue_tail(&iod->sk_rx_q, skb);
		wake_up(&iod->wq);
	}

	kfree(dump);
	return 0;
}

/**
 * handle_cp_crash
 * @dpld: pointer to an instance of dpram_link_device structure
 *
 * Actual handler for the CRASH_EXIT command from a CP.
 */
static void handle_cp_crash(struct dpram_link_device *dpld)
{
	struct link_device *ld = &dpld->ld;
	struct io_device *iod;
	int i;

	if (dpld->forced_cp_crash)
		dpld->forced_cp_crash = false;

	/* Stop network interfaces */
	mif_netif_stop(ld);

	/* Purge the skb_txq in every IPC device (IPC_FMT, IPC_RAW, etc.) */
	for (i = 0; i < ld->max_ipc_dev; i++)
		skb_queue_purge(ld->skb_txq[i]);

	/* Change the modem state to STATE_CRASH_EXIT for the FMT IO device */
	iod = link_get_iod_with_format(ld, IPC_FMT);
	if (iod)
		iod->modem_state_changed(iod, STATE_CRASH_EXIT);

	/* Change the modem state to STATE_CRASH_EXIT for the BOOT IO device */
	iod = link_get_iod_with_format(ld, IPC_BOOT);
	if (iod)
		iod->modem_state_changed(iod, STATE_CRASH_EXIT);
}

/**
 * handle_no_cp_crash_ack
 * @arg: pointer to an instance of dpram_link_device structure
 *
 * Invokes handle_cp_crash() to enter the CRASH_EXIT state if there was no
 * CRASH_ACK from a CP in FORCE_CRASH_ACK_TIMEOUT.
 */
static void handle_no_cp_crash_ack(unsigned long arg)
>>>>>>> fc9b728... update12
{
	struct dpram_link_device *dpld = (struct dpram_link_device *)data;
	struct link_device *ld = &dpld->ld;
	struct io_device *iod;
	struct dpram_rxb *rxb;
	unsigned qlen;
	int i;

	for (i = 0; i < dpld->max_ipc_dev; i++) {
		iod = dpld->iod[i];
		qlen = rxbq_size(&dpld->rxbq[i]);
		while (qlen > 0) {
			rxb = rxbq_get_data_rxb(&dpld->rxbq[i]);
			iod->recv(iod, ld, rxb->data, rxb->len);
			rxb_clear(rxb);
			qlen--;
		}
	}
}

<<<<<<< HEAD
static void dpram_ipc_read(struct dpram_link_device *dpld, int dev, u8 *dst,
	u8 __iomem *src, u32 out, u32 len, u32 qsize)
{
	if ((out + len) <= qsize) {
		/* ----- (out)         (in) ----- */
		/* -----   7f 00 00 7e      ----- */
		memcpy(dst, (src + out), len);
	} else {
		/*       (in) ----------- (out)   */
		/* 00 7e      -----------   7f 00 */
		unsigned len1 = qsize - out;

		/* 1) out -> buffer end */
		memcpy(dst, (src + out), len1);

		/* 2) buffer start -> in */
		dst += len1;
		memcpy(dst, src, (len - len1));
=======
/**
 * trigger_forced_cp_crash
 * @dpld: pointer to an instance of dpram_link_device structure
 *
 * Triggers an enforced CP crash.
 */
static void trigger_forced_cp_crash(struct dpram_link_device *dpld)
{
	struct link_device *ld = &dpld->ld;
#ifdef DEBUG_MODEM_IF
	struct trace_data *trd;
	u8 *dump;
	struct timespec ts;
	getnstimeofday(&ts);
#endif

	if (ld->mode == LINK_MODE_ULOAD) {
		mif_err("%s: <called by %pf> ALREADY in progress\n",
			ld->name, CALLER);
		return;
>>>>>>> fc9b728... update12
	}
}

<<<<<<< HEAD
/*
  ret < 0  : error
  ret == 0 : no data
  ret > 0  : valid data
*/
static int dpram_ipc_recv_data_with_rxb(struct dpram_link_device *dpld, int dev)
{
	struct link_device *ld = &dpld->ld;
	struct dpram_rxb *rxb;
	u8 __iomem *src = get_rx_buff(dpld, dev);
	u32 qsize = get_rx_buff_size(dpld, dev);
	u32 in;
	u32 out;
	u32 rcvd;
	struct mif_irq_map map;

	rcvd = dpram_get_rxq_rcvd(dpld, dev, qsize, &in, &out);
	if (rcvd <= 0)
		return rcvd;

	if (dev == IPC_FMT) {
		set_dpram_map(dpld, &map);
		mif_irq_log(ld->mc->msd, map, "ipc_recv", sizeof("ipc_recv"));
	}
=======
	ld->mode = LINK_MODE_ULOAD;
	dpld->forced_cp_crash = true;

	disable_irq_nosync(dpld->irq);

	dpram_wake_up(dpld);

#ifdef DEBUG_MODEM_IF
	dump = capture_mem_dump(ld, dpld->base, dpld->size);
	if (dump) {
		trd = trq_get_free_slot(&dpld->trace_list);
		memcpy(&trd->ts, &ts, sizeof(struct timespec));
		trd->dev = IPC_DEBUG;
		trd->data = dump;
		trd->size = dpld->size;
	}
#endif
>>>>>>> fc9b728... update12

	/* Allocate an rxb */
	rxb = rxbq_get_free_rxb(&dpld->rxbq[dev]);
	if (!rxb) {
		mif_info("%s: ERR! %s rxbq_get_free_rxb fail\n",
			ld->name, get_dev_name(dev));
		return -ENOMEM;
	}

<<<<<<< HEAD
	/* Read data from each DPRAM buffer */
	dpram_ipc_read(dpld, dev, rxb_put(rxb, rcvd), src, out, rcvd, qsize);
=======
	mif_err("%s: <called by %pf>\n", ld->name, CALLER);

	/* Send CRASH_EXIT command to a CP */
	send_int2cp(dpld, INT_CMD(INT_CMD_CRASH_EXIT));
	get_dpram_status(dpld, TX, msq_get_free_slot(&dpld->stat_list));
>>>>>>> fc9b728... update12

	/* Calculate and set new out */
	out += rcvd;
	if (out >= qsize)
		out -= qsize;
	set_rx_tail(dpld, dev, out);

	return rcvd;
}

/*
  ret < 0  : error
  ret == 0 : no data
  ret > 0  : valid data
*/
static int dpram_ipc_recv_data_with_skb(struct dpram_link_device *dpld, int dev)
{
	struct link_device *ld = &dpld->ld;
	struct io_device *iod = dpld->iod[dev];
	struct sk_buff *skb;
	u8 __iomem *src = get_rx_buff(dpld, dev);
	u32 qsize = get_rx_buff_size(dpld, dev);
	u32 in;
	u32 out;
	u32 rcvd;
	int rest;
	u8 *frm;
	u8 *dst;
	unsigned int len;
	unsigned int pad;
	unsigned int tot;
	struct mif_irq_map map;

<<<<<<< HEAD
	rcvd = dpram_get_rxq_rcvd(dpld, dev, qsize, &in, &out);
	if (rcvd <= 0)
		return rcvd;

	if (dev == IPC_FMT) {
		set_dpram_map(dpld, &map);
		mif_irq_log(ld->mc->msd, map, "ipc_recv", sizeof("ipc_recv"));
	}

	rest = rcvd;
	while (rest > 0) {
		frm = src + out;
		if (unlikely(!sipc5_start_valid(frm[0]))) {
			mif_err("%s: ERR! %s invalid start 0x%02X\n",
				ld->name, get_dev_name(dev), frm[0]);
			skb_queue_purge(&dpld->skb_rxq[dev]);
			return -EBADMSG;
=======
	switch (EXT_CMD_MASK(cmd)) {
	case EXT_CMD_SET_SPEED_LOW:
		if (dpld->dpram->setup_speed) {
			dpld->dpram->setup_speed(DPRAM_SPEED_LOW);
			resp = INT_EXT_CMD(EXT_CMD_SET_SPEED_LOW);
			send_int2cp(dpld, resp);
		}
		break;

	case EXT_CMD_SET_SPEED_MID:
		if (dpld->dpram->setup_speed) {
			dpld->dpram->setup_speed(DPRAM_SPEED_MID);
			resp = INT_EXT_CMD(EXT_CMD_SET_SPEED_MID);
			send_int2cp(dpld, resp);
>>>>>>> fc9b728... update12
		}

<<<<<<< HEAD
		len = sipc5_get_frame_sz16(frm);
		if (unlikely(len > rest)) {
			mif_err("%s: ERR! %s len %d > rest %d\n",
				ld->name, get_dev_name(dev), len, rest);
			skb_queue_purge(&dpld->skb_rxq[dev]);
			return -EBADMSG;
=======
	case EXT_CMD_SET_SPEED_HIGH:
		if (dpld->dpram->setup_speed) {
			dpld->dpram->setup_speed(DPRAM_SPEED_HIGH);
			resp = INT_EXT_CMD(EXT_CMD_SET_SPEED_HIGH);
			send_int2cp(dpld, resp);
>>>>>>> fc9b728... update12
		}

		pad = sipc5_calc_padding_size(len);
		tot = len + pad;

		/* Allocate an skb */
		skb = dev_alloc_skb(tot);
		if (!skb) {
			mif_err("%s: ERR! %s dev_alloc_skb fail\n",
				ld->name, get_dev_name(dev));
			return -ENOMEM;
		}

<<<<<<< HEAD
		/* Read data from each DPRAM buffer */
		dst = skb_put(skb, tot);
		dpram_ipc_read(dpld, dev, dst, src, out, tot, qsize);
		skb_trim(skb, len);
		iod->recv_skb(iod, ld, skb);

		/* Calculate and set new out */
		rest -= tot;
		out += tot;
		if (out >= qsize)
			out -= qsize;
=======
	if (cmd & UDL_RESULT_FAIL) {
		mif_err("%s: ERR! command fail (0x%04X)\n", ld->name, cmd);
		return;
	}

	switch (UDL_CMD_MASK(cmd)) {
	case UDL_CMD_RECV_READY:
		mif_err("%s: [CP->AP] CMD_DL_READY (0x%04X)\n", ld->name, cmd);
#ifdef CONFIG_CDMA_MODEM_CBP72
		mif_err("%s: [AP->CP] CMD_DL_START_REQ (0x%04X)\n",
			ld->name, CMD_DL_START_REQ);
		send_int2cp(dpld, CMD_DL_START_REQ);
#else
		complete(&dpld->udl_cmpl);
#endif
		break;

	default:
		complete(&dpld->udl_cmpl);
>>>>>>> fc9b728... update12
	}

	set_rx_tail(dpld, dev, out);

	return rcvd;
}

static void non_command_handler(struct dpram_link_device *dpld, u16 intr)
{
	struct link_device *ld = &dpld->ld;
	int i = 0;
	int ret = 0;
	u16 tx_mask = 0;

	if (!dpram_ipc_active(dpld))
		return;

	/* Read data from DPRAM */
	for (i = 0; i < dpld->max_ipc_dev; i++) {
		if (dpld->use_skb)
			ret = dpram_ipc_recv_data_with_skb(dpld, i);
		else
			ret = dpram_ipc_recv_data_with_rxb(dpld, i);
		if (ret < 0)
			dpram_reset_rx_circ(dpld, i);

		/* Check and process REQ_ACK (at this time, in == out) */
		if (intr & get_mask_req_ack(dpld, i)) {
			mif_debug("%s: send %s_RES_ACK\n",
				ld->name, get_dev_name(i));
			tx_mask |= get_mask_res_ack(dpld, i);
		}
	}

	if (!dpld->use_skb) {
		/* Schedule soft IRQ for RX */
		tasklet_hi_schedule(&dpld->rx_tsk);
	}

	/* Try TX via DPRAM */
	for (i = 0; i < dpld->max_ipc_dev; i++) {
		if (atomic_read(&dpld->res_required[i]) > 0) {
			ret = dpram_try_ipc_tx(dpld, i);
			if (ret > 0) {
				atomic_set(&dpld->res_required[i], 0);
				tx_mask |= get_mask_send(dpld, i);
			} else if (ret == -ENOSPC) {
				tx_mask |= get_mask_req_ack(dpld, i);
			}
		}
	}

	if (tx_mask) {
		send_intr(dpld, INT_NON_CMD(tx_mask));
		mif_debug("%s: send intr 0x%04X\n", ld->name, tx_mask);
	}
}

<<<<<<< HEAD
static void handle_cp_crash(struct dpram_link_device *dpld)
{
	struct link_device *ld = &dpld->ld;
	struct io_device *iod;
	int i;

	for (i = 0; i < dpld->max_ipc_dev; i++) {
		mif_info("%s: purging %s_skb_txq\b", ld->name, get_dev_name(i));
		skb_queue_purge(ld->skb_txq[i]);
	}

	iod = link_get_iod_with_format(ld, IPC_FMT);
	iod->modem_state_changed(iod, STATE_CRASH_EXIT);

	iod = link_get_iod_with_format(ld, IPC_BOOT);
	iod->modem_state_changed(iod, STATE_CRASH_EXIT);

	iod = link_get_iod_with_channel(ld, PS_DATA_CH_0);
	if (iod)
		iodevs_for_each(iod->msd, iodev_netif_stop, 0);
}

static void handle_no_crash_ack(unsigned long arg)
=======
/**
 * cmd_crash_exit_handler
 * @dpld: pointer to an instance of dpram_link_device structure
 *
 * Handles the CRASH_EXIT command from a CP.
 */
static void cmd_crash_exit_handler(struct dpram_link_device *dpld)
>>>>>>> fc9b728... update12
{
	struct dpram_link_device *dpld = (struct dpram_link_device *)arg;
	struct link_device *ld = &dpld->ld;
#ifdef DEBUG_MODEM_IF
	struct trace_data *trd;
	u8 *dump;
	struct timespec ts;
	getnstimeofday(&ts);
#endif

	mif_err("%s: ERR! No CRASH_EXIT ACK from CP\n", ld->mc->name);

	if (!wake_lock_active(&dpld->wlock))
		wake_lock(&dpld->wlock);

	handle_cp_crash(dpld);
}

<<<<<<< HEAD
static int trigger_force_cp_crash(struct dpram_link_device *dpld)
{
	struct link_device *ld = &dpld->ld;
=======
	dpram_wake_up(dpld);

#ifdef DEBUG_MODEM_IF
	if (!dpld->forced_cp_crash) {
		dump = capture_mem_dump(ld, dpld->base, dpld->size);
		if (dump) {
			trd = trq_get_free_slot(&dpld->trace_list);
			memcpy(&trd->ts, &ts, sizeof(struct timespec));
			trd->dev = IPC_DEBUG;
			trd->data = dump;
			trd->size = dpld->size;
		}
	}
#endif
>>>>>>> fc9b728... update12

	if (ld->mode == LINK_MODE_ULOAD) {
		mif_err("%s: CP crash is already in progress\n", ld->mc->name);
		return 0;
	}

	ld->mode = LINK_MODE_ULOAD;
	mif_err("%s: called by %pf\n", ld->name, __builtin_return_address(0));

	if (dpld->dp_type == CP_IDPRAM)
		dpram_wake_up(dpld);

	send_intr(dpld, INT_CMD(INT_CMD_CRASH_EXIT));

	mif_add_timer(&dpld->crash_ack_timer, FORCE_CRASH_ACK_TIMEOUT,
			handle_no_crash_ack, (unsigned long)dpld);

	return 0;
}

static int dpram_init_ipc(struct dpram_link_device *dpld)
{
	struct link_device *ld = &dpld->ld;
	int i;

	if (ld->mode == LINK_MODE_IPC &&
	    get_magic(dpld) == DPRAM_MAGIC_CODE &&
	    get_access(dpld) == 1)
		mif_info("%s: IPC already initialized\n", ld->name);

	/* Clear pointers in every circular queue */
	for (i = 0; i < dpld->max_ipc_dev; i++) {
		set_tx_head(dpld, i, 0);
		set_tx_tail(dpld, i, 0);
		set_rx_head(dpld, i, 0);
		set_rx_tail(dpld, i, 0);
	}

	/* Initialize variables for efficient TX/RX processing */
	for (i = 0; i < dpld->max_ipc_dev; i++)
		dpld->iod[i] = link_get_iod_with_format(ld, i);
	dpld->iod[IPC_RAW] = link_get_iod_with_format(ld, IPC_MULTI_RAW);

<<<<<<< HEAD
	if (dpld->iod[IPC_RAW]->recv_skb)
		dpld->use_skb = true;

	for (i = 0; i < dpld->max_ipc_dev; i++) {
		spin_lock_init(&dpld->tx_lock[i]);
		atomic_set(&dpld->res_required[i], 0);
		skb_queue_purge(&dpld->skb_rxq[i]);
	}
=======
	/* Initialize variables for TX flow control */
	for (i = 0; i < ld->max_ipc_dev; i++)
		atomic_set(&dpld->res_required[i], 0);
>>>>>>> fc9b728... update12

	/* Enable IPC */
	atomic_set(&dpld->accessing, 0);

	set_magic(dpld, DPRAM_MAGIC_CODE);
	set_access(dpld, 1);
	if (get_magic(dpld) != DPRAM_MAGIC_CODE || get_access(dpld) != 1)
		return -EACCES;

	ld->mode = LINK_MODE_IPC;

	if (wake_lock_active(&dpld->wlock))
		wake_unlock(&dpld->wlock);

	return 0;
}

<<<<<<< HEAD
static void cmd_req_active_handler(struct dpram_link_device *dpld)
{
	send_intr(dpld, INT_CMD(INT_CMD_RES_ACTIVE));
}

static void cmd_crash_reset_handler(struct dpram_link_device *dpld)
{
	struct link_device *ld = &dpld->ld;
	struct io_device *iod = NULL;

	ld->mode = LINK_MODE_ULOAD;

	if (!wake_lock_active(&dpld->wlock))
		wake_lock(&dpld->wlock);

	mif_err("%s: Recv 0xC7 (CRASH_RESET)\n", ld->name);

	iod = link_get_iod_with_format(ld, IPC_FMT);
	iod->modem_state_changed(iod, STATE_CRASH_RESET);

	iod = link_get_iod_with_format(ld, IPC_BOOT);
	iod->modem_state_changed(iod, STATE_CRASH_RESET);
}

static void cmd_crash_exit_handler(struct dpram_link_device *dpld)
{
	struct link_device *ld = &dpld->ld;
	u32 size = dpld->dpctl->dp_size;
	char *dpram_buff = NULL;

	ld->mode = LINK_MODE_ULOAD;

	if (!wake_lock_active(&dpld->wlock))
		wake_lock(&dpld->wlock);

	mif_err("%s: Recv 0xC9 (CRASH_EXIT)\n", ld->name);

	if (dpld->dp_type == CP_IDPRAM)
		dpram_wake_up(dpld);

	dpram_buff = kzalloc(size + (MAX_MIF_SEPA_SIZE * 2), GFP_ATOMIC);
	if (!dpram_buff) {
		mif_err("DPRAM dump failed!!\n");
	} else {
		memset(dpram_buff, 0, size + (MAX_MIF_SEPA_SIZE * 2));
		memcpy(dpram_buff, MIF_SEPARATOR_DPRAM, MAX_MIF_SEPA_SIZE);
		memcpy(dpram_buff + MAX_MIF_SEPA_SIZE, &size, sizeof(u32));
		dpram_buff += (MAX_MIF_SEPA_SIZE * 2);
		dpram_dump_memory(ld, dpram_buff);
	}

	del_timer(&dpld->crash_ack_timer);

	if (dpld->ext_op && dpld->ext_op->crash_log)
		dpld->ext_op->crash_log(dpld);

	handle_cp_crash(dpld);
}

=======
/**
 * reset_dpram_ipc
 * @dpld: pointer to an instance of dpram_link_device structure
 *
 * Reset DPRAM with IPC map.
 */
static void reset_dpram_ipc(struct dpram_link_device *dpld)
{
	int i;
	struct link_device *ld = &dpld->ld;

	dpld->set_access(dpld, 0);

	/* Clear pointers in every circular queue */
	for (i = 0; i < ld->max_ipc_dev; i++) {
		dpld->set_txq_head(dpld, i, 0);
		dpld->set_txq_tail(dpld, i, 0);
		dpld->set_rxq_head(dpld, i, 0);
		dpld->set_rxq_tail(dpld, i, 0);
	}

	dpld->set_magic(dpld, DPRAM_MAGIC_CODE);
	dpld->set_access(dpld, 1);
}

/**
 * cmd_phone_start_handler
 * @dpld: pointer to an instance of dpram_link_device structure
 *
 * Handles the PHONE_START command from a CP.
 */
>>>>>>> fc9b728... update12
static void cmd_phone_start_handler(struct dpram_link_device *dpld)
{
	struct link_device *ld = &dpld->ld;
	struct io_device *iod;

<<<<<<< HEAD
	dpram_init_ipc(dpld);
=======
	mif_err("%s: Recv 0xC8 (CP_START)\n", ld->name);
>>>>>>> fc9b728... update12

	iod = link_get_iod_with_format(ld, IPC_FMT);
	if (!iod) {
		mif_err("%s: ERR! no iod\n", ld->name);
		return;
	}

	init_dpram_ipc(dpld);

	iod->modem_state_changed(iod, STATE_ONLINE);

<<<<<<< HEAD
	mif_info("%s: Send 0xC2 (INIT_END)\n", ld->name);
	send_intr(dpld, INT_CMD(INT_CMD_INIT_END));
}

static void command_handler(struct dpram_link_device *dpld, u16 cmd)
=======
	if (dpld->ext_op && dpld->ext_op->cp_start_handler) {
		dpld->ext_op->cp_start_handler(dpld);
	} else {
		mif_err("%s: Send 0xC2 (INIT_END)\n", ld->name);
		send_int2cp(dpld, INT_CMD(INT_CMD_INIT_END));
	}
}

/**
 * cmd_handler: processes a DPRAM command from a CP
 * @dpld: pointer to an instance of dpram_link_device structure
 * @cmd: DPRAM command from a CP
 */
static void cmd_handler(struct dpram_link_device *dpld, u16 cmd)
>>>>>>> fc9b728... update12
{
	struct link_device *ld = &dpld->ld;

	switch (INT_CMD_MASK(cmd)) {
	case INT_CMD_REQ_ACTIVE:
		cmd_req_active_handler(dpld);
		break;

	case INT_CMD_CRASH_RESET:
		dpld->dpram_init_status = DPRAM_INIT_STATE_NONE;
		cmd_crash_reset_handler(dpld);
		break;

	case INT_CMD_CRASH_EXIT:
		dpld->dpram_init_status = DPRAM_INIT_STATE_NONE;
		cmd_crash_exit_handler(dpld);
		break;

	case INT_CMD_PHONE_START:
		dpld->dpram_init_status = DPRAM_INIT_STATE_READY;
		cmd_phone_start_handler(dpld);
		complete_all(&ld->init_cmpl);
		break;

	case INT_CMD_NV_REBUILDING:
		mif_info("%s: NV_REBUILDING\n", ld->name);
		break;

	case INT_CMD_PIF_INIT_DONE:
		complete_all(&ld->pif_cmpl);
		break;

	case INT_CMD_SILENT_NV_REBUILDING:
		mif_info("%s: SILENT_NV_REBUILDING\n", ld->name);
		break;

	case INT_CMD_NORMAL_POWER_OFF:
		/*ToDo:*/
		/*kernel_sec_set_cp_ack()*/;
		break;

	case INT_CMD_REQ_TIME_SYNC:
	case INT_CMD_CP_DEEP_SLEEP:
	case INT_CMD_EMER_DOWN:
		break;
<<<<<<< HEAD
=======

	default:
		mif_info("%s: unknown command 0x%04X\n", ld->name, cmd);
	}
}

/**
 * ipc_rx_work
 * @work: pointer to an instance of the work_struct structure
 *
 * Invokes the recv method in the io_device instance to perform receiving IPC
 * messages from each skb.
 */
static void ipc_rx_work(struct work_struct *work)
{
	struct dpram_link_device *dpld;
	struct link_device *ld;
	struct io_device *iod;
	struct sk_buff *skb;
	int i;

	dpld = container_of(work, struct dpram_link_device, rx_dwork.work);
	ld = &dpld->ld;

	for (i = 0; i < ld->max_ipc_dev; i++) {
		iod = dpld->iod[i];
		while (1) {
			skb = skb_dequeue(ld->skb_rxq[i]);
			if (!skb)
				break;

			if (iod->recv_skb) {
				iod->recv_skb(iod, ld, skb);
			} else {
				iod->recv(iod, ld, skb->data, skb->len);
				dev_kfree_skb_any(skb);
			}
		}
	}
}

/**
 * get_rxq_rcvd
 * @dpld: pointer to an instance of dpram_link_device structure
 * @dev: IPC device (IPC_FMT, IPC_RAW, etc.)
 * @mst: pointer to an instance of mem_status structure
 * OUT @dcst: pointer to an instance of circ_status structure
 *
 * Stores {start address of the buffer in a RXQ, size of the buffer, in & out
 * pointer values, size of received data} into the 'stat' instance.
 *
 * Returns an error code.
 */
static int get_rxq_rcvd(struct dpram_link_device *dpld, int dev,
			struct mem_status *mst, struct circ_status *dcst)
{
	struct link_device *ld = &dpld->ld;

	dcst->buff = get_rxq_buff(dpld, dev);
	dcst->qsize = get_rxq_buff_size(dpld, dev);
	dcst->in = mst->head[dev][RX];
	dcst->out = mst->tail[dev][RX];
	dcst->size = circ_get_usage(dcst->qsize, dcst->in, dcst->out);

	if (circ_valid(dcst->qsize, dcst->in, dcst->out)) {
		mif_debug("%s: %s_RXQ qsize[%u] in[%u] out[%u] rcvd[%u]\n",
			ld->name, get_dev_name(dev), dcst->qsize, dcst->in,
			dcst->out, dcst->size);
		return 0;
	} else {
		mif_err("%s: ERR! %s_RXQ invalid (qsize[%d] in[%d] out[%d])\n",
			ld->name, get_dev_name(dev), dcst->qsize, dcst->in,
			dcst->out);
		return -EIO;
	}
}

/**
 * rx_sipc4_frames
 * @dpld: pointer to an instance of dpram_link_device structure
 * @dev: IPC device (IPC_FMT, IPC_RAW, etc.)
 * @mst: pointer to an instance of mem_status structure
 *
 * Returns
 *   ret < 0  : error
 *   ret == 0 : ILLEGAL status
 *   ret > 0  : valid data
 *
 * Must be invoked only when there is data in the corresponding RXQ.
 *
 * Requires a bottom half (e.g. ipc_rx_task) that will invoke the recv method in
 * the io_device instance.
 */
static int rx_sipc4_frames(struct dpram_link_device *dpld, int dev,
			struct mem_status *mst)
{
	struct link_device *ld = &dpld->ld;
	struct sk_buff *skb;
	u8 *dst;
	struct circ_status dcst;
	int rcvd;

	rcvd = get_rxq_rcvd(dpld, dev, mst, &dcst);
	if (unlikely(rcvd < 0)) {
#ifdef DEBUG_MODEM_IF
		trigger_forced_cp_crash(dpld);
#endif
		goto exit;
	}
	rcvd = dcst.size;

	/* Allocate an skb */
	skb = dev_alloc_skb(rcvd);
	if (!skb) {
		mif_info("%s: ERR! %s dev_alloc_skb fail\n",
			ld->name, get_dev_name(dev));
		rcvd = -ENOMEM;
		goto exit;
	}

	/* Read data from the RXQ */
	dst = skb_put(skb, rcvd);
	circ_read16_from_io(dst, dcst.buff, dcst.qsize, dcst.out, rcvd);

	/* Store the skb to the corresponding skb_rxq */
	skb_queue_tail(ld->skb_rxq[dev], skb);

exit:
	/* Update tail (out) pointer to empty out the RXQ */
	set_rxq_tail(dpld, dev, dcst.in);

	return rcvd;
}

/**
 * rx_sipc5_frames
 * @dpld: pointer to an instance of dpram_link_device structure
 * @dev: IPC device (IPC_FMT, IPC_RAW, etc.)
 * @mst: pointer to an instance of mem_status structure
 *
 * Returns
 *   ret < 0  : error
 *   ret == 0 : ILLEGAL status
 *   ret > 0  : valid data
 *
 * Must be invoked only when there is data in the corresponding RXQ.
 *
 * Requires a recv_skb method in the io_device instance, so this function must
 * be used for only SIPC5.
 */
static int rx_sipc5_frames(struct dpram_link_device *dpld, int dev,
			struct mem_status *mst)
{
	struct link_device *ld = &dpld->ld;
	struct sk_buff *skb;
	/**
	 * variables for the status of the circular queue
	 */
	u8 __iomem *src;
	u8 hdr[SIPC5_MIN_HEADER_SIZE];
	struct circ_status dcst;
	/**
	 * variables for RX processing
	 */
	int qsize;	/* size of the queue			*/
	int rcvd;	/* size of data in the RXQ or error	*/
	int rest;	/* size of the rest data		*/
	int idx;	/* index to the start of current frame	*/
	u8 *frm;	/* pointer to current frame		*/
	u8 *dst;	/* pointer to the destination buffer	*/
	int tot;	/* total length including padding data	*/
	/**
	 * variables for debug logging
	 */
	struct mif_irq_map map;

	/* Get data size in the RXQ and in/out pointer values */
	rcvd = get_rxq_rcvd(dpld, dev, mst, &dcst);
	if (unlikely(rcvd < 0)) {
		mif_err("%s: ERR! rcvd %d < 0\n", ld->name, rcvd);
		goto exit;
	}

	rcvd = dcst.size;
	src = dcst.buff;
	qsize = dcst.qsize;
	idx = dcst.out;

	if (dev == IPC_FMT) {
		set_dpram_map(dpld, &map);
		mif_irq_log(ld->mc->msd, map, "ipc_recv", sizeof("ipc_recv"));
	}

#if 0
	skb = dev_alloc_skb(rcvd);

	/*
	** If there is enough free space for an skb to store received
	** data at once,
	*/
	if (skb) {
		/* Read all data from the RXQ to the skb */
		dst = skb_put(skb, rcvd);
		if (unlikely(dpld->strict_io_access))
			circ_read16_from_io(dst, src, qsize, idx, rcvd);
		else
			circ_read(dst, src, qsize, idx, rcvd);

#ifdef DEBUG_MODEM_IF
		/* Verify data copied to the skb */
		if (ld->aligned && memcmp16_to_io((src + idx), dst, 4)) {
			mif_err("%s: memcmp16_to_io fail\n", ld->name);
			rcvd = -EIO;
			goto exit;
		}
#endif

		/* Store the skb to the corresponding skb_rxq */
		skb_queue_tail(ld->skb_rxq[dev], skb);

		goto exit;
	}

	/*
	** If there was no enough space to store received data at once,
	*/
#endif

	rest = rcvd;
	while (rest > 0) {
		/* Calculate the start of an SIPC5 frame */
		frm = src + idx;

		/* Copy the header in the frame to the header buffer */
		if (unlikely(dpld->strict_io_access))
			memcpy16_from_io(hdr, frm, SIPC5_MIN_HEADER_SIZE);
		else
			memcpy(hdr, frm, SIPC5_MIN_HEADER_SIZE);

		/* Check the config field in the header */
		if (unlikely(!sipc5_start_valid(hdr))) {
			char str[MIF_MAX_STR_LEN];
			snprintf(str, MIF_MAX_STR_LEN, "%s: BAD CONFIG",
				ld->mc->name);
			mif_err("%s: ERR! %s INVALID config 0x%02X\n",
				ld->name, get_dev_name(dev), hdr[0]);
			pr_ipc(1, str, hdr, 4);
			rcvd = -EBADMSG;
			goto exit;
		}

		/* Verify the total length of the frame (data + padding) */
		tot = sipc5_get_total_len(hdr);
		if (unlikely(tot > rest)) {
			char str[MIF_MAX_STR_LEN];
			snprintf(str, MIF_MAX_STR_LEN, "%s: BAD LENGTH",
				ld->mc->name);
			mif_err("%s: ERR! %s tot %d > rest %d\n",
				ld->name, get_dev_name(dev), tot, rest);
			pr_ipc(1, str, hdr, 4);
			rcvd = -EBADMSG;
			goto exit;
		}

		/* Allocate an skb */
		skb = dev_alloc_skb(tot);
		if (!skb) {
			mif_err("%s: ERR! %s dev_alloc_skb fail\n",
				ld->name, get_dev_name(dev));
			rcvd = -ENOMEM;
			goto exit;
		}

		/* Set the attribute of the skb as "single frame" */
		skbpriv(skb)->single_frame = true;

		/* Read the frame from the RXQ */
		dst = skb_put(skb, tot);
		if (unlikely(dpld->strict_io_access))
			circ_read16_from_io(dst, src, qsize, idx, tot);
		else
			circ_read(dst, src, qsize, idx, tot);

#ifdef DEBUG_MODEM_IF
		/* Take a log for debugging */
		if (unlikely(dev == IPC_FMT)) {
			size_t len = (skb->len > 32) ? 32 : skb->len;
			char str[MIF_MAX_STR_LEN];
			snprintf(str, MIF_MAX_STR_LEN, "%s: CP2MIF",
				ld->mc->name);
			pr_ipc(0, str, skb->data, len);
		}
#endif

#ifdef DEBUG_MODEM_IF
		/* Verify data copied to the skb */
		if (ld->aligned && memcmp16_to_io((src + idx), dst, 4)) {
			mif_err("%s: memcmp16_to_io fail\n", ld->name);
			rcvd = -EIO;
			goto exit;
		}
#endif

		/* Store the skb to the corresponding skb_rxq */
		skb_queue_tail(ld->skb_rxq[dev], skb);

		/* Calculate new idx value */
		rest -= tot;
		idx += tot;
		if (unlikely(idx >= qsize))
			idx -= qsize;
	}

exit:
#ifdef DEBUG_MODEM_IF
	if (rcvd < 0)
		trigger_forced_cp_crash(dpld);
#endif

	/* Update tail (out) pointer to empty out the RXQ */
	set_rxq_tail(dpld, dev, dcst.in);

	return rcvd;
}

/**
 * msg_handler: receives IPC messages from every RXQ
 * @dpld: pointer to an instance of dpram_link_device structure
 * @stat: pointer to an instance of mem_status structure
 *
 * 1) Receives all IPC message frames currently in every DPRAM RXQ.
 * 2) Sends RES_ACK responses if there are REQ_ACK requests from a CP.
 * 3) Completes all threads waiting for the corresponding RES_ACK from a CP if
 *    there is any RES_ACK response.
 */
static void msg_handler(struct dpram_link_device *dpld, struct mem_status *stat)
{
	struct link_device *ld = &dpld->ld;
	int i = 0;
	int ret = 0;
	u16 mask = 0;
	u16 intr = stat->int2ap;

	if (!ipc_active(dpld))
		return;

	/* Read data from DPRAM */
	for (i = 0; i < ld->max_ipc_dev; i++) {
		/* Invoke an RX function only when there is data in the RXQ */
		if (unlikely(stat->head[i][RX] == stat->tail[i][RX])) {
			mif_debug("%s: %s_RXQ is empty\n",
				ld->name, get_dev_name(i));
		} else {
			if (unlikely(ld->ipc_version < SIPC_VER_50))
				ret = rx_sipc4_frames(dpld, i, stat);
			else
				ret = rx_sipc5_frames(dpld, i, stat);
			if (ret < 0)
				reset_rxq_circ(dpld, i);
		}
	}

	/* Schedule soft IRQ for RX */
	queue_delayed_work(system_nrt_wq, &dpld->rx_dwork, 0);

	/* Check and process REQ_ACK (at this time, in == out) */
	if (unlikely(intr & INT_MASK_REQ_ACK_SET)) {
		for (i = 0; i < ld->max_ipc_dev; i++) {
			if (intr & get_mask_req_ack(dpld, i)) {
				mif_debug("%s: set %s_RES_ACK\n",
					ld->name, get_dev_name(i));
				mask |= get_mask_res_ack(dpld, i);
			}
		}

		send_int2cp(dpld, INT_NON_CMD(mask));
	}

	/* Check and process RES_ACK */
	if (unlikely(intr & INT_MASK_RES_ACK_SET)) {
		for (i = 0; i < ld->max_ipc_dev; i++) {
			if (intr & get_mask_res_ack(dpld, i)) {
#ifdef DEBUG_MODEM_IF
				mif_info("%s: recv %s_RES_ACK\n",
					ld->name, get_dev_name(i));
				print_circ_status(ld, i, stat);
#endif
				complete(&dpld->req_ack_cmpl[i]);
			}
		}
	}
}

/**
 * cmd_msg_handler: processes a DPRAM command or receives IPC messages
 * @dpld: pointer to an instance of dpram_link_device structure
 * @stat: pointer to an instance of mem_status structure
 *
 * Invokes cmd_handler for a DPRAM command or msg_handler for IPC messages.
 */
static inline void cmd_msg_handler(struct dpram_link_device *dpld,
			struct mem_status *stat)
{
	struct dpram_ext_op *ext_op = dpld->ext_op;
	struct mem_status *mst = msq_get_free_slot(&dpld->stat_list);
	u16 intr = stat->int2ap;

	memcpy(mst, stat, sizeof(struct mem_status));

	if (unlikely(INT_CMD_VALID(intr))) {
		if (ext_op && ext_op->cmd_handler)
			ext_op->cmd_handler(dpld, intr);
		else
			cmd_handler(dpld, intr);
	} else {
		msg_handler(dpld, stat);
	}
}

/**
 * intr_handler: processes an interrupt from a CP
 * @dpld: pointer to an instance of dpram_link_device structure
 * @stat: pointer to an instance of mem_status structure
 *
 * Call flow for normal interrupt handling:
 *   cmd_msg_handler -> cmd_handler -> cmd_xxx_handler
 *   cmd_msg_handler -> msg_handler -> rx_sipc5_frames ->  ...
 */
static inline void intr_handler(struct dpram_link_device *dpld,
			struct mem_status *stat)
{
	char *name = dpld->ld.name;
	u16 intr = stat->int2ap;

	if (unlikely(intr == INT_POWERSAFE_FAIL)) {
		mif_info("%s: intr == INT_POWERSAFE_FAIL\n", name);
		return;
	}

	if (unlikely(EXT_UDL_CMD(intr))) {
		if (likely(EXT_INT_VALID(intr))) {
			if (UDL_CMD_VALID(intr))
				udl_command_handler(dpld, intr);
			else if (EXT_CMD_VALID(intr))
				ext_command_handler(dpld, intr);
			else
				mif_info("%s: ERR! invalid intr 0x%04X\n",
					name, intr);
		} else {
			mif_err("%s: ERR! invalid intr 0x%04X\n", name, intr);
		}

		return;
	}

	if (likely(INT_VALID(intr)))
		cmd_msg_handler(dpld, stat);
	else
		mif_err("%s: ERR! invalid intr 0x%04X\n", name, intr);
}

/**
 * ap_idpram_irq_handler: interrupt handler for an internal DPRAM in an AP
 * @irq: IRQ number
 * @data: pointer to a data
 *
 * 1) Reads the interrupt value
 * 2) Performs interrupt handling
 */
static irqreturn_t ap_idpram_irq_handler(int irq, void *data)
{
	struct dpram_link_device *dpld = (struct dpram_link_device *)data;
	struct link_device *ld = (struct link_device *)&dpld->ld;
	struct modemlink_dpram_data *dpram = dpld->dpram;
	struct mem_status stat;

	if (unlikely(ld->mode == LINK_MODE_OFFLINE))
		return IRQ_HANDLED;

	get_dpram_status(dpld, RX, &stat);

	intr_handler(dpld, &stat);

	if (likely(dpram->clear_int2ap))
		dpram->clear_int2ap();

	return IRQ_HANDLED;
}

/**
 * cp_idpram_irq_handler: interrupt handler for an internal DPRAM in a CP
 * @irq: IRQ number
 * @data: pointer to a data
 *
 * 1) Wakes up the DPRAM
 * 2) Reads the interrupt value
 * 3) Performs interrupt handling
 * 4) Clears the interrupt port (port = memory or register)
 * 5) Allows the DPRAM to sleep
 */
static irqreturn_t cp_idpram_irq_handler(int irq, void *data)
{
	struct dpram_link_device *dpld = (struct dpram_link_device *)data;
	struct link_device *ld = (struct link_device *)&dpld->ld;
	struct dpram_ext_op *ext_op = dpld->ext_op;
	struct mem_status stat;

	if (unlikely(ld->mode == LINK_MODE_OFFLINE)) {
		mif_err("%s: ERR! ld->mode == LINK_MODE_OFFLINE\n", ld->name);
		get_dpram_status(dpld, RX, &stat);
#ifdef DEBUG_MODEM_IF
		print_mem_status(ld, &stat);
#endif
		return IRQ_HANDLED;
	}

	if (dpram_wake_up(dpld) < 0) {
		trigger_forced_cp_crash(dpld);
		return IRQ_HANDLED;
	}

	get_dpram_status(dpld, RX, &stat);

	intr_handler(dpld, &stat);

	if (likely(ext_op && ext_op->clear_int2ap))
		ext_op->clear_int2ap(dpld);

	dpram_allow_sleep(dpld);

	return IRQ_HANDLED;
}

/**
 * ext_dpram_irq_handler: interrupt handler for a normal external DPRAM
 * @irq: IRQ number
 * @data: pointer to a data
 *
 * 1) Reads the interrupt value
 * 2) Performs interrupt handling
 */
static irqreturn_t ext_dpram_irq_handler(int irq, void *data)
{
	struct dpram_link_device *dpld = (struct dpram_link_device *)data;
	struct link_device *ld = (struct link_device *)&dpld->ld;
	struct mem_status stat;

	if (unlikely(ld->mode == LINK_MODE_OFFLINE))
		return IRQ_HANDLED;

	get_dpram_status(dpld, RX, &stat);

	intr_handler(dpld, &stat);

	return IRQ_HANDLED;
}

/**
 * get_txq_space
 * @dpld: pointer to an instance of dpram_link_device structure
 * @dev: IPC device (IPC_FMT, IPC_RAW, etc.)
 * OUT @stat: pointer to an instance of circ_status structure
 *
 * Stores {start address of the buffer in a TXQ, size of the buffer, in & out
 * pointer values, size of free space} into the 'stat' instance.
 *
 * Returns the size of free space in the buffer or an error code.
 */
static int get_txq_space(struct dpram_link_device *dpld, int dev,
			struct circ_status *stat)
{
	struct link_device *ld = &dpld->ld;
	int cnt = 0;
	u32 qsize;
	u32 head;
	u32 tail;
	int space;

	while (1) {
		qsize = get_txq_buff_size(dpld, dev);
		head = get_txq_head(dpld, dev);
		tail = get_txq_tail(dpld, dev);
		space = circ_get_space(qsize, head, tail);

		mif_debug("%s: %s_TXQ{qsize:%u in:%u out:%u space:%u}\n",
			ld->name, get_dev_name(dev), qsize, head, tail, space);

		if (circ_valid(qsize, head, tail))
			break;

		cnt++;
		mif_err("%s: ERR! invalid %s_TXQ{qsize:%d in:%d out:%d "
			"space:%d}, count %d\n",
			ld->name, get_dev_name(dev), qsize, head, tail,
			space, cnt);
		if (cnt >= MAX_RETRY_CNT) {
			space = -EIO;
			break;
		}

		udelay(100);
	}

	stat->buff = get_txq_buff(dpld, dev);
	stat->qsize = qsize;
	stat->in = head;
	stat->out = tail;
	stat->size = space;

	return space;
}

/**
 * write_ipc_to_txq
 * @dpld: pointer to an instance of dpram_link_device structure
 * @dev: IPC device (IPC_FMT, IPC_RAW, etc.)
 * @stat: pointer to an instance of circ_status structure
 * @skb: pointer to an instance of sk_buff structure
 *
 * Must be invoked only when there is enough space in the TXQ.
 */
static void write_ipc_to_txq(struct dpram_link_device *dpld, int dev,
			struct circ_status *stat, struct sk_buff *skb)
{
	struct link_device *ld = &dpld->ld;
	u8 __iomem *buff = stat->buff;
	u32 qsize = stat->qsize;
	u32 in = stat->in;
	u8 *src = skb->data;
	u32 len = skb->len;
	struct mif_irq_map map;

	/* Write data to the TXQ */
	if (unlikely(dpld->strict_io_access))
		circ_write16_to_io(buff, src, qsize, in, len);
	else
		circ_write(buff, src, qsize, in, len);

	/* Update new head (in) pointer */
	set_txq_head(dpld, dev, circ_new_pointer(qsize, in, len));

	/* Take a log for debugging */
	if (dev == IPC_FMT) {
#ifdef DEBUG_MODEM_IF
		char tag[MIF_MAX_STR_LEN];
		snprintf(tag, MIF_MAX_STR_LEN, "%s: MIF2CP", ld->mc->name);
		pr_ipc(0, tag, src, (len > 32 ? 32 : len));
#endif
		set_dpram_map(dpld, &map);
		mif_irq_log(ld->mc->msd, map, "ipc_write", sizeof("ipc_write"));
		mif_ipc_log(MIF_IPC_AP2CP, ld->mc->msd, skb->data, skb->len);
	}

#ifdef DEBUG_MODEM_IF
	/* Verify data written to the TXQ */
	if (ld->aligned && memcmp16_to_io((buff + in), src, 4)) {
		mif_err("%s: memcmp16_to_io fail\n", ld->name);
		trigger_forced_cp_crash(dpld);
	}
#endif
}

/**
 * xmit_ipc_msg
 * @dpld: pointer to an instance of dpram_link_device structure
 * @dev: IPC device (IPC_FMT, IPC_RAW, etc.)
 *
 * Tries to transmit IPC messages in the skb_txq of @dev as many as possible.
 *
 * Returns total length of IPC messages transmitted or an error code.
 */
static int xmit_ipc_msg(struct dpram_link_device *dpld, int dev)
{
	struct link_device *ld = &dpld->ld;
	struct sk_buff_head *txq = ld->skb_txq[dev];
	struct sk_buff *skb;
	unsigned long flags;
	struct circ_status stat;
	int space;
	int copied = 0;

	/* Acquire the spin lock for a TXQ */
	spin_lock_irqsave(&dpld->tx_lock[dev], flags);

	while (1) {
		/* Get the size of free space in the TXQ */
		space = get_txq_space(dpld, dev, &stat);
		if (unlikely(space < 0)) {
#ifdef DEBUG_MODEM_IF
			/* Trigger a enforced CP crash */
			trigger_forced_cp_crash(dpld);
#endif
			/* Empty out the TXQ */
			reset_txq_circ(dpld, dev);
			copied = -EIO;
			break;
		}

		skb = skb_dequeue(txq);
		if (unlikely(!skb))
			break;

		/* Check the free space size comparing with skb->len */
		if (unlikely(space < skb->len)) {
#ifdef DEBUG_MODEM_IF
			struct mem_status mst;
#endif
			/* Set res_required flag for the "dev" */
			atomic_set(&dpld->res_required[dev], 1);

			/* Take the skb back to the skb_txq */
			skb_queue_head(txq, skb);

			mif_info("%s: <called by %pf> NOSPC in %s_TXQ"
				"{qsize:%u in:%u out:%u}, free:%u < len:%u\n",
				ld->name, CALLER, get_dev_name(dev),
				stat.qsize, stat.in, stat.out, space, skb->len);
#ifdef DEBUG_MODEM_IF
			get_dpram_status(dpld, TX, &mst);
			print_circ_status(ld, dev, &mst);
#endif
			copied = -ENOSPC;
			break;
		}

		/* TX only when there is enough space in the TXQ */
		write_ipc_to_txq(dpld, dev, &stat, skb);
		copied += skb->len;
		dev_kfree_skb_any(skb);
	}

	/* Release the spin lock */
	spin_unlock_irqrestore(&dpld->tx_lock[dev], flags);
>>>>>>> fc9b728... update12

	default:
		mif_info("%s: unknown command 0x%04X\n", ld->name, cmd);
	}
}

static void ext_command_handler(struct dpram_link_device *dpld, u16 cmd)
{
	struct link_device *ld = &dpld->ld;
<<<<<<< HEAD
	u16 resp;

	switch (EXT_CMD_MASK(cmd)) {
	case EXT_CMD_SET_SPEED_LOW:
		if (dpld->dpctl->setup_speed) {
			dpld->dpctl->setup_speed(DPRAM_SPEED_LOW);
			resp = INT_EXT_CMD(EXT_CMD_SET_SPEED_LOW);
			send_intr(dpld, resp);
		}
		break;

	case EXT_CMD_SET_SPEED_MID:
		if (dpld->dpctl->setup_speed) {
			dpld->dpctl->setup_speed(DPRAM_SPEED_MID);
			resp = INT_EXT_CMD(EXT_CMD_SET_SPEED_MID);
			send_intr(dpld, resp);
		}
		break;

	case EXT_CMD_SET_SPEED_HIGH:
		if (dpld->dpctl->setup_speed) {
			dpld->dpctl->setup_speed(DPRAM_SPEED_HIGH);
			resp = INT_EXT_CMD(EXT_CMD_SET_SPEED_HIGH);
			send_intr(dpld, resp);
		}
		break;

	default:
		mif_info("%s: unknown command 0x%04X\n", ld->name, cmd);
		break;
	}
=======
	struct completion *cmpl = &dpld->req_ack_cmpl[dev];
	unsigned long timeout = msecs_to_jiffies(dpld->res_ack_wait_timeout);
	int ret;
	u16 mask;

#ifdef DEBUG_MODEM_IF
	mif_info("%s: send %s_REQ_ACK\n", ld->name, get_dev_name(dev));
#endif

	mask = get_mask_req_ack(dpld, dev);
	send_int2cp(dpld, INT_NON_CMD(mask));

	ret = wait_for_completion_interruptible_timeout(cmpl, timeout);
	/* ret == 0 on timeout, ret < 0 if interrupted */
	if (ret < 0) {
		mif_info("%s: %s: wait_for_completion interrupted! (ret %d)\n",
			ld->name, get_dev_name(dev), ret);
		goto exit;
	}

	if (ret == 0) {
		struct mem_status mst;
		get_dpram_status(dpld, TX, &mst);

		mif_info("%s: wait_for_completion TIMEOUT! (no %s_RES_ACK)\n",
			ld->name, get_dev_name(dev));

		/*
		** The TXQ must be checked whether or not it is empty, because
		** an interrupt mask can be overwritten by the next interrupt.
		*/
		if (mst.head[dev][TX] == mst.tail[dev][TX]) {
			ret = get_txq_buff_size(dpld, dev);
#ifdef DEBUG_MODEM_IF
			mif_info("%s: %s_TXQ has been emptied\n",
				ld->name, get_dev_name(dev));
			print_circ_status(ld, dev, &mst);
#endif
		}
	}

exit:
	return ret;
>>>>>>> fc9b728... update12
}

static void udl_command_handler(struct dpram_link_device *dpld, u16 cmd)
{
<<<<<<< HEAD
	struct link_device *ld = &dpld->ld;

	if (cmd & UDL_RESULT_FAIL) {
		mif_info("%s: ERR! Command failed: %04x\n", ld->name, cmd);
		return;
	}

	switch (UDL_CMD_MASK(cmd)) {
	case UDL_CMD_RECV_READY:
		mif_debug("%s: Send CP-->AP RECEIVE_READY\n", ld->name);
		send_intr(dpld, CMD_IMG_START_REQ);
		break;
	default:
		complete_all(&dpld->udl_cmd_complete);
	}
}

static inline void dpram_ipc_rx(struct dpram_link_device *dpld, u16 intr)
{
	if (unlikely(INT_CMD_VALID(intr)))
		command_handler(dpld, intr);
	else
		non_command_handler(dpld, intr);
=======
	int ret;
	u16 mask;

	ret = xmit_ipc_msg(dpld, dev);
	if (ret > 0) {
		mask = get_mask_send(dpld, dev);
		send_int2cp(dpld, INT_NON_CMD(mask));
		get_dpram_status(dpld, TX, msq_get_free_slot(&dpld->stat_list));
	}

	if (ret >= 0)
		atomic_set(&dpld->res_required[dev], 0);

	return ret;
>>>>>>> fc9b728... update12
}

static inline void dpram_intr_handler(struct dpram_link_device *dpld, u16 intr)
{
<<<<<<< HEAD
	char *name = dpld->ld.name;

	if (unlikely(intr == INT_POWERSAFE_FAIL)) {
		mif_info("%s: intr == INT_POWERSAFE_FAIL\n", name);
=======
	struct link_device *ld;
	struct dpram_link_device *dpld;
	unsigned long delay = 0;
	int ret;

	ld = container_of(work, struct link_device, fmt_tx_dwork.work);
	dpld = to_dpram_link_device(ld);

	ret = wait_for_res_ack(dpld, IPC_FMT);
	/* ret < 0 if interrupted */
	if (ret < 0)
>>>>>>> fc9b728... update12
		return;

<<<<<<< HEAD
	if (unlikely(EXT_UDL_CMD(intr))) {
		if (likely(EXT_INT_VALID(intr))) {
			if (UDL_CMD_VALID(intr))
				udl_command_handler(dpld, intr);
			else if (EXT_CMD_VALID(intr))
				ext_command_handler(dpld, intr);
			else
				mif_info("%s: ERR! invalid intr 0x%04X\n",
					name, intr);
		} else {
			mif_info("%s: ERR! invalid intr 0x%04X\n", name, intr);
		}
=======
	/* ret == 0 on timeout */
	if (ret == 0) {
		queue_delayed_work(ld->tx_wq, ld->tx_dwork[IPC_FMT], 0);
>>>>>>> fc9b728... update12
		return;
	}

	if (likely(INT_VALID(intr)))
		dpram_ipc_rx(dpld, intr);
	else
		mif_info("%s: ERR! invalid intr 0x%04X\n", name, intr);
}

static irqreturn_t ap_idpram_irq_handler(int irq, void *data)
{
	struct dpram_link_device *dpld = (struct dpram_link_device *)data;
	struct link_device *ld = (struct link_device *)&dpld->ld;
	u16 int2ap = recv_intr(dpld);

	if (unlikely(ld->mode == LINK_MODE_OFFLINE))
		return IRQ_HANDLED;

	dpram_intr_handler(dpld, int2ap);

	return IRQ_HANDLED;
}

static irqreturn_t cp_idpram_irq_handler(int irq, void *data)
{
<<<<<<< HEAD
	struct dpram_link_device *dpld = (struct dpram_link_device *)data;
	struct link_device *ld = (struct link_device *)&dpld->ld;
	u16 int2ap;
=======
	struct link_device *ld;
	struct dpram_link_device *dpld;
	unsigned long delay = 0;
	int ret;
>>>>>>> fc9b728... update12

	if (unlikely(ld->mode == LINK_MODE_OFFLINE))
		return IRQ_HANDLED;

<<<<<<< HEAD
	if (dpram_wake_up(dpld) < 0) {
		log_dpram_status(dpld);
		trigger_force_cp_crash(dpld);
		return IRQ_HANDLED;
	}

	int2ap = recv_intr(dpld);
=======
	ret = wait_for_res_ack(dpld, IPC_RAW);
	/* ret < 0 if interrupted */
	if (ret < 0)
		return;

	/* ret == 0 on timeout */
	if (ret == 0) {
		queue_delayed_work(ld->tx_wq, ld->tx_dwork[IPC_RAW], 0);
		return;
	}
>>>>>>> fc9b728... update12

	dpram_intr_handler(dpld, int2ap);

	clear_intr(dpld);

<<<<<<< HEAD
	dpram_allow_sleep(dpld);
=======
/**
 * rfs_tx_work: performs TX for RFS IPC device under DPRAM flow control
 * @work: pointer to an instance of the work_struct structure
 *
 * 1) Starts waiting for RES_ACK of RFS IPC device.
 * 2) Returns immediately if the wait is interrupted.
 * 3) Restarts DPRAM flow control if there is a timeout from the wait.
 * 4) Otherwise, it performs processing RES_ACK for RFS IPC device.
 */
static void rfs_tx_work(struct work_struct *work)
{
	struct link_device *ld;
	struct dpram_link_device *dpld;
	unsigned long delay = 0;
	int ret;
>>>>>>> fc9b728... update12

	return IRQ_HANDLED;
}

<<<<<<< HEAD
static irqreturn_t ext_dpram_irq_handler(int irq, void *data)
{
	struct dpram_link_device *dpld = (struct dpram_link_device *)data;
	struct link_device *ld = (struct link_device *)&dpld->ld;
	u16 int2ap = recv_intr(dpld);

	if (unlikely(ld->mode == LINK_MODE_OFFLINE))
		return IRQ_HANDLED;
=======
	ret = wait_for_res_ack(dpld, IPC_RFS);
	/* ret < 0 if interrupted */
	if (ret < 0)
		return;

	/* ret == 0 on timeout */
	if (ret == 0) {
		queue_delayed_work(ld->tx_wq, ld->tx_dwork[IPC_RFS], 0);
		return;
	}
>>>>>>> fc9b728... update12

	dpram_intr_handler(dpld, int2ap);

	return IRQ_HANDLED;
}

<<<<<<< HEAD
static void dpram_send_ipc(struct link_device *ld, int dev,
			struct io_device *iod, struct sk_buff *skb)
{
	struct dpram_link_device *dpld = to_dpram_link_device(ld);
	struct sk_buff_head *txq = ld->skb_txq[dev];
	int ret;
	u16 mask;

	skb_queue_tail(txq, skb);
	if (txq->qlen > 1024) {
		mif_debug("%s: %s txq->qlen %d > 1024\n",
			ld->name, get_dev_name(dev), txq->qlen);
	}

	if (dpld->dp_type == CP_IDPRAM) {
		if (dpram_wake_up(dpld) < 0) {
			trigger_force_cp_crash(dpld);
			return;
		}
	}

	if (!dpram_ipc_active(dpld))
		goto exit;

	if (atomic_read(&dpld->res_required[dev]) > 0) {
		mif_debug("%s: %s_TXQ is full\n", ld->name, get_dev_name(dev));
=======
/**
 * dpram_send_ipc
 * @dpld: pointer to an instance of dpram_link_device structure
 * @dev: IPC device (IPC_FMT, IPC_RAW, etc.)
 *
 * 1) Tries to transmit IPC messages in the skb_txq by invoking xmit_ipc_msg()
 *    function.
 * 2) Sends an interrupt to CP if there is no error from xmit_ipc_msg().
 * 3) Starts DPRAM flow control if xmit_ipc_msg() returns -ENOSPC.
 */
static int dpram_send_ipc(struct dpram_link_device *dpld, int dev)
{
	struct link_device *ld = &dpld->ld;
	int ret;
	u16 mask;

	if (atomic_read(&dpld->res_required[dev]) > 0) {
		mif_info("%s: %s_TXQ is full\n", ld->name, get_dev_name(dev));
		return 0;
	}

	if (dpram_wake_up(dpld) < 0) {
		trigger_forced_cp_crash(dpld);
		return -EIO;
	}

	if (!ipc_active(dpld)) {
		mif_info("%s: IPC is NOT active\n", ld->name);
		ret = -EIO;
>>>>>>> fc9b728... update12
		goto exit;
	}

	ret = dpram_try_ipc_tx(dpld, dev);
	if (ret > 0) {
		mask = get_mask_send(dpld, dev);
<<<<<<< HEAD
		send_intr(dpld, INT_NON_CMD(mask));
	} else if (ret == -ENOSPC) {
		mask = get_mask_req_ack(dpld, dev);
		send_intr(dpld, INT_NON_CMD(mask));
		mif_info("%s: Send REQ_ACK 0x%04X\n", ld->name, mask);
	} else {
		mif_info("%s: dpram_try_ipc_tx fail (err %d)\n", ld->name, ret);
	}

exit:
	if (dpld->dp_type == CP_IDPRAM)
		dpram_allow_sleep(dpld);
=======
		send_int2cp(dpld, INT_NON_CMD(mask));
		get_dpram_status(dpld, TX, msq_get_free_slot(&dpld->stat_list));
		goto exit;
	}

	/* If there was no TX, just exit */
	if (ret == 0)
		goto exit;

	/* At this point, ret < 0 */
	if (ret == -ENOSPC) {
		/* Prohibit DPRAM from sleeping until the TXQ buffer is empty */
		if (dpram_wake_up(dpld) < 0) {
			trigger_forced_cp_crash(dpld);
			goto exit;
		}

		/*----------------------------------------------------*/
		/* dpld->res_required[dev] was set in xmit_ipc_msg(). */
		/*----------------------------------------------------*/

		if (dev == IPC_RAW)
			mif_netif_stop(ld);

		queue_delayed_work(ld->tx_wq, ld->tx_dwork[dev], 0);
	}

exit:
	dpram_allow_sleep(dpld);
	return ret;
}

/**
 * pm_tx_work: performs TX while DPRAM PM is locked
 * @work: pointer to an instance of the work_struct structure
 */
static void pm_tx_work(struct work_struct *work)
{
	struct idpram_pm_data *pm_data;
	struct idpram_pm_op *pm_op;
	struct dpram_link_device *dpld;
	struct link_device *ld;
	struct workqueue_struct *pm_wq = system_nrt_wq;
	int i;
	int ret;
	unsigned long delay = 0;

	pm_data = container_of(work, struct idpram_pm_data, tx_dwork.work);
	dpld = container_of(pm_data, struct dpram_link_device, pm_data);
	ld = &dpld->ld;
	pm_op = dpld->pm_op;

	if (pm_op->locked(dpld)) {
		queue_delayed_work(pm_wq, &pm_data->tx_dwork, delay);
		return;
	}

	/* Here, PM is not locked. */
	for (i = 0; i < ld->max_ipc_dev; i++) {
		ret = dpram_send_ipc(dpld, i);
		if (ret < 0) {
			struct io_device *iod = dpld->iod[i];
			mif_err("%s->%s: ERR! dpram_send_ipc fail (err %d)\n",
				iod->name, ld->name, ret);
		}
	}
}

/**
 * dpram_try_send_ipc
 * @dpld: pointer to an instance of dpram_link_device structure
 * @dev: IPC device (IPC_FMT, IPC_RAW, etc.)
 * @iod: pointer to an instance of the io_device structure
 * @skb: pointer to an skb that will be transmitted
 *
 * 1) Enqueues an skb to the skb_txq for @dev in the link device instance.
 * 2) Tries to transmit IPC messages in the skb_txq by invoking xmit_ipc_msg()
 *    function.
 * 3) Sends an interrupt to CP if there is no error from xmit_ipc_msg().
 * 4) Starts DPRAM flow control if xmit_ipc_msg() returns -ENOSPC.
 */
static void dpram_try_send_ipc(struct dpram_link_device *dpld, int dev,
			struct io_device *iod, struct sk_buff *skb)
{
	struct link_device *ld = &dpld->ld;
	struct idpram_pm_data *pm_data = &dpld->pm_data;
	struct idpram_pm_op *pm_op = dpld->pm_op;
	struct workqueue_struct *pm_wq = system_nrt_wq;
	unsigned long delay = msecs_to_jiffies(10);
	struct sk_buff_head *txq = ld->skb_txq[dev];
	int ret;

	if (unlikely(txq->qlen >= MAX_SKB_TXQ_DEPTH)) {
		mif_info("%s: %s txq->qlen %d >= %d\n", ld->name,
			get_dev_name(dev), txq->qlen, MAX_SKB_TXQ_DEPTH);
		dev_kfree_skb_any(skb);
		return;
	}

	skb_queue_tail(txq, skb);

	if (pm_op && pm_op->locked) {
		if (pm_op->locked(dpld)) {
			queue_delayed_work(pm_wq, &pm_data->tx_dwork, delay);
			return;
		}

		/* Here, PM is not locked. */
		if (work_pending(&pm_data->tx_dwork.work))
			cancel_delayed_work_sync(&pm_data->tx_dwork);
	}

	ret = dpram_send_ipc(dpld, dev);
	if (ret < 0) {
		mif_err("%s->%s: ERR! dpram_send_ipc fail (err %d)\n",
			iod->name, ld->name, ret);
	}
>>>>>>> fc9b728... update12
}

static int dpram_send_cp_binary(struct link_device *ld, struct sk_buff *skb)
{
	struct dpram_link_device *dpld = to_dpram_link_device(ld);

	if (dpld->ext_op && dpld->ext_op->xmit_binary)
		return dpld->ext_op->xmit_binary(dpld, skb);
	else
		return -ENODEV;
}

<<<<<<< HEAD
=======
/**
 * dpram_send
 * @ld: pointer to an instance of the link_device structure
 * @iod: pointer to an instance of the io_device structure
 * @skb: pointer to an skb that will be transmitted
 *
 * Returns the length of data transmitted or an error code.
 *
 * Normal call flow for an IPC message:
 *   dpram_try_send_ipc -> dpram_send_ipc -> xmit_ipc_msg -> write_ipc_to_txq
 *
 * Call flow on PM lock in a DPRAM IPC TXQ:
 *   dpram_try_send_ipc ,,, queue_delayed_work
 *   => pm_tx_work -> dpram_send_ipc -> xmit_ipc_msg -> write_ipc_to_txq
 *
 * Call flow on congestion in a DPRAM IPC TXQ:
 *   dpram_try_send_ipc -> xmit_ipc_msg ,,, queue_delayed_work
 *   => xxx_tx_work -> wait_for_res_ack
 *   => msg_handler
 *   => process_res_ack -> xmit_ipc_msg (,,, queue_delayed_work ...)
 */
>>>>>>> fc9b728... update12
static int dpram_send(struct link_device *ld, struct io_device *iod,
		struct sk_buff *skb)
{
	enum dev_format dev = iod->format;
	int len = skb->len;

	switch (dev) {
	case IPC_FMT:
	case IPC_RAW:
	case IPC_RFS:
		if (likely(ld->mode == LINK_MODE_IPC)) {
<<<<<<< HEAD
			dpram_send_ipc(ld, dev, iod, skb);
=======
			dpram_try_send_ipc(dpld, dev, iod, skb);
>>>>>>> fc9b728... update12
		} else {
			mif_info("%s: ld->mode != LINK_MODE_IPC\n", ld->name);
			dev_kfree_skb_any(skb);
		}
		return len;

	case IPC_BOOT:
		return dpram_send_cp_binary(ld, skb);

	default:
		mif_info("%s: ERR! no TXQ for %s\n", ld->name, iod->name);
		dev_kfree_skb_any(skb);
		return -ENODEV;
	}
}

static int dpram_xmit_boot(struct link_device *ld, struct io_device *iod,
			unsigned long arg)
{
	struct dpram_link_device *dpld = to_dpram_link_device(ld);

	if (dpld->ext_op && dpld->ext_op->xmit_boot)
		return dpld->ext_op->xmit_boot(dpld, arg);
	else
		return -ENODEV;
}

static int dpram_set_dload_magic(struct link_device *ld, struct io_device *iod)
{
	struct dpram_link_device *dpld = to_dpram_link_device(ld);

	ld->mode = LINK_MODE_DLOAD;

	mif_err("%s: magic = 0x%08X\n", ld->name, DP_MAGIC_DMDL);
	iowrite32(DP_MAGIC_DMDL, dpld->dl_map.magic);

	return 0;
}

static int dpram_dload_firmware(struct link_device *ld, struct io_device *iod,
			unsigned long arg)
{
	struct dpram_link_device *dpld = to_dpram_link_device(ld);

	if (dpld->ext_op && dpld->ext_op->firm_update)
		return dpld->ext_op->firm_update(dpld, arg);
	else
		return -ENODEV;
}

static int dpram_force_dump(struct link_device *ld, struct io_device *iod)
{
	struct dpram_link_device *dpld = to_dpram_link_device(ld);
	trigger_forced_cp_crash(dpld);
	return 0;
}

static void dpram_dump_memory(struct link_device *ld, char *buff)
{
	struct dpram_link_device *dpld = to_dpram_link_device(ld);
	u8 __iomem *base = dpld->dpctl->dp_base;
	u32 size = dpld->dpctl->dp_size;

	if (dpld->dp_type == CP_IDPRAM)
		dpram_wake_up(dpld);

	memcpy(buff, base, size);
}

static int dpram_dump_start(struct link_device *ld, struct io_device *iod)
{
	struct dpram_link_device *dpld = to_dpram_link_device(ld);

	if (dpld->ext_op && dpld->ext_op->dump_start)
		return dpld->ext_op->dump_start(dpld);
	else
		return -ENODEV;
}

static int dpram_dump_update(struct link_device *ld, struct io_device *iod,
		unsigned long arg)
{
	struct dpram_link_device *dpld = to_dpram_link_device(ld);

	if (dpld->ext_op && dpld->ext_op->dump_update)
		return dpld->ext_op->dump_update(dpld, arg);
	else
		return -ENODEV;
}

static int dpram_dump_finish(struct link_device *ld, struct io_device *iod,
			unsigned long arg)
{
	struct dpram_link_device *dpld = to_dpram_link_device(ld);

	if (dpld->ext_op && dpld->ext_op->dump_finish)
		return dpld->ext_op->dump_finish(dpld, arg);
	else
		return -ENODEV;
}

static int dpram_ioctl(struct link_device *ld, struct io_device *iod,
		unsigned int cmd, unsigned long arg)
{
	struct dpram_link_device *dpld = to_dpram_link_device(ld);
	int err = 0;

	mif_info("%s: cmd 0x%08X\n", ld->name, cmd);

	switch (cmd) {
	case IOCTL_DPRAM_INIT_STATUS:
		mif_debug("%s: get dpram init status\n", ld->name);
		return dpld->dpram_init_status;

	case IOCTL_MIF_DPRAM_DUMP:
		if (copy_to_user((void __user *)arg, &dpld->size, sizeof(u32)))
			return -EFAULT;

		capture_dpram_snapshot(ld, iod);
		break;

	default:
		if (dpld->ext_ioctl)
			return dpld->ext_ioctl(dpld, iod, cmd, arg);

		mif_err("%s: ERR! invalid cmd 0x%08X\n", ld->name, cmd);
		return -EINVAL;
	}

	return err;
}

static void dpram_remap_std_16k_region(struct dpram_link_device *dpld)
{
	struct dpram_ipc_16k_map *dpram_map;
	struct dpram_ipc_device *dev;

	dpram_map = (struct dpram_ipc_16k_map *)dpld->dp_base;

	/* magic code and access enable fields */
	dpld->ipc_map.magic = (u16 __iomem *)&dpram_map->magic;
	dpld->ipc_map.access = (u16 __iomem *)&dpram_map->access;

	/* FMT */
	dev = &dpld->ipc_map.dev[IPC_FMT];

	strcpy(dev->name, "FMT");
	dev->id = IPC_FMT;

	dev->txq.head = (u16 __iomem *)&dpram_map->fmt_tx_head;
	dev->txq.tail = (u16 __iomem *)&dpram_map->fmt_tx_tail;
	dev->txq.buff = (u8 __iomem *)&dpram_map->fmt_tx_buff[0];
	dev->txq.size = DP_16K_FMT_TX_BUFF_SZ;

	dev->rxq.head = (u16 __iomem *)&dpram_map->fmt_rx_head;
	dev->rxq.tail = (u16 __iomem *)&dpram_map->fmt_rx_tail;
	dev->rxq.buff = (u8 __iomem *)&dpram_map->fmt_rx_buff[0];
	dev->rxq.size = DP_16K_FMT_RX_BUFF_SZ;

	dev->mask_req_ack = INT_MASK_REQ_ACK_F;
	dev->mask_res_ack = INT_MASK_RES_ACK_F;
	dev->mask_send    = INT_MASK_SEND_F;

	/* RAW */
	dev = &dpld->ipc_map.dev[IPC_RAW];

	strcpy(dev->name, "RAW");
	dev->id = IPC_RAW;

	dev->txq.head = (u16 __iomem *)&dpram_map->raw_tx_head;
	dev->txq.tail = (u16 __iomem *)&dpram_map->raw_tx_tail;
	dev->txq.buff = (u8 __iomem *)&dpram_map->raw_tx_buff[0];
	dev->txq.size = DP_16K_RAW_TX_BUFF_SZ;

	dev->rxq.head = (u16 __iomem *)&dpram_map->raw_rx_head;
	dev->rxq.tail = (u16 __iomem *)&dpram_map->raw_rx_tail;
	dev->rxq.buff = (u8 __iomem *)&dpram_map->raw_rx_buff[0];
	dev->rxq.size = DP_16K_RAW_RX_BUFF_SZ;

	dev->mask_req_ack = INT_MASK_REQ_ACK_R;
	dev->mask_res_ack = INT_MASK_RES_ACK_R;
	dev->mask_send    = INT_MASK_SEND_R;

	/* interrupt ports */
	dpld->ipc_map.mbx_cp2ap = (u16 __iomem *)&dpram_map->mbx_cp2ap;
	dpld->ipc_map.mbx_ap2cp = (u16 __iomem *)&dpram_map->mbx_ap2cp;
}

static int dpram_init_boot_map(struct dpram_link_device *dpld)
{
<<<<<<< HEAD
	struct link_device *ld = &dpld->ld;
	u8 __iomem *dp_base;
	int i;

	if (!dpld->dp_base) {
		mif_info("%s: ERR! dpld->dp_base == NULL\n", ld->name);
		return -EINVAL;
	}
	dp_base = dpld->dp_base;

	/* Map for IPC */
	if (dpld->dpctl->ipc_map) {
		memcpy(&dpld->ipc_map, dpld->dpctl->ipc_map,
			sizeof(struct dpram_ipc_map));
	} else {
		if (dpld->dp_size == DPRAM_SIZE_16KB)
			dpram_remap_std_16k_region(dpld);
		else
			return -EINVAL;
	}

	dpld->magic = dpld->ipc_map.magic;
	dpld->access = dpld->ipc_map.access;
	for (i = 0; i < dpld->max_ipc_dev; i++)
		dpld->dev[i] = &dpld->ipc_map.dev[i];
	dpld->mbx2ap = dpld->ipc_map.mbx_cp2ap;
	dpld->mbx2cp = dpld->ipc_map.mbx_ap2cp;
=======
	u8 __iomem *dp_base = dpld->base;
	u32 magic_size = DP_DLOAD_MAGIC_SIZE;
	u32 mbx_size = DP_MBX_SET_SIZE;
>>>>>>> fc9b728... update12

	if (dpld->ext_op && dpld->ext_op->init_boot_map) {
		dpld->ext_op->init_boot_map(dpld);
	} else {
		dpld->bt_map.magic = (u32 *)(dp_base);
<<<<<<< HEAD
		dpld->bt_map.buff = (u8 *)(dp_base + DP_BOOT_BUFF_OFFSET);
		dpld->bt_map.size = dpld->dp_size - 8;
=======
		dpld->bt_map.buff = (u8 *)(dp_base + magic_size);
		dpld->bt_map.space = dpld->size - (magic_size + mbx_size);
>>>>>>> fc9b728... update12
	}

	return 0;
}

static int dpram_init_dload_map(struct dpram_link_device *dpld)
{
	u8 __iomem *dp_base = dpld->base;
	u32 magic_size = DP_DLOAD_MAGIC_SIZE;
	u32 mbx_size = DP_MBX_SET_SIZE;

	if (dpld->ext_op && dpld->ext_op->init_dl_map) {
		dpld->ext_op->init_dl_map(dpld);
	} else {
		dpld->dl_map.magic = (u32 *)(dp_base);
		dpld->dl_map.buff = (u8 *)(dp_base + magic_size);
		dpld->dl_map.space = dpld->size - (magic_size + mbx_size);
	}

	return 0;
}

static int dpram_init_uload_map(struct dpram_link_device *dpld)
{
	u8 __iomem *dp_base = dpld->base;
	u32 magic_size = DP_DLOAD_MAGIC_SIZE;
	u32 mbx_size = DP_MBX_SET_SIZE;

	if (dpld->ext_op && dpld->ext_op->init_ul_map) {
		dpld->ext_op->init_ul_map(dpld);
	} else {
		dpld->ul_map.magic = (u32 *)(dp_base);
		dpld->ul_map.buff = (u8 *)(dp_base + DP_ULOAD_BUFF_OFFSET);
		dpld->ul_map.space = dpld->size - (magic_size + mbx_size);
	}

<<<<<<< HEAD
=======
	return 0;
}

static int dpram_init_ipc_map(struct dpram_link_device *dpld)
{
	int i;
	struct link_device *ld = &dpld->ld;

	if (dpld->ext_op && dpld->ext_op->init_ipc_map) {
		dpld->ext_op->init_ipc_map(dpld);
	} else if (dpld->dpram->ipc_map) {
		memcpy(&dpld->ipc_map, dpld->dpram->ipc_map,
			sizeof(struct dpram_ipc_map));
	} else {
		if (dpld->size == DPRAM_SIZE_16KB)
			dpram_remap_std_16k_region(dpld);
		else
			return -EINVAL;
	}

	dpld->magic = dpld->ipc_map.magic;
	dpld->access = dpld->ipc_map.access;
	for (i = 0; i < ld->max_ipc_dev; i++)
		dpld->dev[i] = &dpld->ipc_map.dev[i];
	dpld->mbx2ap = dpld->ipc_map.mbx_cp2ap;
	dpld->mbx2cp = dpld->ipc_map.mbx_ap2cp;

>>>>>>> fc9b728... update12
	return 0;
}

static void dpram_setup_common_op(struct dpram_link_device *dpld)
{
	dpld->clear_intr = clear_intr;
	dpld->recv_intr = recv_intr;
	dpld->send_intr = send_intr;
	dpld->get_magic = get_magic;
	dpld->set_magic = set_magic;
	dpld->get_access = get_access;
	dpld->set_access = set_access;
	dpld->get_tx_head = get_tx_head;
	dpld->get_tx_tail = get_tx_tail;
	dpld->set_tx_head = set_tx_head;
	dpld->set_tx_tail = set_tx_tail;
	dpld->get_tx_buff = get_tx_buff;
	dpld->get_tx_buff_size = get_tx_buff_size;
	dpld->get_rx_head = get_rx_head;
	dpld->get_rx_tail = get_rx_tail;
	dpld->set_rx_head = set_rx_head;
	dpld->set_rx_tail = set_rx_tail;
	dpld->get_rx_buff = get_rx_buff;
	dpld->get_rx_buff_size = get_rx_buff_size;
	dpld->get_mask_req_ack = get_mask_req_ack;
	dpld->get_mask_res_ack = get_mask_res_ack;
	dpld->get_mask_send = get_mask_send;
<<<<<<< HEAD
	dpld->ipc_rx_handler = dpram_ipc_rx;
}

static int dpram_link_init(struct link_device *ld, struct io_device *iod)
{
	return 0;
=======
	dpld->get_dpram_status = get_dpram_status;
	dpld->ipc_rx_handler = cmd_msg_handler;
	dpld->reset_dpram_ipc = reset_dpram_ipc;
>>>>>>> fc9b728... update12
}

static void dpram_link_terminate(struct link_device *ld, struct io_device *iod)
{
	return;
}

struct link_device *dpram_create_link_device(struct platform_device *pdev)
{
	struct modem_data *mdm_data = NULL;
	struct dpram_link_device *dpld = NULL;
	struct link_device *ld = NULL;
<<<<<<< HEAD
	struct resource *res = NULL;
	resource_size_t res_size;
	struct modemlink_dpram_control *dpctl = NULL;
	unsigned long task_data;
=======
	struct modem_data *modem = NULL;
	struct modemlink_dpram_data *dpram = NULL;
	struct resource *res = NULL;
	resource_size_t res_size;
>>>>>>> fc9b728... update12
	int ret = 0;
	int i = 0;

	/* Get the platform data */
	mdm_data = (struct modem_data *)pdev->dev.platform_data;
	if (!mdm_data) {
		mif_info("ERR! mdm_data == NULL\n");
		goto err;
	}
	mif_info("modem = %s\n", mdm_data->name);
	mif_info("link device = %s\n", mdm_data->link_name);

	if (!mdm_data->dpram_ctl) {
		mif_info("ERR! mdm_data->dpram_ctl == NULL\n");
		goto err;
	}
<<<<<<< HEAD
	dpctl = mdm_data->dpram_ctl;

	/* Alloc DPRAM link device structure */
	dpld = kzalloc(sizeof(struct dpram_link_device), GFP_KERNEL);
	if (!dpld) {
		mif_info("ERR! kzalloc dpld fail\n");
		goto err;
	}
	ld = &dpld->ld;

	/* Retrieve modem data and DPRAM control data from the modem data */
	ld->mdm_data = mdm_data;
	ld->name = mdm_data->link_name;
	ld->ipc_version = mdm_data->ipc_version;

	/* Retrieve the most basic data for IPC from the modem data */
	dpld->dpctl = dpctl;
	dpld->dp_type = dpctl->dp_type;

	if (mdm_data->ipc_version < SIPC_VER_50) {
		if (!dpctl->max_ipc_dev) {
			mif_info("ERR! no max_ipc_dev\n");
			goto err;
		}

		ld->aligned = dpctl->aligned;
		dpld->max_ipc_dev = dpctl->max_ipc_dev;
=======
	mif_info("modem = %s\n", modem->name);
	mif_info("link device = %s\n", modem->link_name);

	/*
	** Retrieve modem data and DPRAM control data from the modem data
	*/
	ld->mdm_data = modem;
	ld->name = modem->link_name;
	ld->ipc_version = modem->ipc_version;

	if (!modem->dpram) {
		mif_err("ERR! no modem->dpram\n");
		goto err;
	}
	dpram = modem->dpram;

	dpld->dpram = dpram;
	dpld->type = dpram->type;
	dpld->ap = dpram->ap;
	dpld->strict_io_access = dpram->strict_io_access;

	if (ld->ipc_version < SIPC_VER_50) {
		if (!modem->max_ipc_dev) {
			mif_err("%s: ERR! no max_ipc_dev\n", ld->name);
			goto err;
		}

		ld->aligned = dpram->aligned;
		ld->max_ipc_dev = modem->max_ipc_dev;
>>>>>>> fc9b728... update12
	} else {
		ld->aligned = 1;
		dpld->max_ipc_dev = MAX_SIPC5_DEV;
	}

<<<<<<< HEAD
	/* Set attributes as a link device */
	ld->init_comm = dpram_link_init;
=======
	/*
	** Set attributes as a link device
	*/
>>>>>>> fc9b728... update12
	ld->terminate_comm = dpram_link_terminate;
	ld->send = dpram_send;
	ld->xmit_boot = dpram_xmit_boot;
	ld->dload_start = dpram_set_dload_magic;
	ld->firm_update = dpram_dload_firmware;
	ld->force_dump = dpram_force_dump;
	ld->dump_start = dpram_dump_start;
	ld->dump_update = dpram_dump_update;
	ld->dump_finish = dpram_dump_finish;
	/* IOCTL extension */
	ld->ioctl = dpram_ioctl;

	INIT_LIST_HEAD(&ld->list);

	skb_queue_head_init(&ld->sk_fmt_tx_q);
	skb_queue_head_init(&ld->sk_raw_tx_q);
	skb_queue_head_init(&ld->sk_rfs_tx_q);
	ld->skb_txq[IPC_FMT] = &ld->sk_fmt_tx_q;
	ld->skb_txq[IPC_RAW] = &ld->sk_raw_tx_q;
	ld->skb_txq[IPC_RFS] = &ld->sk_rfs_tx_q;

<<<<<<< HEAD
	/* Set up function pointers */
	dpram_setup_common_op(dpld);
	dpld->dpram_dump = dpram_dump_memory;
	dpld->ext_op = dpram_get_ext_op(mdm_data->modem_type);
	if (dpld->ext_op && dpld->ext_op->ioctl)
		dpld->ext_ioctl = dpld->ext_op->ioctl;

	/* Retrieve DPRAM resource */
	if (!dpctl->dp_base) {
		res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
=======
	skb_queue_head_init(&ld->sk_fmt_rx_q);
	skb_queue_head_init(&ld->sk_raw_rx_q);
	skb_queue_head_init(&ld->sk_rfs_rx_q);
	ld->skb_rxq[IPC_FMT] = &ld->sk_fmt_rx_q;
	ld->skb_rxq[IPC_RAW] = &ld->sk_raw_rx_q;
	ld->skb_rxq[IPC_RFS] = &ld->sk_rfs_rx_q;

	init_completion(&ld->init_cmpl);
	init_completion(&ld->pif_cmpl);

	/*
	** Set up function pointers
	*/
	dpram_setup_common_op(dpld);
	dpld->ext_op = dpram_get_ext_op(modem->modem_type);
	if (dpld->ext_op) {
		if (dpld->ext_op->ioctl)
			dpld->ext_ioctl = dpld->ext_op->ioctl;

		if (dpld->ext_op->wakeup && dpld->ext_op->sleep)
			dpld->need_wake_up = true;
	}

	/*
	** Retrieve DPRAM resource
	*/
	if (!dpram->base) {
		res = platform_get_resource_byname(pdev, IORESOURCE_MEM,
						STR_DPRAM_BASE);
>>>>>>> fc9b728... update12
		if (!res) {
			mif_info("%s: ERR! platform_get_resource fail\n",
				ld->name);
			goto err;
		}
		res_size = resource_size(res);

<<<<<<< HEAD
		dpctl->dp_base = ioremap_nocache(res->start, res_size);
		dpctl->dp_size = res_size;
	}
	dpld->dp_base = dpctl->dp_base;
	dpld->dp_size = dpctl->dp_size;

	mif_info("%s: dp_type %d, aligned %d, dp_base 0x%08X, dp_size %d\n",
		ld->name, dpld->dp_type, ld->aligned, (int)dpld->dp_base,
		dpld->dp_size);
=======
		dpram->base = ioremap_nocache(res->start, res_size);
		if (!dpram->base) {
			mif_err("%s: ERR! ioremap_nocache for BASE fail\n",
				ld->name);
			goto err;
		}
		dpram->size = res_size;
	}
	dpld->base = dpram->base;
	dpld->size = dpram->size;

	mif_info("%s: type %d, aligned %d, base 0x%08X, size %d\n",
		ld->name, dpld->type, ld->aligned, (int)dpld->base, dpld->size);

	/*
	** Retrieve DPRAM SFR resource if exists
	*/
	res = platform_get_resource_byname(pdev, IORESOURCE_MEM,
					STR_DPRAM_SFR_BASE);
	if (res) {
		res_size = resource_size(res);
		dpld->sfr_base = ioremap_nocache(res->start, res_size);
		if (!dpld->sfr_base) {
			mif_err("%s: ERR! ioremap_nocache for SFR fail\n",
				ld->name);
			goto err;
		}
	}
>>>>>>> fc9b728... update12

	/*
	** Initialize DPRAM maps (physical map -> logical map)
	*/
	ret = dpram_init_boot_map(dpld);
	if (ret < 0) {
		mif_err("%s: ERR! dpram_init_boot_map fail (err %d)\n",
			ld->name, ret);
		goto err;
	}

	ret = dpram_init_dload_map(dpld);
	if (ret < 0) {
		mif_err("%s: ERR! dpram_init_dload_map fail (err %d)\n",
			ld->name, ret);
		goto err;
	}

	ret = dpram_init_uload_map(dpld);
	if (ret < 0) {
		mif_err("%s: ERR! dpram_init_uload_map fail (err %d)\n",
			ld->name, ret);
		goto err;
	}

	ret = dpram_init_ipc_map(dpld);
	if (ret < 0) {
<<<<<<< HEAD
		mif_info("%s: ERR! dpram_table_init fail (err %d)\n",
=======
		mif_err("%s: ERR! dpram_init_ipc_map fail (err %d)\n",
>>>>>>> fc9b728... update12
			ld->name, ret);
		goto err;
	}

	if (dpram->res_ack_wait_timeout > 0)
		dpld->res_ack_wait_timeout = dpram->res_ack_wait_timeout;
	else
		dpld->res_ack_wait_timeout = RES_ACK_WAIT_TIMEOUT;

	/* Disable IPC */
<<<<<<< HEAD
	set_magic(dpld, 0);
	set_access(dpld, 0);
	dpld->dpram_init_status = DPRAM_INIT_STATE_NONE;

	/* Initialize locks, completions, and bottom halves */
	snprintf(dpld->wlock_name, DP_MAX_NAME_LEN, "%s_wlock", ld->name);
	wake_lock_init(&dpld->wlock, WAKE_LOCK_SUSPEND, dpld->wlock_name);

	init_completion(&dpld->dpram_init_cmd);
	init_completion(&dpld->modem_pif_init_done);
	init_completion(&dpld->udl_start_complete);
	init_completion(&dpld->udl_cmd_complete);
	init_completion(&dpld->dump_start_complete);
	init_completion(&dpld->dump_recv_done);

	task_data = (unsigned long)dpld;
	tasklet_init(&dpld->rx_tsk, dpram_ipc_rx_task, task_data);

	/* Prepare SKB queue head for RX processing */
	for (i = 0; i < dpld->max_ipc_dev; i++)
		skb_queue_head_init(&dpld->skb_rxq[i]);

	/* Prepare RXB queue */
	qsize = DPRAM_MAX_RXBQ_SIZE;
	for (i = 0; i < dpld->max_ipc_dev; i++) {
		bsize = rxbq_get_page_size(get_rx_buff_size(dpld, i));
		dpld->rxbq[i].size = qsize;
		dpld->rxbq[i].in = 0;
		dpld->rxbq[i].out = 0;
		dpld->rxbq[i].rxb = rxbq_create_pool(bsize, qsize);
		if (!dpld->rxbq[i].rxb) {
			mif_info("%s: ERR! %s rxbq_create_pool fail\n",
				ld->name, get_dev_name(i));
			goto err;
		}
		mif_info("%s: %s rxbq_pool created (bsize:%d, qsize:%d)\n",
			ld->name, get_dev_name(i), bsize, qsize);
	}

=======
	if (!dpram->disabled) {
		set_magic(dpld, 0);
		set_access(dpld, 0);
	}
	dpld->init_status = DPRAM_INIT_STATE_NONE;

	/*
	** Initialize locks, completions, and bottom halves
	*/
	snprintf(dpld->wlock_name, MIF_MAX_NAME_LEN, "%s_wlock", ld->name);
	wake_lock_init(&dpld->wlock, WAKE_LOCK_SUSPEND, dpld->wlock_name);

	init_completion(&dpld->udl_cmpl);
	init_completion(&dpld->crash_cmpl);

	for (i = 0; i < ld->max_ipc_dev; i++)
		init_completion(&dpld->req_ack_cmpl[i]);

	INIT_DELAYED_WORK(&dpld->rx_dwork, ipc_rx_work);

	for (i = 0; i < ld->max_ipc_dev; i++) {
		spin_lock_init(&dpld->tx_lock[i]);
		atomic_set(&dpld->res_required[i], 0);
	}

	ld->tx_wq = create_singlethread_workqueue("dpram_tx_wq");
	if (!ld->tx_wq) {
		mif_err("%s: ERR! fail to create tx_wq\n", ld->name);
		goto err;
	}
	INIT_DELAYED_WORK(&ld->fmt_tx_dwork, fmt_tx_work);
	INIT_DELAYED_WORK(&ld->raw_tx_dwork, raw_tx_work);
	INIT_DELAYED_WORK(&ld->rfs_tx_dwork, rfs_tx_work);
	ld->tx_dwork[IPC_FMT] = &ld->fmt_tx_dwork;
	ld->tx_dwork[IPC_RAW] = &ld->raw_tx_dwork;
	ld->tx_dwork[IPC_RFS] = &ld->rfs_tx_dwork;

#ifdef DEBUG_MODEM_IF
	spin_lock_init(&dpld->stat_list.lock);
	spin_lock_init(&dpld->trace_list.lock);
#endif

>>>>>>> fc9b728... update12
	/* Prepare a multi-purpose miscellaneous buffer */
	dpld->buff = kzalloc(dpld->dp_size, GFP_KERNEL);
	if (!dpld->buff) {
		mif_info("%s: ERR! kzalloc dpld->buff fail\n", ld->name);
		goto err;
	}

<<<<<<< HEAD
	/* Retrieve DPRAM IRQ GPIO# */
	dpld->gpio_dpram_int = mdm_data->gpio_dpram_int;

	/* Retrieve DPRAM IRQ# */
	if (!dpctl->dpram_irq) {
		dpctl->dpram_irq = platform_get_irq_byname(pdev, "dpram_irq");
		if (dpctl->dpram_irq < 0) {
			mif_info("%s: ERR! platform_get_irq_byname fail\n",
				ld->name);
=======
	/*
	** Retrieve DPRAM IRQ GPIO#, IRQ#, and IRQ flags
	*/
	dpld->gpio_int2ap = modem->gpio_ipc_int2ap;
	dpld->gpio_cp_status = modem->gpio_cp_status;
	dpld->gpio_cp_wakeup = modem->gpio_cp_wakeup;
	if (dpram->type == AP_IDPRAM) {
		if (!modem->gpio_ipc_int2cp) {
			mif_err("%s: ERR! no gpio_ipc_int2cp\n", ld->name);
>>>>>>> fc9b728... update12
			goto err;
		}
		dpld->gpio_int2cp = modem->gpio_ipc_int2cp;
	}
	dpld->irq = dpctl->dpram_irq;

<<<<<<< HEAD
	/* Retrieve DPRAM IRQ flags */
	if (!dpctl->dpram_irq_flags)
		dpctl->dpram_irq_flags = (IRQF_NO_SUSPEND | IRQF_TRIGGER_LOW);
	dpld->irq_flags = dpctl->dpram_irq_flags;

	/* Register DPRAM interrupt handler */
	snprintf(dpld->irq_name, DP_MAX_NAME_LEN, "%s_irq", ld->name);
=======
	dpld->irq = modem->irq_ipc_int2ap;

	if (modem->irqf_ipc_int2ap)
		dpld->irq_flags = modem->irqf_ipc_int2ap;
	else
		dpld->irq_flags = (IRQF_NO_SUSPEND | IRQF_TRIGGER_LOW);

	/*
	** Initialize power management (PM) for AP_IDPRAM
	*/
	if (dpld->type == AP_IDPRAM) {
		dpld->pm_op = idpram_get_pm_op(dpld->ap);
		if (!dpld->pm_op) {
			mif_err("%s: no pm_op for AP_IDPRAM\n", ld->name);
			goto err;
		}

		ret = dpld->pm_op->pm_init(dpld, modem, pm_tx_work);
		if (ret) {
			mif_err("%s: pm_init fail (err %d)\n", ld->name, ret);
			goto err;
		}
	}

	/*
	** Register DPRAM interrupt handler
	*/
	snprintf(dpld->irq_name, MIF_MAX_NAME_LEN, "%s_irq", ld->name);
>>>>>>> fc9b728... update12
	if (dpld->ext_op && dpld->ext_op->irq_handler)
		dpld->irq_handler = dpld->ext_op->irq_handler;
	else if (dpld->dp_type == CP_IDPRAM)
		dpld->irq_handler = cp_idpram_irq_handler;
	else if (dpld->dp_type == AP_IDPRAM)
		dpld->irq_handler = ap_idpram_irq_handler;
	else
		dpld->irq_handler = ext_dpram_irq_handler;

<<<<<<< HEAD
	ret = dpram_register_isr(dpld->irq, dpld->irq_handler, dpld->irq_flags,
=======
	ret = mif_register_isr(dpld->irq, dpld->irq_handler, dpld->irq_flags,
>>>>>>> fc9b728... update12
				dpld->irq_name, dpld);
	if (ret)
		goto err;
	else
		return ld;

err:
	if (dpld) {
		kfree(dpld->buff);
		kfree(dpld);
	}

	return NULL;
}

