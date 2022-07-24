#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static void syscall_handler(struct intr_frame *);

void syscall_init(void)
{
	intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
	// Modification begins
	lock_init(&file_lock);
	// Modification ends
}

// Modification begins
static void syscall_handler(struct intr_frame *f)
{
	int *syscall_number = f -> esp;
	int *argument_header = f -> esp;
	int return_value = 0;

	if (is_user_vaddr (syscall_number)) {
		if (*syscall_number == SYS_EXIT) {
			if (is_user_vaddr (argument_header + 1))
				exit_syscall (*(argument_header + 1));
			else
				exit_syscall (-1);
		}
		else if (*syscall_number == SYS_EXEC) {
			if (is_user_vaddr (argument_header + 1))
				return_value = exec_syscall ((const char *) *(argument_header + 1));
			else
				exit_syscall (-1);
		}
		else if (*syscall_number == SYS_WAIT) {
			if (is_user_vaddr (argument_header + 1))
				return_value = wait_syscall ((pid_t) * (argument_header + 1));
			else
				exit_syscall (-1);
		}
		else if (*syscall_number == SYS_CREATE) {
			if (is_user_vaddr (argument_header + 1) && is_user_vaddr (argument_header + 2))
				return_value = create_syscall ((const char*) *(argument_header + 1), (unsigned) * (argument_header + 2));
			else
				exit_syscall (-1);
		}
		else if (*syscall_number == SYS_REMOVE) {
			if (is_user_vaddr (argument_header + 1))
				return_value = remove_syscall ((const char*) *(argument_header + 1));
			else
				exit_syscall (-1);
		}
		else if (*syscall_number == SYS_OPEN) {
			if (is_user_vaddr (argument_header + 1))
				return_value = open_syscall ((const char*) *(argument_header + 1));
			else
				exit_syscall (-1);
		}
		else if (*syscall_number == SYS_FILESIZE) {
			if (is_user_vaddr (argument_header + 1))
				return_value = filesize_syscall (*(argument_header + 1));
			else
				exit_syscall (-1);
		}
		else if (*syscall_number == SYS_READ) {
			if (is_user_vaddr (argument_header + 1) && is_user_vaddr (argument_header + 2) && is_user_vaddr (argument_header + 3))
				return_value = read_syscall (*(argument_header + 1), (void*) *(argument_header + 2), (unsigned) *(argument_header + 3));
			else
				exit_syscall (-1);
		}
		else if (*syscall_number == SYS_WRITE) {
			if (is_user_vaddr (argument_header + 1) && is_user_vaddr (argument_header + 2) && is_user_vaddr (argument_header + 3))
				return_value = write_syscall(*(argument_header + 1), (const void *) *(argument_header + 2), (unsigned) *(argument_header + 3));
			else
				exit_syscall(-1);
		}
		else if (*syscall_number == SYS_CLOSE) {
			if (is_user_vaddr (argument_header + 1))
				close_syscall (*(argument_header + 1));
			else
				exit_syscall (-1);
		}
		else
			exit_syscall (-1);
	}
	else
		exit_syscall (-1);

	f -> eax = return_value;
}

static int fd_to_use = 2;

struct file_struct* find_file_by_fd (int fid)
{
	struct thread* current_thread = thread_current();
	struct list_elem* element;
	for (element = list_begin(&current_thread -> files); element != list_end(&current_thread->files); element = list_next(element)) {
		struct file_struct* this_file = list_entry(element, struct file_struct, thread_file_elem);
		if (this_file -> fid == fid)
			return this_file;
	}

	return NULL;
}

void exit_syscall (int status)
{
	struct thread *current_thread = thread_current();

	if (lock_held_by_current_thread(&file_lock))
		lock_release(&file_lock);

	while (list_empty(&current_thread -> files) == false) {
		struct list_elem* element = list_begin(&current_thread -> files);
		close_syscall  (list_entry (element, struct file_struct, thread_file_elem) -> fid);
	}

	current_thread -> return_status = status;
	printf("%s: exit(%d)\n", current_thread -> name, current_thread -> return_status);
	thread_exit();
}

