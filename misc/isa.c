#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "isa.h"
#include "bus.h"

/* Are we running in GUI mode? */
extern int gui_mode;

/* Bytes Per Line = Block size of memory */
#define BPL 32

struct {
	char *name;
	int id;
} reg_table[REG_ERR + 1] = { { "%eax", REG_EAX }, { "%ecx", REG_ECX }, { "%edx",
		REG_EDX }, { "%ebx", REG_EBX }, { "%esp", REG_ESP },
		{ "%ebp", REG_EBP }, { "%esi", REG_ESI }, { "%edi", REG_EDI }, { "----",
				REG_ERR }, { "----", REG_ERR }, { "----", REG_ERR }, { "----",
				REG_ERR }, { "----", REG_ERR }, { "----", REG_ERR }, { "----",
				REG_ERR }, { "----", REG_NONE }, { "----", REG_ERR } };

reg_id_t find_register(char *name) {
	int i;
	for (i = 0; i < REG_NONE; i++)
		if (!strcmp(name, reg_table[i].name))
			return reg_table[i].id;
	return REG_ERR;
}

char *reg_name(reg_id_t id) {
	if (id >= 0 && id < REG_NONE)
		return reg_table[id].name;
	else
		return reg_table[REG_NONE].name;
}

/* Is the given register ID a valid program register? */
int reg_valid(reg_id_t id) {
	return id >= 0 && id < REG_NONE && reg_table[id].id == id;
}

instr_t instruction_set[] = { { "nop", HPACK(I_NOP, F_NONE), 1, NO_ARG, 0, 0,
		NO_ARG, 0, 0 }, { "halt", HPACK(I_HALT, F_NONE), 1, NO_ARG, 0, 0,
		NO_ARG, 0, 0 }, { "rrmovl", HPACK(I_RRMOVL, F_NONE), 2, R_ARG, 1, 1,
		R_ARG, 1, 0 },
/* Conditional move instructions are variants of RRMOVL */
{ "cmovle", HPACK(I_RRMOVL, C_LE), 2, R_ARG, 1, 1, R_ARG, 1, 0 }, { "cmovl",
HPACK(I_RRMOVL, C_L), 2, R_ARG, 1, 1, R_ARG, 1, 0 }, { "cmove",
HPACK(I_RRMOVL, C_E), 2, R_ARG, 1, 1, R_ARG, 1, 0 }, { "cmovne",
HPACK(I_RRMOVL, C_NE), 2, R_ARG, 1, 1, R_ARG, 1, 0 }, { "cmovge",
HPACK(I_RRMOVL, C_GE), 2, R_ARG, 1, 1, R_ARG, 1, 0 }, { "cmovg",
HPACK(I_RRMOVL, C_G), 2, R_ARG, 1, 1, R_ARG, 1, 0 },
/* arg1hi indicates number of bytes */
{ "irmovl", HPACK(I_IRMOVL, F_NONE), 6, I_ARG, 2, 4, R_ARG, 1, 0 }, { "rmmovl",
HPACK(I_RMMOVL, F_NONE), 6, R_ARG, 1, 1, M_ARG, 1, 0 }, { "mrmovl",
HPACK(I_MRMOVL, F_NONE), 6, M_ARG, 1, 0, R_ARG, 1, 1 }, { "rmswap",
HPACK(I_RMSWAP, F_NONE), 6, R_ARG, 1, 1, M_ARG, 1, 0 }, { "addl",
HPACK(I_ALU, A_ADD), 2, R_ARG, 1, 1, R_ARG, 1, 0 }, { "subl",
HPACK(I_ALU, A_SUB), 2, R_ARG, 1, 1, R_ARG, 1, 0 }, { "andl",
HPACK(I_ALU, A_AND), 2, R_ARG, 1, 1, R_ARG, 1, 0 }, { "xorl",
HPACK(I_ALU, A_XOR), 2, R_ARG, 1, 1, R_ARG, 1, 0 },
/* arg1hi indicates number of bytes */
{ "jmp", HPACK(I_JMP, C_YES), 5, I_ARG, 1, 4, NO_ARG, 0, 0 }, { "jle",
HPACK(I_JMP, C_LE), 5, I_ARG, 1, 4, NO_ARG, 0, 0 }, { "jl",
HPACK(I_JMP, C_L), 5, I_ARG, 1, 4, NO_ARG, 0, 0 }, { "je",
HPACK(I_JMP, C_E), 5, I_ARG, 1, 4, NO_ARG, 0, 0 }, { "jne",
HPACK(I_JMP, C_NE), 5, I_ARG, 1, 4, NO_ARG, 0, 0 }, { "jge",
HPACK(I_JMP, C_GE), 5, I_ARG, 1, 4, NO_ARG, 0, 0 }, { "jg",
HPACK(I_JMP, C_G), 5, I_ARG, 1, 4, NO_ARG, 0, 0 }, { "call",
HPACK(I_CALL, F_NONE), 5, I_ARG, 1, 4, NO_ARG, 0, 0 }, { "ret",
HPACK(I_RET, F_NONE), 1, NO_ARG, 0, 0, NO_ARG, 0, 0 }, { "pushl",
HPACK(I_PUSHL, F_NONE), 2, R_ARG, 1, 1, NO_ARG, 0, 0 }, { "popl",
HPACK(I_POPL, F_NONE), 2, R_ARG, 1, 1, NO_ARG, 0, 0 }, { "iaddl",
HPACK(I_IADDL, F_NONE), 6, I_ARG, 2, 4, R_ARG, 1, 0 }, { "leave",
HPACK(I_LEAVE, F_NONE), 1, NO_ARG, 0, 0, NO_ARG, 0, 0 },
/* this is just a hack to make the I_POP2 code have an associated name */
{ "pop2", HPACK(I_POP2, F_NONE), 0, NO_ARG, 0, 0, NO_ARG, 0, 0 },

/* For allocation instructions, arg1hi indicates number of bytes */
{ ".byte", 0x00, 1, I_ARG, 0, 1, NO_ARG, 0, 0 }, { ".word", 0x00, 2, I_ARG, 0,
		2, NO_ARG, 0, 0 }, { ".long", 0x00, 4, I_ARG, 0, 4, NO_ARG, 0, 0 }, {
NULL, 0, 0, NO_ARG, 0, 0, NO_ARG, 0, 0 } };

