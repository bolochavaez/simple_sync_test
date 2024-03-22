#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/uio.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "sync.h"
#define __USE_GNU 1
#include <fcntl.h>

int io_file_reset_pos(IOFile * io_file){
    lseek(io_file->fd, 0, SEEK_SET);
    io_file->current_position = 0;
    return 0;
}

int io_file_get_size(IOFile * io_file){
    struct stat stats;
    fstat(io_file->fd, &stats);
    return (int)stats.st_size;
}

int delete_io_file(IOFile * io_file){
    close(io_file->fd);
    free(io_file);
    return 1;
}

int io_file_print_stats(IOFile * io_file, long int time_seconds){
    printf("writes: %llu \n", io_file->writes);
    printf("MB per second: %d \n", io_file->writes/ 1024 / 1024  / time_seconds);
    printf("reads: %llu \n", io_file->reads);
    printf("MB per second: %d \n", io_file->reads/ 1024 / 1024  / time_seconds);
}

int read_file(IOFile * io_file, off_t block_size, off_t offset){
    if (offset != 0) lseek(io_file->fd, offset, SEEK_SET);
    int * buffer = (int *) malloc(sizeof(int) * block_size);
    int bytes_read = read(io_file->fd, buffer, block_size);
    if(bytes_read == -1){
        perror("read error: ");
    }
    io_file->current_position += bytes_read;
    io_file->reads += bytes_read;
    free(buffer);
    return bytes_read;
}

int write_file(IOFile * io_file, off_t block_size, off_t offset){
    if (offset != 0) lseek(io_file->fd, offset, SEEK_SET);
    int * buffer = (int *) malloc(sizeof(int) * block_size);
    for( int i = 0; i < block_size; i++){
        buffer[i] = rand();
    }
    int bytes_written = write(io_file->fd, buffer, block_size);
    if(bytes_written == -1){
        perror("write error: ");
    }

    io_file->current_position += bytes_written;
    io_file->writes += bytes_written;
    free(buffer);
    return bytes_written;
}

IOFile * io_file(const char * file_name, int direct){
    int open_flags = O_RDWR | O_CREAT;
    if(direct) open_flags = open_flags | O_DIRECT;
    int file_desc = open(file_name, open_flags, S_IRWXU | S_IRWXG | S_IRWXO);
    if (file_desc == -1){
        perror("could not open file");
    }
    IOFile * io_file_object = (IOFile*) malloc(sizeof(IOFile));
    * io_file_object = (IOFile){file_desc, 0, 0, 0, direct};
    return io_file_object;
}

int write_worker(IOFile * io_file, long int time_seconds){
    time_t c_time = time(NULL);
    long int int_time_start = (long int) c_time;
    long int int_current_time = int_time_start;
    printf("starting time: %ld\n", c_time);
    printf("timestamp: %ld\n", int_current_time);
    printf("diff: %ld\n", int_current_time - int_time_start);
    while((int_current_time - int_time_start) < time_seconds){
        if  (io_file->current_position >=  io_file_get_size(io_file)){
            io_file_reset_pos(io_file);
        }
        write_file(io_file, 1024, 0);
        int_current_time = (long int) time(NULL);
    }
    return 1;
}

int read_worker(IOFile * io_file, long int time_seconds){
    time_t c_time = time(NULL);
    long int int_time_start = (long int) c_time;
    long int int_current_time = int_time_start;
    printf("starting time: %ld\n", c_time);
    printf("timestamp: %ld\n", int_current_time);
    printf("diff: %ld\n", int_current_time - int_time_start);
    while((int_current_time - int_time_start) < time_seconds){
        if  (io_file->current_position >=  io_file_get_size(io_file)){
            io_file_reset_pos(io_file);
        }
        read_file(io_file, 1024, 0);
        int_current_time = (long int) time(NULL);
    }
    return 1;
}


//typedef struct workload {
//    int bs;
//    int time_based;
//    int time;
//    int size;
//    int rw;
//} Workload;
//

//typedef struct worker {
//    IOFile * io_file;
//    Workload * workload;
//} worker;


Workload* workload(off_t bs, int time_based, long int time, off_t size, int rw, off_t offset, int sequential){
    Workload * workload = (Workload*) malloc(sizeof(Workload));
    workload->bs = bs;
    workload->time_based = time_based;
    workload->time = time;
    workload->size = size;
    workload->rw = rw;
    workload->offset = offset;
    workload->sequential = sequential;
    return workload;
}


Worker* worker(IOFile * io_file, Workload * workload, IOEngine * io_engine){
    Worker * worker = (Worker*) malloc(sizeof(Worker));
    worker->io_file = io_file;
    worker->workload = workload;
    worker->io_engine = io_engine;
    return worker;
}

int worker_run(Worker * worker){
    time_t c_time = time(NULL);
    long int int_time_start = (long int) c_time;
    long int int_current_time = int_time_start;
    printf("starting time: %ld\n", c_time);
    printf("timestamp: %ld\n", int_current_time);
    printf("diff: %ld\n", int_current_time - int_time_start);
    while((int_current_time - int_time_start) < worker->workload->time){
        if  (worker->io_file->current_position + worker->workload->bs >=  worker->workload->size){
            io_file_reset_pos(worker->io_file);
        }
	if(worker->workload->rw == 0) {
            worker->io_engine->read_file(worker->io_file, worker->workload->bs, worker->workload->offset);
	} else if (worker->workload->rw == 1){
            worker->io_engine->write_file(worker->io_file, worker->workload->bs, worker->workload->offset);
	} else {
           printf("invalid operation type: %d\n", worker->workload->rw);

	}
        int_current_time = (long int) time(NULL);
    }
    return 1;

}

//define the io engines here
static IOEngine sync_engine = {
        .write_file = write_file,
	.read_file = read_file
};


int main(int argc, char * argv[]){
    //who cares
    srand(1);
    int opt;
    long int runtime = 3;
    char filename[128];
    strcpy(filename, "filename");
    while(( opt = getopt(argc, argv, "t:f:")) != -1 ){
        switch(opt) {
            case 't':
                runtime = (long int) atoi(optarg);
		break;
            case 'f':
                strcpy(filename, optarg);
		break;

	default:
		printf("Invalid parameter");
		exit(EXIT_FAILURE);
	}

    }

    IOFile * test_file =  io_file(filename, 1);
    Workload * test_workload = workload(1024, 1, runtime, 1000000, 1, 0, 1);
    Worker * test_worker = worker(test_file, test_workload, &sync_engine);
    worker_run(test_worker);
    io_file_print_stats(test_file, runtime);
    delete_io_file(test_file);
}
