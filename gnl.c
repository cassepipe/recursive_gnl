#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

#ifndef BUFLEN
#	define BUFLEN  1
#endif

char	*gnl_strchrnul(char *str, char c)
{
		while (*str)
		{
			if (*str == c)
				return (str);
			str++;
		}
		return (str);
}

void	gnl_memcpy(char *dest, const char *src, size_t n)
{
	if (dest && src)
	{
		while (n--)
		{
			*dest = *src;
			dest++;
			src++;
		}
	}
}

char	*gnl_memjoin(char *str1, size_t len1, char *str2, size_t len2)
{
	size_t size;
	char *ret;

	size = len1 + len2 + 1;
	ret = malloc(size);
	if (!ret)
		return (NULL);
	ret[size - 1] = '\0';
	gnl_memcpy(ret, str1, len1);
	gnl_memcpy(ret + len1, str2, len2);
	return (ret);
}

int	rec_next_line(int fd, char **line, char *buffer, char **bufstart, size_t line_len)
{
	char  *newline_or_null_byte;
	char *old_line;
	int	r;

	newline_or_null_byte = gnl_strchrnul(*bufstart, '\n');
	old_line  =  *line;
	*line = gnl_memjoin(*line, line_len, *bufstart, newline_or_null_byte - *bufstart);
	free(old_line);
	if (*newline_or_null_byte == '\n')
	{
		*bufstart = newline_or_null_byte + 1;
		return (1);
	}
	line_len += newline_or_null_byte - *bufstart;
	*bufstart = buffer;
	r = read(fd, buffer, BUFLEN);
	if (r < BUFLEN)
	{
		bzero(&buffer[r], BUFLEN - r);
	}
	if (r == 0 && line_len == 0)
		return (0);
	if (r == 0)
		return (1);
	return(rec_next_line(fd, line, buffer, bufstart, line_len));

}

int	get_next_line(int fd, char **line)
{
		static char buffer[BUFLEN + 1];
		static char *bufstart;
		size_t line_len;

		*line = NULL;
		line_len = 0;
		if (!bufstart)
			bufstart = &buffer[BUFLEN];
		return (rec_next_line(fd, line, buffer, &bufstart, line_len));
}

int main(int ac, char **av)
{
		if (ac != 2)
		{
			puts("Wrong number of  args !");
			return (1);
		}

		int fd;

		fd = open(av[1], O_RDONLY);
		if (fd == -1)
		{
			puts("Pb with file");
			return 1;
		}

		char *line;

		while (get_next_line(fd, &line) == 1)
		{
				puts(line);
				free(line);
		}
		free(line);
}