instr_t invalid_instr = { "XXX", 0, 0, NO_ARG, 0, 0, NO_ARG, 0, 0 };

instr_ptr find_instr(char *name) {
	int i;
	for (i = 0; instruction_set[i].name; i++)
		if (strcmp(instruction_set[i].name, name) == 0)
			return &instruction_set[i];
	return NULL;
}

/* Return name of instruction given its encoding */
char *iname(int instr) {
	int i;
	for (i = 0; instruction_set[i].name; i++) {
		if (instr == instruction_set[i].code)
			return instruction_set[i].name;
	}
	return "<bad>";
}

instr_ptr bad_instr() {
	return &invalid_instr;
}

mem_t init_mem(int len, bool_t is_reg) {
	mem_t result = (mem_t) malloc(sizeof(mem_rec));
	if (is_reg) {
		len = ((len + BPL - 1) / BPL) * BPL;
		result->len = len;
		result->contents = (byte_t *) calloc(len, 1);
		result->shared = NULL;
		result->bus = NULL;
		result->cache = NULL;
	} else {
		assert(len==0 || len== MEM_SIZE);
		result->len = len;
		result->contents = (byte_t *) calloc(OWN_MEMORY_SIZE, 1);
		result->shared = get_shared_memory();
		result->bus = get_bus();
		result->cache = new_cache();
	}
	return result;
}

void clear_mem(mem_t m) {
	if (m->cache)
		memset(m->contents, 0, OWN_MEMORY_SIZE);
	else
		memset(m->contents, 0, m->len);
}

void free_mem(mem_t m) {
	free((void *) m->contents);
	free((void *) m);
}

mem_t copy_mem(mem_t oldm) {
	mem_t newm = init_mem(oldm->len, oldm->cache == NULL);
	if (oldm->cache)
		memcpy(newm->contents, oldm->contents, OWN_MEMORY_SIZE);
	else
		memcpy(newm->contents, oldm->contents, oldm->len);
	return newm;
}

bool_t diff_mem(mem_t oldm, mem_t newm, FILE *outfile) {
	word_t pos;
	int len = newm->len;
	bool_t diff = FALSE;
	if (newm->cache)
		len = OWN_MEMORY_SIZE;
	for (pos = 0; (!diff || outfile) && pos < len; pos += 4) {
		word_t ov = 0;
		word_t nv = 0;
		get_word_val(oldm, pos, &ov);
		get_word_val(newm, pos, &nv);
		if (nv != ov) {
			diff = TRUE;
			if (outfile)
				fprintf(outfile, "0x%.4x:\t0x%.8x\t0x%.8x\n", pos, ov, nv);
		}
	}
	return diff;
}

int hex2dig(char c) {
	if (isdigit((int)c))
		return c - '0';
	if (isupper((int)c))
		return c - 'A' + 10;
	else
		return c - 'a' + 10;
}

