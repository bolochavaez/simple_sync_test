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
} IOFile;

int io_file_reset_pos(IOFile *);
int io_file_get_size(IOFile *);
int delete_io_file(IOFile *);
int io_file_print_stats(IOFile *, long int);
int read_file(IOFile *, int);
int write_file(IOFile *, int);
IOFile * io_file(const char *);
int write_worker(IOFile *, long int);
int read_worker(IOFile *, long int);
