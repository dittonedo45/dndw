/*XXX This Document was modified on 1636717512 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <archive.h>
#include <archive_entry.h>
#include <libavformat/avio.h>
#include <getopt.h>

char buf[10540];

static int dn_cst_reader(struct archive *a, void *d, const void **b)
{
    int ret = 0;
    do {
	ret =
	    avio_read((AVIOContext *) d, (*b = buf, (unsigned char *) *b),
		      sizeof(buf));
    } while (ret < 0 && ret != AVERROR_EOF);

    return ret == AVERROR_EOF ? 0 : ret;
}

#include <jansson.h>
static json_t *dndw_parse_opt(char *o)
{
    char *v = strrchr(o, '=');
    if (!v)
	return 0;
    json_t *a = json_object();
    json_object_set(a, "value", json_string(v + 1));
    char *key = 0;
    asprintf(&key, "%.*s", v - o, o);
    json_object_set(a, "key", key);
    free(key);

    return a;
}

static void dndw_help(int l, char **ags, struct option *op, int o)
{

}

int main(signed Argsc, char *(Args[]))
{
    struct option lopts[] = {
	{ "help", 0, 0, 'h' },
	{ "option", 1, 0, 'o' },
	{ "url", 1, 0, 'u' },
	{ "path", 1, 0, 'p' },
	{ "verbose", 0, 0, 'v' },
	{.name = 0 }
    };

    if (Argsc == 1) {
	dndw_help(lopts, Args, lopts, 0);
	exit(1);
    }
    json_t *arch = json_object();

    json_t *arr = json_array();
    json_t *url = json_string("");
    json_t *path = json_string(getcwd(0, 0));
    json_t *verbose = json_boolean(0);
    json_t *nh = json_boolean(0);

    struct {
	const char *name;
	json_t *obj;
    } *p, mp[] = {
	{ "options", arr },
	{ "url", url },
	{ "path", path },
	{ "verbose", verbose },
	{.name = NULL }
    };
    p = mp;

    while (p && p->name) {
	json_object_set(arch, p->name, p->obj);
	p++;
    }

    do {
	int c = getopt_long(Argsc, Args, "ho:u:p:v", lopts, 0);
	if (c == -1)
	    break;
	switch (c) {
	case 'o':
	    {
		json_t *o = dndw_parse_opt(optarg);
		if (o)
		    json_array_append(arr, o);
	    }
	    break;
	case 'u':
	    json_string_set(url, optarg);
	    break;
	case 'p':
	    json_string_set(path, optarg);
	    break;
	case 'v':
	    json_object_set(arch, "verbose", json_boolean(1));
	    break;
	};
    } while (1);
    if (json_boolean_value(nh)) {
	dndw_help(lopts, Args, lopts, 0);
	return 3;
    }
    if (!strlen(json_string_value(url)))
	return 2;

    int ret = 0;
    do {
	const char *url = json_string_value(url);
	const char *path = json_string_value(path);
	AVDictionary *opts = 0;

	if (strlen(path)) {
	    if (chdir(path))
		break;
	}

	struct archive *a = archive_read_new();
	struct archive_entry *e = archive_entry_new();
	AVIOContext *ioc = 0;

	{
	    json_t *el;
	    int i;
	    json_array_foreach(arr, i, el)
// Add json_t *el -> AVDictionary* (XXX)
	    {
		json_t *k = json_object_get(el, "key");
		json_t *v = json_object_get(el, "value");

		if (!k || !v)
		    continue;

		const char *key = json_string_value(k);
		const char *val = json_string_value(v);
		if (!val || !key)
		    continue;
		av_dict_set(&opts, key, val, 0);
	    }
	}

	if (json_boolean_value(verbose)) {
	    av_log_level(AV_LOG_VERBOSE);
	}

	ret = avio_open2(&ioc, url, AVIO_FLAG_READ, 0, &opts);

	do {
	    if (ret < 0)
		break;
	    archive_read_set_callback_data(a, ioc);
	    archive_read_set_read_callback(a, dn_cst_reader);
	    archive_read_set_seek_callback(a, 0);

	    archive_read_support_format_all(a);
	    archive_read_support_filter_all(a);

	    ret = archive_read_open1(a);

	    if (ret != ARCHIVE_OK)
		break;
	    while (1) {
		ret = archive_read_next_header(a, &e);
		if (ret == ARCHIVE_EOF)
		    break;
		else if (ret == ARCHIVE_RETRY)
		    continue;
		else if (ret == ARCHIVE_OK) {
		    puts(archive_entry_pathname(e));
		    archive_read_extract(a, e, 0);
		}
	    }
	} while (0);

	avio_close(ioc);

	archive_read_close(a);
	archive_entry_free(e);
    } while (0);

    return (ret == ARCHIVE_OK) == 0;
}
