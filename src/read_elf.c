/**
 * File              : src/injector/inject.c
 * Author            : tmaraval <no-mail@mail.com>
 * Date              : 06.10.2018
 * Last Modified Date: 06.10.2018
 * Last daModified By  : tmaraval <no-mail@mail.com>
 */

#include "../inc/packer.h"

void	*open_and_map(char *input_file, size_t *size)
{
	void *data;
	int  fd;

	if ((fd = open(input_file, O_RDONLY)) == -1)
	{
		perror("Open : ");
		exit(-1);
	}
	*size = lseek(fd, 0, SEEK_END);
	if ((data = mmap((void *)0, *size, PROT_READ | PROT_WRITE, MAP_PRIVATE,
			fd, 0)) == MAP_FAILED)
	{
		perror("mmap : ");
		exit(-1);
	}
#ifdef DEBUG
	dprintf(2, "Loaded file : |%s| size : |%ld|\n", input_file, *size);
#endif
	return (data);	
}

t_elf *read_elf(char *input_file)
{
	void *data;
	t_elf	*elf;
	size_t size;

	data = open_and_map(input_file, &size);
	if (!(elf = malloc(sizeof(t_elf))))
	{
		perror("Malloc :");
		exit(-1);
	}
	if (!(elf->ehdr = malloc(sizeof(Elf64_Ehdr))))
	{
		perror("Malloc :");
		exit(-1);
	}
	/* Copying ELF header */
	memset(elf->ehdr, 0, sizeof(Elf64_Ehdr));
	memcpy(elf->ehdr, data, sizeof(Elf64_Ehdr));
	
	/* Copying Prog header */
	if (!(elf->phdr = malloc(sizeof(Elf64_Phdr) * elf->ehdr->e_phnum)))
	{
		perror("Malloc :");
		exit(-1);
	}
	memcpy(elf->phdr, (uint8_t *)data + elf->ehdr->e_phoff,
			sizeof(Elf64_Phdr) * elf->ehdr->e_phnum);
	/* Reading section header */
	if (!(elf->shdr = malloc(sizeof(Elf64_Shdr) * elf->ehdr->e_shnum)))
	{
		perror("Malloc :");
		exit(-1);
	}
	memset(elf->shdr, 0, sizeof(Elf64_Shdr) * elf->ehdr->e_shnum);
	for (int i = 0; i < elf->ehdr->e_shnum; i++)
	{
		memcpy(&elf->shdr[i], 
				(uint8_t *)data + elf->ehdr->e_shoff + i * sizeof(Elf64_Shdr),
				sizeof(Elf64_Shdr));
	}
	/* Reading segment data */
	if (!(elf->segment_data = malloc(sizeof(uint8_t *) * elf->ehdr->e_shnum)))
	{
		perror("Malloc :");
		exit(-1);
	}
	memset(elf->segment_data, 0, sizeof(uint8_t *) * elf->ehdr->e_shnum);
	for (int i = 0; i < elf->ehdr->e_shnum; i++)
	{
		if (elf->shdr[i].sh_type == SHT_NOBITS)
			elf->segment_data[i] = (uint8_t *)0;
		else
		{
			elf->segment_data[i] = malloc(elf->shdr[i].sh_size);
			memset(elf->segment_data[i], 0, elf->shdr[i].sh_size);
			memcpy(elf->segment_data[i],
					(uint8_t *)data + elf->shdr[i].sh_offset,
				       	elf->shdr[i].sh_size);
		}

	}	
	return (elf);
}

