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

char *get_full_path(char *base, char *filename)
{
    size_t len = ft_strlen(base) + ft_strlen(filename);
    char *path = malloc(len + 2);
    ft_strlcpy(path, base, len + 2);
    ft_strlcat(path, "/", len + 2);
    ft_strlcat(path, filename, len + 2);
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

int check_all_files(ls_config *config, char *name)
{
    if (config->flag & FLAG_ALL)
        return 1;
    if (name[0] == '.')
        return 0;
    return 1;
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
