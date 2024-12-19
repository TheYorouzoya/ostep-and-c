#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define CHUNK_SIZE sysconf(_SC_PAGESIZE)         /* chunk size in bytes */

#define handle_error(msg)                               \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error_en(en, msg)                \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while(0)


/* a single zip unit */
typedef struct {
    char c;                     /* character */
    int  count;                 /* number of times character appears */
} zip_unit;

/* a chunk is made up of a number of zip units */
typedef struct {
    int      size;              /* number of units in chunk */
    zip_unit *units;            /* the zip units themselves */
} zip_chunk;

/* zip data for a file is made up of a number of zip chunks */
typedef struct {
    size_t 	size;             /* number of chunks in file */
    zip_chunk 	*chunks;          /* the zip chunks themselves */
} zip_data;


/* argument data passed to each thread */
typedef struct {
    pthread_t thread_id;        /* thread's id return by pthread_create */
    int       thread_num;       /* thread number assigned by main */
    char	  *fp;              /* file pointer the thread is working on */
    zip_data  *zdp;             /* file's zip data pointer where to deposit chunk data */
} t_info;


size_t 		total_chunks;   /* total number of chunks in a file */
size_t 		current_chunk;  /* current chunk the thread is working on */
__off_t file_size;

/* lock for getting chunks */
static pthread_mutex_t mutex_chunk = PTHREAD_MUTEX_INITIALIZER;


void *process_chunk(void *);

void *output_zip_data(void *);

zip_unit*  output_chunk(zip_chunk *, zip_unit *);

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("pzip: file1 [file2 ...]\n");
        exit(EXIT_FAILURE);
    }

    struct stat statbuf;
    int fd, nthreads, worker, procs;
    char *fp;
    zip_data z_data[argc - 1];  /* each file's zip data holder */
    pthread_t writers[argc - 1]; /* threads that process and output zip data */


    for (int i = 1, j = 0; i < argc; i++, j++) {
        fd = open(argv[i], O_RDONLY);
        if (fd == -1) {
            handle_error("open");
        }

        if (stat(argv[i], &statbuf) == -1) {
            handle_error("stat");
        }

        file_size = statbuf.st_size;

        fp = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);

        total_chunks = (file_size / CHUNK_SIZE) + 1;
        current_chunk = 0;

        procs = get_nprocs();
        /* number of threads = min(processors available, chunks to process) */
        nthreads = (procs <= total_chunks) ? procs : total_chunks;

        t_info *tinfo = calloc(nthreads, sizeof(*tinfo));

        if (tinfo == NULL) {
            /* calloc failed */
            handle_error("calloc");
        }

        z_data[j].size = total_chunks;
        z_data[j].chunks = malloc(sizeof(zip_chunk) * total_chunks);

        for (int tnum = 0; tnum < nthreads; tnum++) {
            tinfo[tnum].thread_num = tnum + 1;
            tinfo[tnum].fp = fp;
            tinfo[tnum].zdp = &z_data[j];

            worker = pthread_create(&tinfo[tnum].thread_id, NULL, &process_chunk, &tinfo[tnum]);

            if (worker != 0) {
                handle_error_en(worker, "pthread_create");
            }
        }

        for (int tnum = 0; tnum < nthreads; tnum++) {
            worker = pthread_join(tinfo[tnum].thread_id, NULL);
            if (worker != 0) {
                handle_error_en(worker, "pthread_join");
            }
        }


        free(tinfo);

        munmap(fp, statbuf.st_size);
        close(fd);

        worker = pthread_create(&writers[j], NULL, &output_zip_data, &z_data[j]);
        if (worker != 0) {
            handle_error_en(worker, "pthread_create");
        }
    }

    for (int tnum = 0; tnum < argc - 1; tnum++) {
        worker = pthread_join(writers[tnum], NULL);
        if (worker != 0) {
            handle_error_en(worker, "pthread_join");
        }
    }


    pthread_mutex_destroy(&mutex_chunk);
    return EXIT_SUCCESS;
}

