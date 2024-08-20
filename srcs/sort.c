/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 16:16:54 by tratanat          #+#    #+#             */
/*   Updated: 2024/08/19 20:23:06 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"

ls_file *merge(ls_file *a, ls_file *b)
{
    ls_file *result = NULL;

    if (a == NULL)
        return (b);
    else if (b == NULL)
        return (a);

    if (strcmp(a->name, b->name) <= 0)
    {
        result = a;
        result->next = merge(a->next, b);
    }
    else
    {
        result = b;
        result->next = merge(a, b->next);
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

void merge_sort(ls_file **file_list)
{
    ls_file *head = *file_list;
    ls_file *a;
    ls_file *b;

    if ((head == NULL) || (head->next == NULL))
        return;

    list_split(head, &a, &b);

    merge_sort(&a);
    merge_sort(&b);

    *file_list = merge(a, b);
}