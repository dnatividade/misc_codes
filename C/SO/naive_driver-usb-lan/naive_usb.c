/* usb_naive_usb.c: a dummy net driver - Naive = Naive Is a Virtual Ethernet
 *
 *  NaIVE is based in "dummy network driver" and "skel USB device driver".

	The purpose of this driver is to provide access to Arduino connected
	on local machine, creating a network device.

		Diego Natividade and Lair Servulo

		Based on:	
			Nick Holloway, 27th May 1994
			Alan Cox, 30th May 1994
*/

/** INFORMATION
 *  -----------
 * "naive_usb_" functions = USB functions
 * "naive_net_" functions = NETWORK functions
 *
**/


//Includes to NETWORK ////////////////
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/rtnetlink.h>
#include <net/rtnetlink.h>
#include <linux/u64_stats_sync.h>
#include <linux/buffer_head.h>

#include <linux/io.h>

#define DRV_NAME	"NaIVE"
#define DRV_VERSION	"0.2"

//Includes to USB ///////////////////
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <linux/usb.h>
#include <linux/uaccess.h>

#define VENDOR_ID_QINHENG	0x1a86
#define PRODUCT_ID_HL340	0x7523


//table of devices that work with this driver
//actualy, only QinHeng HL-340 Usb-Serial adapter (Arduino like)
static struct usb_device_id usb_naive_table [] = {
	{ USB_DEVICE(VENDOR_ID_QINHENG, PRODUCT_ID_HL340) },
	{ }
};
MODULE_DEVICE_TABLE (usb, usb_naive_table);


//Get a minor range for your devices from the usb maintainer
#define USB_SKEL_MINOR_BASE	192


//Structure to hold all of our device specific stuff
struct usb_naive_usb {
	struct usb_device *	udev;			/* the usb device for this device */
	struct usb_interface *	interface;		/* the interface for this device */
	unsigned char *		bulk_in_buffer;		/* the buffer to receive data */
	size_t			bulk_in_size;		/* the size of the receive buffer */
	__u8			bulk_in_endpointAddr;	/* the address of the bulk in endpoint */
	__u8			bulk_out_endpointAddr;	/* the address of the bulk out endpoint */
	struct kref		kref;
};
#define to_usb_naive_dev(d) container_of(d, struct usb_naive_usb, kref)


/////////////////////////////////////////////////////////////////////////////
//Functions interfaces ///////////////////////////////////////////////////////
//Network functions
static int __init naive_net_init_one(void);
static int __init naive_net_init_module(void);
static int net_naive_create_interface(void);

static int naive_net_dev_init(struct net_device *dev);
static void naive_net_dev_uninit(struct net_device *dev);
static netdev_tx_t naive_net_xmit(struct sk_buff *skb, struct net_device *dev);
static void set_multicast_list(struct net_device *dev);
static struct rtnl_link_stats64 *naive_net_get_stats64(struct net_device *dev, struct rtnl_link_stats64 *stats);
static int naive_net_change_carrier(struct net_device *dev, bool new_carrier);

static void naive_net_setup(struct net_device *dev);
static int naive_net_validate(struct nlattr *tb[], struct nlattr *data[]);


//USB functions
static struct usb_driver usb_naive_driver;
static int usb_naive_probe(struct usb_interface *interface, const struct usb_device_id *id);
static void usb_naive_disconnect(struct usb_interface *interface);
//static ssize_t usb_naive_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos);
static ssize_t usb_naive_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *ppos);
static int usb_naive_open(struct inode *inode, struct file *file);
static int usb_naive_release(struct inode *inode, struct file *file);
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
//NETWORK Structs///////////////////////////////////////////
static struct rtnl_link_ops naive_net_link_ops __read_mostly = {
	.kind		= DRV_NAME,
	.setup		= naive_net_setup,
	.validate	= naive_net_validate,
};

static const struct net_device_ops naive_net_netdev_ops = {
	.ndo_init		= naive_net_dev_init,
	.ndo_uninit		= naive_net_dev_uninit,
	.ndo_start_xmit		= naive_net_xmit,
	.ndo_set_rx_mode	= set_multicast_list,
	.ndo_get_stats64	= naive_net_get_stats64,
	.ndo_change_carrier	= naive_net_change_carrier,
//	.ndo_validate_addr	= eth_validate_addr,
//	.ndo_set_mac_address	= eth_mac_addr,
};


