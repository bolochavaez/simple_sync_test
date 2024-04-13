#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>

#ifndef IO_FILE

typedef struct io_file {
    int fd;
    unsigned long long int current_position;
    unsigned long long int writes;
    unsigned long long int reads;
    int direct;
    unsigned long long int * lba_history; 
    unsigned long long int * bs_history; 
    unsigned long long int history_size;
    off_t size;
} IOFile;

int io_file_reset_pos(IOFile *);
int io_file_get_size(IOFile *);
int delete_io_file(IOFile *);
int io_file_print_stats(IOFile *, long int);
int read_file(IOFile *, off_t, off_t );
int write_file(IOFile *, off_t, off_t );
IOFile * io_file(const char *, int, off_t);


#define IO_FILE
#endif
