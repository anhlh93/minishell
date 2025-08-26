#include "minishell.h"
#include "libft.h"

/**
 * @brief Execute external command by searching through PATH directories
 * @param cmd Command structure containing command name and arguments
 * @param shell Shell state for environment access
 * @param path Array of PATH directories to search (can be NULL)
 *
 * Execution process:
 * 1. If path is provided, search each directory for the command
 * 2. Construct full path by joining directory + "/" + command_name
 * 3. Check if constructed path exists and is accessible
 * 4. Attempt execution with execve()
 * 5. If command not found in any PATH directory, exit with 127
 *
 * Memory management:
 * - Frees path array before exit
 * - Frees temporary path strings during search
 *
 * Exit codes:
 * - 127: Command not found
 * - -1: execve() failed but file was found (shouldn't happen normally)
 *
 * This function always exits the process - never returns to caller.
 */

void	ft_execve(t_cmds *cmd, t_shell *shell, char **path)
{
	int		i;
	char	*tmp;

	i = 0;
	while (path && path[i])
	{
		tmp = ft_strjoin(path[i], "/");
		tmp = ft_strjoin_free(tmp, cmd->str[0]);
		if (access(tmp, F_OK) == 0
			&& execve(tmp, cmd->str, shell->env) == -1)
		{
			ft_free_arr(path);
			perror(cmd->str[0]);
			exit(-1);
		}
		free(tmp);
		i++;
	}
	ft_free_arr(path);
	ft_putstr_fd("minishell: ", STDERR);
	ft_putstr_fd(cmd->str[0], STDERR);
	ft_putstr_fd(": command not found\n", STDERR);
	exit(127);
}

/**
 * @brief Wait for all child processes in pipeline and handle their exit status
 * @param shell Shell state containing command list with process IDs
 *
 * Process management:
 * - Iterates through all commands in the pipeline
 * - Waits for each child process to complete
 * - Analyzes exit status to determine how process terminated
 * - Sets global return value based on exit status or signal
 *
 * Exit status handling:
 * - Normal exit: Uses WEXITSTATUS() to get exit code
 * - Signal termination: Maps signals to conventional exit codes
 *   - SIGINT (Ctrl-C): 130
 *   - SIGQUIT (Ctrl-\): 131
 *
 * Signal propagation:
 * - If any process exits with 130 or 131, sets shell->stop = true
 * - This prevents further command execution in interactive mode
 *
 * Uses global g_return_value to store the final exit status
 */

void	ft_waitpid(t_shell *shell)
{
	t_cmds	*curr;
	int		status;
	int		sig;

	curr = shell->cmds;
	while (curr)
	{
		waitpid(curr->pid, &status, 0);
		if (WIFEXITED(status))
			g_return_value = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
		{
			sig = WTERMSIG(status);
			if (sig == SIGINT)
				g_return_value = 130;
			else if (sig == SIGQUIT)
				g_return_value = 131;
		}
		if (g_return_value == 130 || g_return_value == 131)
			shell->stop = true;
		curr = curr->next;
	}
}
/**
 * @brief Handle single built-in command execution without forking
 * @param shell Shell state containing the single command to execute
 * @return true if command was a built-in and executed, false otherwise
 *
 * Optimization for single built-in commands:
 * - Avoids unnecessary forking for built-ins when no pipeline exists
 * - Preserves original stdin/stdout for restoration after redirections
 * - Handles redirections properly for built-in commands
 *
 * Process:
 * 1. Check if the only command is a built-in
 * 2. Save current stdin/stdout file descriptors
 * 3. Apply any redirections (>, <, >>, <<)
 * 4. Execute the built-in command
 * 5. Restore original stdin/stdout
 *
*/

bool	single_cmd(t_shell *shell)
{
	int		save_stdin;
	int		save_stdout;

	if (is_builtin(shell->cmds))
	{
		save_stdin = ft_dup(STDIN);
		save_stdout = ft_dup(STDOUT);
		if (handle_redirections(shell->cmds, shell))
			execute_builtin(shell->cmds, shell);
		ft_dup2(save_stdin, STDIN);
		ft_dup2(save_stdout, STDOUT);
		return (true);
	}
	return (false);
}
