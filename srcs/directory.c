/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   directory.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/23 20:42:11 by tratanat          #+#    #+#             */
/*   Updated: 2024/08/23 21:36:42 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"
#include "libft.h"

t_directory *new_directory(char *path) {
    // if (*path == '\0')
    //     return NULL;
    t_directory *new = malloc(sizeof(t_directory));
    new->path = ft_strdup(path);
    new->next = NULL;
    new->files = malloc(sizeof(ls_file *));
    *new->files = NULL;
    new->size_col_max = 0;
    new->links_col_max = 0;
    new->owner_col_max = 0;
    new->group_col_max = 0;
    new->block_total = 0;
    new->total_files = 0;
    new->ignore_dir = 0;
    return new;
}

void check_columns(ls_config *config, t_directory *dir, ls_file *file) {
    if (dir == NULL || file == NULL)
        return;
    if (!(config->flag & FLAG_ALL) && file->name[0] == '.')
        return;
    file->size = ft_itoa(file->filestat->st_size);
    int len = ft_strlen(file->size);
    if (len > dir->size_col_max)
        dir->size_col_max = len;

    file->links = ft_itoa(file->filestat->st_nlink);
    len = ft_strlen(file->links);
    if (len > dir->links_col_max)
        dir->links_col_max = len;

    struct passwd *pw = getpwuid(file->filestat->st_uid);
    if (pw == NULL || pw->pw_name == NULL)
        file->owner = ft_itoa(file->filestat->st_uid);
    else
        file->owner = ft_strdup(pw->pw_name);
    len = ft_strlen(file->owner);
    if (len > dir->owner_col_max)
        dir->owner_col_max = len;

    struct group *grp = getgrgid(file->filestat->st_gid);
    if (grp == NULL || grp->gr_name == NULL)
        file->group = ft_itoa(file->filestat->st_gid);
    else
        file->group = ft_strdup(grp->gr_name);
    len = ft_strlen(file->group);
    if (len > dir->group_col_max)
        dir->group_col_max = len;

    char *time;
    time = ctime(&file->filestat->st_mtime);
    file->time = malloc(13);
    ft_strlcpy(file->time, time + 4, 13);

    time = ctime(&file->filestat->st_atime);
    file->access_time = malloc(13);
    ft_strlcpy(file->access_time, time + 4, 13);

    dir->block_total += file->filestat->st_blocks;
}

void add_recursive_path(ls_config *config, t_directory *dir) {
    ls_file *file = *(dir->files);
    t_directory *head = NULL;
    t_directory *last = NULL;
    char *full_path = NULL;

    while (file != NULL) {
        if (check_folder(file->filestat) && (config->flag & FLAG_RECURSIVE) &&
            check_relative_filename(file->name) &&
            check_all_files(config, file->name)) {
            full_path = get_full_path(dir->path, file->name);
            t_directory *new_dir = new_directory(full_path);
            free(full_path);

            if (head == NULL)
                head = new_dir;
            if (last != NULL)
                last->next = new_dir;
            last = new_dir;
        }
        file = file->next;
    }
    if (head == NULL)
        return;
    if (config->directories != NULL) {
        if (last != NULL)
            last->next = config->directories->next;
        config->directories->next = head;
    } else
        config->directories = head;
}

ls_file *get_new_file(ls_config *config, struct dirent *fileinfo,
                      t_directory *dir) {
    ls_file *cur_file = malloc(sizeof(ls_file));
    cur_file->name = ft_strdup(fileinfo->d_name);
    cur_file->name_length = ft_strlen(cur_file->name);
    cur_file->filestat = malloc(sizeof(struct stat));
    char *full_path = get_full_path(dir->path, cur_file->name);
    lstat(full_path, cur_file->filestat);
    free(full_path);
    cur_file->next = NULL;
    dir->total_files++;
    check_columns(config, dir, cur_file);
    return cur_file;
}

void parse_directory(ls_config *config, t_directory *dir) {
    struct dirent *fileinfo;
    void *fd;
    ls_file *cur_file = NULL;
    ls_file *last_file = *dir->files;
    char *block_total = NULL;

    if (dir->ignore_dir == 0 && config->flag & FLAG_TITLE) {
        ft_putstr_fd(dir->path, STDOUT_FILENO);
        ft_putstr_fd(":\r\n", STDOUT_FILENO);
    }
    errno = 0;
    if (!dir->ignore_dir) {
        fd = opendir(dir->path);
        if (fd == NULL) {
            ft_putstr_fd(config->appname, STDERR_FILENO);
            ft_putstr_fd(": cannot open directory '", STDERR_FILENO);
            ft_putstr_fd(dir->path, STDERR_FILENO);
            ft_putstr_fd("'", STDERR_FILENO);
            if (errno == EACCES)
                ft_putstr_fd(": Permission denied\r\n", STDERR_FILENO);
            return;
        }
        fileinfo = readdir(fd);

        while (fileinfo) {
            if (!(config->flag & FLAG_ALL) && fileinfo->d_name[0] == '.') {
                fileinfo = readdir(fd);
                continue;
            }
            // cur_file = malloc(sizeof(ls_file));
            // cur_file->name = ft_strdup(fileinfo->d_name);
            // cur_file->name_length = ft_strlen(cur_file->name);
            // cur_file->filestat = malloc(sizeof(struct stat));
            // full_path = get_full_path(dir->path, cur_file->name);
            // lstat(full_path, cur_file->filestat);
            // free(full_path);
            cur_file = get_new_file(config, fileinfo, dir);
            cur_file->next = NULL;
            if (last_file == NULL)
                *dir->files = cur_file;
            else
                last_file->next = cur_file;
            // dir->total_files++;
            // check_columns(config, dir, cur_file);
            last_file = cur_file;
            fileinfo = readdir(fd);
        }
        closedir(fd);
    }
    merge_sort(config, dir->files);
    if (config->flag & FLAG_REVERSE)
        reverse_list(dir->files);
    if (dir->ignore_dir == 0)
        add_recursive_path(config, dir);
    if (dir->ignore_dir == 0 && config->flag & FLAG_LIST) {
        ft_putstr_fd("total ", STDOUT_FILENO);
        block_total = ft_itoa(dir->block_total / 2);
        ft_putstr_fd(block_total, STDOUT_FILENO);
        ft_putstr_fd("\r\n", STDOUT_FILENO);
        free(block_total);
    }
    print_files(config, dir);
    free_files(dir);
}

void parse_dir_list(ls_config *config) {
    t_directory *cur = config->directories;

    while (cur != NULL) {
        parse_directory(config, cur);
        free(cur->path);
        cur = cur->next;
        free(config->directories);
        config->directories = cur;
        if (config->flag & FLAG_TITLE && cur != NULL)
            ft_putstr_fd("\r\n", STDOUT_FILENO);
    }
}