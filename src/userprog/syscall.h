#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "lib/user/syscall.h"

void syscall_init(void);

struct lock file_lock;
struct file_struct* find_file_by_fd (int fid);

void exit_syscall (int status);
pid_t exec_syscall (const char *command);
int wait_syscall (pid_t pid);
bool create_syscall (const char *file, unsigned SIZE_INITIAL);
bool remove_syscall (const char *file);
int open_syscall (const char *file);
int filesize_syscall (int fd);
int read_syscall (int fd, void *buffer, unsigned N);
int write_syscall (int fd, const void *buffer, unsigned N);
void close_syscall (int fd);

#endif /* userprog/syscall.h */
