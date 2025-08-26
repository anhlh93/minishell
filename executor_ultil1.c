#include "minishell.h"
#include "libft.h"


/**
 * @brief Check if a command is a shell built-in
 * @param cmd Command structure containing the command name to check
 * @return true if the command is a built-in, false otherwise
 *
 * Supported built-in commands:
 * - pwd: Print working directory
 * - echo: Display text with optional flags
 * - cd: Change directory
 * - export: Set environment variables
 * - unset: Remove environment variables
 * - env: Display environment variables
 * - exit: Exit the shell
 *
 * Used for command routing to determine execution method
 */

bool	is_builtin(t_cmds *cmd)
{
	if (ft_strncmp(cmd->str[0], "pwd", 4) == 0)
		return (true);
	else if (ft_strncmp(cmd->str[0], "echo", 5) == 0)
		return (true);
	else if (ft_strncmp(cmd->str[0], "cd", 3) == 0)
		return (true);
	else if (ft_strncmp(cmd->str[0], "export", 7) == 0)
		return (true);
	else if (ft_strncmp(cmd->str[0], "unset", 6) == 0)
		return (true);
	else if (ft_strncmp(cmd->str[0], "env", 4) == 0)
		return (true);
	else if (ft_strncmp(cmd->str[0], "exit", 5) == 0)
		return (true);
	return (false);
}

/**
 * @brief Wrapper for dup() system call with error handling
 * @param fd File descriptor to duplicate
 * @return New file descriptor pointing to the same file
 *
 * Creates a duplicate of the given file descriptor.
 * Used primarily for saving stdin/stdout before redirections
 * so they can be restored later.
 *
 * Exits the program on failure with perror message.
 */

int	ft_dup(int fd)
{
	int	new_fd;

	new_fd = dup(fd);
	if (new_fd == -1)
	{
		perror("dup");
		exit(-1);
	}
	return (new_fd);
}

/**
 * @brief Wrapper for dup2() system call with error handling and cleanup
 * @param fd1 Source file descriptor to duplicate from
 * @param fd2 Target file descriptor to duplicate to
 *
 * Duplicates fd1 to fd2, making fd2 point to the same file as fd1.
 * Automatically closes fd1 after successful duplication to prevent
 * file descriptor leaks.
 *
 * Common usage:
 * - ft_dup2(pipe[1], STDOUT) - redirect stdout to pipe write end
 * - ft_dup2(pipe[0], STDIN) - redirect stdin from pipe read end
 * - ft_dup2(file_fd, STDOUT) - redirect stdout to file
 *
 * Exits the program on failure with perror message.
 */


void	ft_dup2(int fd1, int fd2)
{
	if (dup2(fd1, fd2) == -1)
	{
		perror("dup2");
		exit(-1);
	}
	close(fd1);
}

/**
 * @brief Wrapper for fork() system call with error handling
 * @return Process ID of child process (0 in child, positive in parent)
 *
 * Creates a new child process that is an exact copy of the parent.
 * Used in pipeline execution where each command runs in its own process.
 *
 * Return values:
 * - 0: Code is running in the child process
 * - Positive: Code is running in parent, value is child's PID
 * - Negative: fork() failed (handled by exit)
 *
 * Exits the program on failure with perror message.
 */

pid_t	ft_fork(void)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		exit(-1);
	}
	return (pid);
}

/**
 * @brief Wrapper for pipe() system call with error handling
 * @param fd Array of 2 integers to store pipe file descriptors
 *
 * Creates a pipe for inter-process communication.
 * fd[0] becomes the read end, fd[1] becomes the write end.
 *
 * Usage in pipelines:
 * - Parent creates pipe before forking
 * - Child processes connect stdin/stdout to appropriate pipe ends
 * - Data flows from one command's stdout to next command's stdin
 *
 * Exits the program on failure with perror message.
 */

void	ft_pipe(int fd[2])
{
	if (pipe(fd) == -1)
	{
		perror("pipe");
		exit(-1);
	}
}
