/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tabular.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/21 10:30:08 by tratanat          #+#    #+#             */
/*   Updated: 2024/08/23 19:57:48 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"
#include "libft.h"

int get_terminal_size()
{
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    return w.ws_col;
}

column_info *get_col_info(t_directory *directory)
{
    ls_file *files = *directory->files;
    int file_count = directory->total_files;
    const int line_length = get_terminal_size();
    const int min_cols = line_length / 3 - 1;
    int max_cols = min_cols < file_count ? min_cols : file_count;
    column_info **infos = malloc(sizeof(column_info *) * max_cols);
    if (infos == NULL)
        return NULL;
    column_info *rtn = NULL;
    char **names = malloc(sizeof(char *) * file_count);
    if (names == NULL)
        return NULL;

    for (int i = 0; i < max_cols; i++)
    {
        infos[i] = malloc(sizeof(column_info));
        infos[i]->col_count = i + 1;
        infos[i]->row_count = (file_count + (infos[i]->col_count - 1)) / infos[i]->col_count;
        infos[i]->total_width = infos[i]->col_count * 2 - 2;
        infos[i]->cols = malloc(sizeof(int) * (infos[i]->col_count));
        infos[i]->names = NULL;
        for (int j = 0; j < infos[i]->col_count; j++)
            infos[i]->cols[j] = 0;
        infos[i]->is_valid = 1;
    }

    ls_file *cur = files;
    int idx = 0;
    while (cur)
    {
        for (int i = 0; i < max_cols; i++)
        {
            if (infos[i]->is_valid == 0)
                break;
            if (cur->name_length > infos[i]->cols[idx / infos[i]->row_count])
            {
                infos[i]->total_width += cur->name_length - infos[i]->cols[idx / infos[i]->row_count];
                infos[i]->cols[idx / infos[i]->row_count] = cur->name_length;
            }
            if (infos[i]->total_width >= line_length && infos[i]->col_count > 1)
                infos[i]->is_valid = 0;
        }
        names[idx++] = cur->name;
        cur = cur->next;
    }
    for (int i = max_cols - 1; i >= 0; i--)
    {
        if (infos[i]->is_valid == 0)
        {
            free(infos[i]->cols);
            free(infos[i]);
            continue;
        }
        if (rtn == NULL)
        {
            rtn = infos[i];
            continue;
        }
        if (infos[i]->row_count == rtn->row_count)
        {
            free(rtn->cols);
            free(rtn);
            rtn = infos[i];
            continue;
        }
        free(infos[i]->cols);
        free(infos[i]);
    }
    free(infos);
    rtn->file_count = file_count;
    rtn->names = names;
    return rtn;
}

void print_tabular(t_directory *directory)
{
    column_info *col_info = get_col_info(directory);
    if (col_info == NULL)
        return;
    int padding = 0;
    char *name = NULL;
    const int row_count = col_info->row_count;
    int offset = 0;

    for (int row = 0; row < row_count; row++)
    {
        for (int col = 0; col < col_info->col_count; col++)
        {
            offset = (col * row_count) + row;
            if (offset >= col_info->file_count)
                continue;
            name = col_info->names[offset];
            padding = col_info->cols[col] - ft_strlen(name);
            ft_putstr_fd(name, 1);
            for (int i = 0; i < padding; i++)
                ft_putstr_fd(" ", STDOUT_FILENO);
            ft_putstr_fd("  ", STDOUT_FILENO);
        }
        ft_putstr_fd("\n", STDOUT_FILENO);
    }
    free(col_info->names);
    free(col_info->cols);
    free(col_info);
}