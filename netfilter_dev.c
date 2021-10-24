#include <linux/fs.h>
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/kdev_t.h>
#include <linux/device/class.h>
#include <linux/device.h>
#include <linux/netfilter.h>
#include <linux/skbuff.h>
#define NF_NL_DEV	("nf_nl_dev")
struct nf_nl_dev {
	dev_t major;
	struct class *class;
	struct device *device;
	struct sk_buff_head *list_head;
	struct nf_hook_ops hook_ops;

	wait_queue_head_t waitqueue;	
};

struct nf_nl_dev *dev = NULL;
static int nf_hook_init(struct nf_nl_dev *dev)
{
	return nf_register_net_hook(&init_net,&(dev->hook_ops));
}
static void nf_hook_uinit(struct nf_nl_dev *dev)
{
	nf_unregister_net_hook(&init_net,&(dev->hook_ops));
}

#define NF_SK_LEN	(100)
static unsigned int   nf_hook_op(void *priv,struct sk_buff *skb,const struct nf_hook_state *state)
{
	struct sk_buff *n = NULL;
	struct sk_buff_head *list_head = dev->list_head;
	if(skb_queue_len(list_head) >= NF_SK_LEN)
		goto out;	
	n = skb_copy(skb,GFP_ATOMIC);
	if(n)
		__skb_queue_before(list_head,(struct sk_buff *)list_head,n);
	wake_up_interruptible(&dev->waitqueue);	
out:
	return NF_ACCEPT;	
}

static int sk_head_init(struct nf_nl_dev *dev)
{
	struct sk_buff_head *list_head = dev->list_head;
	list_head = kmalloc(sizeof(struct sk_buff_head),GFP_ATOMIC);
	if(!(list_head))
		return ENOMEM;
	skb_queue_head_init(list_head);
	dev->hook_ops.pf = NFPROTO_NETDEV;
	dev->hook_ops.hooknum = NF_NETDEV_INGRESS;
	dev->hook_ops.hook = nf_hook_op;

	return 0;
	
}

static void sk_head_uinit(struct nf_nl_dev *dev)
{
	kfree(dev->list_head);
	dev->list_head = NULL;
}

static struct sk_buff *get_skb (struct sk_buff_head *list_head)
{

	if(skb_queue_empty(list_head))
		return NULL;

	return skb_dequeue(list_head);
}


static int nf_ops_open(struct inode *inode,struct file *file)
{
	int ret = 0;
	file->private_data = (void *)dev;
	ret = sk_head_init(dev);
	if(ret)
		goto err0;
	ret = nf_hook_init(dev);
	if(ret)
		goto err1;
err1:
err0:
	return 0;
}

static int nf_ops_close(struct inode *inode,struct file *file)
{
	nf_hook_uinit(dev);
	sk_head_uinit(dev);
	return 0;
}

static __poll_t nf_ops_poll(struct file *file,struct poll_table_struct *wait)
{
	__poll_t ret = 0;
	poll_wait(file,&dev->waitqueue,wait);
	if(!skb_queue_empty(dev->list_head))
		ret |= EPOLLIN | EPOLLRDNORM;

	return ret;
}
static ssize_t nf_ops_read(struct file *file,char __user *buf,size_t size,loff_t *off)
{
	int ret = 0;
	struct sk_buff *n = NULL;
	if(size < sizeof(struct sk_buff))
		return -ENOMEM;
	n = get_skb(dev->list_head);
	if(!n)
		return -EAGAIN;
	ret = copy_to_user(buf,(char *)n,sizeof(struct sk_buff));
	if(ret)
		return -EFAULT;
	return sizeof(struct sk_buff);
}
struct file_operations nf_dev_ops = {
	.open = nf_ops_open,
	.read = nf_ops_read,
	.poll = nf_ops_poll,
	.release = nf_ops_close,
};

static int nf_dev_init(void)
{
	int ret = 0;
	dev = kmalloc(sizeof(struct nf_nl_dev),GFP_ATOMIC);
	if(!dev)
		return -ENOMEM;

	ret = register_chrdev(0,NF_NL_DEV,&nf_dev_ops);
	if(ret > 0){
		printk("major = %d\n",ret);
		dev->major = ret;
	}else {
		return ret;
	}
	dev->class = class_create(THIS_MODULE,NF_NL_DEV);
	if(IS_ERR(dev->class)){
		ret = PTR_ERR(dev->class);
		goto err1;
	}

	dev->device = device_create(dev->class,NULL,MKDEV(dev->major,0),(void *)dev,"%s",NF_NL_DEV);
	if(IS_ERR(dev->device)){
		ret = PTR_ERR(dev->device);
		goto err2;
	}
	init_waitqueue_head(&(dev->waitqueue));

	return 0;
err2:
	class_destroy(dev->class);
err1:
	unregister_chrdev(dev->major,NF_NL_DEV);
	return ret;
}

static void  nf_dev_uinit(void)
{
	if(!dev){
		device_destroy(dev->class,MKDEV(dev->major,0));
		class_destroy(dev->class);
		unregister_chrdev(dev->major,NF_NL_DEV);
	}
	
}


module_init(nf_dev_init);
module_exit(nf_dev_uinit);
MODULE_LICENSE("GPL");

