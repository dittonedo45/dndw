# dndw (Ditto Nedo's Downloader) , "Archive Downloader And extractor on Go. \*-)"

The dndw is a program for downloading and extracting internet archives into your box.
The tool is a demostration of the way to intergrate the libav\* and  libarchive.

# Intentions

To write a program that will take a good care of unstable networks.
To write a program that will download an archive and extract it without storing the archive file.

# Compile

gcc main.c -lavutil -larchive -lavformat -o dndw

# Running

./dndw \<url\>

# Examples

./dndw 'ftp://ftp.gnu.org/gnu/ed/ed-1.4.tar.gz'