////////////////////////////////////////////////////////////
//USB Structs///////////////////////////////////////////////
static struct file_operations usb_naive_fops = {
	.owner =	THIS_MODULE,
	.open =		usb_naive_open,
	.release =	usb_naive_release,
	.write =	usb_naive_write,
//	.read =		usb_naive_read,
};

static struct usb_driver usb_naive_driver = {
	.name = "naive",
	.id_table = usb_naive_table,
	.probe = usb_naive_probe,
	.disconnect = usb_naive_disconnect,
};

static struct usb_class_driver usb_naive_class = {
	.name = "naive%d",
	.fops = &usb_naive_fops,
	.minor_base = USB_SKEL_MINOR_BASE,
};
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

//Aux. structs ////////////////////////////////////////////////////////////
static int numnaives = 1;

union packet {
	struct sk_buff skb;
	unsigned char bytes[sizeof(struct sk_buff)];
};

union usb_stream {
	struct urb urb_stream;
	unsigned char byets[sizeof(struct urb)];
};
///////////////////////////////////////////////////////////////////////////


static void usb_naive_delete(struct kref *kref) {	
	struct usb_naive_usb *dev = to_usb_naive_dev(kref);

	usb_put_dev(dev->udev);
	kfree (dev->bulk_in_buffer);
	kfree (dev);
}

static int usb_naive_open(struct inode *inode, struct file *file) {
	struct usb_naive_usb *dev;
	struct usb_interface *interface;
	int subminor;
	int retval = 0;

	subminor = iminor(inode);

	interface = usb_find_interface(&usb_naive_driver, subminor);
	if (!interface) {
		pr_err("%s - error, can't find device for minor %d", __FUNCTION__, subminor);
		retval = -ENODEV;
		goto exit;
	}

	dev = usb_get_intfdata(interface);
	if (!dev) {
		retval = -ENODEV;
		goto exit;
	}
	
	//increment our usage count for the device
	kref_get(&dev->kref);

	//save our object in the file's private structure
	file->private_data = dev;

exit:
	return retval;
}

static int usb_naive_release(struct inode *inode, struct file *file) {
	struct usb_naive_usb *dev;

	dev = (struct usb_naive_usb *)file->private_data;
	if (dev == NULL)
		return -ENODEV;

	//decrement the count on our device
	kref_put(&dev->kref, usb_naive_delete);
	return 0;
}

/* NOT USED YET
static ssize_t usb_naive_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos) {
	struct usb_naive_usb *dev;
	int retval = 0;

	dev = (struct usb_naive_usb *)file->private_data;
	
	//do a blocking bulk read to get data from the device
	retval = usb_bulk_msg(dev->udev,
				  usb_rcvbulkpipe(dev->udev, dev->bulk_in_endpointAddr),
				  dev->bulk_in_buffer,
				  min(dev->bulk_in_size, count),
				  (int *) &count, HZ*10);

	//if the read was successful, copy the data to userspace
	if (!retval) {
		if (copy_to_user(buffer, dev->bulk_in_buffer, count))
			retval = -EFAULT;
		else
			retval = count;
	}

	return retval;
}
*/


static void usb_naive_write_bulk_callback(struct urb *urb) {
	struct usb_naive_usb *dev = urb->context;

	//sync/async unlink faults aren't errors
	if (urb->status && 
		!(urb->status == -ENOENT || 
		 urb->status == -ECONNRESET ||
		 urb->status == -ESHUTDOWN)) {
			dev_dbg(&dev->interface->dev,
			"%s - nonzero write bulk status received: %d",
			__FUNCTION__, urb->status);
	}

	//free up our allocated buffer
	usb_free_coherent(urb->dev, 
		urb->transfer_buffer_length, 
		urb->transfer_buffer, 
		urb->transfer_dma);
}


