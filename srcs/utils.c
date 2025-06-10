/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/23 20:44:25 by tratanat          #+#    #+#             */
/*   Updated: 2024/08/23 20:54:18 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"
#include "libft.h"
#include <unistd.h>

int add_arg_file(ls_config *config, char *path) {
    if (config->directories == NULL) {
        config->directories = new_directory("");
        if (config->directories == NULL)
            return 1;
        config->last = config->directories;
        config->directories->ignore_dir = 1;
    } else if (!config->directories->ignore_dir) {
        t_directory *new = new_directory("");
        if (new == NULL)
            return 1;
        new->next = config->directories;
        new->ignore_dir = 1;
        config->directories = new;
        if (config->last == NULL) {
            config->last = new;
        }
    }
    ls_file *new_file = malloc(sizeof(ls_file));
    new_file->name = ft_strdup(path);
    new_file->name_length = ft_strlen(new_file->name);
    new_file->filestat = malloc(sizeof(struct stat));
    new_file->next = *config->directories->files;
    lstat(path, new_file->filestat);
    *config->directories->files = new_file;
    config->directories->total_files++;
    check_columns(config, config->directories, new_file);
    return 0;
}

int add_path(ls_config *config, char *path) {
    if (check_path_folder(path) > 0) {
        t_directory *new = new_directory(path);
        if (new == NULL)
            return 1;
        if (config->last == NULL)
            config->directories = new;
        else
            config->last->next = new;
        config->last = new;
    } else if (check_path_folder(path) == -1) {
        ft_putstr_fd(config->appname, STDOUT_FILENO);
        ft_putstr_fd(": cannot access '", STDOUT_FILENO);
        ft_putstr_fd(path, STDOUT_FILENO);
        ft_putstr_fd("': No such file or directory\n", STDOUT_FILENO);
        config->exit_code = 2;
    } else {
        if (add_arg_file(config, path))
            return 1;
    }
    return 0;
}

char *get_full_path(char *base, char *filename) {
    size_t len = ft_strlen(base) + ft_strlen(filename);
    char *path = malloc(len + 2);
    ft_strlcpy(path, base, len + 2);
    ft_strlcat(path, "/", len + 2);
    ft_strlcat(path, filename, len + 2);
    return path;
}

int check_relative_filename(char *path) {
    if (!ft_strcmp(path, ".") || !ft_strcmp(path, ".."))
        return 0;
    return 1;
}

int check_folder(struct stat *filestat) {
    return (filestat->st_mode & S_IFMT) == S_IFDIR;
}

int check_path_folder(const char *path) {
    struct stat filestat;
    if (stat(path, &filestat) == -1)
        return -1;
    return check_folder(&filestat);
}

int check_all_files(ls_config *config, char *name) {
    if (config->flag & FLAG_ALL)
        return 1;
    if (name[0] == '.')
        return 0;
    return 1;
}

void free_files(t_directory *dir) {
    ls_file *cur = *(dir->files);
    while (cur) {
        free(cur->filestat);
        free(cur->name);
        free(cur->size);
        free(cur->links);
        free(cur->owner);
        free(cur->group);
        free(cur->time);
        free(cur->access_time);
        cur = cur->next;
        free(*(dir->files));
        *dir->files = cur;
    }
    free(dir->files);
}
