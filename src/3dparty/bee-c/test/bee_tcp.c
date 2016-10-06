#include "test.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <msgpuck.h>

#include <bee/bee.h>

#include <bee/beer_net.h>
#include <bee/beer_opt.h>

#include "common.h"

#define header() note("*** %s: prep ***", __func__)
#define footer() note("*** %s: done ***", __func__)

int
beer_request_set_sspace(struct beer_stream *s, struct beer_request *req,
		       const char *space, uint32_t slen)
{
	int32_t sno = beer_get_spaceno(s, space, slen);
	if (sno == -1) return -1;
	return beer_request_set_space(req, sno);
}

int
beer_request_set_sspacez(struct beer_stream *s, struct beer_request *req,
			const char *space)
{
	return beer_request_set_sspace(s, req, space, strlen(space));
}

int
beer_request_set_sindex(struct beer_stream *s, struct beer_request *req,
		       const char *index, uint32_t ilen)
{
	int32_t ino = beer_get_indexno(s, req->space_id, index, ilen);
	if (ino == -1) return -1;
	return beer_request_set_index(req, ino);
}

int
beer_request_set_sindexz(struct beer_stream *s, struct beer_request *req,
			const char *index)
{
	return beer_request_set_sindex(s, req, index, strlen(index));
}

/*
static int
test_() {
	plan(0);
	header();

	footer();
	return check_plan();
}
*/

static int
test_connect_tcp() {
	plan(7);
	header();

	char uri[128] = {0};
	snprintf(uri, 128, "%s%s%s", "test:test@", "localhost:",
		 getenv("PRIMARY_PORT"));

	struct beer_stream *s = NULL; s = beer_net(NULL);
	isnt(s, NULL, "Checking that stream is allocated");
	is  (s->alloc, 1, "Checking s->alloc");
	isnt(beer_set(s, BEER_OPT_URI, uri), -1, "Setting URI");
	isnt(beer_connect(s), -1, "Connecting");
//	isnt(beer_authenticate(s), -1, "Authenticating");
	beer_close(s);
	beer_stream_free(s);

	struct beer_stream sa = {0}; beer_net(&sa);
	is  (sa.alloc, 0, "Checking sa.alloc");
	isnt(beer_set(&sa, BEER_OPT_URI, uri), -1, "Setting URI");
	isnt(beer_connect(&sa), -1, "Connecting");
//	isnt(beer_authenticate(&sa), -1, "Authenticating");
	beer_close(&sa);
	beer_stream_free(&sa);

	footer();
	return check_plan();
}

