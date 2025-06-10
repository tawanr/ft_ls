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
    config.input_args = 0;
    config.exit_code = 0;
    if (argc > 0) {
        parse_args(&config, argc - 1, &argv[1]);
    }
    if (config.directories == NULL && config.input_args == 0) {
        if (add_path(&config, "."))
            return config.exit_code > 0 ? config.exit_code : 1;
    }
    if (config.directories == NULL) {
        return config.exit_code;
    }
    // validate_path(&config);
    parse_dir_list(&config);
    return config.exit_code;
}