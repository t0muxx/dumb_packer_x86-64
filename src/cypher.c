/**
 * File              : src/cypher.c
 * Author            : tmaraval <no-mail@mail.com>
 * Date              : 09.10.2018
 * Last Modified Date: 09.10.2018
 * Last Modified By  : tmaraval <no-mail@mail.com>
 */

#include "packer.h"

static const char *key = "ABCDEFGH";

int 	utils_get_sect_nbr(t_elf *elf, char *name)
{
	char *sname;

	for (int i = 0; i < elf->ehdr->e_shnum; i++)
	{
		sname = (char *)(elf->segment_data[elf->ehdr->e_shstrndx] + elf->shdr[i].sh_name); 
		if (!(strcmp(sname, name)))
			return (i);
	}
	return (-1);
}

int	cypher_text(uint8_t **t_segdata, int t_indx, size_t size)
{
	int j = 0;
	int i = 0;

#ifdef DEBUG
	dprintf(2, "Cyphering |%lu|\n", size);
#endif
	while (i < (int)size)
	{
		j = 0;
		while (key[j])
		{
			t_segdata[t_indx][i] = t_segdata[t_indx][i] ^ key[j];
			i++;
			j++;
		}
	}
	return (0);
}
