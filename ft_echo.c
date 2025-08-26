#include "minishell.h"
#include "libft.h"

/**
 * @brief Validates and advances past consecutive -n flags
 * @param i Current argument index to check from
 * @param cmd Command structure containing argument array
 * @return Updated index after all valid -n flags, or original index if invalid flag found
 *
 * Continues parsing consecutive -n flags (like -n, -nn, -nnn) until it finds
 * an invalid flag or reaches the end of -n flags. This handles bash behavior
 * where multiple -n flags can be chained together.
 */
static int	last_check(int i, t_cmds *cmd)
{
	int	j;
	while ((ft_strncmp(cmd->str[i], "-n", 2) == 0))
	{
		j = 1;
		while (cmd->str[i][j] == 'n')
			j++;
		if (cmd->str[i][j] != 0)
			break ;
		else
			i++;
	}
	return (i);
}

/**
 * @brief Processes the first -n flag and determines echo behavior
 * @param cmd Command structure containing argument array
 * @param i Current argument index (should be 1 for first argument)
 * @param check Pointer to character position tracker within argument
 * @param n Pointer to newline flag (1 = print newline, 0 = suppress newline)
 * @return Updated argument index to start printing from
 *
 * Examines the first argument to determine if it's a valid -n flag.
 * If valid, sets the newline suppression flag and advances to check
 * for additional consecutive -n flags via last_check().
 */
static int	check_n(t_cmds *cmd, int i, int *check, int *n)
{
	if ((ft_strncmp(cmd->str[i], "-n", 2) == 0) && i == 1)
	{
		while (cmd->str[i][*check] && cmd->str[i][*check] == 'n')
			*check += 1;
		if (cmd->str[i][*check] == 0)
		{
			*check = 0;
			*n = 0;
			i++;
		}
		else
			i = 1;
		if (i > 0)
			i = last_check(i, cmd);
	}
	return (i);
}

/**
 * @brief Implementation of echo builtin command
 * @param cmd Command structure containing arguments and metadata
 * @return Always returns 0 (echo command always succeeds)
 *
 * Mimics bash echo behavior:
 * - Prints all arguments separated by spaces
 * - Supports -n flag to suppress trailing newline
 * - Handles multiple consecutive -n flags (bash behavior)
 * - Writes output to stdout (file descriptor 1)
 *
 * Examples:
 *   echo hello world        -> "hello world\n"
 *   echo -n hello world     -> "hello world"
 *   echo -nn hello world    -> "hello world"
 *   echo -n -n hello world  -> "hello world"
 */
int	ft_echo(t_cmds *cmd)
{
	int	i;
	int	n;
	int	check;
	i = 1;
	n = 1;
	check = 1;
	i = check_n(cmd, i, &check, &n);
	while (cmd->str[i])
	{
		ft_putstr_fd(cmd->str[i], 1);
		if (cmd->str[i + 1] != 0)
			ft_putchar_fd(' ', 1);
		i++;
	}
	if (n && check)
		ft_putchar_fd('\n', 1);
	return (0);
}
