/**
 * File              : packer.h
 * Author            : tmaraval <no-mail@mail.com>
 * Date              : 06.10.2018
 * Last Modified Date: 06.10.2018
 * Last Modified By  : tmaraval <no-mail@mail.com>
 */

#ifndef __PACKER_H__
# define __PACKER_H__
# define DEBUG 1

# include <sys/stat.h>
# include <fcntl.h>
# include <stdio.h>
# include <unistd.h>
# include <sys/mman.h>
# include <elf.h>
# include <string.h>
# include <stdlib.h>
# include <sys/user.h>

typedef struct 	s_elf
{
	Elf64_Ehdr *ehdr;
	Elf64_Phdr *phdr;
	Elf64_Shdr *shdr;
	uint8_t	   **segment_data;

}		t_elf;

int	write_bin(char *output_file, t_elf *elf);
t_elf *read_elf(char *input_file);
int32_t		add_section(t_elf *elf, void *payload, size_t payload_size);
void			write_file(t_elf *elf);
void	*open_and_map(char *input_file, size_t *size);
void	*loader_get(char *payload_file, size_t *size);
int	cypher_text(uint8_t **t_segdata, int t_indx, size_t size);
int 	utils_get_sect_nbr(t_elf *elf, char *name);
#endif
