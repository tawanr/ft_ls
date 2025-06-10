/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ls.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/17 16:45:41 by tratanat          #+#    #+#             */
/*   Updated: 2024/08/23 21:28:15 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __FT_LS_H__
#define __FT_LS_H__

#include <dirent.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define FLAG_LIST (1 << 0)
#define FLAG_RECURSIVE (1 << 1)
#define FLAG_ALL (1 << 2)
#define FLAG_REVERSE (1 << 3)
#define FLAG_TIME (1 << 4)
#define FLAG_TITLE (1 << 5)
#define FLAG_UNSORTED (1 << 6)
#define FLAG_ACCESS_TIME (1 << 7)
#define FLAG_DIRECTORY (1 << 8)

typedef struct t_file {
    struct stat *filestat;
    struct t_file *next;
    char *name;
    int name_length;
    char *size;
    char *links;
    char *owner;
    char *group;
    char *time;
    char *access_time;
} ls_file;

typedef struct s_directory {
    char *path;
    struct s_directory *next;
    struct t_file **files;
    int size_col_max;
    int links_col_max;
    int owner_col_max;
    int group_col_max;
    int total_files;
    long int block_total;
    int ignore_dir;
} t_directory;

typedef struct {
    int flag;
    char *appname;
    int input_args;
    int exit_code;
    t_directory *directories;
    t_directory *last;
} ls_config;

typedef struct {
    int col_count;
    int row_count;
    int total_width;
    int *cols;
    char **names;
    int is_valid;
    int file_count;
} column_info;

void merge_sort(ls_config *config, ls_file **file_list);
void reverse_list(ls_file **file_list);
void print_tabular(t_directory *directory);
void print_files(ls_config *config, t_directory *dir);
int parse_args(ls_config *config, int argc, char **argv);
void validate_path(ls_config *config);
int add_path(ls_config *config, char *path);
void parse_dir_list(ls_config *config);
char *get_full_path(char *base, char *filename);
int check_relative_filename(char *path);
int check_folder(struct stat *filestat);
int check_all_files(ls_config *config, char *name);
void free_files(t_directory *dir);
t_directory *new_directory(char *path);
int check_path_folder(const char *path);
int add_arg_file(ls_config *config, char *path);
ls_file *get_new_file(ls_config *config, struct dirent *fileinfo,
                      t_directory *dir);
void check_columns(ls_config *config, t_directory *dir, ls_file *file);
char *check_current_year(const char *time);

#endif