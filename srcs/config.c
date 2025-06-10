/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/23 20:39:04 by tratanat          #+#    #+#             */
/*   Updated: 2024/08/23 21:30:02 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"
#include "libft.h"

int parse_flags(ls_config *config, char *flag) {
    while (*(++flag) != '\0') {
        if (*flag == 'l')
            config->flag |= FLAG_LIST;
        else if (*flag == 'R') {
            config->flag |= FLAG_RECURSIVE;
            config->flag |= FLAG_TITLE;
        } else if (*flag == 'a')
            config->flag |= FLAG_ALL;
        else if (*flag == 'r')
            config->flag |= FLAG_REVERSE;
        else if (*flag == 't')
            config->flag |= FLAG_TIME;
        else if (*flag == 'u')
            config->flag |= FLAG_ACCESS_TIME;
        else if (*flag == 'f') {
            config->flag |= FLAG_UNSORTED;
            config->flag ^= FLAG_ALL;
        }
    }
    return 0;
}

int parse_args(ls_config *config, int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        if (argv[i][0] == '-')
            parse_flags(config, argv[i]);
        else {
            if (add_path(config, argv[i]))
                return 1;
            config->input_args++;
        }
    }
    if (config->directories != NULL && config->directories->next != NULL)
        config->flag |= FLAG_TITLE;
    return 0;
}

void validate_path(ls_config *config) {
    struct stat filestat;
    t_directory *cur = config->directories;
    t_directory *prev = NULL;

    while (cur) {
        if (cur->ignore_dir == 1 || stat(cur->path, &filestat) >= 0) {
            prev = cur;
            cur = cur->next;
            continue;
        }
        ft_putstr_fd(config->appname, STDOUT_FILENO);
        ft_putstr_fd(": cannot access '", STDOUT_FILENO);
        ft_putstr_fd(cur->path, STDOUT_FILENO);
        ft_putstr_fd("': No such file or directory\n", STDOUT_FILENO);
        config->exit_code = 2;
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