static ssize_t usb_naive_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *ppos) {
	struct usb_naive_usb *dev;
	int retval = 0;
	struct urb *urb = NULL;
	char *buf = NULL;

	dev = (struct usb_naive_usb *)file->private_data;

	//verify that we actually have some data to write
	if (count == 0)
		goto exit;

	//create a urb, and a buffer for it, and copy the data to the urb
	urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!urb) {
		retval = -ENOMEM;
		goto error;
	}

	buf = usb_alloc_coherent(dev->udev, count, GFP_KERNEL, &urb->transfer_dma);
	if (!buf) {
		retval = -ENOMEM;
		goto error;
	}
	if (copy_from_user(buf, user_buffer, count)) {
		retval = -EFAULT;
		goto error;
	}

	//initialize the urb properly
	usb_fill_bulk_urb(urb, dev->udev,
		usb_sndbulkpipe(dev->udev, dev->bulk_out_endpointAddr),
		buf, count, usb_naive_write_bulk_callback, dev);
	urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

	//send the data out the bulk port
	retval = usb_submit_urb(urb, GFP_KERNEL);
	if (retval) {
		pr_err("%s - failed submitting write urb, error %d", __FUNCTION__, retval);
		goto error;
	}

	//release our reference to this urb, the USB core will eventually free it entirely
	usb_free_urb(urb);

exit:
	return count;

error:
	usb_free_coherent(dev->udev, count, buf, urb->transfer_dma);
	usb_free_urb(urb);
	kfree(buf);
	return retval;
}


//Device connected on USB...
static int usb_naive_probe(struct usb_interface *interface, const struct usb_device_id *id) {
	struct usb_naive_usb *dev = NULL;
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	size_t buffer_size;
	int i;
	int retval = -ENOMEM;

	//allocate memory for our device state and initialize it
	dev = kzalloc(sizeof(struct usb_naive_usb), GFP_KERNEL);
	if (!dev) {
		pr_err("Out of memory");
		goto error;
	}
	kref_init(&dev->kref);

	dev->udev = usb_get_dev(interface_to_usbdev(interface));
	dev->interface = interface;

	//set up the endpoint information
	//use only the first bulk-in and bulk-out endpoints
	iface_desc = interface->cur_altsetting;
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
		endpoint = &iface_desc->endpoint[i].desc;

		if (!dev->bulk_in_endpointAddr &&
			(endpoint->bEndpointAddress & USB_DIR_IN) &&
			((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
					== USB_ENDPOINT_XFER_BULK)) {
			//we found a bulk in endpoint
			buffer_size = endpoint->wMaxPacketSize;
			dev->bulk_in_size = buffer_size;
			dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
			dev->bulk_in_buffer = kmalloc(buffer_size, GFP_KERNEL);
			if (!dev->bulk_in_buffer) {
				pr_err("Could not allocate bulk_in_buffer");
				goto error;
			}
		}

		if (!dev->bulk_out_endpointAddr &&
			!(endpoint->bEndpointAddress & USB_DIR_IN) &&
			((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
					== USB_ENDPOINT_XFER_BULK)) {
			//we found a bulk out endpoint
			dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
		}
	}
	if (!(dev->bulk_in_endpointAddr && dev->bulk_out_endpointAddr)) {
		pr_err("Could not find both bulk-in and bulk-out endpoints");
		goto error;
	}

	//save our data pointer in this interface device
	usb_set_intfdata(interface, dev);

	//we can register the device now, as it is ready
	retval = usb_register_dev(interface, &usb_naive_class);
	if (retval) {
		//something prevented us from registering this driver
		pr_err("Not able to get a minor for this device.");
		usb_set_intfdata(interface, NULL);
		goto error;
	}

	//let the user know what node this device is now attached to
	dev_info(&interface->dev, "NaIVE connected on [naive%d]", interface->minor);

	///////////////////////////////
	net_naive_create_interface();//
	///////////////////////////////
	
	return 0;

error:
	if (dev)
		kref_put(&dev->kref, usb_naive_delete);

	return retval;
}

//Device disconnected from USB...
static void usb_naive_disconnect(struct usb_interface *interface) {
	struct usb_naive_usb *dev;
	int minor = interface->minor;
	
	//Network/////////////////////////////////
	//deregister this driver with the network
	rtnl_link_unregister(&naive_net_link_ops);
	//////////////////////////////////////////
	
	dev = usb_get_intfdata(interface);
	usb_set_intfdata(interface, NULL);

	//give back our minor
	usb_deregister_dev(interface, &usb_naive_class);

	//decrement our usage count
	kref_put(&dev->kref, usb_naive_delete);

	dev_info(&interface->dev, "NaIVE [#%d] disconnected", minor);
}


//Open USB device
struct file *file_open(const char *path, int flags, int rights) {
    struct file *filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if (IS_ERR(filp)) {
       err = PTR_ERR(filp);
       return NULL;
   }
   return filp;
}

//Close USB device
void file_close(struct file *file) {
	filp_close(file, NULL);
}

//Read USB device
int file_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) {
	mm_segment_t oldfs;
	int ret;

	oldfs = get_fs();
	set_fs(get_ds());

	ret = vfs_read(file, data, size, &offset);

	set_fs(oldfs);
	return ret;
}   

