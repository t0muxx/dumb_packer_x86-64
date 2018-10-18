/**
 * File              : src/add_section.c
 * Author            : tmaraval <no-mail@mail.com>
 * Date              : 07.10.2018
 * Last Modified Date: 07.10.2018
 * Last Modified By  : tmaraval <no-mail@mail.com>
 */

#include "../inc/packer.h"

static Elf64_Shdr	new_section = {
  .sh_name = (uint32_t)0,
  .sh_type = (uint32_t)SHT_PROGBITS,
  .sh_flags = (uint64_t)SHF_EXECINSTR | SHF_ALLOC,
  .sh_addr = (Elf64_Addr)0,
  .sh_offset = (Elf64_Off)0,
  .sh_size = (uint64_t)0,
  .sh_link = (uint32_t)0,
  .sh_info = (uint32_t)0,
  .sh_addralign = (uint64_t)16,
  .sh_entsize = (uint64_t)0,
};

int32_t		insert_section_header(t_elf *elf, uint16_t last_section, uint16_t last_ptload,
		size_t payload_size)
{
	Elf64_Shdr	*new_shdr;
	
	elf->ehdr->e_shnum += 1;
	
	/* we malloc a new shdr and copie old value from until last_ptload */
	if (!(new_shdr = malloc(sizeof(Elf64_Shdr) * elf->ehdr->e_shnum)))
	{
		perror("Malloc : ");
		exit(-1);
	}
#ifdef DEBUG
	dprintf(2, "we have |%d| section header\n", elf->ehdr->e_shnum);
#endif
	memset(new_shdr, 0, sizeof(Elf64_Shdr) * elf->ehdr->e_shnum);	
	for (int i = 0; i <= last_section; i++)
		memcpy(&new_shdr[i], &elf->shdr[i], sizeof(Elf64_Shdr));
#ifdef DEBUG
//	dprintf(2, "oldname = |%d|\n", new_shdr[i].sh_name);
//	dprintf(2, "newname = |%d|\n", elf->shdr[i].sh_name);
#endif
	/* we fill information about our newsection */
#ifdef DEBUG
	dprintf(2, "Add new section header number |%d|\n", last_section + 1);
#endif
	memcpy(&new_shdr[last_section + 1], &new_section, sizeof(Elf64_Shdr));
	new_shdr[last_section + 1].sh_size = payload_size;
	/* pk le new offset = elf->phdr[last_ptload].p_offset +
	 * elf->phdr[last_ptload].p_memsz et non filesz ?? */
	new_shdr[last_section + 1].sh_offset = 
		elf->phdr[last_ptload].p_offset + elf->phdr[last_ptload].p_memsz;
	new_shdr[last_section + 1].sh_addr = 
		elf->phdr[last_ptload].p_vaddr + elf->phdr[last_ptload].p_memsz;	
	/* the new section header has been added still need to copy the rest of 
	 * section header */
	memcpy(&new_shdr[last_section + 2], 
			&elf->shdr[last_section + 1], 
			sizeof(Elf64_Shdr) * (elf->ehdr->e_shnum - last_section - 2));
	/* need to update the offset after our new section head */
	for (int i = last_section + 1; i < elf->ehdr->e_shnum - 1; i++)
	{
		new_shdr[i + 1].sh_offset = new_shdr[i].sh_offset + new_shdr[i].sh_size;
		new_shdr[i + 1].sh_addr += payload_size;
	}
	elf->shdr = new_shdr;
	elf->ehdr->e_shoff = elf->shdr[elf->ehdr->e_shnum - 1].sh_offset + elf->shdr[elf->ehdr->e_shnum - 1].sh_size;
	elf->ehdr->e_entry = new_shdr[last_section + 1].sh_addr;
	elf->ehdr->e_shstrndx += 1;
	return (0);
}

