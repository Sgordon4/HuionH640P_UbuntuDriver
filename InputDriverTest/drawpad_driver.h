#ifndef DRAPAD_DRIVER_H
#define DRAPAD_DRIVER_H

static int __init drawpad_init(void);
static void __exit drawpad_exit(void);




/* Called when a process, which already opened the dev file, attempts to
   read from it.
*/
static ssize_t drawpad_read(struct file *filp,
    char *buffer,    /* The buffer to fill with data */
    size_t length,   /* The length of the buffer     */
    loff_t *offset); /* Our offset in the file       */

/*  Called when a process writes to dev file: echo "hi" > /dev/hello */
static ssize_t drawpad_write(struct file *filp,
    const char *buff,
    size_t len,
    loff_t *off);


/* Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"*/
static int drawpad_open(struct inode *inode, struct file *file);

/* Called when a process closes the device file */
static int drawpad_release(struct inode *inode, struct file *file);


#endif
