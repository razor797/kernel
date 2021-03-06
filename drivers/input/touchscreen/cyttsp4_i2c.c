/*
 * cyttsp4_i2c.c
 * Cypress TrueTouch(TM) Standard Product V4 I2C Driver module.
 * For use with Cypress Txx4xx parts.
 * Supported parts include:
 * TMA4XX
 * TMA1036
 *
 * Copyright (C) 2012 Cypress Semiconductor
 * Copyright (C) 2011 Sony Ericsson Mobile Communications AB.
 *
 * Author: Aleksej Makarov <aleksej.makarov@sonyericsson.com>
 * Modified by: Cypress Semiconductor for test with device
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2, and only version 2, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Contact Cypress Semiconductor at www.cypress.com <ttdrivers@cypress.com>
 *
 */

#include <linux/cyttsp4_bus.h>
#include <linux/cyttsp4_core.h>
#include "cyttsp4_i2c.h"

#include <linux/delay.h>
#include <linux/hrtimer.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>
<<<<<<< HEAD
/* key led */
#include <linux/regulator/consumer.h>
#define PRESS_KEY 1	/* Fixed value */
#define RELEASE_KEY 0	/* Fixed value */
static int led_status;
extern struct class *sec_class;
=======
>>>>>>> fc9b728... update12

#define CY_I2C_DATA_SIZE  (3 * 256)

struct cyttsp4_i2c {
	struct i2c_client *client;
	u8 wr_buf[CY_I2C_DATA_SIZE];
	struct hrtimer timer;
	struct mutex lock;
	atomic_t timeout;
};

static int cyttsp4_i2c_read_block_data(struct cyttsp4_i2c *ts_i2c, u8 addr,
	size_t length, void *values)
{
<<<<<<< HEAD
	struct regulator *vreg_led = NULL;
	int ret;
	int data;
	sscanf(buf, "%d\n", &data);

	pr_info("[TSP] %s : LED buf is %d\n", __func__, data);

	if (vreg_led == NULL) {
		vreg_led = regulator_get(NULL, "KEYLED_3P3V");
		if (IS_ERR(vreg_led)) {
			pr_err("tsp: Fail to register vreg_led(KEYLED_3P3V) in touch driver\n");
			return size;
		}
	}
	if (data == 1 && led_status == 0) {
		ret = regulator_enable(vreg_led);
		if (ret)
			pr_err("tsp: Fail to enable led\n");
		led_status = 1;
	} else if (data == 2 && led_status == 1) {
		ret = regulator_disable(vreg_led);
		if (ret)
			pr_err("tsp: Fail to disable led\n");
		led_status = 0;
	}
=======
	int rc;
>>>>>>> fc9b728... update12

	/* write addr */
	rc = i2c_master_send(ts_i2c->client, &addr, sizeof(addr));
	if (rc < 0)
		return rc;
	else if (rc != sizeof(addr))
		return -EIO;

	/* read data */
	rc = i2c_master_recv(ts_i2c->client, values, length);

	return (rc < 0) ? rc : rc != length ? -EIO : 0;
}

static int cyttsp4_i2c_write_block_data(struct cyttsp4_i2c *ts_i2c, u8 addr,
	size_t length, const void *values)
{
	int rc;

	if (sizeof(ts_i2c->wr_buf) < (length + 1))
		return -ENOMEM;

	ts_i2c->wr_buf[0] = addr;
	memcpy(&ts_i2c->wr_buf[1], values, length);
	length += 1;

	/* write data */
	rc = i2c_master_send(ts_i2c->client, ts_i2c->wr_buf, length);

	return (rc < 0) ? rc : rc != length ? -EIO : 0;
}

static int cyttsp4_i2c_write(struct cyttsp4_adapter *adap, u8 addr,
	const void *buf, int size)
{
	struct cyttsp4_i2c *ts = dev_get_drvdata(adap->dev);
	int rc;

	pm_runtime_get_noresume(adap->dev);
	mutex_lock(&ts->lock);
	rc = cyttsp4_i2c_write_block_data(ts, addr, size, buf);
	mutex_unlock(&ts->lock);
	pm_runtime_put_noidle(adap->dev);

	return rc;
}

static int cyttsp4_i2c_read(struct cyttsp4_adapter *adap, u8 addr,
	void *buf, int size)
{
	struct cyttsp4_i2c *ts = dev_get_drvdata(adap->dev);
	int rc;

	pm_runtime_get_noresume(adap->dev);
	mutex_lock(&ts->lock);
	rc = cyttsp4_i2c_read_block_data(ts, addr, size, buf);
	mutex_unlock(&ts->lock);
	pm_runtime_put_noidle(adap->dev);

	return rc;
}

static struct cyttsp4_ops ops = {
	.write = cyttsp4_i2c_write,
	.read = cyttsp4_i2c_read,
};

