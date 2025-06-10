/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/17 16:46:26 by tratanat          #+#    #+#             */
/*   Updated: 2024/08/23 20:42:31 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ls.h"
#include "libft.h"

int main(int argc, char *argv[]) {
    ls_config config;
    config.flag = 0;
    config.directories = NULL;
    config.last = NULL;
    config.appname = argv[0];
    if (argc > 0) {
        parse_args(&config, argc - 1, &argv[1]);
    }
    if (config.directories == NULL) {
        add_path(&config, ".");
    }
    validate_path(&config);
    parse_dir_list(&config);
    return 0;
}