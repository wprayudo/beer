#include <msgpuck.h>

#include <bee/beer_proto.h>

struct beer_iheader {
	char header[25];
	char *end;
};

static inline int
encode_header(struct beer_iheader *hdr, uint32_t code, uint64_t sync)
{
	memset(hdr, 0, sizeof(struct beer_iheader));
	char *h = mp_encode_map(hdr->header, 2);
	h = mp_encode_uint(h, BEER_CODE);
	h = mp_encode_uint(h, code);
	h = mp_encode_uint(h, BEER_SYNC);
	h = mp_encode_uint(h, sync);
	hdr->end = h;
	return 0;
}

static inline size_t
mp_sizeof_luint32(uint64_t num) {
	if (num <= UINT32_MAX)
		return 1 + sizeof(uint32_t);
	return 1 + sizeof(uint64_t);
}

static inline char *
mp_encode_luint32(char *data, uint64_t num) {
	if (num <= UINT32_MAX) {
		data = mp_store_u8(data, 0xce);
		return mp_store_u32(data, num);
	}
	data = mp_store_u8(data, 0xcf);
	return mp_store_u64(data, num);
}