#define LINELEN 4096
int load_mem(mem_t m, FILE *infile, int report_error) {
	assert(m->shared != NULL); //m should be a normal memory
	/* Read contents of .yo file */
	char buf[LINELEN];
	char c, ch, cl;
	int byte_cnt = 0;
	int lineno = 0;
	word_t bytepos = 0;
	int empty_line = 1;
	int addr = 0;
	char hexcode[15];

#ifdef HAS_GUI
	/* For display */
	int line_no = 0;
	char line[LINELEN];
#endif /* HAS_GUI */   

	int index = 0;

	while (fgets(buf, LINELEN, infile)) {
		int cpos = 0;
		empty_line = 1;
		lineno++;
		/* Skip white space */
		while (isspace((int)buf[cpos]))
			cpos++;

		if (buf[cpos] != '0' || (buf[cpos + 1] != 'x' && buf[cpos + 1] != 'X'))
			continue; /* Skip this line */
		cpos += 2;

		/* Get address */
		bytepos = 0;
		while (isxdigit((int)(c=buf[cpos]))) {
			cpos++;
			bytepos = bytepos * 16 + hex2dig(c);
		}

		while (isspace((int)buf[cpos]))
			cpos++;

		if (buf[cpos++] != ':') {
			if (report_error) {
				fprintf(stderr, "Error reading file. Expected colon\n");
				fprintf(stderr, "Line %d:%s\n", lineno, buf);
				fprintf(stderr, "Reading '%c' at position %d\n", buf[cpos],
						cpos);
			}
			return 0;
		}

		addr = bytepos;

		while (isspace((int)buf[cpos]))
			cpos++;

		index = 0;

		/* Get code */
		while (isxdigit((int)(ch=buf[cpos++])) &&
		isxdigit((int)(cl=buf[cpos++]))) {
			byte_t byte = 0;
			if (bytepos >= OWN_MEMORY_SIZE) {
				// it should only be loaded to my own memory space
				if (report_error) {
					fprintf(stderr,
							"Error reading file. Invalid address. 0x%x\n",
							bytepos);
					fprintf(stderr, "Line %d:%s\n", lineno, buf);
				}
				return 0;
			}
			byte = hex2dig(ch) * 16 + hex2dig(cl);
			m->contents[bytepos++] = byte;
			byte_cnt++;
			empty_line = 0;
			hexcode[index++] = ch;
			hexcode[index++] = cl;
		}
		/* Fill rest of hexcode with blanks */
		for (; index < 12; index++)
			hexcode[index] = ' ';
		hexcode[index] = '\0';

#ifdef HAS_GUI
		if (gui_mode) {
			/* Now get the rest of the line */
			while (isspace((int)buf[cpos]))
			cpos++;
			cpos++; /* Skip over '|' */

			index = 0;
			while ((c = buf[cpos++]) != '\0' && c != '\n') {
				line[index++] = c;
			}
			line[index] = '\0';
			if (!empty_line)
			report_line(line_no++, addr, hexcode, line);
		}
#endif /* HAS_GUI */
	}
	return byte_cnt;
}

cache_blk_t find_cache_blk(cache_t cache, word_t addr) {
	//find the corresponding cache block
	int tag = GET_ADDR_TAG(addr);
	int set = GET_ADDR_SET(addr);
	int i;
	for (i = 0; i < NUM_WAYS; ++i) {
		cache_blk_t blk = cache->blks[set] + i;
		if (blk->tag == tag && blk->valid == TRUE)
			return blk;
	}
	return NULL;
}

cache_blk_t load_cache(mem_t mem, word_t addr) {
	//load the block from memory to cache
	cache_t cache = mem->cache;

	//broadcast it so we can get the correct value
	broadcast(mem, 'R', addr);

	//now, need to pick a block to replace
	int set = GET_ADDR_SET(addr);
	int tag = GET_ADDR_TAG(addr);
	word_t block_start = addr - GET_ADDR_BLOCK(addr);

	cache_blk_t rep = NULL;

	//if there is a invalid block, pick it

	int i;
	for (i = 0; i < NUM_WAYS; ++i) {
		cache_blk_t blk = cache->blks[set] + i;
		if (!blk->valid)
			rep = blk;
	}

	//otherwise pick a random one
	if (rep == NULL) {
		rep = cache->blks[set] + (rand() % NUM_WAYS);
		commit_cache(mem, rep, BLOCK_START(rep->tag,set));
	}
	rep->tag = tag;
	rep->valid = TRUE;
	rep->dirty = FALSE;

	if (addr < OWN_MEMORY_SIZE) {
		memcpy(rep->contents, mem->contents + block_start,
				sizeof(byte_t) * BLOCK_SZIE);
	} else {
		memcpy(rep->contents, mem->shared + block_start - OWN_MEMORY_SIZE,
				sizeof(byte_t) * BLOCK_SZIE);
	}

	return rep;
}

