#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

/* in bytes */
#define DEFAULT_BUFFER_SIZE (long)1 * 8*1024*1024

struct buffer {
    /**
     * Size of the buffer in bytes.
     */
    size_t size;

    /**
     * Buffer's internal memory.
     */
    unsigned char *data;
};

/**
 * Prints usage info to stdout.
 */
void print_usage() {
}

/**
 * Parses string as buffer size. Prints error messages to stderr (in which case ends with failure).
 *
 * Input string must be an integer followed by unit. Allowed units are: B, KB, MB, GB, KiB, MiB, GiB.
 *
 * @param str  String containing the buffer size.
 * @param size Memory where to write the extracted buffer size (in bytes).
 * @return 0 on success, 1 on failure.
 */
int parse_size(const char *str, size_t *size) {
}

/**
 * Reads data from stdin and stores them into the buffer memory.
 *
 * @param buf Must be of type (struct buffer *), which must be initialized.
 */
void *consume(void *buffer) {
}


/**
 * Writes data from the buffer memory to stdout.
 *
 * @param buf Must be of type (struct buffer *), which must be initialized.
 */
void *supply(void *buffer) {
}

/**
 * @return 0 on success, 1 on failure.
 */
int main(int argc, char *argv[]) {
    struct buffer buf;

    pthread_t consumer_th, supplier_th;
    pthread_attr_t thread_attr;

    if (argc > 2 || (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0))) {
        print_usage();
        return argc == 2 ? 0 : 1;
    }

    if (argc < 2) {
        buf.size = DEFAULT_BUFFER_SIZE;
    } else if (parse_size(argv[1], &buf.size) != 0) {
        return 1;
    }

    buf.data = malloc(buf.size);
    if (buf.data == NULL) {
        fprintf(stderr, "Error: Could not allocate buffer memory (%zu bytes).\n", buf.size);
        return 1;
    }

    if (pthread_attr_init(&thread_attr) != 0) {
        fprintf(stderr, "Error: Could not initialize thread attributes.\n");
        return 1;
    }

    if (pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE) != 0) {
        fprintf(stderr, "Error: Could not set thread detach state attribute.\n");
        return 1;
    }

    if (pthread_create(&consumer_th, &thread_attr, &consume, &buf) != 0) {
        fprintf(stderr, "Error: Could not create consumer thread.\n");
        return 1;
    }

    if (pthread_create(&supplier_th, &thread_attr, &supply, &buf) != 0) {
        fprintf(stderr, "Error: Could not create supplier thread.\n");
        return 1;
    }

    if (pthread_attr_destroy(&thread_attr) != 0) {
        fprintf(stderr, "Warning: Could not destroy thread attributes.\n");
    }

    // todo: wait for completion

    free(buf.data);

    return 0;
}
