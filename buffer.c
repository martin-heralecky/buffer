#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
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
    size_t value;
    char unit[4] = {0};

    // todo: check integer overflow
    // todo: check trailing characters
    if (sscanf(str, "%zu%3s", &value, unit) != 2) {
        fprintf(stderr, "Error: Invalid buffer size.\n");
        return 1;
    }

    if (value == 0) {
        fprintf(stderr, "Error: Buffer size must be a non-zero integer.\n");
        return 1;
    }

    /* unit coefficient */
    long coef;

    if (strcmp("B", unit) == 0) {
        coef = 1;
    } else if (strcmp("KB", unit) == 0) {
        coef = 1000;
    } else if (strcmp("MB", unit) == 0) {
        coef = (long)1 * 1000*1000;
    } else if (strcmp("GB", unit) == 0) {
        coef = (long)1 * 1000*1000*1000;
    } else if (strcmp("KiB", unit) == 0) {
        coef = 1024;
    } else if (strcmp("MiB", unit) == 0) {
        coef = (long)1 * 1024*1024;
    } else if (strcmp("GiB", unit) == 0) {
        coef = (long)1 * 1024*1024*1024;
    } else {
        fprintf(stderr, "Error: Invalid unit. Must be one of: B, KB, MB, GB, KiB, MiB, GiB\n");
        return 1;
    }

    /* unit coefficient doesn't fit to size_t OR provided value (unit applied) doesn't fit to size_t */
    if ((size_t)coef < coef || value > SIZE_MAX / coef) {
        fprintf(stderr, "Error: Buffer size is too high.\n");
        return 1;
    }

    *size = value * coef;
    return 0;
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
