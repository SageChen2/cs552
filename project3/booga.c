/*
 * example.c -- the bare example char module
 * This example shows how to use a semaphore to avoid race conditions
 * in updating a global data structure inside a driver.
 */
#include <linux/module.h>
#include <linux/kernel.h>  /* printk() */
#include <linux/version.h> /* printk() */
#include <linux/init.h>	   /*  for module_init and module_cleanup */
#include <linux/slab.h>	   /*  for kmalloc/kfree */
#include <linux/fs.h>	   /* everything... */
#include <linux/errno.h>   /* error codes */
#include <linux/types.h>   /* size_t */
#include <linux/proc_fs.h> /* for the proc filesystem */
#include <linux/seq_file.h>
#include <linux/random.h>
#include <linux/uaccess.h>
#include "booga.h" /* local definitions */

static int booga_major = BOOGA_MAJOR;
static int booga_nr_devs = BOOGA_NR_DEVS; /* number of bare example devices */
module_param(booga_major, int, 0);
module_param(booga_nr_devs, int, 0);

MODULE_AUTHOR("Zixiao Chen");
MODULE_LICENSE("GPL v2");


int stat;
static booga_stats *booga_device_stats;
static struct proc_dir_entry *booga_proc_file;

//(mod)
char * randomPhrases[] = {"booga! booga!", "googoo! gaagaa!","neka! maka!","wooga! wooga!"};
int num;

static ssize_t booga_read(struct file *, char *, size_t, loff_t *);
static ssize_t booga_write(struct file *, const char *, size_t, loff_t *);
static int booga_open(struct inode *, struct file *);
static int booga_release(struct inode *, struct file *);
static int booga_proc_open(struct inode *inode, struct file *file);
extern void get_random_bytes(void *buf, int nbytes); //(mod)


/*  The different file operations */
/* The syntax you see below is an extension to gcc. The prefered */
/* way to init structures is to use C99 Taged syntax */
/* static struct file_operations example_fops = { */
/* 		    .read    =       example_read, */
/* 			.write   =       example_write, */
/* 			.open    =       example_open, */
/* 			.release =       example_release */
/* }; */
/*  This is where we define the standard read,write,open and release function */
/*  pointers that provide the drivers main functionality. */
static struct file_operations booga_fops = {
	read : booga_read,
	write : booga_write,
	open : booga_open,
	release : booga_release,
};


/* The file operations struct holds pointers to functions that are defined by */
/* driver impl. to perform operations on the device. What operations are needed */
/* and what they should do is dependent on the purpose of the driver. */
static const struct file_operations booga_proc_fops = {
	.owner = THIS_MODULE,
	.open = booga_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

/*
 * Open and close
 */
static int booga_open(struct inode *inode, struct file *filp)
{
	 num = NUM(inode->i_rdev);


	if (num >= booga_nr_devs)
		return -ENODEV;



	
	filp->f_op = &booga_fops;
	 //(mod) might be wrong

	/* need to protect this with a semaphore if multiple processes
		   will invoke this driver to prevent a race condition */
	if (down_interruptible(&booga_device_stats->sem))
		return (-ERESTARTSYS);
	booga_device_stats->num_open++;
	
	booga_device_stats->numOfOpens[num]++;

	up(&booga_device_stats->sem);

	try_module_get(THIS_MODULE);
	return 0; /* success */
}

static int booga_release(struct inode *inode, struct file *filp)
{
	/* need to protect this with a semaphore if multiple processes
		   will invoke this driver to prevent a race condition */
	if (down_interruptible(&booga_device_stats->sem))
		return (-ERESTARTSYS);
	booga_device_stats->num_close++;
	up(&booga_device_stats->sem);

	module_put(THIS_MODULE);
	
	return 0;
}

/*
 * Data management: read and write
 */

static ssize_t booga_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
       char randval;
	   int choice;
	   char* randString; 
	   
	   int indexOfCurrentChar = 0;
		char* combinedPhrase = (char*)kmalloc(sizeof(char)*count, GFP_KERNEL);
		int i = 0;
	   
	    printk("<1>example_read invoked.\n");
		/* need to protect this with a semaphore if multiple processes
		   will invoke this driver to prevent a race condition */


		// if (down_interruptible (&booga_device_stats->sem))
		// 		return (-ERESTARTSYS);

		get_random_bytes(&randval, 1);
		choice = (randval & 0x7F) % 4; //index

		randString = randomPhrases[choice]; //(mod) index into the array to get the phrase
		booga_device_stats->numOfStringOutput[choice]++;
											//count is the number of characters being read

		while(indexOfCurrentChar < count) //write a of the length of the count
		{
			
			if(randString[i] == '\0') //if reaches to the end of the first random string
				i = 0;	//reset

			combinedPhrase[indexOfCurrentChar] = randString[i];


			
			indexOfCurrentChar++;
			i++;

		}


		stat = __copy_to_user(buf, combinedPhrase, count);//(mod)
		
		if(stat<0){

				printk("Could not read the bytes\n");
		
		}

		if (down_interruptible (&booga_device_stats->sem))
				return (-ERESTARTSYS);

		booga_device_stats->num_read+=count;
		up(&booga_device_stats->sem);
		return count;

}

