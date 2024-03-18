#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>

typedef struct io_file {
    int fd;
    int current_position;
    unsigned long long int writes;
    unsigned long long int reads;
} IOFile;


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

int read_file(IOFile * io_file, int block_size){
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

int write_file(IOFile * io_file, int block_size){
    int * buffer = (int *) malloc(sizeof(int) * block_size);
    for( int i = 0; i < block_size; i++){
        buffer[i] = rand();
    }
    int bytes_written = write(io_file->fd, buffer, block_size);
    if(bytes_written == -1){
        perror("read error: ");
    }

    io_file->current_position += bytes_written;
    io_file->writes += bytes_written;
    free(buffer);
    return bytes_written;
}


IOFile * io_file(const char * file_name){
    int file_desc = open(file_name, O_RDWR | O_CREAT);
    if (file_desc == -1){
        perror("could not open file");
    }
    IOFile * io_file_object = (IOFile*) malloc(sizeof(IOFile));
    * io_file_object = (IOFile){file_desc, 0, 0, 0};
    return io_file_object;
}


int delete_io_file(IOFile * io_file){
    close(io_file->fd);
    free(io_file);
    return 1;
}

int write_worker(IOFile * io_file, long int time_seconds){
    time_t c_time = time(NULL);
    long int int_time_start = (long int) c_time;
    long int int_current_time = int_time_start;
    printf("starting time: %ld\n", c_time);
    printf("timestamp: %ld\n", int_current_time);
    printf("diff: %ld\n", int_current_time - int_time_start);
    while((int_current_time - int_time_start) < time_seconds){
        write_file(io_file, 1024);
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
        read_file(io_file, 1024);
        int_current_time = (long int) time(NULL);
    }
    return 1;
}




int main(){
    //who cares
    srand(1);

    IOFile * test_file =  io_file("filename");
    write_worker(test_file, 3);
    printf("writes: %llu \n", test_file->writes);
    printf("MB per second: %d \n", test_file->writes/ 1024 / 1024  / 3);
    io_file_reset_pos(test_file);
    read_worker(test_file, 3);
    printf("reads: %llu \n", test_file->reads);
    printf("MB per second: %d \n", test_file->reads/ 1024 / 1024  / 3);
    delete_io_file(test_file);
}
