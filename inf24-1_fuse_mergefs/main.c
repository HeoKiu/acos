#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

#define FUSE_USE_VERSION 30

#include <fuse.h>

#define max_number_of_files 100
#define max_path_size 500

typedef struct {
    int if_directory;
    int error_code;
    struct stat stat_;
    char path_[max_path_size];
} my_file_t;

typedef struct {
    char directories[max_number_of_files][max_path_size];
    size_t directories_number;
} file_system_t;
/////////////////////
static file_system_t my_file_system_;

my_file_t find_file(const char *path) {
    char path_[max_path_size];
    struct stat stat_;
    my_file_t my_file;
    my_file.error_code = ENOENT;
    my_file.if_directory = 0;
    int exists = 0;
    for (uint32_t i = 0; i < my_file_system_.directories_number; i += 1) {
        sprintf(path_, "%s%s", my_file_system_.directories[i], path);
        if (0 == stat(path_, &stat_)) {
            if (stat_.st_mtime > my_file.stat_.st_mtime || 0 == exists) {
                my_file.error_code = 0;
                my_file.if_directory = (0 != (stat_.st_mode & S_IFDIR));
                sprintf(my_file.path_, "%s", path_);
                my_file.stat_ = stat_;
                exists = 1;
            }
        } else if (0 == exists) {
            my_file.error_code = errno;
        }
    }
    return my_file;
}

int custom_stat(const char *path, struct stat *st, struct fuse_file_info *fi) {
    // check root
    if (0 == strcmp("/", path)) {
        st->st_mode = 0555 | S_IFDIR;
        st->st_nlink = 2;
        return 0;
    }
    my_file_t file = find_file(path);
    if (file.error_code) {
        return -file.error_code;
    }
    if (file.if_directory) {
        *st = file.stat_;
        st->st_mode = S_IFDIR | 0555;
    } else {
        *st = file.stat_;
        st->st_mode = S_IFREG | 0444;
    }
    return 0;
}

int check_new_file(char files_dict[max_number_of_files][max_path_size], char *path) {
    for (size_t i = 0; i < max_number_of_files; i += 1) {
        if (!files_dict[i][0]) {
            strcpy(files_dict[i], path);
            return 1;
        } else if (0 == strncmp(files_dict[i], path, strlen(path) + 1)) {
            return 0;
        }
    }
    return 0;
}

int custom_readdir(const char *path, void *out, fuse_fill_dir_t filler, off_t off,
                   struct fuse_file_info *fi, enum fuse_readdir_flags flags) {
    char path_[max_path_size], files_dict[max_number_of_files][max_path_size];
    strcpy(path_, path);
    filler(out, ".", NULL, 0, 0);
    filler(out, "..", NULL, 0, 0);

    if (1 == strlen(path_)) {
        path_[0] = 0;
    }

    for (uint32_t i = 0; i < my_file_system_.directories_number; i += 1) {
        char cwd[max_path_size], base_path[max_path_size];
        getcwd(cwd, max_path_size);
        sprintf(base_path, "%s%s", my_file_system_.directories[i], path_);
        struct dirent *entity;
        DIR *dir;
        if (NULL != (dir = opendir(base_path))) {
            while (NULL != (entity = readdir(dir))) {
                if ('.' != entity->d_name[0] && check_new_file  (files_dict, entity->d_name)) {
                    filler(out, entity->d_name, NULL, 0, 0);
                }
            }
            closedir(dir);
        }
    }

    return 0;
}

int custom_open(const char *path, struct fuse_file_info *fi) {
    my_file_t my_file = find_file(path);
    if (my_file.error_code) {
        return -my_file.error_code;
    }
    if ((fi->flags & O_ACCMODE) != O_RDONLY) {
        return -EACCES;
    }
    int file_fd;
    if (-1 == (file_fd = open(my_file.path_, O_RDONLY))) {
        return -errno;
    } else {
        fi->fh = file_fd;
        return 0;
    }
}

int custom_close(const char *path, struct fuse_file_info *fi) {
    ssize_t my_result = close((int) fi->fh);
    if (-1 == my_result) {
        return -errno;
    } else {
        return 0;
    }
}

int custom_read(const char *path, char *out, size_t size, off_t off, struct fuse_file_info *file_info) {
    ssize_t my_result = read((int) file_info->fh, out, size);
    if (-1 == my_result) {
        return -errno;
    } else {
        return (int) my_result;
    }
}

struct fuse_operations custom_operations = {
        .readdir = custom_readdir,
        .getattr = custom_stat,
        .open    = custom_open,
        .read    = custom_read,
        .release = custom_close,
};

typedef struct {
    char *src;
} custom_options_t;
struct fuse_opt opt_specifications[] = {
        {"--src %s", offsetof(custom_options_t, src), 0},
        {NULL, 0,                                     0}
};
custom_options_t custom_options;

int main(int argc, char *argv[]) {
    struct fuse_args my_fuse_args_ = FUSE_ARGS_INIT(argc, argv);

    fuse_opt_parse(&my_fuse_args_, &custom_options, opt_specifications, NULL);

    /// opening file system
    char *ptr = strtok(custom_options.src, ":");
    char cwd[max_path_size];
    getcwd(cwd, max_path_size);
    size_t cntr = 0;
    while (ptr) {
        sprintf(my_file_system_.directories[cntr], "%s/%s", cwd, ptr);
        cntr += 1;
        ptr = strtok(0, ":");
    }
    my_file_system_.directories_number = cntr;

    int return_ans = fuse_main(my_fuse_args_.argc, my_fuse_args_.argv, &custom_operations, NULL);

    my_file_system_.directories_number = 0;

    return return_ans;
}