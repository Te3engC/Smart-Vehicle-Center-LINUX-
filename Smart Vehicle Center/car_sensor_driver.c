#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/ktime.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/of.h>

#define DHT_TIMEOUT 20000

struct __packed sensor_packet {
    int distance;
    int temperature;
    int humidity;
    unsigned long ts;
};

struct car_sensor_dev {
    struct gpio_desc *dht11;
    struct gpio_desc *trig;
    struct gpio_desc *echo;
    int irq;
    ktime_t echo_start;
    int last_dist;
    struct completion hscr_done;
    dev_t dev_num;
    struct cdev cdev;
    struct class *class;
};

/* DHT11 读取逻辑：带超时保护 */
static int read_dht_safe(struct gpio_desc *gpio, int *t, int *h) {
    u8 data[5] = {0};
    int i, j, timeout;
    unsigned long flags;

    local_irq_save(flags);
    preempt_disable();

    gpiod_direction_output(gpio, 0);
    msleep(20);
    gpiod_direction_input(gpio);
    
    // 超时检测：等待 DHT11 拉低响应
    timeout = DHT_TIMEOUT;
    while(gpiod_get_value(gpio)) if (--timeout <= 0) goto out;
    timeout = DHT_TIMEOUT;
    while(!gpiod_get_value(gpio)) if (--timeout <= 0) goto out;
    timeout = DHT_TIMEOUT;
    while(gpiod_get_value(gpio)) if (--timeout <= 0) goto out;

    for(j = 0; j < 5 ; j++) {
        for(i = 0; i < 8; i++) {
            timeout = DHT_TIMEOUT;
            while(!gpiod_get_value(gpio)) if (--timeout <= 0) goto out;
            udelay(35);
            if(gpiod_get_value(gpio)) {
                data[j] |= (1 << (7-i));
                timeout = DHT_TIMEOUT;
                while(gpiod_get_value(gpio)) if (--timeout <= 0) goto out;
            }
        }
    }

out:
    preempt_enable();
    local_irq_restore(flags);
    if(data[4] != (u8)(data[0]+data[1]+data[2]+data[3])) return -1;
    *h = data[0]; *t = data[2];
    return 0;
}

static int get_median_distance(struct car_sensor_dev *dev) {
    int i, ret, buf[3] = {0};
    for(i = 0; i < 3; i++) {
        reinit_completion(&dev->hscr_done);
        gpiod_set_value(dev->trig, 1);
        udelay(12);
        gpiod_set_value(dev->trig, 0);

        // 关键：允许信号中断
        ret = wait_for_completion_interruptible_timeout(&dev->hscr_done, msecs_to_jiffies(50));
        if (ret == -ERESTARTSYS) return -ERESTARTSYS;
        buf[i] = dev->last_dist;
    }
    return buf[1]; 
}

static irqreturn_t hcsr_isr(int irq, void *id) {
    struct car_sensor_dev *sensor = id;
    if(gpiod_get_value(sensor->echo)) {
        sensor->echo_start = ktime_get();
    } else {
        s64 dur = ktime_to_us(ktime_sub(ktime_get(), sensor->echo_start));
        sensor->last_dist = (int)(dur * 17 / 1000);
        complete(&sensor->hscr_done);
    }
    return IRQ_HANDLED;
}

static ssize_t sensor_read(struct file *f, char __user *buf, size_t count, loff_t *ppos) {
    struct car_sensor_dev *dev = container_of(f->f_inode->i_cdev, struct car_sensor_dev, cdev);
    struct sensor_packet packet;
    int dist = get_median_distance(dev);
    if (dist == -ERESTARTSYS) return -ERESTARTSYS;
    packet.distance = dist;
    read_dht_safe(dev->dht11, &packet.temperature, &packet.humidity);
    packet.ts = jiffies;
    return copy_to_user(buf, &packet, sizeof(packet)) ? -EFAULT : sizeof(packet);
}

static struct file_operations fops = { .owner = THIS_MODULE, .read = sensor_read };

static int car_probe(struct platform_device *pdev) {
	int ret;
    struct car_sensor_dev *dev = devm_kzalloc(&pdev->dev, sizeof(*dev), GFP_KERNEL);
    dev->trig = devm_gpiod_get(&pdev->dev, "trig", GPIOD_OUT_LOW);
    dev->echo = devm_gpiod_get(&pdev->dev, "echo", GPIOD_IN);
    dev->dht11 = devm_gpiod_get(&pdev->dev, "dht11", GPIOD_ASIS);
    dev->irq = platform_get_irq(pdev, 0);

	if (dev->irq < 0) {
        dev_err(&pdev->dev, "Failed to get IRQ\n");
        return dev->irq;
    }

    // 捕获返回值并增加判断逻辑
    ret = devm_request_irq(&pdev->dev, dev->irq, hcsr_isr, 
                           IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, 
                           "car_sensor", dev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to request IRQ: %d\n", ret);
        return ret; 
    }

    init_completion(&dev->hscr_done);

    alloc_chrdev_region(&dev->dev_num, 0, 1, "car_sensor");
    cdev_init(&dev->cdev, &fops);
    cdev_add(&dev->cdev, dev->dev_num, 1);
    dev->class = class_create(THIS_MODULE, "car_class");
    device_create(dev->class, NULL, dev->dev_num, NULL, "car_sensor");
    platform_set_drvdata(pdev, dev);
    return 0;
}

static const struct of_device_id car_ids[] = {{.compatible = "rockchip,car-io-v1"},{}};
static struct platform_driver car_driver = {
    .probe = car_probe,
    .driver = { .name = "rk3568-car-sys", .of_match_table = car_ids }
};
module_platform_driver(car_driver);
MODULE_LICENSE("GPL");