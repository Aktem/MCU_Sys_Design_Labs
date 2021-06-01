
/*moved all prototypes and includes into the headerfile*/
#include "multiplier.h" 

#define PHY_ADDR XPAR_MULTIPLY_0_S00_AXI_BASEADDR
#define MEMSIZE XPAR_MULTIPLY_0_S00_HIGHADDR - XPAR_MULTIPLY_0_S00_BASEADDR+1
#define DEVICE_NAME "multiplier"
// Don't need buffer because we are not creating a buffer to write to
// we are writing to virtual memory



// virtual address 
void* virt_addr; 


/*
*	This function is called when the module is loaded and registers a
*	device for the driver to use. It also allocates memory to the driver
*	via ioremap
*/
int my_init(void)
{
	// Allocating memory before registering the device to prevent race condition
	virt_addr = ioremap (PHY_ADDR, MEMSIZE);
	printk("The address of virt is %p and of physical is %d.", virt_addr , PHY_ADDR);
	
	// Checking to make sure memory was mapped
	if (virt_addr == NULL)
	{
		printk(KERN_ALERT "Unable to map needed memory\n");
		return 10;
	}
	
	
	/* This function call registers a device and returns a major number associated with it.*/
	Major = register_chrdev(0, DEVICE_NAME, &fops);
	if (Major < 0) {		
		/* Make sure you release any other resources you've already
		grabbed if you get here so you don't leave the kernel in a
		broken state. */
		printk(KERN_ALERT "Registering char device failed with %d\n", Major);
		return Major;
	}
	
	printk(KERN_INFO "Registered a device with dynamic Major number of %d\n", Major);
  
	printk(KERN_INFO "Create a device file for this device with this command:\n'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);

	return 0;		/* success */
}


/*
 * This function is called when the module is unloaded, it releases
 * the device file.
 */
void my_cleanup(void)
{
	unregister_chrdev(Major, DEVICE_NAME);
	printk("Device %s has been unregistered.\n",DEVICE_NAME);
}


/* 
 * Called when a process tries to open the device file, like "cat
 * /dev/my_chardev".  Link to this function placed in file operations
 * structure for our device file.
 */
static int device_open(struct inode *inode, struct file *file)
{
  static int counter = 0;	/* note this static variable only gets
				   initialized the first time this
				   function is called. */
  
  /* In these case we are only allowing one process to hold the device
     file open at a time. */
  if (Device_Open)		/* Device_Open is my flag for the
				   usage of the device file (definied
				   in my_chardev.h)  */
    return -EBUSY;		/* Failure to open device is given
				   back to the userland program. */

  Device_Open++;		/* Keeping the count of the device
				   opens. */

  /* Create a string to output when the device is opened.  This string
     is given to the user program in device_read. Note: We are using
     the "current" task structure which contains information about the
     process that opened the device file.*/
  sprintf(msg, "\"%s\" (pid %i): This device has been accessed %d times\n", current->comm, current->pid, counter++);
  msg_Ptr = msg;
  try_module_get(THIS_MODULE);	/* increment the module use count
				   (make sure this is accurate or you
				   won't be able to remove the module
				   later. */
  
  return 0;
}


/* 
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
  Device_Open--;		/* We're now ready for our next
				   caller */
  
  /* 
   * Decrement the usage count, or else once you opened the file,
   * you'll never get get rid of the module.
   */
  module_put(THIS_MODULE);
  printk("Device has been released.\n");
  
  return 0;
}


/* 
 * Called when a process, which already opened the dev file, attempts
 * to read from it.
 */
static ssize_t device_read(struct file *filp, /* see include/linux/fs.h*/
			   char *buffer,      /* buffer to fill with
						 data */
			   size_t length,     /* length of the
						 buffer  */
			   loff_t * offset)
{
	int bytes_read = 0;
	
	// reading from virtual memory
	int reg_data[3];
	for (int i=0; i<3; i++)
	{
		reg_data[i] = ioread32(virt_addr+ (i*4));
	}
	
	//Convert int to Char
	char* msg_rd = (char*)reg_data;
	
	//Write data to buffer in userspace
	while(length>0)
	{
		put_user(*(msg_rd++), buffer++);
		length--;
		bytes_read++;
	}
	
	//Examples say most read function return number of bytes put into the buffer
	return bytes_read;
}



/*Called when a process writes to dev file*/
static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	char msg[8];
	int i = 0;
	
	//get data from userland
	while(len>0){
		get_user(msg[i], buff++);
		
		i++;
		len--;
	}
	
	//cast to int
	int* int_msg = (int*) msg;
	
	// write to virtual memory
	for (int j=0; j<2; j++)
	{
		iowrite32(int_msg[i], virt_addr + (i*4));
	}
}


/*Info for modinfo*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Matthew Love");
MODULE_DESCRIPTION("Module reads from three registers and writes to two for multiplication hardware.");

/*for init and exit*/
module_init(my_init);
module_exit(my_cleanup);