static int
test_object() {
	plan(109);
	header();

	struct beer_stream *s = NULL; s = beer_object(NULL);

	const char bb1[] = "\xcd\x04\xbb\xd1\xfb\x45\xc0\xbc\x49\x27\x6d\x20\x74\x6f"
		  "\x74\x61\x6c\x79\x20\x64\x75\x63\x6b\x2c\x20\x69\x20\x63\x61\x6e"
		  "\x20\x71\x75\x61\x63\x6b\xbc\x49\x27\x6d\x20\x74\x6f\x74\x61\x6c"
		  "\x79\x20\x64\x75\x63\x6b\x2c\x20\x69\x20\x63\x61\x6e\x20\x71\x75"
		  "\x61\x63\x6b";
	size_t bb1_len = sizeof(bb1) - 1;

	char str1[] = "I'm totaly duck, i can quack";
	ssize_t str1_len = strlen(str1);

	isnt(s, NULL, "Checking that object is allocated");
	is  (s->alloc, 1, "Checking s->alloc");
	is  (beer_object_add_int(s, 1211),  3, "encoding int > 0");
	is  (beer_object_add_int(s, -1211), 3, "encoding int < 0");
	is  (beer_object_add_nil(s), 1, "encoding nil");
	is  (beer_object_add_str(s, str1, str1_len), str1_len + 1, "encoding str");
	is  (beer_object_add_strz(s, str1), str1_len + 1, "encoding strz");
	is  (check_sbytes(s, bb1, bb1_len), 0, "Check bytestring");

	is  (beer_object_type(s, BEER_SBO_SPARSE), -1, "Check type set (must fail)");

	const char bb2[] = "\xdd\x00\x00\x00\x04\xc0\xc0\xc0\xc0";
	size_t bb2_len = sizeof(bb2) - 1;

	is  (beer_object_reset(s), 0, "Reset bytestring");
	is  (beer_object_type(s, BEER_SBO_SPARSE), 0, "Check type set (must be ok)");
	is  (beer_object_add_array(s, 0), 5, "Sparse array");
	is  (beer_object_add_nil(s), 1, "encoding nil");
	is  (beer_object_add_nil(s), 1, "encoding nil");
	is  (beer_object_add_nil(s), 1, "encoding nil");
	is  (beer_object_add_nil(s), 1, "encoding nil");
	is  (beer_object_container_close(s), 0, "Closing container");
	is  (beer_object_container_close(s), -1, "Erroneous container close");
	is  (check_sbytes(s, bb2, bb2_len), 0, "Check bytestring");

	const char bb3[] = "\x94\xc0\xc0\xc0\xc0";
	size_t bb3_len = sizeof(bb3) - 1;

	is  (beer_object_reset(s), 0, "Reset bytestring");
	is  (beer_object_type(s, BEER_SBO_PACKED), 0, "Check type set (must be ok)");
	is  (beer_object_add_array(s, 0), 1, "Packed array");
	is  (beer_object_add_nil(s), 1, "encoding nil");
	is  (beer_object_add_nil(s), 1, "encoding nil");
	is  (beer_object_add_nil(s), 1, "encoding nil");
	is  (beer_object_add_nil(s), 1, "encoding nil");
	is  (beer_object_container_close(s), 0, "Closing container");
	is  (beer_object_container_close(s), -1, "Erroneous container close");
	is  (check_sbytes(s, bb3, bb3_len), 0, "Check bytestring");

	/*
	 * {"menu": {
  	 *    "popup": {
    	 *      "menuitem": [
      	 *        {"value": "New", "onclick": "CreateNewDoc()"},
      	 *        {"value": "Open", "onclick": "OpenDoc()"},
      	 *        {"value": "Close", "onclick": "CloseDoc()"}
    	 *      ]
  	 *    }
	 *    "id": "file",
  	 *    "value": "File",
	 * }}
	 */
	const char bb4[] = "\x81\xa4\x6d\x65\x6e\x75\x83\xa5\x70\x6f\x70\x75\x70\x81"
			   "\xa8\x6d\x65\x6e\x75\x69\x74\x65\x6d\x93\x82\xa7\x6f\x6e"
			   "\x63\x6c\x69\x63\x6b\xae\x43\x72\x65\x61\x74\x65\x4e\x65"
			   "\x77\x44\x6f\x63\x28\x29\xa5\x76\x61\x6c\x75\x65\xa3\x4e"
			   "\x65\x77\x82\xa7\x6f\x6e\x63\x6c\x69\x63\x6b\xa9\x4f\x70"
			   "\x65\x6e\x44\x6f\x63\x28\x29\xa5\x76\x61\x6c\x75\x65\xa4"
			   "\x4f\x70\x65\x6e\x82\xa7\x6f\x6e\x63\x6c\x69\x63\x6b\xaa"
			   "\x43\x6c\x6f\x73\x65\x44\x6f\x63\x28\x29\xa5\x76\x61\x6c"
			   "\x75\x65\xa5\x43\x6c\x6f\x73\x65\xa2\x69\x64\xa4\x66\x69"
			   "\x6c\x65\xa5\x76\x61\x6c\x75\x65\xa4\x46\x69\x6c\x65";

	size_t bb4_len = sizeof(bb4) - 1;

	is  (beer_object_reset(s), 0, "Reset bytestring");
	is  (beer_object_type(s, BEER_SBO_PACKED), 0, "Check type set (must be ok)");
	is  (beer_object_add_map(s, 0), 1,			"Packing map-1");
	is  (beer_object_add_strz(s, "menu"), 5,			"  Packing key (str)");
	is  (beer_object_add_map(s, 0), 1,			"   Packing value (map-2)");
	is  (beer_object_add_strz(s, "popup"), 6,		"  Packing key (str)");
	is  (beer_object_add_map(s, 0), 1,			"   Packing value (map-3)");
	is  (beer_object_add_strz(s, "menuitem"), 9,		"     Packing key (str)");
	is  (beer_object_add_array(s, 0), 1,			"      Packing value (array)");
	is  (beer_object_add_map(s, 0), 1,			"        Packing value (map-4)");
	is  (beer_object_add_strz(s, "onclick"), 8,		"          Packing key (str)");
	is  (beer_object_add_strz(s, "CreateNewDoc()"), 15,	"           Packing value (str)");
	is  (beer_object_add_strz(s, "value"), 6,		"          Packing key (str)");
	is  (beer_object_add_strz(s, "New"), 4,			"           Packing value (str)");
	is  (beer_object_container_close(s), 0,			"        Closing map-4");
	is  (beer_object_add_map(s, 0), 1,			"        Packing value (map-5)");
	is  (beer_object_add_strz(s, "onclick"), 8,		"          Packing key (str)");
	is  (beer_object_add_strz(s, "OpenDoc()"), 10,		"           Packing value (str)");
	is  (beer_object_add_strz(s, "value"), 6,		"          Packing key (str)");
	is  (beer_object_add_strz(s, "Open"), 5,			"           Packing value (str)");
	is  (beer_object_container_close(s), 0,			"        Closing map-5");
	is  (beer_object_add_map(s, 0), 1,			"        Packing value (map-6)");
	is  (beer_object_add_strz(s, "onclick"), 8,		"          Packing key (str)");
	is  (beer_object_add_strz(s, "CloseDoc()"), 11,		"           Packing value (str)");
	is  (beer_object_add_strz(s, "value"), 6,		"          Packing key (str)");
	is  (beer_object_add_strz(s, "Close"), 6,		"           Packing value (str)");
	is  (beer_object_container_close(s), 0,			"        Closing map-6");
	is  (beer_object_container_close(s), 0,			"      Closing map-3");
	is  (beer_object_container_close(s), 0,			"    Closing array");
	is  (beer_object_add_strz(s, "id"), 3,			"  Packing key (str)");
	is  (beer_object_add_strz(s, "file"), 5,			"   Packing value (str)");
	is  (beer_object_add_strz(s, "value"), 6,		"  Packing key (str)");
	is  (beer_object_add_strz(s, "File"), 5,			"   Packing value (str)");
	is  (beer_object_container_close(s), 0,			"  Closing map-2");
	is  (beer_object_container_close(s), 0,			"Closing map-1");
	is  (check_sbytes(s, bb4, bb4_len), 0,			"Check bytestring");

	const char bb5[] = "\xdf\x00\x00\x00\x01\xa4\x6d\x65\x6e\x75\xdf\x00\x00\x00"
			   "\x03\xa5\x70\x6f\x70\x75\x70\xdf\x00\x00\x00\x01\xa8\x6d"
			   "\x65\x6e\x75\x69\x74\x65\x6d\xdd\x00\x00\x00\x03\xdf\x00"
			   "\x00\x00\x02\xa7\x6f\x6e\x63\x6c\x69\x63\x6b\xae\x43\x72"
			   "\x65\x61\x74\x65\x4e\x65\x77\x44\x6f\x63\x28\x29\xa5\x76"
			   "\x61\x6c\x75\x65\xa3\x4e\x65\x77\xdf\x00\x00\x00\x02\xa7"
			   "\x6f\x6e\x63\x6c\x69\x63\x6b\xa9\x4f\x70\x65\x6e\x44\x6f"
			   "\x63\x28\x29\xa5\x76\x61\x6c\x75\x65\xa4\x4f\x70\x65\x6e"
			   "\xdf\x00\x00\x00\x02\xa7\x6f\x6e\x63\x6c\x69\x63\x6b\xaa"
			   "\x43\x6c\x6f\x73\x65\x44\x6f\x63\x28\x29\xa5\x76\x61\x6c"
			   "\x75\x65\xa5\x43\x6c\x6f\x73\x65\xa2\x69\x64\xa4\x66\x69"
			   "\x6c\x65\xa5\x76\x61\x6c\x75\x65\xa4\x46\x69\x6c\x65";

	size_t bb5_len = sizeof(bb5) - 1;

	is  (beer_object_reset(s), 0, "Reset bytestring");
	is  (beer_object_type(s, BEER_SBO_SPARSE), 0, "Check type set (must be ok)");
	is  (beer_object_add_map(s, 0), 5,			"Packing map-1");
	is  (beer_object_add_strz(s, "menu"), 5,			"  Packing key (str)");
	is  (beer_object_add_map(s, 0), 5,			"   Packing value (map-2)");
	is  (beer_object_add_strz(s, "popup"), 6,		"  Packing key (str)");
	is  (beer_object_add_map(s, 0), 5,			"   Packing value (map-3)");
	is  (beer_object_add_strz(s, "menuitem"), 9,		"     Packing key (str)");
	is  (beer_object_add_array(s, 0), 5,			"      Packing value (array)");
	is  (beer_object_add_map(s, 0), 5,			"        Packing value (map-4)");
	is  (beer_object_add_strz(s, "onclick"), 8,		"          Packing key (str)");
	is  (beer_object_add_strz(s, "CreateNewDoc()"), 15,	"           Packing value (str)");
	is  (beer_object_add_strz(s, "value"), 6,		"          Packing key (str)");
	is  (beer_object_add_strz(s, "New"), 4,			"           Packing value (str)");
	is  (beer_object_container_close(s), 0,			"        Closing map-4");
	is  (beer_object_add_map(s, 0), 5,			"        Packing value (map-5)");
	is  (beer_object_add_strz(s, "onclick"), 8,		"          Packing key (str)");
	is  (beer_object_add_strz(s, "OpenDoc()"), 10,		"           Packing value (str)");
	is  (beer_object_add_strz(s, "value"), 6,		"          Packing key (str)");
	is  (beer_object_add_strz(s, "Open"), 5,			"           Packing value (str)");
	is  (beer_object_container_close(s), 0,			"        Closing map-5");
	is  (beer_object_add_map(s, 0), 5,			"        Packing value (map-6)");
	is  (beer_object_add_strz(s, "onclick"), 8,		"          Packing key (str)");
	is  (beer_object_add_strz(s, "CloseDoc()"), 11,		"           Packing value (str)");
	is  (beer_object_add_strz(s, "value"), 6,		"          Packing key (str)");
	is  (beer_object_add_strz(s, "Close"), 6,		"           Packing value (str)");
	is  (beer_object_container_close(s), 0,			"        Closing map-6");
	is  (beer_object_container_close(s), 0,			"      Closing map-3");
	is  (beer_object_container_close(s), 0,			"    Closing array");
	is  (beer_object_add_strz(s, "id"), 3,			"  Packing key (str)");
	is  (beer_object_add_strz(s, "file"), 5,			"   Packing value (str)");
	is  (beer_object_add_strz(s, "value"), 6,		"  Packing key (str)");
	is  (beer_object_add_strz(s, "File"), 5,			"   Packing value (str)");
	is  (beer_object_container_close(s), 0,			"  Closing map-2");
	is  (beer_object_container_close(s), 0,			"Closing map-1");
	is  (check_sbytes(s, bb5, bb5_len), 0,			"Check bytestring");

	const char bb6[] = "\xdc\x00\x20\xc0\xc0\xc0\xc0\xc0\xc0\xc0\xc0\xc0\xc0\xc0"
			   "\xc0\xc0\xc0\xc0\xc0\xc0\xc0\xc0\xc0\xc0\xc0\xc0\xc0\xc0"
			   "\xc0\xc0\xc0\xc0\xc0\xc0\xc0";

	size_t bb6_len = sizeof(bb6) - 1;

	is  (beer_object_reset(s), 0, "Reset bytestring");
	is  (beer_object_type(s, BEER_SBO_PACKED), 0, "Check type set (must be ok)");
	is  (beer_object_add_array(s, 0), 1, "Packing array (size more 1 byte)");
	for (int i = 0; i < 32; ++i) beer_object_add_nil(s);
	is  (beer_object_container_close(s), 0, "Closing array (size more 1 byte)");
	is  (check_sbytes(s, bb6, bb6_len), 0, "Check bytestring");

	is  (beer_object_reset(s), 0, "Reset bytestring");
	isnt(beer_object_format(s, "{%s{%s{%s[{%s%s%s%s}{%s%s%s%s}{%s%s%s%s}]}%s%s%s%s}}",
			       "menu", "popup", "menuitem",
			       "onclick", "CreateNewDoc()", "value", "New",
			       "onclick", "OpenDoc()", "value", "Open",
			       "onclick", "CloseDoc()", "value", "Close",
			       "id", "file", "value", "File"), -1, "Pack with format");
	is  (check_sbytes(s, bb4, bb4_len), 0, "Check bytestring");

	beer_stream_free(s);

	footer();
	return check_plan();
}

