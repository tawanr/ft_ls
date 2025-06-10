/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 01:17:24 by tratanat          #+#    #+#             */
/*   Updated: 2024/08/23 20:53:37 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
#define LIBFT_H

#include <stdlib.h>
#include <unistd.h>

size_t ft_strlen(const char *s);
int ft_atoi(const char *str);
char *ft_strdup(const char *str);
char *ft_itoa(long int n);
char *ft_strmapi(char const *s, char (*f)(unsigned int, char));
char *ft_strjoin(char const *s1, char const *s2);
int ft_putstr_fd(char *s, int fd);
int ft_strcmp(const char *s1, const char *s2);
int ft_strncmp(const char *s1, const char *s2, size_t n);
int ft_strlcpy(char *dst, const char *src, size_t dstsize);
int ft_strlcat(char *dst, const char *src, size_t dstsize);

#endif
