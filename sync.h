#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>

typedef struct io_file {
    int fd;
    int current_position;
    unsigned long long int writes;
    unsigned long long int reads;
    int direct;
} IOFile;

typedef struct workload {
    off_t bs;
    int time_based;
    long int time;
    off_t size;
    off_t offset;
    int rw;
    int sequential;
} Workload;

typedef struct io_engine {
    int (*read_file) (IOFile *, off_t, off_t); 
    int (*write_file) (IOFile *, off_t, off_t); 
} IOEngine;

typedef struct worker {
    IOFile * io_file;
    Workload * workload;
    IOEngine * io_engine;
} Worker;

int io_file_reset_pos(IOFile *);
int io_file_get_size(IOFile *);
int delete_io_file(IOFile *);
int io_file_print_stats(IOFile *, long int);
int read_file(IOFile *, off_t, off_t );
int write_file(IOFile *, off_t, off_t );
IOFile * io_file(const char *, int);
int write_worker(IOFile *, long int);
int read_worker(IOFile *, long int);