//Write USB device
int file_write(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) {
	mm_segment_t oldfs;
	int ret;

	oldfs = get_fs();
	set_fs(get_ds());

	ret = vfs_write(file, data, size, &offset);

	set_fs(oldfs);
	return ret;
}
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

/* fake multicast ability */
static void set_multicast_list(struct net_device *dev) {
	printk(KERN_DEBUG "pkt recv [<-]\n"); //DIEGO//

	/////////////////////////////////
	// *** NOT IMPLEMENTED YET *** //
	/////////////////////////////////
}


//Statistics
struct pcpu_dstats {
	u64			tx_packets;
	u64			tx_bytes;
	struct u64_stats_sync	syncp;
};


//Statistics
static struct rtnl_link_stats64 *naive_net_get_stats64(struct net_device *dev, struct rtnl_link_stats64 *stats) {
	int i;

	for_each_possible_cpu(i) {
		const struct pcpu_dstats *dstats;
		u64 tbytes, tpackets;
		unsigned int start;

		dstats = per_cpu_ptr(dev->dstats, i);
		do {
			start = u64_stats_fetch_begin_irq(&dstats->syncp);
			tbytes = dstats->tx_bytes;
			tpackets = dstats->tx_packets;
		} while (u64_stats_fetch_retry_irq(&dstats->syncp, start));
		stats->tx_bytes += tbytes;
		stats->tx_packets += tpackets;
	}
	return stats;
}


//Transmit data
static netdev_tx_t naive_net_xmit(struct sk_buff *skb, struct net_device *dev) {
	struct pcpu_dstats *dstats;
	//int i, ret_usb;
	union packet p;
	//union usb_stream str;
	struct sk_buff *aux;
	
	int ii=0;
	int tam=0;
	char str[2000];
	int ite=0;

	struct file *f;	
	//char buffer[p.skb.len];
	int jj=0;
        //char device_name[15];
	//memcpy(device_name, "/dev/", 5);
	//memcpy(device_name+5, usb_naive_class.name, 6);
	//f = file_open(device_name, O_RDWR | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO);
	f = file_open("/dev/naive0", O_RDWR | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO);

	///////////////////////////////////////////////////////////////
	printk(KERN_INFO "\nFRAME START [->]");
	aux = skb;
	while(aux != NULL) {
		p.skb = *aux;
		//void* vptr_test = &p;
		memcpy(str+ii*p.skb.len, p.skb.data, p.skb.len);
		tam+=p.skb.len;
		aux = aux->next;
		ite++;
	}

	printk(KERN_ERR "ITERATIONS: %d", ite);
	printk(KERN_WARNING "Frame len: %d", tam); //print packt length
	for (jj=0; jj<tam; jj++) { //print packet content (hex)
		if (jj == 0)
			printk(KERN_INFO "\\%02x", str[jj] & 0xff);
		else
			printk(KERN_CONT "\\%02x", str[jj] & 0xff);
	}

	file_write(f, 0, str, tam); //write packet on USB device
	//printk(KERN_ERR "DATA_LEN: %d", p.skb.data_len); //print data length (for debug)

	u64_stats_update_begin(&dstats->syncp); //statistics
	dstats->tx_packets++;
	dstats->tx_bytes += skb->len;
	u64_stats_update_end(&dstats->syncp);

	dev_kfree_skb(skb); //free packet

	printk(KERN_INFO "FRAME END [.]"); //print

	return NETDEV_TX_OK;
}

//Init
static int naive_net_dev_init(struct net_device *dev) {
	dev->dstats = netdev_alloc_pcpu_stats(struct pcpu_dstats);
	if (!dev->dstats)
		return -ENOMEM;

	return 0;
}


//Free statistics
static void naive_net_dev_uninit(struct net_device *dev) {
	free_percpu(dev->dstats);
}


static int naive_net_change_carrier(struct net_device *dev, bool new_carrier) {
	if (new_carrier)
		netif_carrier_on(dev);
	else
		netif_carrier_off(dev);
	return 0;
}


