#include <linux/init.h>
#include <linux/module.h>


#include <net/bluetooth/bluetooth.h>
#include <net/bluetooth/hci_core.h>
//#include <net/bluetooth/hci.h>

MODULE_LICENSE("Dual BSD/GPL");

int openDev(struct hci_dev *hdev);

static int __init bluerandom_init(void) {
//static int  bluerandom_init(void) {
    int retDescriptor1;//, retBLEScan;

    struct hci_dev *hdev = NULL;

    printk(KERN_ALERT "Hello, world BLUERANDOM\n");
    
    retDescriptor1 = openDev(hdev);

    if (retDescriptor1< 0)
        return -1;
    
    return 0;
}

static void __exit bluerandom_exit(void) {
//static void bluerandom_exit(void) {
    printk(KERN_ALERT "Goodbye, cruel world\n");
}

//EXPORT_SYMBOL(hci_dev_get);


//Make a connection to local adapter.
int openDev(struct hci_dev *hdev) {
    int ret;
    
    hdev = hci_dev_get(0); //lets go hardcoding...
    ret = hci_dev_open(hdev->id);

    if (ret < 0) {
        printk(KERN_ALERT"Error on opening HCI device. ");
    }
    return ret;
}





module_init(bluerandom_init);
module_exit(bluerandom_exit);
//MODULE_LICENSE("GPL");