void *process_chunk(void *arg) {
    if (current_chunk >= total_chunks) {
        /* no chunks left to process */
        pthread_exit(NULL);
    }


    t_info *t = arg;            /* data passed to the thread */

    while (current_chunk <= total_chunks) {
        pthread_mutex_lock(&mutex_chunk);
        int mychunk = current_chunk++;            /* get chunk number */
        pthread_mutex_unlock(&mutex_chunk);

        if (mychunk >= total_chunks) {
            /* somehow went past chunk limit */
            pthread_exit(NULL);
        }

        char *fp = t->fp;             /* get file pointer from argument */
        fp += (CHUNK_SIZE * mychunk); /* reposition the pointer to chunk location */

        int count = 0;          /* count the current unit's instances */
        char previous;           /* the current unit */

        /* initialize current zip chunk */
        zip_chunk *cur_chunk = &t->zdp->chunks[mychunk];
        cur_chunk->size = 0;

        cur_chunk->units = malloc(sizeof(zip_unit));
        if (cur_chunk->units == NULL) {
            /* malloc failed */
            exit(EXIT_FAILURE);
        }

        int i = 0, limit = 0;
        if (mychunk == (total_chunks - 1)) {
            __off_t extra = (CHUNK_SIZE * total_chunks) - file_size;
            limit = CHUNK_SIZE - extra;
        } else {
            limit = CHUNK_SIZE;
        }


        /* process chunk */
        for (previous = fp[0]; i < limit; i++) {
            if (fp[i] == previous) {
                count++;
                continue;
            }

            cur_chunk->size++;

            zip_unit *temp_units = realloc(cur_chunk->units, sizeof(zip_unit) * cur_chunk->size);
            if (temp_units == NULL) {
                /* realloc failed */
                exit(EXIT_FAILURE);
            }
            temp_units[cur_chunk->size - 1].c = previous;
            temp_units[cur_chunk->size - 1].count = count;

            cur_chunk->units = temp_units;
            previous = fp[i];
            count = 1;
        }

        cur_chunk->size++;

        zip_unit *temp_units = realloc(cur_chunk->units, sizeof(zip_unit) * cur_chunk->size);
        if (temp_units == NULL) {
            /* realloc failed */
            exit(EXIT_FAILURE);
        }
        temp_units[cur_chunk->size - 1].c = previous;
        temp_units[cur_chunk->size - 1].count = count;

        cur_chunk->units = temp_units;

    }

    return NULL;
}


void *output_zip_data(void *arg) {
    zip_data *data = arg;
    zip_chunk *chunks = data->chunks;

    zip_chunk *curr_chunk;
    zip_unit *last_unit = NULL;

    size_t chnk_num;
    chnk_num = data->size;

    for (int chunk = 0; chunk < chnk_num; chunk++) {
        curr_chunk = &chunks[chunk];
        last_unit = output_chunk(curr_chunk, last_unit);
    }

    fwrite(&last_unit->count, sizeof(last_unit->count), 1, stdout);
    fwrite(&last_unit->c, sizeof(last_unit->c), 1, stdout);

    for (int chunk = 0; chunk < chnk_num; chunk++) {
        curr_chunk = &chunks[chunk];
        free(curr_chunk->units);
    }
    free(chunks);

    return NULL;
}


zip_unit* output_chunk(zip_chunk *chunk, zip_unit *prev) {
    zip_unit *units;
    zip_unit *curr;

    size_t size = chunk->size;

    units = chunk->units;

    if (prev != NULL) {
        if (units[0].c == prev->c) {
            /* last chunk unit was the same as starting unit, accumulate */
            units[0].count += prev->count;
        } else {
            /* last chunk unit was different, output it */
            fwrite(&prev->count, sizeof(prev->count), 1, stdout);
            fwrite(&prev->c, sizeof(prev->c), 1, stdout);
        }
    }

    /* output until the last zip unit */
    for (int i = 0; i < size - 1; i++) {
        curr = &units[i];
        fwrite(&curr->count, sizeof(curr->count), 1, stdout);
        fwrite(&curr->c, sizeof(curr->c), 1, stdout);
    }

    return &units[size - 1];
}
