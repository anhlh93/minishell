#include "minishell.h"
#include "libft.h"

/**
 * @brief Execute command from current directory or with absolute/relative path
 * @param cmd Command structure containing command name and arguments
 * @param shell Shell state containing environment and configuration
 * @return true if command was executed successfully, false if not applicable
 *
 * This function handles:
 * - Empty command validation (exits with 127)
 * - Commands with '/' (absolute/relative paths)
 * - File accessibility checks before execution
 * - Direct execve() calls for path-based commands
 */
static bool	execute_currdir(t_cmds *cmd, t_shell *shell)
{
	// Handle empty command case
	if (cmd->str[0][0] == '\0')
	{
		ft_putstr_fd("minishell: : command not found\n", STDERR);
		exit(127);
	}

	// Only handle commands with path separators
	if (ft_strchr(cmd->str[0], '/') == NULL)
		return (false);

	// Check if file exists and is accessible
	if (access(cmd->str[0], F_OK) == 0)
	{
		// Execute the command directly with full path
		if (execve(cmd->str[0], cmd->str, shell->env) == -1)
		{
			ft_putstr_fd("minishell: ", STDERR);
			perror(cmd->str[0]);
			exit(126);
		}
	}
	return (false);
}

/**
 * @brief Execute built-in shell commands
 * @param cmd Command structure containing command name and arguments
 * @param shell Shell state for environment and configuration access
 * @return 1 if command was a built-in and executed, 0 if not a built-in
 *
 * Supported built-ins:
 * - pwd: Print working directory
 * - echo: Display text with optional -n flag
 * - cd: Change directory
 * - export: Set environment variables
 * - unset: Remove environment variables
 * - env: Display environment variables
 * - exit: Exit the shell
 *
 * Uses global g_return_value to store the exit status of executed built-ins
 */
int	execute_builtin(t_cmds *cmd, t_shell *shell)
{
	if (ft_strncmp(cmd->str[0], "pwd", 4) == 0)
		g_return_value = ft_pwd();
	else if (ft_strncmp(cmd->str[0], "echo", 5) == 0)
		g_return_value = ft_echo(cmd);
	else if (ft_strncmp(cmd->str[0], "cd", 3) == 0)
		g_return_value = ft_cd(cmd, shell);
	else if (ft_strncmp(cmd->str[0], "export", 7) == 0)
		g_return_value = ft_export(cmd, shell, 0);
	else if (ft_strncmp(cmd->str[0], "unset", 6) == 0)
		g_return_value = ft_unset(cmd, shell);
	else if (ft_strncmp(cmd->str[0], "env", 4) == 0)
		g_return_value = ft_env(shell->env);
	else if (ft_strncmp(cmd->str[0], "exit", 5) == 0)
		g_return_value = ft_exit(cmd);
	else
		return (0); // Not a built-in command
	return (1); // Successfully executed built-in
}

/**
 * @brief Execute a single command with PATH resolution
 * @param cmd Command structure containing command name and arguments
 * @param shell Shell state containing environment variables and PATH
 *
 * Execution priority:
 * 1. Check if command is empty (exit with status 0)
 * 2. Try built-in commands first
 * 3. Try current directory/absolute path execution
 * 4. Search PATH environment variable for external commands
 * 5. Fallback to execution without PATH if PATH not found
 *
 * This function always exits the process after execution attempt
 */
static void	execute_cmd(t_cmds *cmd, t_shell *shell)
{
	int		i;

	i = 0;
	// Handle empty command
	if (!cmd->str[0])
		exit(0);

	// Try built-in first, then current directory execution
	if (!execute_builtin(cmd, shell) && !execute_currdir(cmd, shell))
	{
		// Search for PATH environment variable
		while (shell->env && shell->env[i])
		{
			if (ft_strncmp(shell->env[i], "PATH=", 5) == 0)
				break ;
			i++;
		}

		// Execute with PATH if found, otherwise try without PATH
		if (shell->env[i])
			ft_execve(cmd, shell, ft_split(&shell->env[i][5], ':'));
		ft_execve(cmd, shell, NULL);
	}
	exit(g_return_value);
}

/**
 * @brief Handle pipe setup and process creation for pipeline commands
 * @param cmd Current command in the pipeline
 * @param prev_fd File descriptor from previous command's output (or -1 for first)
 * @param shell Shell state for environment and configuration
 *
 * Pipeline management:
 * - Sets up signal handling for pipeline execution
 * - Creates pipe if current command has a next command
 * - Forks child process for command execution
 * - In child: sets up input/output redirection via pipes
 * - Handles redirections before command execution
 * - Parent returns immediately to continue pipeline setup
 */
static void	handle_pipes(t_cmds *cmd, int prev_fd, t_shell *shell)
{}

/**
 * @brief Main execution entry point - connects entire command pipeline
 * @param shell Shell state containing parsed commands and environment
 *
 * Execution flow:
 * 1. Process all heredocs first (<<)
 * 2. Check for single command optimization
 * 3. Create pipeline by iterating through linked command list
 * 4. For each command: setup pipes, fork process, manage file descriptors
 * 5. Wait for all child processes to complete
 *
 * Pipeline coordination:
 * - Maintains prev_fd to chain commands together
 * - Properly closes file descriptors to avoid leaks
 * - Handles shell->stop flag for early termination
 */
void	execute(t_shell *shell)
{
	t_cmds	*curr;
	int		prev_fd;

	// Process heredocs before any command execution
	handle_heredocs(shell);

	prev_fd = -1;
	curr = shell->cmds; // Start with first command

	// Optimization: handle single command without unnecessary forking
	if (curr->next == NULL && single_cmd(shell))
		return ;

	// Execute pipeline: iterate through all commands
	while (curr && !shell->stop)
	{
		handle_pipes(curr, prev_fd, shell);

		// Clean up file descriptors
		close(prev_fd); // Close previous read end
		prev_fd = curr->pipefd[0]; // Save current read end for next command
		close(curr->pipefd[1]); // Close current write end

		curr = curr->next; // Move to next command
	}
	ft_waitpid(shell); 	// Wait for all child processes to complete
}
