#ifndef __BOOGA_H
#define __BOOGA_H

#ifndef BOOGA_MAJOR
#define BOOGA_MAJOR 0
#endif

#ifndef BOOGA_NR_DEVS
#define BOOGA_NR_DEVS 4
#endif


#define TYPE(dev) (MINOR(dev) >>4)
#define NUM(dev) (MINOR(dev) & 0xf)

struct booga_stats
{
    long int num_open;
    long int num_read;
    long int num_write;
    long int num_close;
    struct semaphore sem;
    long int numOfOpens[4];
    long int numOfStringOutput[4];
    
};
typedef struct booga_stats booga_stats;

#endif
