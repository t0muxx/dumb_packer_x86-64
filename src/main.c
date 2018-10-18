/**
 * File              : main.c
 * Author            : tmaraval <no-mail@mail.com>
 * Date              : 06.10.2018
 * Last Modified Date: 06.10.2018
 * Last Modified By  : tmaraval <no-mail@mail.com>
 */

#include "../inc/packer.h"

int	pack_it(char *input_file, char *output_file, char *payload_file)
{
	t_elf	*elf;
	void 	*payload;
	size_t  payload_size;
	int     t_index;

	#if DEBUG == 1
		printf("Packing |%s|\n", input_file);
	#endif
	elf = read_elf(input_file);
	payload = loader_get(payload_file, &payload_size);
	add_section(elf, payload, payload_size);
	if (!(strcmp(payload_file, "uncypher")))
	{
		t_index = utils_get_sect_nbr(elf, ".text");
		cypher_text(elf->segment_data, t_index, elf->shdr[t_index].sh_size);
	}
	write_bin(output_file, elf);	
	return (0);
}

int	main(int argc, char **argv)
{
	if (argc != 4)
	{
		printf("Usage : ./dumb_packer input_bin output_bin\n");
		return (0);	
	}
	pack_it(argv[1], argv[2], argv[3]);
	return (0);
}
