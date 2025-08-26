#include "minishell.h"
#include "libft.h"

/**
 * @brief Get current working directory using system call wrapper
 * @details Allocates buffer and retrieves current working directory path.
 *          Uses PATH_MAX to ensure sufficient buffer size for any valid path.
 *          Handles memory cleanup on failure to prevent leaks.
 * @return Allocated string containing current directory path, or NULL on failure
 * @note Caller is responsible for freeing the returned string
 * @note Uses ft_malloc for consistent memory allocation tracking
 */
char	*ft_getcwd(void)
{
	char	*buf;

	buf = ft_malloc(PATH_MAX * sizeof(char));
	if (getcwd(buf, PATH_MAX))
		return (buf);
	free(buf);
	return (NULL);
}

/**
 * @brief Implementation of pwd built-in command
 * @details Prints current working directory to stdout followed by newline.
 *          Mimics bash pwd behavior with no options support as per requirements.
 *          Handles error cases by returning appropriate exit status.
 * @return 0 on success, 1 on failure (bash-compatible exit codes)
 * @note Part of mandatory built-in commands as specified in PRD
 * @note No options parsing required - pwd accepts no flags per specification
 */
int	ft_pwd(void)
{
	char	*pwd;

	pwd = ft_getcwd();
	if (pwd)
	{
		printf("%s\n", pwd);
		free(pwd);
		return (0);
	}
	return (1);
}