static int
test_request_01(char *uri) {
	plan(8);
	header();

	struct beer_stream *beer = NULL; beer = beer_net(NULL);
	isnt(beer, NULL, "Check connection creation");
	isnt(beer_set(beer, BEER_OPT_URI, uri), -1, "Setting URI");
	isnt(beer_connect(beer), -1, "Connecting");
//	isnt(beer_authenticate(beer), -1, "Authenticating");

	struct beer_stream *arg; arg = beer_object(NULL);
	isnt(arg, NULL, "Check object creation");
	is  (beer_object_add_array(arg, 0), 1, "Append elem");

	struct beer_request *req1 = beer_request_call(NULL);
	beer_request_set_funcz(req1, "test_4");
	beer_request_set_tuple(req1, arg);
	uint64_t sync1 = beer_request_compile(beer, req1);

	struct beer_request *req2 = beer_request_eval(NULL);
	beer_request_set_exprz(req2, "return test_4()");
	beer_request_set_tuple(req2, arg);
	uint64_t sync2 = beer_request_compile(beer, req2);

	const char bb1[] = "\xce\x00\x00\x00\x10\x82\x00\x06\x01\x01\x82\x22\xa6\x74"
			   "\x65\x73\x74\x5f\x34\x21\x90\xce\x00\x00\x00\x19\x82\x00"
			   "\x08\x01\x02\x82\x27\xaf\x72\x65\x74\x75\x72\x6e\x20\x74"
			   "\x65\x73\x74\x5f\x34\x28\x29\x21\x90";
	size_t bb1_len = sizeof(bb1) - 1;

	const char bb2[] = "\x83\x00\xce\x00\x00\x00\x00\x01\xcf\x00\x00\x00\x00\x00"
			   "\x00\x00\x01\x05\xce\x00\x00\x00\x37\x81\x30\xdd\x00\x00"
			   "\x00\x01\x91\xa4\x74\x65\x73\x74";
	size_t bb2_len = sizeof(bb2) - 1;

	const char bb3[] = "\x83\x00\xce\x00\x00\x00\x00\x01\xcf\x00\x00\x00\x00\x00"
			   "\x00\x00\x02\x05\xce\x00\x00\x00\x37\x81\x30\xdd\x00\x00"
			   "\x00\x01\xa4\x74\x65\x73\x74";
	size_t bb3_len = sizeof(bb3) - 1;

	is  (check_nbytes(beer, bb1, bb1_len), 0, "Check requests");
	beer_flush(beer);

	struct beer_iter it; beer_iter_reply(&it, beer);
	while (beer_next(&it)) {
		struct beer_reply *r = BEER_IREPLY_PTR(&it);
		if (r->sync == sync1) {
			is  (check_rbytes(r, bb2, bb2_len), 0, "Check resp1");
		} else if (r->sync == sync2) {
			is  (check_rbytes(r, bb3, bb3_len), 0, "Check resp2");
		} else {
			assert(0);
		}
	}

	footer();
	return check_plan();
}