int32_t		add_section_data(t_elf *elf, uint16_t last_section, void *payload, size_t payload_size)
{
	uint8_t		**new_segdata;
	int		j = 0;

#ifdef DEBUG
	dprintf(2, "we have |%d| section\n", elf->ehdr->e_shnum);
#endif
	if (!(new_segdata = malloc(sizeof(uint8_t *) * elf->ehdr->e_shnum)))
	{
		perror("Malloc :");
		exit(-1);
	}
	memset(new_segdata, 0, sizeof(uint8_t *) * elf->ehdr->e_shnum);
	/* we copy the old value until where we need to put our loader */
#ifdef DEBUG
	dprintf(2, "Copying until section num |%d|\n", last_section);
#endif
	for (int i = 0; i <= last_section; i++)
	{
		if (elf->shdr[i].sh_type == SHT_NOBITS)
			new_segdata[i] = (uint8_t *)0;
		else
		{
			new_segdata[i] = malloc(elf->shdr[i].sh_size);
			memset(new_segdata[i], 0, elf->shdr[i].sh_size);
			memcpy(new_segdata[i], elf->segment_data[i],
				       	elf->shdr[i].sh_size);
		}
		j = i;
	}
	last_section++;
#ifdef DEBUG
	dprintf(2, "Copied |%d| section before loader\n", j);
	dprintf(2, "Loader will be in section num |%d|\n", last_section);
	dprintf(2, "at offset |%#lx|\n", elf->shdr[last_section].sh_offset);
#endif
	/* we copy our loader */
	new_segdata[last_section] = malloc(payload_size);
	memcpy(new_segdata[last_section], payload, payload_size);
	/* we copy the rest of the segment data after our loader */
	for (int i = last_section + 1; i < elf->ehdr->e_shnum; i++)
	{
		if (elf->shdr[i].sh_type == SHT_NOBITS)
			new_segdata[i] = (uint8_t *)0;
		else
		{
			new_segdata[i] = malloc(elf->shdr[i].sh_size);
			memset(new_segdata[i], 0, elf->shdr[i].sh_size);
			memcpy(new_segdata[i],
					elf->segment_data[i - 1],
				       	elf->shdr[i].sh_size);
		}

	}
	elf->segment_data = new_segdata;
	return (0);
}

int 		loader_set_entryp(void *loader, size_t len, long pat, long val)
{
	unsigned char *p = (unsigned char *)loader;
	long v;
	int i, r;

	for (i = 0; i < (int )len; i++)
	{
		v = *((long *)(p+i));
		r = v ^pat;
		if (r == 0)
		{
			printf ("+ Pattern %lx found at offset %d -> %lx\n", pat, i, val);
			*((long *)(p + i)) = val;
			return 0;
		}
	}
	return (-1);
}

int32_t		add_section(t_elf *elf, void *payload, size_t payload_size)
{
	uint16_t last_section = -1;
	uint16_t last_ptload = -1;

#ifdef DEBUG
	dprintf(2, "Old sectionheader number |%d|\n", elf->ehdr->e_shnum);
	dprintf(2, "Old entry_point |%#lx|\n", elf->ehdr->e_entry);
#endif
	/* finding last pt_load section */
	for (int i = 0; i < elf->ehdr->e_phnum; i++)
	{
		if (elf->phdr[i].p_type == PT_LOAD)
			last_ptload = i;
	}

	/* finding last section */
	for (int i = 0; i < elf->ehdr->e_shnum; i ++)
	{
		Elf64_Phdr	*phdr = &elf->phdr[last_ptload];
		Elf64_Shdr	*shdr = &elf->shdr[i];
		if (shdr->sh_addr + shdr->sh_size >= phdr->p_vaddr + phdr->p_memsz) 
			last_section = i;
	}
	while (loader_set_entryp(payload, payload_size,
				0x11111111, (long)elf->ehdr->e_entry) == 0)
		;
	loader_set_entryp(payload, payload_size, 0x22222222,
			(long)elf->shdr[utils_get_sect_nbr(elf, ".text")].sh_size);
	insert_section_header(elf, last_section, last_ptload, payload_size);
	add_section_data(elf, last_section, payload, payload_size);
	last_section += 1;
	
	uint64_t	size = elf->phdr[last_ptload].p_memsz + payload_size;
  	elf->phdr[last_ptload].p_memsz = size;
  	elf->phdr[last_ptload].p_filesz = size;

  	for (uint16_t i = 0; i < elf->ehdr->e_phnum; i++) {
    		if(elf->phdr[i].p_type == PT_LOAD) {
    	  		elf->phdr[i].p_flags = PF_X | PF_W | PF_R;
    		}
	}

  	uint16_t	shnum = elf->ehdr->e_shnum;
	elf->ehdr->e_shoff = elf->shdr[shnum - 1].sh_offset + elf->shdr[shnum - 1].sh_size;
#ifdef DEBUG
	dprintf(2, "elf->ehdr->shoff = |%#lx|\n", elf->ehdr->e_shoff);
#endif
	return (0);
}