static void naive_net_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info) {
	strlcpy(info->driver, DRV_NAME, sizeof(info->driver));
	strlcpy(info->version, DRV_VERSION, sizeof(info->version));
}

static const struct ethtool_ops naive_net_ethtool_ops = {
	.get_drvinfo            = naive_net_get_drvinfo,
};

static void naive_net_setup(struct net_device *dev) {
	ether_setup(dev);

	//Initialize the device structure.
	dev->netdev_ops = &naive_net_netdev_ops;
	dev->ethtool_ops = &naive_net_ethtool_ops;
	dev->destructor = free_netdev;

	//Fill in device structure with ethernet-generic values.
	dev->flags |= IFF_NOARP;
	dev->flags &= ~IFF_MULTICAST;
	dev->priv_flags |= IFF_LIVE_ADDR_CHANGE | IFF_NO_QUEUE;
	dev->features	|= NETIF_F_SG | NETIF_F_FRAGLIST;
	dev->features	|= NETIF_F_ALL_TSO | NETIF_F_UFO;
	dev->features	|= NETIF_F_HW_CSUM | NETIF_F_HIGHDMA | NETIF_F_LLTX;
	dev->features	|= NETIF_F_GSO_ENCAP_ALL;
	dev->hw_features |= dev->features;
	dev->hw_enc_features |= dev->features;
	eth_hw_addr_random(dev);
}

static int naive_net_validate(struct nlattr *tb[], struct nlattr *data[]) {
	if (tb[IFLA_ADDRESS]) {
		if (nla_len(tb[IFLA_ADDRESS]) != ETH_ALEN)
			return -EINVAL;
		if (!is_valid_ether_addr(nla_data(tb[IFLA_ADDRESS])))
			return -EADDRNOTAVAIL;
	}
	return 0;
}

//Number of NaIVE devices to be set up by this module.
module_param(numnaives, int, 0);
MODULE_PARM_DESC(numnaives, "Number of NaIVE pseudo devices");


static int __init naive_net_init_one(void) {
	struct net_device *dev_naive_net;
	int err;

	//device name
	dev_naive_net = alloc_netdev(0, "NaIVE%d", NET_NAME_UNKNOWN, naive_net_setup);
	if (!dev_naive_net)
		return -ENOMEM;

	dev_naive_net->rtnl_link_ops = &naive_net_link_ops;
	err = register_netdevice(dev_naive_net);
	if (err < 0)
		goto err;
	return 0;

err:
	free_netdev(dev_naive_net);
	return err;
}


//Create Network Interface
static int net_naive_create_interface(void){
	int i, err = 0;

	printk(KERN_INFO "Creating NaIVE NETWORK INterfACe...");

	rtnl_lock();
	err = __rtnl_link_register(&naive_net_link_ops);
	if (err < 0)
		goto out;

	for (i = 0; i < numnaives && !err; i++) {
		err = naive_net_init_one();
		cond_resched();
	}
	if (err < 0)
		__rtnl_link_unregister(&naive_net_link_ops);

out:
	rtnl_unlock();

	return err;
}


//Driver initialization
//run on UP
static int __init naive_net_init_module(void) {
	int err = 0;

//// DESACOPLATED: net_naive_created_interface() ////
//
//	rtnl_lock();
//	err = __rtnl_link_register(&naive_net_link_ops);
//	if (err < 0)
//		goto out;
//
//	for (i = 0; i < numnaives && !err; i++) {
//		err = naive_net_init_one();
//		cond_resched();
//	}
//	if (err < 0)
//		__rtnl_link_unregister(&naive_net_link_ops);
//
//out:
//	rtnl_unlock();
///////////////////////////////////////////////////////

	//register this driver with the USB subsystem
	err = usb_register(&usb_naive_driver);
	if (err)
		pr_err("NaIVE usb_register failed. Error number %d", err);

	return err;
}


//Driver finalization
//run on DOWN
static void __exit naive_net_cleanup_module(void) {
	//deregister this driver with the network
	rtnl_link_unregister(&naive_net_link_ops);
	//deregister this driver with the USB subsystem
	usb_deregister(&usb_naive_driver);
}

module_init(naive_net_init_module);
module_exit(naive_net_cleanup_module);

MODULE_LICENSE("GPL");
MODULE_ALIAS_RTNL_LINK(DRV_NAME);
MODULE_VERSION(DRV_VERSION);