static int
test_request_02(char *uri) {
	plan(17);
	header();

	const char bb1[] = "\x83\x00\xce\x00\x00\x00\x00\x01\xcf\x00\x00\x00\x00\x00"
			   "\x00\x00\x01\x05\xce\x00\x00\x00\x37\x81\x30\xdd\x00\x00"
			   "\x00\x01\x97\xcd\x01\x19\x01\xa7\x5f\x76\x73\x70\x61\x63"
			   "\x65\xa7\x73\x79\x73\x76\x69\x65\x77\x00\x80\x97\x82\xa4"
			   "\x6e\x61\x6d\x65\xa2\x69\x64\xa4\x74\x79\x70\x65\xa3\x6e"
			   "\x75\x6d\x82\xa4\x6e\x61\x6d\x65\xa5\x6f\x77\x6e\x65\x72"
			   "\xa4\x74\x79\x70\x65\xa3\x6e\x75\x6d\x82\xa4\x6e\x61\x6d"
			   "\x65\xa4\x6e\x61\x6d\x65\xa4\x74\x79\x70\x65\xa3\x73\x74"
			   "\x72\x82\xa4\x6e\x61\x6d\x65\xa6\x65\x6e\x67\x69\x6e\x65"
			   "\xa4\x74\x79\x70\x65\xa3\x73\x74\x72\x82\xa4\x6e\x61\x6d"
			   "\x65\xab\x66\x69\x65\x6c\x64\x5f\x63\x6f\x75\x6e\x74\xa4"
			   "\x74\x79\x70\x65\xa3\x6e\x75\x6d\x82\xa4\x6e\x61\x6d\x65"
			   "\xa5\x66\x6c\x61\x67\x73\xa4\x74\x79\x70\x65\xa3\x73\x74"
			   "\x72\x82\xa4\x6e\x61\x6d\x65\xa6\x66\x6f\x72\x6d\x61\x74"
			   "\xa4\x74\x79\x70\x65\xa1\x2a";
	size_t bb1_len = sizeof(bb1) - 1;
	const char bb2[] = "\xce\x00\x00\x00\x1c\x82\x00\x01\x01\x01\x84\x10\xcd\x01"
			   "\x19\x11\x02\x12\xce\xff\xff\xff\xff\x20\x91\xa7\x5f\x76"
			   "\x73\x70\x61\x63\x65";
	size_t bb2_len = sizeof(bb2) - 1;

	struct beer_stream *beer = NULL; beer = beer_net(NULL);
	isnt(beer, NULL, "Check connection creation");
	isnt(beer_set(beer, BEER_OPT_URI, uri), -1, "Setting URI");
	isnt(beer_connect(beer), -1, "Connecting");
//	isnt(beer_authenticate(beer), -1, "Authenticating");

	struct beer_stream *key = NULL; key = beer_object(NULL);
	isnt(key, NULL, "Check object creation");
	is  (beer_object_add_array(key, 1), 1, "Create key");
	is  (beer_object_add_strz(key, "_vspace"), 8, "Create key");

	struct beer_request *req = NULL;
	req = beer_request_select(NULL);
	isnt(req, NULL, "Check request creation");
	is  (req->hdr.type, BEER_OP_SELECT, "Check that we inited select");
	is  (beer_request_set_space(req, 281), 0, "Set space");
	is  (beer_request_set_index(req, 2), 0, "Set index");
	is  (beer_request_set_key(req, key), 0, "Set key");
	isnt(beer_request_compile(beer, req), -1, "Compile request");
	is  (check_nbytes(beer, bb2, bb2_len), 0, "Check request");
	isnt(beer_flush(beer), -1, "Send package to server");
	beer_request_free(req);

	struct beer_reply reply;
	isnt(beer_reply_init(&reply), NULL, "Init reply");
	isnt(beer->read_reply(beer, &reply), -1, "Read reply");
	is  (check_rbytes(&reply, bb1, bb1_len), 0, "Check reply");
	beer_reply_free(&reply);

	footer();
	return check_plan();
}

