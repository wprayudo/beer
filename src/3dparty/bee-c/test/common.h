#ifndef _TEST_COMMON_H_
#define _TEST_COMMON_H_

struct mp_dump_stack {
	enum mp_type type;
	char start_flag;
	size_t lft;
};

void hex_dump (const char *desc, const char *addr, size_t len);
void hex_dump_c (const char *addr, size_t len);

void mp_dump (const char *addr, size_t len);

int check_bbytes(const char *buf, size_t buf_size,
		 const char *bb, size_t bb_size);

int check_rbytes(struct beer_reply *s,
		 const char *bb, size_t bb_size);
int check_sbytes(struct beer_stream *s,
		 const char *bb, size_t bb_size);
int check_nbytes(struct beer_stream *s,
		 const char *bb, size_t bb_size);
int dump_schema(struct beer_stream *s);

#endif /* _TEST_COMMON_H_ */
