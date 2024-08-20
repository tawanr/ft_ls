/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/17 16:46:26 by tratanat          #+#    #+#             */
/*   Updated: 2024/08/20 12:15:29 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"

int parse_flags(ls_config *config, char *flag)
{
    while (*(++flag) != '\0')
    {
        if (*flag == 'l')
        {
            config->flag |= FLAG_LIST;
        }
        else if (*flag == 'R')
        {
            config->flag |= FLAG_RECURSIVE;
            config->flag |= FLAG_TITLE;
        }
        else if (*flag == 'a')
        {
            config->flag |= FLAG_ALL;
        }
        else if (*flag == 'r')
        {
            config->flag |= FLAG_REVERSE;
        }
        else if (*flag == 't')
        {
            config->flag |= FLAG_TIME;
        }
    }
    return 0;
}

int add_path(ls_config *config, char *path)
{
    if (*path == '\0')
        return 1;
    t_directory *new = malloc(sizeof(t_directory));
    new->path = strdup(path);
    new->next = NULL;
    new->files = malloc(sizeof(ls_file *));
    *new->files = NULL;
    new->size_col_max = 0;
    new->links_col_max = 0;
    new->owner_col_max = 0;
    new->group_col_max = 0;
    new->block_total = 0;
    if (config->last == NULL)
        config->directories = new;
    else
        config->last->next = new;
    config->last = new;
    return 0;
}

void print_debug(ls_config *config)
{
    if (config->flag & FLAG_LIST)
    {
        printf("Flag List Enabled\n");
    }
    if (config->flag & FLAG_RECURSIVE)
    {
        printf("Flag Recursive Enabled\n");
    }
    t_directory *cur = config->directories;
    while (cur != NULL)
    {
        printf("Directory: %s\n", cur->path);
        cur = cur->next;
    }
}

int parse_args(ls_config *config, int argc, char **argv)
{
    for (int i = 0; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            parse_flags(config, argv[i]);
        }
        else
        {
            add_path(config, argv[i]);
        }
    }
    if (config->directories != NULL && config->directories->next != NULL)
        config->flag |= FLAG_TITLE;
    // print_debug(config);
    return 0;
}

char *get_full_path(char *base, char *filename)
{
    size_t len = strlen(base) + strlen(filename);
    char *path = malloc(len + 2);
    strcpy(path, base);
    strcat(path, "/");
    strcat(path, filename);
    return path;
}

int check_relative_filename(char *path)
{
    if (!strcmp(path, ".") || !strcmp(path, ".."))
        return 0;
    return 1;
}

void print_acl(struct stat *filestat)
{
    char *rtn = malloc(sizeof(char) * 11);
    rtn[10] = '\0';
    for (int i = 0; i < 10; i++)
        rtn[i] = '-';
    if ((filestat->st_mode & S_IFMT) == S_IFDIR)
        rtn[0] = 'd';
    else if ((filestat->st_mode & S_IFMT) == S_IFBLK)
        rtn[0] = 'b';
    else if ((filestat->st_mode & S_IFMT) == S_IFCHR)
        rtn[0] = 'c';
    else if ((filestat->st_mode & S_IFMT) == S_IFLNK)
        rtn[0] = 'l';
    else if ((filestat->st_mode & S_IFMT) == S_IFSOCK)
        rtn[0] = 's';
    else if ((filestat->st_mode & S_IFMT) == S_IFIFO)
        rtn[0] = 'p';
    if (filestat->st_mode & S_IRUSR)
        rtn[1] = 'r';
    if (filestat->st_mode & S_IWUSR)
        rtn[2] = 'w';
    if (filestat->st_mode & S_IXUSR)
        rtn[3] = 'x';
    if (filestat->st_mode & S_IRGRP)
        rtn[4] = 'r';
    if (filestat->st_mode & S_IWGRP)
        rtn[5] = 'w';
    if (filestat->st_mode & S_IXGRP)
        rtn[6] = 'x';
    if (filestat->st_mode & S_IROTH)
        rtn[7] = 'r';
    if (filestat->st_mode & S_IWOTH)
        rtn[8] = 'w';
    if (filestat->st_mode & S_IXOTH)
        rtn[9] = 'x';
    printf("%s ", rtn);
    free(rtn);
}

void print_column(char *value, int column_size)
{
    int padding = column_size - strlen(value);

    for (int i = 0; i < padding; i++)
        printf(" ");
    printf("%s ", value);
}

void print_files(ls_config *config, t_directory *dir)
{
    ls_file *cur = *dir->files;

    while (cur)
    {
        if (!(config->flag & FLAG_ALL) && cur->name[0] == '.')
        {
            cur = cur->next;
            continue;
        }
        if (config->flag & FLAG_LIST)
        {
            print_acl(cur->filestat);
            print_column(cur->links, dir->links_col_max);
            print_column(cur->owner, dir->owner_col_max);
            print_column(cur->group, dir->group_col_max);
            print_column(cur->size, dir->size_col_max);
            print_column(cur->time, 12);
        }
        printf("%s", cur->name);
        if (config->flag & FLAG_LIST)
            printf("\n");
        else
            printf("  ");
        cur = cur->next;
    }
    if (!(config->flag & FLAG_LIST))
        printf("\n");
}

void check_columns(ls_config *config, t_directory *dir, ls_file *file)
{
    if (dir == NULL || file == NULL)
        return;
    if (!(config->flag & FLAG_ALL) && file->name[0] == '.')
        return;
    char buff[256];
    sprintf(buff, "%ld", file->filestat->st_size);
    file->size = strdup(buff);
    int len = strlen(file->size);
    if (len > dir->size_col_max)
        dir->size_col_max = len;

    sprintf(buff, "%ld", file->filestat->st_nlink);
    file->links = strdup(buff);
    len = strlen(file->links);
    if (len > dir->links_col_max)
        dir->links_col_max = len;

    struct passwd *pw = getpwuid(file->filestat->st_uid);
    file->owner = strdup(pw->pw_name);
    len = strlen(file->owner);
    if (len > dir->owner_col_max)
        dir->owner_col_max = len;

    struct group *grp = getgrgid(file->filestat->st_gid);
    file->group = strdup(grp->gr_name);
    len = strlen(file->group);
    if (len > dir->group_col_max)
        dir->group_col_max = len;

    char *time;
    time = ctime(&file->filestat->st_mtime);
    file->time = malloc(13);
    strncpy(file->time, time + 4, 12);
    file->time[12] = '\0';

    dir->block_total += file->filestat->st_blocks;
}

void parse_directory(ls_config *config, t_directory *dir)
{
    struct dirent *fileinfo;
    void *fd;
    struct stat filestat;
    ls_file *cur_file = NULL;
    ls_file *last_file = *dir->files;

    if (stat(dir->path, &filestat) < 0)
    {
        // TODO: Print error on non-existent directory
        printf("Error opening directory: %s\n", dir->path);
        return;
    }
    if (config->flag & FLAG_TITLE)
        printf("%s:\n", dir->path);
    fd = opendir(dir->path);
    fileinfo = readdir(fd);
    while (fileinfo)
    {
        cur_file = malloc(sizeof(ls_file));
        cur_file->name = strdup(fileinfo->d_name);
        cur_file->filestat = malloc(sizeof(struct stat));
        stat(get_full_path(dir->path, cur_file->name), cur_file->filestat);
        cur_file->next = NULL;
        if (last_file == NULL)
            *dir->files = cur_file;
        else
            last_file->next = cur_file;
        check_columns(config, dir, cur_file);
        last_file = cur_file;
        if (fileinfo->d_type == DT_DIR &&
            (config->flag & FLAG_RECURSIVE) &&
            check_relative_filename(cur_file->name))
        {
            add_path(config, get_full_path(dir->path, cur_file->name));
        }
        fileinfo = readdir(fd);
    }
    closedir(fd);
    merge_sort(config, dir->files);
    if (config->flag & FLAG_LIST)
        printf("total %ld\n", dir->block_total / 2);
    print_files(config, dir);
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
            printf("\n");
    }
}

int main(int argc, char *argv[])
{
    ls_config config;
    config.flag = 0;
    config.directories = NULL;
    config.last = NULL;
    if (argc > 0)
    {
        parse_args(&config, argc - 1, &argv[1]);
    }
    if (config.directories == NULL)
    {
        add_path(&config, ".");
    }
    parse_dir_list(&config);
    return 0;
}