static int
test_request_03(char *uri) {
	plan(17);
	header();

	const char bb1[] = "\x83\x00\xce\x00\x00\x00\x00\x01\xcf\x00\x00\x00\x00\x00"
			   "\x00\x00\x01\x05\xce\x00\x00\x00\x37\x81\x30\xdd\x00\x00"
			   "\x00\x01\x97\xcd\x01\x19\x01\xa7\x5f\x76\x73\x70\x61\x63"
			   "\x65\xa7\x73\x79\x73\x76\x69\x65\x77\x00\x80\x97\x82\xa4"
			   "\x6e\x61\x6d\x65\xa2\x69\x64\xa4\x74\x79\x70\x65\xa3\x6e"
			   "\x75\x6d\x82\xa4\x6e\x61\x6d\x65\xa5\x6f\x77\x6e\x65\x72"
			   "\xa4\x74\x79\x70\x65\xa3\x6e\x75\x6d\x82\xa4\x6e\x61\x6d"
			   "\x65\xa4\x6e\x61\x6d\x65\xa4\x74\x79\x70\x65\xa3\x73\x74"
			   "\x72\x82\xa4\x6e\x61\x6d\x65\xa6\x65\x6e\x67\x69\x6e\x65"
			   "\xa4\x74\x79\x70\x65\xa3\x73\x74\x72\x82\xa4\x6e\x61\x6d"
			   "\x65\xab\x66\x69\x65\x6c\x64\x5f\x63\x6f\x75\x6e\x74\xa4"
			   "\x74\x79\x70\x65\xa3\x6e\x75\x6d\x82\xa4\x6e\x61\x6d\x65"
			   "\xa5\x66\x6c\x61\x67\x73\xa4\x74\x79\x70\x65\xa3\x73\x74"
			   "\x72\x82\xa4\x6e\x61\x6d\x65\xa6\x66\x6f\x72\x6d\x61\x74"
			   "\xa4\x74\x79\x70\x65\xa1\x2a";
	size_t bb1_len = sizeof(bb1) - 1;

	const char bb2[] = "\xce\x00\x00\x00\x1c\x82\x00\x01\x01\x01\x84\x10\xcd\x01"
			   "\x19\x11\x02\x12\xce\xff\xff\xff\xff\x20\x91\xa7\x5f\x76"
			   "\x73\x70\x61\x63\x65";
	size_t bb2_len = sizeof(bb2) - 1;

	struct beer_stream *beer = NULL; beer = beer_net(NULL);
	isnt(beer, NULL, "Check connection creation");
	isnt(beer_set(beer, BEER_OPT_URI, uri), -1, "Setting URI");
	isnt(beer_connect(beer), -1, "Connecting");
//	isnt(beer_authenticate(beer), -1, "Authenticating");

	struct beer_stream *key = NULL; key = beer_object(NULL);
	isnt(key, NULL, "Check object creation");
	is  (beer_object_add_array(key, 1), 1, "Create key");
	is  (beer_object_add_strz(key, "_vspace"), 8, "Create key");

	struct beer_request *req = NULL; req = beer_request_select(NULL);
	isnt(req, NULL, "Check request creation");
	is  (req->hdr.type, BEER_OP_SELECT, "Check that we inited select");
	is  (beer_request_set_sspace(beer, req, "_vspace", 7), 0, "Set space");
	is  (beer_request_set_sindex(beer, req, "name", 4), 0, "Set index");
	is  (beer_request_set_key(req, key), 0, "Set key");
	isnt(beer_request_compile(beer, req), -1, "Compile request");
	is  (check_nbytes(beer, bb2, bb2_len), 0, "Check request");
	isnt(beer_flush(beer), -1, "Send package to server");
	beer_request_free(req);

	struct beer_reply reply;
	isnt(beer_reply_init(&reply), NULL, "Init reply");
	isnt(beer->read_reply(beer, &reply), -1, "Read reply");
	is  (check_rbytes(&reply, bb1, bb1_len), 0, "Check reply");
	beer_reply_free(&reply);

	footer();
	return check_plan();
}