static ssize_t booga_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	printk("<1>example_write invoked.\n");
		/* need to protect this with a semaphore if multiple processes
		   will invoke this driver to prevent a race condition */
		
	
		
		
		// if (down_interruptible (&booga_device_stats->sem))
		// 		return (-ERESTARTSYS);

		//if trying to write to booga 3
		if(num == 3){

			send_sig(SIGTERM,current,0);//0 means that the signal has been sent by a User Mode process, current is the process itself
		}
		

		if (down_interruptible (&booga_device_stats->sem))
				return (-ERESTARTSYS);
		
				//copy to user
		booga_device_stats->num_write+=count;
		
		
		up(&booga_device_stats->sem);

		return count; // pretend that count bytes were written
}

static void init_booga_device_stats(void)
{

	int i,j;
	booga_device_stats->num_read = 0;
	booga_device_stats->num_write = 0;
	booga_device_stats->num_open = 0;
	booga_device_stats->num_close = 0;

	//intialize all the struct data fields
	//int i,j;

	for(i=0; i<4; i++){
		booga_device_stats->numOfOpens[i] = 0; // for every different kind of string, they appear 0 times in the beginning.
	}

	for(j=0; j<4; j++){

		booga_device_stats->numOfStringOutput[j] = 0; 
	}



	sema_init(&booga_device_stats->sem, 1);
}

static int booga_proc_show(struct seq_file *m, void *v)
{
	int i,j;
	
	seq_printf(m, "bytes read = %ld\n", booga_device_stats->num_read);
	seq_printf(m, "bytes written = %ld\n", booga_device_stats->num_write);

	
	seq_printf(m, "number of opens:\n");
	for(i = 0; i < 4; i++)
			seq_printf(m, "/dev/booga%d = %ld times\n", i, booga_device_stats->numOfOpens[i]);

	seq_printf(m, "strings output:\n");
	for(j = 0; j < 4; j++)
	seq_printf(m, "%s = %ld times\n", randomPhrases[j], booga_device_stats->numOfStringOutput[j]);





	return 0;
}

static int booga_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, booga_proc_show, NULL);
}
static int __init booga_init(void)
{
	int result;

	/*
		 * Register your major, and accept a dynamic number
		 */
	result = register_chrdev(booga_major, "booga", &booga_fops);
	if (result < 0)
	{
		printk(KERN_WARNING "booga: can't get major %d\n", booga_major);
		return result;
	}
	if (booga_major == 0)
		booga_major = result; /* dynamic */
	printk("<1> booga device driver version 4: loaded at major number %d\n", booga_major);

	booga_device_stats = (booga_stats *)kmalloc(sizeof(booga_stats), GFP_KERNEL);
	if (!booga_device_stats)
	{
		result = -ENOMEM;
		goto fail_malloc;
	}
	init_booga_device_stats();

	/* We assume that the /proc/driver exists. Otherwise we need to use proc_mkdir to
		 * create it as follows: proc_mkdir("driver", NULL);
		 */
	booga_proc_file = proc_create("driver/booga", 0, NULL, &booga_proc_fops);
	if (!booga_proc_file)
	{
		result = -ENOMEM;
		goto fail_malloc;
	}

	return 0;
fail_malloc:
	unregister_chrdev(booga_major, "booga");
	return result;
}

static __exit void booga_cleanup(void)
{
	remove_proc_entry("driver/booga", NULL /* parent dir */);
	kfree(booga_device_stats);
	unregister_chrdev(booga_major, "booga");
	printk("<1> booga device driver version 4: unloaded\n");
}

module_init(booga_init);
module_exit(booga_cleanup);

/* vim: set ts=4: */