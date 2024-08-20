/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 16:16:54 by tratanat          #+#    #+#             */
/*   Updated: 2024/08/20 11:59:10 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"

ls_file *merge(ls_file *a, ls_file *b, int (*comp)(ls_file *, ls_file *))
{
    ls_file *result = NULL;

    if (a == NULL)
        return (b);
    else if (b == NULL)
        return (a);

    if (comp(a, b) <= 0)
    {
        result = a;
        result->next = merge(a->next, b, comp);
    }
    else
    {
        result = b;
        result->next = merge(a, b->next, comp);
    }
    return result;
}

void list_split(ls_file *source, ls_file **front, ls_file **back)
{
    ls_file *fast = source->next;
    ls_file *slow = source;

    while (fast != NULL)
    {
        fast = fast->next;
        if (fast != NULL)
        {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *front = source;
    *back = slow->next;
    slow->next = NULL;
}

int default_comp(ls_file *a, ls_file *b)
{
    return strcmp(a->name, b->name);
}

int reverse_comp(ls_file *a, ls_file *b)
{
    return strcmp(b->name, a->name);
}

int time_comp(ls_file *a, ls_file *b)
{
    return b->filestat->st_mtime - a->filestat->st_mtime;
}

int reverse_time_comp(ls_file *a, ls_file *b)
{
    return a->filestat->st_mtime - b->filestat->st_mtime;
}

void merge_sort(ls_config *config, ls_file **file_list)
{
    ls_file *head = *file_list;
    ls_file *a;
    ls_file *b;

    if ((head == NULL) || (head->next == NULL))
        return;

    list_split(head, &a, &b);

    merge_sort(config, &a);
    merge_sort(config, &b);

    int (*comp)(ls_file *, ls_file *) = &default_comp;
    if (config->flag & FLAG_TIME && config->flag & FLAG_REVERSE)
        comp = &reverse_time_comp;
    else if (config->flag & FLAG_TIME)
        comp = &time_comp;
    else if (config->flag & FLAG_REVERSE)
        comp = &reverse_comp;

    *file_list = merge(a, b, comp);
}