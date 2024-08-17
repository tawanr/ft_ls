/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/17 16:46:26 by tratanat          #+#    #+#             */
/*   Updated: 2024/08/17 18:32:03 by tratanat         ###   ########.fr       */
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
    print_debug(config);
    return 0;
}

void print_ls(ls_config *config)
{
    t_directory *cur = config->directories;
    struct dirent *fileinfo;
    void *fd;
    struct stat filestat;

    while (cur != NULL)
    {
        if (stat(cur->path, &filestat) < 0)
        {
            // TODO: Print error on non-existent directory
            printf("Error opening directory: %s\n", cur->path);
            cur = cur->next;
            continue;
        }
        if (config->flag | FLAG_TITLE)
            printf("%s:\n", cur->path);
        fd = opendir(cur->path);
        fileinfo = readdir(fd);
        while (fileinfo)
        {
            printf("%s %ld ", fileinfo->d_name, filestat.st_size);
            if (fileinfo->d_type == DT_DIR)
            {
                strcat();
            }
            fileinfo = readdir(fd);
        }
        cur = cur->next;
        closedir(fd);
    }
    printf("\n");
    if (config->flag | FLAG_TITLE)
        printf("\n");
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
    print_ls(&config);
    return 0;
}