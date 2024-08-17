/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ls.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/17 16:45:41 by tratanat          #+#    #+#             */
/*   Updated: 2024/08/17 18:27:42 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __FT_LS__
#define __FT_LS__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

const int FLAG_LIST = 1 << 0;
const int FLAG_RECURSIVE = 1 << 1;
const int FLAG_ALL = 1 << 2;
const int FLAG_REVERSE = 1 << 3;
const int FLAG_TIME = 1 << 4;
const int FLAG_TITLE = 1 << 5;

typedef struct s_directory
{
    char *path;
    struct s_directory *next;
} t_directory;

typedef struct
{
    int flag;
    t_directory *directories;
    t_directory *last;
} ls_config;

#endif