static int
test_request_04(char *uri) {
	plan(20);
	header();

	const char bb1[] = "\xce\x00\x00\x00\x1c\x82\x00\x01\x01\x01\x86\x10\xcd\x01"
			   "\x19\x11\x02\x12\x01\x13\x01\x14\x06\x20\x91\xa7\x5f\x76"
			   "\x73\x70\x61\x63\x65";
	size_t bb1_len = sizeof(bb1) - 1;

	const char bb2[] = "\x83\x00\xce\x00\x00\x00\x00\x01\xcf\x00\x00\x00\x00\x00"
			   "\x00\x00\x01\x05\xce\x00\x00\x00\x37\x81\x30\xdd\x00\x00"
			   "\x00\x01\x97\xcd\x02\x01\x01\xa7\x6d\x73\x67\x70\x61\x63"
			   "\x6b\xa5\x6d\x65\x6d\x74\x78\x00\x80\x90";
	size_t bb2_len = sizeof(bb2) - 1;

	struct beer_stream *beer = NULL; beer = beer_net(NULL);
	isnt(beer, NULL, "Check connection creation");
	isnt(beer_set(beer, BEER_OPT_URI, uri), -1, "Setting URI");
	isnt(beer_connect(beer), -1, "Connecting");
//	isnt(beer_authenticate(beer), -1, "Authenticating");

	struct beer_stream *key = NULL; key = beer_object(NULL);
	isnt(key, NULL, "Check object creation");
	is  (beer_object_add_array(key, 1), 1, "Create key");
	is  (beer_object_add_strz(key, "_vspace"), 8, "Create key");

	struct beer_request *req = NULL; req = beer_request_select(NULL);
	isnt(req, NULL, "Check request creation");
	is  (req->hdr.type, BEER_OP_SELECT, "Check that we inited select");
	is  (beer_request_set_sspace(beer, req, "_vspace", 7), 0, "Set space");
	is  (beer_request_set_sindex(beer, req, "name", 4), 0, "Set index");
	is  (beer_request_set_key(req, key), 0, "Set key");
	is  (beer_request_set_offset(req, 1), 0, "Set offset");
	is  (beer_request_set_limit(req, 1), 0, "Set limit");
	is  (beer_request_set_iterator(req, BEER_ITER_GT), 0, "Set iterator");
	isnt(beer_request_compile(beer, req), -1, "Compile request");
	is  (check_nbytes(beer, bb1, bb1_len), 0, "Check request");
	isnt(beer_flush(beer), -1, "Send package to server");
	beer_request_free(req);
	beer_stream_free(key);

	struct beer_reply reply;
	isnt(beer_reply_init(&reply), NULL, "Init reply");
	isnt(beer->read_reply(beer, &reply), -1, "Read reply");
	is  (check_rbytes(&reply, bb2, bb2_len), 0, "Check reply");
	beer_reply_free(&reply);

	footer();
	return check_plan();
}

