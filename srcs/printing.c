/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printing.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/23 20:31:39 by tratanat          #+#    #+#             */
/*   Updated: 2024/08/23 21:20:32 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"
#include "libft.h"

void print_acl(struct stat *filestat) {
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
    ft_putstr_fd(rtn, STDOUT_FILENO);
    ft_putstr_fd(" ", STDOUT_FILENO);
    free(rtn);
}

void print_column(char *value, int column_size) {
    int padding = column_size - strlen(value);

    for (int i = 0; i < padding; i++)
        ft_putstr_fd(" ", STDOUT_FILENO);
    ft_putstr_fd(value, STDOUT_FILENO);
    ft_putstr_fd(" ", STDOUT_FILENO);
}

void print_files(ls_config *config, t_directory *dir) {
    ls_file *cur = *dir->files;

    if (!(config->flag & FLAG_LIST)) {
        print_tabular(dir);
        return;
    }
    while (cur) {
        if (!(config->flag & FLAG_ALL) && cur->name[0] == '.') {
            cur = cur->next;
            continue;
        }
        if (config->flag & FLAG_LIST) {
            print_acl(cur->filestat);
            print_column(cur->links, dir->links_col_max);
            print_column(cur->owner, dir->owner_col_max);
            if (!(config->flag & FLAG_NO_OWNER))
                print_column(cur->group, dir->group_col_max);
            print_column(cur->size, dir->size_col_max);
            if (config->flag & FLAG_ACCESS_TIME)
                print_column(cur->access_time, 12);
            else
                print_column(cur->time, 12);
            ft_putstr_fd(cur->name, STDOUT_FILENO);
            if ((cur->filestat->st_mode & S_IFMT) == S_IFLNK) {
                ft_putstr_fd(" -> ", STDOUT_FILENO);
                char link[256];
                char *full_path = get_full_path(dir->path, cur->name);
                int len = readlink(full_path, link, 256);
                link[len] = '\0';
                ft_putstr_fd(link, STDOUT_FILENO);
                free(full_path);
            }
            ft_putstr_fd("\r\n", STDOUT_FILENO);
        }
        cur = cur->next;
    }
}