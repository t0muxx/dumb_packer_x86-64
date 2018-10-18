/**
 * File              : src/loader_utils.c
 * Author            : tmaraval <no-mail@mail.com>
 * Date              : 08.10.2018
 * Last Modified Date: 08.10.2018
 * Last Modified By  : tmaraval <no-mail@mail.com>
 */

#include "../inc/packer.h"

int 	loader_patch_entryp(void *data, int len, long pattern, long value)
{
	unsigned char *p = (unsigned char *)data;
	long val;
	int i;
	int r;

	for (i = 0; i < len; i++)
	{
		val = *((long *)(p + i));
		r = val ^ pattern;

		if (r == 0)
		{
#ifdef DEBUG
	dprintf(2, "Pattern %lx found at offset %d -> %lx\n",
			pattern, i, value); 
#endif 
			*((long *)p + i) = value;
			return (0);
		}
	}
	return (-1);
}	

Elf64_Shdr *loader_get_section(void *data, char *name)
{
	Elf64_Ehdr 	*ehdr = (Elf64_Ehdr *)data;
	Elf64_Shdr      *shdr = (Elf64_Shdr *)(data + ehdr->e_shoff);
	Elf64_Shdr 	*sht_str_tab = &shdr[ehdr->e_shstrndx];
	char 		*sname;

	for (int i = 0; i < ehdr->e_shnum; i++)
	{
		sname = (char *)(data + sht_str_tab->sh_offset + shdr[i].sh_name);
		if (!(strcmp(sname, name)))
		{
#ifdef DEBUG
			dprintf(2, "Found |%s| at offset |%#lx|\n", sname, shdr[i].sh_offset);
#endif
			return (&shdr[i]);
		}
	}
	return (NULL);
}


void	*loader_get(char *payload_file, size_t *size)
{
	void *data;
	void *l_data;
	Elf64_Shdr *p_text_sec;
	size_t tmp;

	data = open_and_map(payload_file, size);
	/* now we need to find the actual code of ou payload
	 * which is in the .text section */
	p_text_sec = loader_get_section(data, (char *)".text");
	if (!(l_data = malloc(p_text_sec->sh_size)))
	{
		perror("Malloc : ");
		exit(-1);
	}
	memset(l_data, 0, p_text_sec->sh_size);
	memcpy(l_data, data + p_text_sec->sh_offset, p_text_sec->sh_size);
	tmp = *size;
	*size = p_text_sec->sh_size;
	munmap(data, tmp);
	return (l_data);
}