void commit_cache(mem_t mem, cache_blk_t blk, word_t addr) {
	//commit the cache into memory at addr
	word_t block_start = addr - GET_ADDR_BLOCK(addr);
	if (addr < OWN_MEMORY_SIZE)
		memcpy(mem->contents + block_start, blk->contents,
				sizeof(byte_t) * BLOCK_SZIE);
	else
		memcpy(mem->shared + block_start - OWN_MEMORY_SIZE, blk->contents,
				sizeof(byte_t) * BLOCK_SZIE);
	blk->dirty = FALSE;
}

void enter_bus(mem_t mem) {

	int* bus = mem->bus;
//	acquire_lock(bus_lock, mem);

	assert(bus[BUS_TYPE] == 0);
	my_id = bus[BUS_CLIENT_COUNT]++;
	assert(bus[BUS_CLIENT_COUNT] <= 2);

//	release_lock(bus_lock);
//	fprintf(stderr, "HI~\n");
}

void leave_bus(mem_t mem) {
	int* bus = mem->bus;
	acquire_lock(bus_lock, mem);

	//bye bye~
	//commit everything and go away~
	int i, j;
	for (i = 0; i < (1 << SET_LENGTH); ++i) {
		for (j = 0; j < NUM_WAYS; ++j) {
			cache_blk_t blk = mem->cache->blks[i] + j;
			if (blk->valid && blk->dirty)
				commit_cache(mem, blk, BLOCK_START(blk->tag,i));
		}
	}

	assert(bus[BUS_TYPE] == 0);
	bus[BUS_EXITED] |= 1 << my_id;

	release_lock(bus_lock);
}

void broadcast(mem_t mem, int type, int addr) {
//my own memory need not be broadcasted
	if (addr < OWN_MEMORY_SIZE)
		return;
	int*bus = mem->bus;

	if (bus[BUS_CLIENT_COUNT] == 1) {
		return;
	}

	int other_id = 1 - my_id;
	if (bus[BUS_CLIENT_COUNT] == 2 && (bus[BUS_EXITED] >> other_id & 1)) {
		return;
	}

	fprintf(stderr, "Boradcast! %d %c %d\n", my_id, (char) type, addr);

	//There should be no message left here
	assert(bus[BUS_TYPE] == 0);

	bus[BUS_ID] = my_id;
	bus[BUS_TYPE] = type;
	bus[BUS_ADDR] = addr;
//waiting on this broadcast to be finished
	sem_wait(broadcast_finished);
}

void response(mem_t mem) {
	int* bus = mem->bus;
	if (bus[BUS_TYPE] == 0) {
		return;
	}

	word_t id = bus[BUS_ID];
	word_t type = bus[BUS_TYPE];
	word_t addr = bus[BUS_ADDR];

	if (type == 0 || my_id == id) {
		return;
	}

	cache_blk_t blk = find_cache_blk(mem->cache, addr);
	if (blk == NULL) {
		clear_bus(bus);
		sem_post(broadcast_finished);
		return;
	}

	if (type == 'R') {
		if (blk->dirty) {
			commit_cache(mem, blk, addr);
		}
	}

	if (type == 'W') {
		blk->valid = FALSE;
	}

	clear_bus(bus);
	sem_post(broadcast_finished);
	return;
}

bool_t get_byte_val(mem_t m, word_t pos, byte_t *dest) {
	if (pos < 0 || pos >= m->len)
		return FALSE;

	if (m->cache == NULL) {
		*dest = m->contents[pos];
		return TRUE;
	}

	bool_t need_lock = !is_my_lock(bus_lock);
	if (need_lock) {
		acquire_lock(bus_lock, m);
	}

	cache_blk_t blk = find_cache_blk(m->cache, pos);
	if (blk == NULL) {
		blk = load_cache(m, pos);
	}

	word_t offset = GET_ADDR_BLOCK(pos);
	*dest = blk->contents[offset];
	if (need_lock) {
		release_lock(bus_lock);
	}
	return TRUE;
}

bool_t get_word_val(mem_t m, word_t pos, word_t *dest) {
//	fprintf(stdout, "reading: %d\n", pos);
	int i;
	word_t val;
	if (pos < 0 || pos + 4 > m->len)
		return FALSE;
	if (m->cache == NULL) {
		*dest = *(int*) (m->contents + pos);
		return TRUE;
	}
	bool_t need_lock = !is_my_lock(bus_lock);
	if (need_lock) {
		acquire_lock(bus_lock, m);
	}
	val = 0;
	for (i = 0; i < 4; i++) {
		byte_t b;
		if (!get_byte_val(m, pos + i, &b)) {
			if (need_lock) {
				release_lock(bus_lock);
			}
			return FALSE;
		}
		val = val | b << (8 * i);
	}
	*dest = val;
	if (need_lock) {
		release_lock(bus_lock);
	}
	return TRUE;
}