static int
test_request_05(char *uri) {
	plan(261);
	header();

	struct beer_stream *beer = NULL; beer = beer_net(NULL);
	isnt(beer, NULL, "Check connection creation");
	isnt(beer_set(beer, BEER_OPT_URI, uri), -1, "Setting URI");
	isnt(beer_connect(beer), -1, "Connecting");
	beer_stream_reqid(beer, 0);

	for (int i = 0; i < 10; ++i) {
		char ex[128] = {0};
		size_t ex_len = snprintf(ex, 128, "examplestr %d %d", i, i*i);

		struct beer_stream *val = beer_object(NULL);
		beer_object_format(val, "[%d%d%.*s]", i, i + 10, ex_len, ex);

		struct beer_request *req = beer_request_insert(NULL);
		isnt(req, NULL, "Check request creation");
		is  (req->hdr.type, BEER_OP_INSERT, "Check that we inited delete");
		is  (beer_request_set_sspacez(beer, req, "test"), 0, "Set space");
		is  (beer_request_set_tuple(req, val), 0, "Set tuple");
		isnt(beer_request_compile(beer, req), -1, "Compile request");

		beer_request_free(req);
		beer_stream_free(val);
	}

	isnt(beer_flush(beer), -1, "Send package to server");

	struct beer_iter it; beer_iter_reply(&it, beer);
	while (beer_next(&it)) {
		struct beer_reply *r = BEER_IREPLY_PTR(&it);
		uint32_t i = r->sync, str_len = 0;
		char ex[128] = {0};
		size_t ex_len = snprintf(ex, 128, "examplestr %d %d", i, i*i);
		isnt(r->data, NULL, "check that we get answer");
		const char *data = r->data;
		is  (mp_typeof(*data), MP_ARRAY, "Check array");
		is  (mp_decode_array(&data), 1, "Check array, again");
		is  (mp_decode_array(&data), 3, "And again (another)");
		ok  (mp_typeof(*data) == MP_UINT &&
		     mp_decode_uint(&data) == i &&
		     mp_typeof(*data) == MP_UINT &&
		     mp_decode_uint(&data) == i + 10 &&
		     mp_typeof(*data) == MP_STR &&
		     strncmp(mp_decode_str(&data, &str_len), ex, ex_len) == 0,
		     "Check fields");
	}

	beer_stream_reqid(beer, 0);

	for (int i = 0; i < 5; ++i) {
		char ex[128] = {0};
		size_t ex_len = snprintf(ex, 128, "anotherexamplestr %d %d", i, i*i);

		struct beer_stream *val = beer_object(NULL);
		beer_object_format(val, "[%d%d%.*s]", i, i + 5, ex_len, ex);

		struct beer_request *req = beer_request_replace(NULL);
		isnt(req, NULL, "Check request creation");
		is  (req->hdr.type, BEER_OP_REPLACE, "Check that we inited replace");
		is  (beer_request_set_sspacez(beer, req, "test"), 0, "Set space");
		is  (beer_request_set_tuple(req, val), 0, "Set tuple");
		isnt(beer_request_compile(beer, req), -1, "Compile request");

		beer_request_free(req);
		beer_stream_free(val);
	}

	isnt(beer_flush(beer), -1, "Send package to server");

	while (beer_next(&it)) {
		struct beer_reply *r = BEER_IREPLY_PTR(&it);
		uint32_t i = r->sync, str_len = 0;
		char ex[128] = {0};
		size_t ex_len = 0;
		ex_len = snprintf(ex, 128, "anotherexamplestr %d %d", i, i*i);
		isnt(r->data, NULL, "check that we get answer");
		const char *data = r->data;
		is  (mp_typeof(*data), MP_ARRAY, "Check array");
		is  (mp_decode_array(&data), 1, "Check array, again");
		is  (mp_decode_array(&data), 3, "And again (another)");
		ok  (mp_typeof(*data) == MP_UINT &&
		     mp_decode_uint(&data) == i &&
		     mp_typeof(*data) == MP_UINT &&
		     mp_decode_uint(&data) == i + 5 &&
		     mp_typeof(*data) == MP_STR &&
		     strncmp(mp_decode_str(&data, &str_len), ex, ex_len) == 0,
		     "Check fields");
	}

	struct beer_stream *key = NULL; key = beer_object(NULL);
	isnt(key, NULL, "Check object creation");
	is  (beer_object_add_array(key, 0), 1, "Create key");
	struct beer_request *req = beer_request_select(NULL);
	is (beer_request_set_sspacez(beer, req, "test"), 0, "Set space");
	beer_request_set_key(req, key);
	beer_request_compile(beer, req);

	beer_flush(beer);

	struct beer_reply reply; beer_reply_init(&reply);
	beer->read_reply(beer, &reply);
	const char *data = reply.data;

	is  (mp_typeof(*data), MP_ARRAY, "Check array");
	is  (mp_decode_array(&data), 10, "Check array, again");

	uint32_t arrsz = 10;
	uint32_t str_len = 0;

	while (arrsz-- > 0) {
		is  (mp_decode_array(&data), 3, "And again (another)");
		is  (mp_typeof(*data), MP_UINT, "check int");
		uint32_t sz = mp_decode_uint(&data);
		is  (mp_typeof(*data), MP_UINT, "check int");
		uint32_t sz_z = sz + 10; if (sz < 5) sz_z -= 5;
		is  (mp_decode_uint(&data), sz_z, "check int val");
		char ex[128] = {0};
		size_t ex_len = 0;
		if (sz < 5)
			ex_len = snprintf(ex, 128, "anotherexamplestr %d %d",
					  sz, sz*sz);
		else
			ex_len = snprintf(ex, 128, "examplestr %d %d", sz, sz*sz);
		ok  (mp_typeof(*data) == MP_STR &&
		     strncmp(mp_decode_str(&data, &str_len), ex, ex_len) == 0,
		     "Check str");
	}

	beer_request_free(req);
	beer_stream_free(key);

	beer_stream_reqid(beer, 0);

	for (int i = 0; i < 10; ++i) {
		struct beer_stream *key = beer_object(NULL);
		beer_object_format(key, "[%d]", i);

		struct beer_request *req = beer_request_delete(NULL);
		isnt(req, NULL, "Check request creation");
		is  (req->hdr.type, BEER_OP_DELETE, "Check that we inited replace");
		is  (beer_request_set_sspacez(beer, req, "test"), 0, "Set space");
		is  (beer_request_set_tuple(req, key), 0, "Set tuple");
		isnt(beer_request_compile(beer, req), -1, "Compile request");

		beer_request_free(req);
		beer_stream_free(key);
	}

	isnt(beer_flush(beer), -1, "Send package to server");

	while (beer_next(&it)) {
		struct beer_reply *r = BEER_IREPLY_PTR(&it);
		uint32_t i = r->sync, str_len = 0;
		char ex[128] = {0};
		size_t ex_len = 0;
		if (i < 5)
			ex_len = snprintf(ex, 128, "anotherexamplestr %d %d",
					  i, i*i);
		else
			ex_len = snprintf(ex, 128, "examplestr %d %d", i, i*i);
		isnt(r->data, NULL, "check that we get answer");
		const char *data = r->data;
		is  (mp_typeof(*data), MP_ARRAY, "Check array");
		is  (mp_decode_array(&data), 1, "Check array, again");
		is  (mp_decode_array(&data), 3, "And again (another)");
		ok  (mp_typeof(*data) == MP_UINT &&
		     mp_decode_uint(&data) == i &&
		     mp_typeof(*data) == MP_UINT &&
		     mp_decode_uint(&data) == i + 5 &&
		     mp_typeof(*data) == MP_STR &&
		     strncmp(mp_decode_str(&data, &str_len), ex, ex_len) == 0,
		     "Check fields");
	}

	beer_stream_free(beer);

	footer();
	return check_plan();
}

static inline int
test_msgpack_array_iter() {
	plan(32);
	header();

	/* simple array and array of arrays */
	struct beer_stream *sa, *aofa;
	sa = beer_object(NULL); aofa = beer_object(NULL);
	isnt(beer_object_format(sa, "[%s%d%s%d]", "hello", 11, "world", 12),
			       -1, "Pack with format sa");
	isnt(beer_object_format(aofa, "[[%d%d] [%d%d] [%d%d]]", 1, 2, 3,
			       4, 5, 6), -1, "Pack with format aofa");
	size_t sz = 0;
	struct beer_iter *it = beer_iter_array_object(NULL, sa);
	isnt(it, NULL, "check allocation");
	while (beer_next(it)) {
		uint32_t str_len = 0;
		const char *pos = BEER_IARRAY_ELEM(it);
		switch (sz) {
		case (0):
			is  (mp_typeof(*pos), MP_STR, "check str type");
			is  (strncmp(mp_decode_str(&pos, &str_len), "hello", 5),
			     0, "check str value");
			is  (str_len, 5, "check string length");
			break;
		case (1):
			is  (mp_typeof(*pos), MP_UINT, "check int type");
			is  (mp_decode_uint(&pos), 11, "check int");
			break;
		case (2):
			is  (mp_typeof(*pos), MP_STR, "check str type");
			is  (strncmp(mp_decode_str(&pos, &str_len), "world", 5),
			     0, "check str value");
			is  (str_len, 5, "check string length");
			break;
		case (3):
			is  (mp_typeof(*pos), MP_UINT, "check int type");
			is  (mp_decode_uint(&pos), 12, "check int");
			break;
		default:
			fail("unreacheable section");
		}
		sz += 1;
	}
	beer_iter_free(it);
	it = beer_iter_array_object(NULL, aofa);
	isnt(it, NULL, "check allocation");
	sz = 1;
	while (beer_next(it)) {
		const char *pos = BEER_IARRAY_ELEM(it);
		size_t elen = BEER_IARRAY_ELEM_END(it) - pos;
		is  (mp_typeof(*pos), MP_ARRAY, "check for array");
		struct beer_iter *wit = beer_iter_array(NULL, pos, elen);
		isnt(wit, NULL, "check allocation");
		while (beer_next(wit)) {
			const char *wpos = BEER_IARRAY_ELEM(wit);
			is  (mp_typeof(*wpos), MP_UINT, "check int type");
			is  (mp_decode_uint(&wpos), sz, "check int");
			sz += 1;
		}
		beer_iter_free(wit);
	}

	footer();
	return check_plan();
}

