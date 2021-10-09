#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFSIZE 10

static const char wrong_args[] = "Wrong number of args\n";
static const char file_error[] = "Problem with file\n";

static void	gnl_memcpy(char *dest, const char *src, size_t n)
{
		while (n--)
		{
			*dest = *src;
			src++;
			dest++;
		}
}

static char *gnl_memjoin(char *str1, const char *str2, size_t len1, size_t len2)
{
	size_t	size;
	char	*ret;

	size = len1 + len2 + 1;
	ret = malloc(size);
	if (!ret)
		return (NULL);
	ret[size - 1] = '\0';
	gnl_memcpy(ret, str1, len1);
	gnl_memcpy(ret + len1, str2, len2);
	return (ret);
}

static int get_that_line(int fd, char **line, char *buffer, char **bufstart, size_t line_len)
{
	char	*newline;
	char	*old_line;
	ssize_t	r;

	newline = strchr(*bufstart, '\n');
	old_line = *line;
	if (newline)
	{
		/*Base case*/
		*line = gnl_memjoin(*line, *bufstart, line_len, newline - *bufstart);
		free (old_line);
		*bufstart = newline + 1;
		return (1);
	}
	else
	{
		/*Recursive branch*/
		*line = gnl_memjoin(*line, *bufstart, line_len, &buffer[BUFSIZE] - *bufstart);
		free (old_line);
		line_len += &buffer[BUFSIZE] - *bufstart;
		*bufstart = buffer;
		r = read(fd, buffer, BUFSIZE);
		if (r < 1)
			return (r);
		if (r < BUFSIZE)
			buffer[r] = '\n';
		if (r < BUFSIZE)
			bzero(&buffer[r], BUFSIZE - r);
		return (get_that_line(fd, line, buffer, bufstart, line_len));
	}
}

int	get_next_line(int fd, char **line)
{
	static char buffer[BUFSIZE + 1];
	static char *bufstart;
	static size_t line_len;

	*line = NULL;
	line_len = 0;
	if (*buffer == '\0')
	{
		bufstart = &buffer[BUFSIZE];
	}
	return(get_that_line(fd, line, buffer, &bufstart, line_len));
}

int main(int ac, char **av)
{
	if (ac != 2)
	{
		write(1, wrong_args, sizeof wrong_args);
		return (1);
	}

	int fd;

	fd = open(av[1], O_RDONLY);
	if (fd == -1)
	{
		write(1, file_error, sizeof(file_error));
		return (1);
	}

	char *line;
	while (get_next_line(fd, &line))
	{
		puts(line);
		free(line);
	}

}