bool_t set_byte_val(mem_t m, word_t pos, byte_t val) {
	if (pos < 0 || pos >= m->len)
		return FALSE;

	if (!m->cache) {
		m->contents[pos] = val;
		return TRUE;
	}

	bool_t need_lock = !is_my_lock(bus_lock);
	if (need_lock) {
		acquire_lock(bus_lock, m);
	}
	cache_blk_t blk = find_cache_blk(m->cache, pos);
	if (blk == NULL) {
		blk = load_cache(m, pos);
	}

	broadcast(m, 'W', pos);
	blk->contents[GET_ADDR_BLOCK(pos)] = val;
	blk->dirty = TRUE;
	if (need_lock) {
		release_lock(bus_lock);
	}
	return TRUE;
}

bool_t set_word_val(mem_t m, word_t pos, word_t val) {
	int i;
	if (pos < 0 || pos + 4 > m->len)
		return FALSE;
	if (!m->cache) {
		*(int*) (m->contents + pos) = val;
		return TRUE;
	}
	bool_t need_lock = !is_my_lock(bus_lock);
	if (need_lock) {
		acquire_lock(bus_lock, m);
	}
	for (i = 0; i < 4; i++) {
		if (!set_byte_val(m, pos + i, val & 0xFF)) {
			if (need_lock) {
				release_lock(bus_lock);
			}
			return FALSE;
		}
		val >>= 8;
	}
	if (need_lock) {
		release_lock(bus_lock);
	}
	return TRUE;
}

bool_t get_and_set_byte_val(mem_t m, word_t pos, byte_t*dest, byte_t val) {
	if (pos < 0 || pos + 4 > m->len)
		return FALSE;
	if (!m->cache) {
		*dest = m->contents[pos];
		m->contents[pos] = val;
		return TRUE;
	}
	bool_t need_lock = !is_my_lock(bus_lock);
	if (need_lock) {
		acquire_lock(bus_lock, m);
	}
	bool_t ok = FALSE;
	if (get_byte_val(m, pos, dest)) {
		if (set_byte_val(m, pos, val)) {
			ok = TRUE;
		}
	}
	if (need_lock) {
		release_lock(bus_lock);
	}
	return ok;
}

bool_t get_and_set_word_val(mem_t m, word_t pos, word_t*dest, word_t val) {
	if (pos < 0 || pos + 4 > m->len)
		return FALSE;
	if (!m->cache) {
		*dest = *(int*) (m->contents + pos);
		*(int*) (m->contents + pos) = val;
		return TRUE;
	}
	bool_t need_lock = !is_my_lock(bus_lock);
	if (need_lock) {
		acquire_lock(bus_lock, m);
	}
	bool_t ok = FALSE;
	if (get_word_val(m, pos, dest) && set_word_val(m, pos, val)) {
		ok = TRUE;
	}
	if (need_lock) {
		release_lock(bus_lock);
	}
	return ok;
}

void dump_memory(FILE *outfile, mem_t m, word_t pos, int len) {
	int i, j;
	while (pos % BPL) {
		pos--;
		len++;
	}

	len = ((len + BPL - 1) / BPL) * BPL;

	if (pos + len > m->len)
		len = m->len - pos;

	for (i = 0; i < len; i += BPL) {
		word_t val = 0;
		fprintf(outfile, "0x%.4x:", pos + i);
		for (j = 0; j < BPL; j += 4) {
			get_word_val(m, pos + i + j, &val);
			fprintf(outfile, " %.8x", val);
		}
	}
}

mem_t init_reg() {
	return init_mem(32, 1);
}

void free_reg(mem_t r) {
	free_mem(r);
}

mem_t copy_reg(mem_t oldr) {
	return copy_mem(oldr);
}

bool_t diff_reg(mem_t oldr, mem_t newr, FILE *outfile) {
	word_t pos;
	int len = oldr->len;
	bool_t diff = FALSE;
	if (newr->len < len)
		len = newr->len;
	for (pos = 0; (!diff || outfile) && pos < len; pos += 4) {
		word_t ov = 0;
		word_t nv = 0;
		get_word_val(oldr, pos, &ov);
		get_word_val(newr, pos, &nv);
		if (nv != ov) {
			diff = TRUE;
			if (outfile)
				fprintf(outfile, "%s:\t0x%.8x\t0x%.8x\n",
						reg_table[pos / 4].name, ov, nv);
		}
	}
	return diff;
}

word_t get_reg_val(mem_t r, reg_id_t id) {
	word_t val = 0;
	if (id >= REG_NONE)
		return 0;
	get_word_val(r, id * 4, &val);
	return val;
}

void set_reg_val(mem_t r, reg_id_t id, word_t val) {
	if (id < REG_NONE) {
		set_word_val(r, id * 4, val);
#ifdef HAS_GUI
		if (gui_mode) {
			signal_register_update(id, val);
		}
#endif /* HAS_GUI */
	}
}

