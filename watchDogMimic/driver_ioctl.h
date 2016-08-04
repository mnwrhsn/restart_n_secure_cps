#include <linux/ioctl.h>
#define QUERY_GET_VALUE _IOR('q', 1, int *)
#define QUERY_CLEAR_VALUE _IO('q', 2)
#define QUERY_SET_VALUE _IOW('q', 3, int *)
 