pid_t exec_syscall (const char *command)
{
	tid_t tid;
	lock_acquire (&file_lock);
	tid = process_execute (command);
	lock_release (&file_lock);
	return tid;
}

int wait_syscall (pid_t pid)
{
	return process_wait (pid);
}

bool create_syscall (const char *file, unsigned SIZE_INITIAL)
{
	if (file != NULL) {
		lock_acquire (&file_lock);
		bool flag_for_success = filesys_create (file, SIZE_INITIAL);
		lock_release (&file_lock);
		return flag_for_success;
	} else {
		exit_syscall (-1);
		return false;
	}
}

bool remove_syscall (const char *file)
{
	if (file != NULL) {
		lock_acquire (&file_lock);
		bool flag_for_success = filesys_remove (file);
		lock_release (&file_lock);
		return flag_for_success;
	} else {
		exit_syscall(-1);
		return false;
	}
}

int open_syscall (const char *file)
{
	if (file == NULL) {
		exit_syscall (-1);
		return -1;
	}

	ASSERT (file != NULL);

	lock_acquire (&file_lock);
	struct file* file_to_open_file = filesys_open(file);
	lock_release (&file_lock);
	if (file_to_open_file == NULL) {
		return -1;
	}

	struct file_struct* file_to_open_file_struct = (struct file_struct*) malloc (sizeof(struct file_struct));
	if (file_to_open_file_struct == NULL) {
		file_close (file_to_open_file);
		return -1;
	}

	lock_acquire (&file_lock);

	file_to_open_file_struct -> f = file_to_open_file;

	file_to_open_file_struct -> fid = fd_to_use ++;
	struct thread* current_thread = thread_current();
	list_push_back (&current_thread -> files, &file_to_open_file_struct -> thread_file_elem);

	lock_release (&file_lock);

	return file_to_open_file_struct -> fid;
}

int filesize_syscall (int fd)
{
	lock_acquire (&file_lock);
	struct file_struct* file_to_check = find_file_by_fd (fd);
	lock_release (&file_lock);

	if (file_to_check == NULL)
		return -1;

	lock_acquire (&file_lock);
	int N = file_length (file_to_check -> f);
	lock_release (&file_lock);

	return N;
}

int read_syscall (int fd, void *buffer, unsigned N)
{
	if (is_user_vaddr (buffer) == false || is_user_vaddr (buffer + N) == false)
		exit_syscall(-1);

	if (fd == STDIN_FILENO) {
		int i;
		for (i = 0; i < N; i++) {
			*(uint8_t *) (buffer + i) = input_getc();
		}
		return N;
	} else if (fd == STDOUT_FILENO) {
		return -1;
	} else {
		lock_acquire (&file_lock);
		struct file_struct* file_to_read = find_file_by_fd (fd);
		lock_release (&file_lock);

		if (file_to_read == NULL)
			return -1;

		lock_acquire (&file_lock);
		int return_value = file_read (file_to_read -> f, buffer, N);
		lock_release (&file_lock);

		return return_value;
	}
}

int write_syscall (int fd, const void *buffer, unsigned N)
{	
	if (is_user_vaddr(buffer) == false || is_user_vaddr(buffer + N) == false) {
		exit_syscall(-1);
	}

	if (fd == STDIN_FILENO) {
		return -1;
	} else if (fd == STDOUT_FILENO) {
		putbuf (buffer, N);
		return N;
	} else {
		lock_acquire (&file_lock);
		struct file_struct* file_to_write = find_file_by_fd (fd);
		lock_release (&file_lock);

		if (file_to_write == NULL) {
			return -1;
		}

		lock_acquire (&file_lock);
		int return_value = file_write (file_to_write -> f, buffer, N);
		lock_release (&file_lock);
		return return_value;
	}
}

void close_syscall (int fd)
{
	lock_acquire(&file_lock);
	struct file_struct* file_to_close = find_file_by_fd (fd);
	lock_release(&file_lock);

	if (file_to_close == NULL)
		exit_syscall(-1);

	lock_acquire(&file_lock);
	list_remove(&file_to_close -> thread_file_elem);

	file_close (file_to_close -> f);

	free(file_to_close);
	lock_release(&file_lock);
}
// Modification ends