void dump_reg(FILE *outfile, mem_t r) {
	reg_id_t id;
	for (id = 0; reg_valid(id); id++) {
		fprintf(outfile, "   %s  ", reg_table[id].name);
	}
	fprintf(outfile, "\n");
	for (id = 0; reg_valid(id); id++) {
		word_t val = 0;
		get_word_val(r, id * 4, &val);
		fprintf(outfile, " %x", val);
	}
	fprintf(outfile, "\n");
}

struct {
	char symbol;
	int id;
} alu_table[A_NONE + 1] = { { '+', A_ADD }, { '-', A_SUB }, { '&', A_AND }, {
		'^', A_XOR }, { '?', A_NONE } };

char op_name(alu_t op) {
	if (op < A_NONE)
		return alu_table[op].symbol;
	else
		return alu_table[A_NONE].symbol;
}

word_t compute_alu(alu_t op, word_t argA, word_t argB) {
	word_t val;
	switch (op) {
	case A_ADD:
		val = argA + argB;
		break;
	case A_SUB:
		val = argB - argA;
		break;
	case A_AND:
		val = argA & argB;
		break;
	case A_XOR:
		val = argA ^ argB;
		break;
	default:
		val = 0;
	}
	return val;
}

cc_t compute_cc(alu_t op, word_t argA, word_t argB) {
	word_t val = compute_alu(op, argA, argB);
	bool_t zero = (val == 0);
	bool_t sign = ((int) val < 0);
	bool_t ovf;
	switch (op) {
	case A_ADD:
		ovf = (((int) argA < 0) == ((int) argB < 0))
				&& (((int) val < 0) != ((int) argA < 0));
		break;
	case A_SUB:
		ovf = (((int) argA > 0) == ((int) argB < 0))
				&& (((int) val < 0) != ((int) argB < 0));
		break;
	case A_AND:
	case A_XOR:
		ovf = FALSE;
		break;
	default:
		ovf = FALSE;
	}
	return PACK_CC(zero,sign,ovf);

}

char *cc_names[8] = { "Z=0 S=0 O=0", "Z=0 S=0 O=1", "Z=0 S=1 O=0",
		"Z=0 S=1 O=1", "Z=1 S=0 O=0", "Z=1 S=0 O=1", "Z=1 S=1 O=0",
		"Z=1 S=1 O=1" };

char *cc_name(cc_t c) {
	int ci = c;
	if (ci < 0 || ci > 7)
		return "???????????";
	else
		return cc_names[c];
}

/* Status types */

char *stat_names[] = { "BUB", "AOK", "HLT", "ADR", "INS", "PIP" };

char *stat_name(stat_t e) {
	if (e < 0 || e > STAT_PIP)
		return "Invalid Status";
	return stat_names[e];
}

/**************** Implementation of ISA model ************************/

state_ptr new_state(int memlen) {
	state_ptr result = (state_ptr) malloc(sizeof(state_rec));
	result->pc = 0;
	result->r = init_reg();
	result->m = init_mem(memlen, 0);
	result->cc = DEFAULT_CC;
	return result;
}

void free_state(state_ptr s) {
	free_reg(s->r);
	free_mem(s->m);
	free((void *) s);
}

state_ptr copy_state(state_ptr s) {
	state_ptr result = (state_ptr) malloc(sizeof(state_rec));
	result->pc = s->pc;
	result->r = copy_reg(s->r);
	result->m = copy_mem(s->m);
	result->cc = s->cc;
	return result;
}

bool_t diff_state(state_ptr olds, state_ptr news, FILE *outfile) {
	bool_t diff = FALSE;

	if (olds->pc != news->pc) {
		diff = TRUE;
		if (outfile) {
			fprintf(outfile, "pc:\t0x%.8x\t0x%.8x\n", olds->pc, news->pc);
		}
	}
	if (olds->cc != news->cc) {
		diff = TRUE;
		if (outfile) {
			fprintf(outfile, "cc:\t%s\t%s\n", cc_name(olds->cc),
					cc_name(news->cc));
		}
	}
	if (diff_reg(olds->r, news->r, outfile))
		diff = TRUE;
	if (diff_mem(olds->m, news->m, outfile))
		diff = TRUE;
	return diff;
}

/* Branch logic */
bool_t cond_holds(cc_t cc, cond_t bcond) {
	bool_t zf = GET_ZF(cc);
	bool_t sf = GET_SF(cc);
	bool_t of = GET_OF(cc);
	bool_t jump = FALSE;

	switch (bcond) {
	case C_YES:
		jump = TRUE;
		break;
	case C_LE:
		jump = (sf ^ of) | zf;
		break;
	case C_L:
		jump = sf ^ of;
		break;
	case C_E:
		jump = zf;
		break;
	case C_NE:
		jump = zf ^ 1;
		break;
	case C_GE:
		jump = sf ^ of ^ 1;
		break;
	case C_G:
		jump = (sf ^ of ^ 1) & (zf ^ 1);
		break;
	default:
		jump = FALSE;
		break;
	}
	return jump;
}

