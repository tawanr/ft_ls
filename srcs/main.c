/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/17 16:46:26 by tratanat          #+#    #+#             */
/*   Updated: 2024/08/23 20:33:58 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"
#include "libft.h"

int parse_flags(ls_config *config, char *flag)
{
    while (*(++flag) != '\0')
    {
        if (*flag == 'l')
            config->flag |= FLAG_LIST;
        else if (*flag == 'R')
        {
            config->flag |= FLAG_RECURSIVE;
            config->flag |= FLAG_TITLE;
        }
        else if (*flag == 'a')
            config->flag |= FLAG_ALL;
        else if (*flag == 'r')
            config->flag |= FLAG_REVERSE;
        else if (*flag == 't')
            config->flag |= FLAG_TIME;
        else if (*flag == 'u')
            config->flag |= FLAG_ACCESS_TIME;
        else if (*flag == 'f')
        {
            config->flag |= FLAG_UNSORTED;
            config->flag ^= FLAG_ALL;
        }
    }
    return 0;
}

t_directory *new_directory(char *path)
{
    if (*path == '\0')
        return NULL;
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
    return new;
}

int add_path(ls_config *config, char *path)
{
    t_directory *new = new_directory(path);
    if (new == NULL)
        return 1;
    if (config->last == NULL)
        config->directories = new;
    else
        config->last->next = new;
    config->last = new;
    return 0;
}

int parse_args(ls_config *config, int argc, char **argv)
{
    for (int i = 0; i < argc; i++)
    {
        if (argv[i][0] == '-')
            parse_flags(config, argv[i]);
        else
            add_path(config, argv[i]);
    }
    if (config->directories != NULL && config->directories->next != NULL)
        config->flag |= FLAG_TITLE;
    return 0;
}

char *get_full_path(char *base, char *filename)
{
    size_t len = ft_strlen(base) + ft_strlen(filename);
    char *path = malloc(len + 2);
    strcpy(path, base);
    strcat(path, "/");
    strcat(path, filename);
    return path;
}

int check_relative_filename(char *path)
{
    if (!ft_strcmp(path, ".") || !ft_strcmp(path, ".."))
        return 0;
    return 1;
}

int check_folder(struct stat *filestat)
{
    return (filestat->st_mode & S_IFMT) == S_IFDIR;
}

void check_columns(ls_config *config, t_directory *dir, ls_file *file)
{
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
    strncpy(file->time, time + 4, 12);
    file->time[12] = '\0';

    time = ctime(&file->filestat->st_atime);
    file->access_time = malloc(13);
    strncpy(file->access_time, time + 4, 12);
    file->access_time[12] = '\0';

    dir->block_total += file->filestat->st_blocks;
}

int check_all_files(ls_config *config, char *name)
{
    if (config->flag & FLAG_ALL)
        return 1;
    if (name[0] == '.')
        return 0;
    return 1;
}

void add_recursive_path(ls_config *config, t_directory *dir)
{
    ls_file *file = *(dir->files);
    t_directory *head = NULL;
    t_directory *last = NULL;
    char *full_path = NULL;

    while (file != NULL)
    {
        if (check_folder(file->filestat) &&
            (config->flag & FLAG_RECURSIVE) &&
            check_relative_filename(file->name) &&
            check_all_files(config, file->name))
        {
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
    if (config->directories != NULL)
    {
        if (last != NULL)
            last->next = config->directories->next;
        config->directories->next = head;
    }
    else
        config->directories = head;
}

void free_files(t_directory *dir)
{
    ls_file *cur = *(dir->files);
    while (cur)
    {
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

void parse_directory(ls_config *config, t_directory *dir)
{
    struct dirent *fileinfo;
    void *fd;
    struct stat filestat;
    ls_file *cur_file = NULL;
    ls_file *last_file = *dir->files;
    char *full_path = NULL;

    if (config->flag & FLAG_TITLE)
    {
        ft_putstr_fd(dir->path, STDOUT_FILENO);
        ft_putstr_fd(":\n", STDOUT_FILENO);
    }
    fd = opendir(dir->path);
    fileinfo = readdir(fd);

    while (fileinfo)
    {
        if (!(config->flag & FLAG_ALL) && fileinfo->d_name[0] == '.')
        {
            fileinfo = readdir(fd);
            continue;
        }
        cur_file = malloc(sizeof(ls_file));
        cur_file->name = ft_strdup(fileinfo->d_name);
        cur_file->name_length = ft_strlen(cur_file->name);
        cur_file->filestat = malloc(sizeof(struct stat));
        full_path = get_full_path(dir->path, cur_file->name);
        stat(full_path, cur_file->filestat);
        free(full_path);
        cur_file->next = NULL;
        if (last_file == NULL)
            *dir->files = cur_file;
        else
            last_file->next = cur_file;
        dir->total_files++;
        check_columns(config, dir, cur_file);
        last_file = cur_file;
        fileinfo = readdir(fd);
    }
    closedir(fd);
    merge_sort(config, dir->files);
    if (config->flag & FLAG_REVERSE)
        reverse_list(dir->files);
    add_recursive_path(config, dir);
    if (config->flag & FLAG_LIST)
        printf("total %ld\n", dir->block_total / 2);
    print_files(config, dir);
    free_files(dir);
}

void parse_dir_list(ls_config *config)
{
    t_directory *cur = config->directories;

    while (cur != NULL)
    {
        parse_directory(config, cur);
        free(cur->path);
        cur = cur->next;
        free(config->directories);
        config->directories = cur;
        if (config->flag & FLAG_TITLE && cur != NULL)
            ft_putstr_fd("\n", STDOUT_FILENO);
    }
}

void validate_path(ls_config *config)
{
    struct stat filestat;
    t_directory *cur = config->directories;
    t_directory *prev = NULL;

    while (cur)
    {
        if (stat(cur->path, &filestat) >= 0)
        {
            prev = cur;
            cur = cur->next;
            continue;
        }
        ft_putstr_fd(config->appname, STDERR_FILENO);
        ft_putstr_fd(": cannot access '", STDERR_FILENO);
        ft_putstr_fd(cur->path, STDERR_FILENO);
        ft_putstr_fd("': No such file or directory\n", STDERR_FILENO);
        if (prev == NULL)
            config->directories = cur->next;
        else
            prev->next = cur->next;
        free(cur->path);
        free(*(cur->files));
        free(cur->files);
        free(cur);
        cur = prev == NULL ? config->directories : prev->next;
    }
}

int main(int argc, char *argv[])
{
    ls_config config;
    config.flag = 0;
    config.directories = NULL;
    config.last = NULL;
    config.appname = argv[0];
    if (argc > 0)
    {
        parse_args(&config, argc - 1, &argv[1]);
    }
    if (config.directories == NULL)
    {
        add_path(&config, ".");
    }
    validate_path(&config);
    parse_dir_list(&config);
    return 0;
}