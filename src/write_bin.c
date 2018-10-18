/**
 * File              : src/write_bin.c
 * Author            : tmaraval <no-mail@mail.com>
 * Date              : 06.10.2018
 * Last Modified Date: 06.10.2018
 * Last Modified By  : tmaraval <no-mail@mail.com>
 */

#include "../inc/packer.h"

/*  _________________
 * |   ELF HEADER   |
 * |________________|
 * |   Prog header  |
 * |________________|
 * |	.text	    |
 * |________________|
 * |	  ...	    |
 * |________________|
 * |    .data	    |
 * |________________|
 * | Section header |
 * |________________|
 *
 *
 * */

static uint64_t	off = 0;

/*static inline void	write_to_file(int fd, void *data, uint64_t size)
{
  if (write(fd, data, size) != (ssize_t)size) {
    perror("write");
    exit(-1);
  }
  off += size;
}

static void		pad_zero(int fd, uint64_t end)
{
  static const char	c = 0;

  while (off < end) {
    write_to_file(fd, (void *)&c, 1);
  }
}

void			write_file(t_elf *elf)
{
  int			fd;

  if ((fd = open(FILENAME, O_CREAT | O_WRONLY, 0744)) < 0) {
    perror("open");
    return ;
  }

  write_to_file(fd, elf->ehdr, sizeof(Elf64_Ehdr));
  pad_zero(fd, elf->ehdr->e_phoff);
  write_to_file(fd, elf->phdr, sizeof(Elf64_Phdr) * elf->ehdr->e_phnum);

  for (uint16_t id = 0; id < elf->ehdr->e_shnum; id += 1) {
    if (elf->shdr[id].sh_type != SHT_NOBITS) {
      pad_zero(fd, elf->shdr[id].sh_offset);
      write_to_file(fd, elf->segment_data[id], elf->shdr[id].sh_size);
    }
  }
  pad_zero(fd, elf->ehdr->e_shoff);

  for (uint16_t id = 0; id < elf->ehdr->e_shnum; id += 1) {
    write_to_file(fd, &elf->shdr[id], sizeof(Elf64_Shdr));
  }

  close(fd);

  printf("file created: '%s'\n", FILENAME);
}*/

void	write_to(int fd, void *data, uint64_t size)
{
	if (write(fd, data, size) != (ssize_t)size) 
	{
		perror("write");
		exit(-1);
  	}
  off += size;
}	

void	zero_pad(int fd, uint64_t until)
{
	char c;

	c = 0;
	while (off < until)
		write_to(fd, &c, 1);
}

int	write_bin(char *output_file, t_elf *elf)
{
	int i = 0;
	int fd = 0;

	/* We write to file */
	if ((fd = open(output_file, O_WRONLY | O_CREAT | O_EXCL, 0744)) == -1)
	{
		perror("2 : Open : ");
		exit(-1);
	}
	write_to(fd, elf->ehdr, sizeof(Elf64_Ehdr));
	zero_pad(fd, elf->ehdr->e_phoff);
	write_to(fd, elf->phdr, sizeof(Elf64_Phdr) * elf->ehdr->e_phnum);
	for (i = 0; i < (int)elf->ehdr->e_shnum; i++)
	{
		if (elf->shdr[i].sh_type != SHT_NOBITS)
		{
      			zero_pad(fd, elf->shdr[i].sh_offset);
			write_to(fd, elf->segment_data[i], elf->shdr[i].sh_size);
		}
	}
	zero_pad(fd, elf->ehdr->e_shoff);
  	for (i = 0; i < elf->ehdr->e_shnum; i++) 
    		write_to(fd, &elf->shdr[i], sizeof(Elf64_Shdr));
	close(fd);
	return (0);
}