static inline int
test_msgpack_mapa_iter() {
	plan(34);
	header();

	/* simple map and map with arrays */
	struct beer_stream *sm, *mofa;
	isnt(sm = beer_object(NULL), NULL, "check allocation");
	isnt(mofa = beer_object(NULL), NULL, "check allocation");
	isnt(beer_object_format(sm, "{%s%d%s%d}", "hello", 11, "world", 12),
			       -1, "Pack with format s,");
	isnt(beer_object_format(mofa, "{%s [%d%d] %s [%d%d]}",
			       "hello", 0, 1,
			       "world", 2, 3), -1, "Pack with format aofa");

	size_t sz = 0;
	struct beer_iter *it = beer_iter_map_object(NULL, sm);
	isnt(it, NULL, "check allocation");
	while (beer_next(it)) {
		uint32_t str_len = 0;
		const char *kpos = BEER_IMAP_KEY(it);
		const char *vpos = BEER_IMAP_VAL(it);
		switch (sz) {
		case (0):
			is  (mp_typeof(*kpos), MP_STR, "check str type");
			is  (strncmp(mp_decode_str(&kpos, &str_len), "hello", 5),
			     0, "check str value");
			is  (str_len, 5, "check string length");
			is  (mp_typeof(*vpos), MP_UINT, "check int type");
			is  (mp_decode_uint(&vpos), 11, "check int");
			break;
		case (1):
			is  (mp_typeof(*kpos), MP_STR, "check str type");
			is  (strncmp(mp_decode_str(&kpos, &str_len), "world", 5),
			     0, "check str value");
			is  (str_len, 5, "check string length");
			is  (mp_typeof(*vpos), MP_UINT, "check int type");
			is  (mp_decode_uint(&vpos), 12, "check int");
			break;
		default:
			fail("unreacheable section");
		}
		sz += 1;
	}
	beer_iter_free(it);
	it = beer_iter_map_object(NULL, mofa);
	isnt(it, NULL, "check allocation");
	sz = 0;
	while (beer_next(it)) {
		const char *kpos = BEER_IMAP_KEY(it);
		uint32_t str_len = 0;
		if (sz == 0) {
			is  (mp_typeof(*kpos), MP_STR, "check str type");
			is  (strncmp(mp_decode_str(&kpos, &str_len), "hello", 5),
			     0, "check str value");
			is  (str_len, 5, "check string length");
		} else {
			is  (mp_typeof(*kpos), MP_STR, "check str type");
			is  (strncmp(mp_decode_str(&kpos, &str_len), "world", 5),
			     0, "check str value");
			is  (str_len, 5, "check string length");
		}
		const char *vpos = BEER_IMAP_VAL(it);
		size_t vlen = BEER_IMAP_VAL_END(it) - vpos;
		is  (mp_typeof(*vpos), MP_ARRAY, "check for array");
		struct beer_iter *wit = beer_iter_array(NULL, vpos, vlen);
		isnt(wit, NULL, "check allocation");
		while (beer_next(wit)) {
			const char *wpos = BEER_IARRAY_ELEM(wit);
			is  (mp_typeof(*wpos), MP_UINT, "check int type");
			is  (mp_decode_uint(&wpos), sz, "check int");
			sz += 1;
		}
		beer_iter_free(wit);
	}

	footer();
	return check_plan();
}

/*
static inline int
test_msgpack_mapa_iter() {
	plan(5);
	header();

	struct beer_stream *beer = NULL; beer = beer_net(NULL);
	isnt(beer, NULL, "Check connection creation");
	isnt(beer_set(beer, BEER_OPT_URI, uri), -1, "Setting URI");
	isnt(beer_connect(beer), -1, "Connecting");
	beer_stream_reqid(beer, 0);

	struct beer_request *req = beer_request_replace(NULL);
	isnt(req, NULL, "Check request creation");
	is  (req->hdr.type, BEER_OP_REPLACE, "Check that we inited replace");
	is  (beer_request_set_sspacez(beer, req, "test"), 0, "Set space");
	is  (beer_request_set_tuple_format(req, "[%d%s%d%s]", 1, "hello", 2, "world"), 0, "Set tuple");
	isnt(beer_request_compile(beer, req), -1, "Compile request");
	beer_request_free(req);

	struct beer_request *req = beer_request_upsert(NULL);
	isnt(req, NULL, "Check request creation");
	is  (req->hdr.type, BEER_OP_UPDATE, "CHeck that we inited upsert");
	is  (beer_request_set_sspacez(beer, req, "test"), 0, "Set space");
	is  (beer_request_set_tuple(req, val), 0, ""
	beer_stream_free(val);
	beer_stream_free(val);

	footer();
	return check_plan();
}
*/
int main() {
	plan(9);

	char uri[128] = {0};
	snprintf(uri, 128, "%s%s%s", "test:test@", "localhost:", getenv("PRIMARY_PORT"));

	test_connect_tcp();
	test_object();
	test_request_01(uri);
	test_request_02(uri);
	test_request_03(uri);
	test_request_04(uri);
	test_request_05(uri);
	test_msgpack_array_iter();
	test_msgpack_mapa_iter();

	return check_plan();
}