/* Execute single instruction.  Return status. */
stat_t step_state(state_ptr s, FILE *error_file) {
	word_t argA, argB;
	byte_t byte0 = 0;
	byte_t byte1 = 0;
	itype_t hi0;
	alu_t lo0;
	reg_id_t hi1 = REG_NONE;
	reg_id_t lo1 = REG_NONE;
	bool_t ok1 = TRUE;
	word_t cval = 0;
	word_t okc = TRUE;
	word_t val, dval;
	bool_t need_regids;
	bool_t need_imm;
	word_t ftpc = s->pc; /* Fall-through PC */

	if (!get_byte_val(s->m, ftpc, &byte0)) {
		if (error_file)
			fprintf(error_file, "PC = 0x%x, Invalid instruction address\n",
					s->pc);
		return STAT_ADR;
	}
	ftpc++;

	hi0 = HI4(byte0);
	lo0 = LO4(byte0);

	need_regids = (hi0 == I_RRMOVL || hi0 == I_ALU || hi0 == I_PUSHL
			|| hi0 == I_POPL || hi0 == I_IRMOVL || hi0 == I_RMMOVL
			|| hi0 == I_MRMOVL || hi0 == I_IADDL || hi0 == I_RMSWAP);

	if (need_regids) {
		ok1 = get_byte_val(s->m, ftpc, &byte1);
		ftpc++;
		hi1 = HI4(byte1);
		lo1 = LO4(byte1);
	}

	need_imm = (hi0 == I_IRMOVL || hi0 == I_RMMOVL || hi0 == I_MRMOVL
			|| hi0 == I_JMP || hi0 == I_CALL || hi0 == I_IADDL
			|| hi0 == I_RMSWAP);

	if (need_imm) {
		okc = get_word_val(s->m, ftpc, &cval);
		ftpc += 4;
	}

	switch (hi0) {
	case I_NOP:
		s->pc = ftpc;
		break;
	case I_HALT:
		return STAT_HLT;
		break;
	case I_RRMOVL: /* Both unconditional and conditional moves */
		if (!ok1) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid instruction address\n",
						s->pc);
			return STAT_ADR;
		}
		if (!reg_valid(hi1)) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid register ID 0x%.1x\n",
						s->pc, hi1);
			return STAT_INS;
		}
		if (!reg_valid(lo1)) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid register ID 0x%.1x\n",
						s->pc, lo1);
			return STAT_INS;
		}
		val = get_reg_val(s->r, hi1);
		if (cond_holds(s->cc, lo0))
			set_reg_val(s->r, lo1, val);
		s->pc = ftpc;
		break;
	case I_IRMOVL:
		if (!ok1) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid instruction address\n",
						s->pc);
			return STAT_ADR;
		}
		if (!okc) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid instruction address",
						s->pc);
			return STAT_INS;
		}
		if (!reg_valid(lo1)) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid register ID 0x%.1x\n",
						s->pc, lo1);
			return STAT_INS;
		}
		set_reg_val(s->r, lo1, cval);
		s->pc = ftpc;
		break;
	case I_RMMOVL:
		if (!ok1) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid instruction address\n",
						s->pc);
			return STAT_ADR;
		}
		if (!okc) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid instruction address\n",
						s->pc);
			return STAT_INS;
		}
		if (!reg_valid(hi1)) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid register ID 0x%.1x\n",
						s->pc, hi1);
			return STAT_INS;
		}
		if (reg_valid(lo1))
			cval += get_reg_val(s->r, lo1);
		val = get_reg_val(s->r, hi1);
		if (!set_word_val(s->m, cval, val)) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid data address 0x%x\n",
						s->pc, cval);
			return STAT_ADR;
		}
		s->pc = ftpc;
		break;
	case I_MRMOVL:
		if (!ok1) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid instruction address\n",
						s->pc);
			return STAT_ADR;
		}
		if (!okc) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid instruction addres\n",
						s->pc);
			return STAT_INS;
		}
		if (!reg_valid(hi1)) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid register ID 0x%.1x\n",
						s->pc, hi1);
			return STAT_INS;
		}
		if (reg_valid(lo1))
			cval += get_reg_val(s->r, lo1);
		if (!get_word_val(s->m, cval, &val))
			return STAT_ADR;
		set_reg_val(s->r, hi1, val);
		s->pc = ftpc;
		break;
	case I_ALU:
		if (!ok1) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid instruction address\n",
						s->pc);
			return STAT_ADR;
		}
		argA = get_reg_val(s->r, hi1);
		argB = get_reg_val(s->r, lo1);
		val = compute_alu(lo0, argA, argB);
		set_reg_val(s->r, lo1, val);
		s->cc = compute_cc(lo0, argA, argB);
		s->pc = ftpc;
		break;
	case I_JMP:
		if (!ok1) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid instruction address\n",
						s->pc);
			return STAT_ADR;
		}
		if (!okc) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid instruction address\n",
						s->pc);
			return STAT_ADR;
		}
		if (cond_holds(s->cc, lo0))
			s->pc = cval;
		else
			s->pc = ftpc;
		break;
	case I_CALL:
		if (!ok1) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid instruction address\n",
						s->pc);
			return STAT_ADR;
		}
		if (!okc) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid instruction address\n",
						s->pc);
			return STAT_ADR;
		}
		val = get_reg_val(s->r, REG_ESP) - 4;
		set_reg_val(s->r, REG_ESP, val);
		if (!set_word_val(s->m, val, ftpc)) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid stack address 0x%x\n",
						s->pc, val);
			return STAT_ADR;
		}
		s->pc = cval;
		break;
	case I_RET:
		/* Return Instruction.  Pop address from stack */
		dval = get_reg_val(s->r, REG_ESP);
		if (!get_word_val(s->m, dval, &val)) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid stack address 0x%x\n",
						s->pc, dval);
			return STAT_ADR;
		}
		set_reg_val(s->r, REG_ESP, dval + 4);
		s->pc = val;
		break;
	case I_PUSHL:
		if (!ok1) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid instruction address\n",
						s->pc);
			return STAT_ADR;
		}
		if (!reg_valid(hi1)) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid register ID 0x%.1x\n",
						s->pc, hi1);
			return STAT_INS;
		}
		val = get_reg_val(s->r, hi1);
		dval = get_reg_val(s->r, REG_ESP) - 4;
		set_reg_val(s->r, REG_ESP, dval);
		if (!set_word_val(s->m, dval, val)) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid stack address 0x%x\n",
						s->pc, dval);
			return STAT_ADR;
		}
		s->pc = ftpc;
		break;
	case I_POPL:
		if (!ok1) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid instruction address\n",
						s->pc);
			return STAT_ADR;
		}
		if (!reg_valid(hi1)) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid register ID 0x%.1x\n",
						s->pc, hi1);
			return STAT_INS;
		}
		dval = get_reg_val(s->r, REG_ESP);
		set_reg_val(s->r, REG_ESP, dval + 4);
		if (!get_word_val(s->m, dval, &val)) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid stack address 0x%x\n",
						s->pc, dval);
			return STAT_ADR;
		}
		set_reg_val(s->r, hi1, val);
		s->pc = ftpc;
		break;
	case I_LEAVE:
		dval = get_reg_val(s->r, REG_EBP);
		set_reg_val(s->r, REG_ESP, dval + 4);
		if (!get_word_val(s->m, dval, &val)) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid stack address 0x%x\n",
						s->pc, dval);
			return STAT_ADR;
		}
		set_reg_val(s->r, REG_EBP, val);
		s->pc = ftpc;
		break;
	case I_IADDL:
		if (!ok1) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid instruction address\n",
						s->pc);
			return STAT_ADR;
		}
		if (!okc) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid instruction address",
						s->pc);
			return STAT_INS;
		}
		if (!reg_valid(lo1)) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid register ID 0x%.1x\n",
						s->pc, lo1);
			return STAT_INS;
		}
		argB = get_reg_val(s->r, lo1);
		val = argB + cval;
		set_reg_val(s->r, lo1, val);
		s->cc = compute_cc(A_ADD, cval, argB);
		s->pc = ftpc;
		break;
	case I_RMSWAP:
		if (!ok1) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid instruction address\n",
						s->pc);
			return STAT_ADR;
		}
		if (!okc) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid instruction address\n",
						s->pc);
			return STAT_INS;
		}
		if (!reg_valid(hi1)) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid register ID 0x%.1x\n",
						s->pc, hi1);
			return STAT_INS;
		}
		if (reg_valid(lo1))
			cval += get_reg_val(s->r, lo1);
		if (!get_word_val(s->m, cval, &val))
			return STAT_ADR;
		if (!set_word_val(s->m, cval, 1)) {
			if (error_file)
				fprintf(error_file, "PC = 0x%x, Invalid data address 0x%x\n",
						s->pc, cval);
			return STAT_ADR;
		}
		set_reg_val(s->r, hi1, val);
		s->pc = ftpc;
		break;
	default:
		if (error_file)
			fprintf(error_file, "PC = 0x%x, Invalid instruction %.2x\n", s->pc,
					byte0);
		return STAT_INS;
	}
	return STAT_AOK;
}
