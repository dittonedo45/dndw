/*XXX This Document was modified on 1636717512 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <archive.h>
#include <archive_entry.h>
#include <libavformat/avio.h>

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

int main(signed Argsc, char *(Args[]))
{

    struct archive *a = archive_read_new();
    struct archive_entry *e = archive_entry_new();
    AVIOContext *ioc = 0;

    int ret = 0;

    ret = avio_open(&ioc, Args[1], AVIO_FLAG_READ);

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

      end:
	;;
    } while (0);

    avio_close(ioc);

    archive_read_close(a);
    archive_entry_free(e);

    return (ret == AVERROR_EOF) == 0;
}