static int __devinit cyttsp4_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *i2c_id)
{
<<<<<<< HEAD
	struct regulator *vreg_led = NULL;
	struct cyttsp4_i2c *ts;
	int retval = 0;
	/* key led */
	struct device *sec_touchkey;
=======
	struct cyttsp4_i2c *ts_i2c;
	struct device *dev = &client->dev;
	char const *adap_id = dev_get_platdata(dev);
	char const *id;
	int rc;
>>>>>>> fc9b728... update12

	dev_info(dev, "%s: Starting %s probe...\n", __func__, CYTTSP4_I2C_NAME);

	dev_dbg(dev, "%s: debug on\n", __func__);
	dev_vdbg(dev, "%s: verbose debug on\n", __func__);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(dev, "%s: fail check I2C functionality\n", __func__);
		rc = -EIO;
		goto error_alloc_data_failed;
	}

	ts_i2c = kzalloc(sizeof(struct cyttsp4_i2c), GFP_KERNEL);
	if (ts_i2c == NULL) {
		dev_err(dev, "%s: Error, kzalloc.\n", __func__);
		rc = -ENOMEM;
		goto error_alloc_data_failed;
	}

	mutex_init(&ts_i2c->lock);
	ts_i2c->client = client;
	client->dev.bus = &i2c_bus_type;
	i2c_set_clientdata(client, ts_i2c);
	dev_set_drvdata(&client->dev, ts_i2c);

	if (adap_id)
		id = adap_id;
	else
		id = CYTTSP4_I2C_NAME;

	dev_dbg(dev, "%s: add adap='%s' (CYTTSP4_I2C_NAME=%s)\n", __func__, id,
		CYTTSP4_I2C_NAME);

<<<<<<< HEAD
	/* key led */
	vreg_led = regulator_get(NULL, "KEYLED_3P3V");
	if (IS_ERR(vreg_led))
		goto err_vreg_led;
	led_status = 0;
	sec_touchkey = device_create(sec_class, NULL, 0, NULL, "sec_touchkey");
	if (device_create_file(sec_touchkey,
				&dev_attr_brightness) < 0) {
		pr_err("Failed to create device file(%s)!\n",
				dev_attr_brightness.attr.name);
=======
	pm_runtime_enable(&client->dev);

	rc = cyttsp4_add_adapter(id, &ops, dev);
	if (rc) {
		dev_err(dev, "%s: Error on probe %s\n", __func__,
			CYTTSP4_I2C_NAME);
		goto add_adapter_err;
>>>>>>> fc9b728... update12
	}

	dev_info(dev, "%s: Successful probe %s\n", __func__, CYTTSP4_I2C_NAME);

	return 0;

<<<<<<< HEAD
err_vreg_led:
	pr_err("tsp: Fail to register vreg_led(KEYLED_3P3V) in touch driver\n");
	regulator_put(vreg_led);
	return retval;
=======
add_adapter_err:
	pm_runtime_disable(&client->dev);
	dev_set_drvdata(&client->dev, NULL);
	i2c_set_clientdata(client, NULL);
	kfree(ts_i2c);
error_alloc_data_failed:
	return rc;
>>>>>>> fc9b728... update12
}

/* registered in driver struct */
static int __devexit cyttsp4_i2c_remove(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	struct cyttsp4_i2c *ts_i2c = dev_get_drvdata(dev);
	char const *adap_id = dev_get_platdata(dev);
	char const *id;

	if (adap_id)
		id = adap_id;
	else
		id = CYTTSP4_I2C_NAME;

	dev_info(dev, "%s\n", __func__);
	cyttsp4_del_adapter(id);
	pm_runtime_disable(&client->dev);
	dev_set_drvdata(&client->dev, NULL);
	i2c_set_clientdata(client, NULL);
	kfree(ts_i2c);
	return 0;
}

static const struct i2c_device_id cyttsp4_i2c_id[] = {
	{ CYTTSP4_I2C_NAME, 0 },  { }
};

static struct i2c_driver cyttsp4_i2c_driver = {
	.driver = {
		.name = CYTTSP4_I2C_NAME,
		.owner = THIS_MODULE,
	},
	.probe = cyttsp4_i2c_probe,
	.remove = __devexit_p(cyttsp4_i2c_remove),
	.id_table = cyttsp4_i2c_id,
};

static int __init cyttsp4_i2c_init(void)
{
	int rc = i2c_add_driver(&cyttsp4_i2c_driver);

	pr_info("%s: Cypress TTSP I2C Touchscreen Driver (Built %s) rc=%d\n",
		 __func__, CY_DRIVER_DATE, rc);
	return rc;
}
module_init(cyttsp4_i2c_init);

static void __exit cyttsp4_i2c_exit(void)
{
	i2c_del_driver(&cyttsp4_i2c_driver);
	pr_info("%s: module exit\n", __func__);
}
module_exit(cyttsp4_i2c_exit);

MODULE_ALIAS(CYTTSP4_I2C_NAME);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Cypress TrueTouch(R) Standard Product (TTSP) I2C driver");
MODULE_AUTHOR("Cypress");
MODULE_DEVICE_TABLE(i2c, cyttsp4_i2c_id);
