#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <libgen.h>

int video_filter(const struct dirent *entry) {
    if (entry->d_type != DT_REG) return 0;
    const char *ext = strrchr(entry->d_name, '.');
    if (!ext) return 0;
    return (strcasecmp(ext, ".m4a") == 0 ||
			strcasecmp(ext, ".3gp") == 0 ||
			strcasecmp(ext, ".mp3") == 0 ||
			strcasecmp(ext, ".mp4") == 0 ||
			strcasecmp(ext, ".mkv") == 0 ||
            strcasecmp(ext, ".avi") == 0 ||
            strcasecmp(ext, ".mov") == 0 ||
            strcasecmp(ext, ".webm") == 0 ||
            strcasecmp(ext, ".flv") == 0 ||
            strcasecmp(ext, ".m4v") == 0);
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    char *target_file = realpath(argv[1], NULL);
    if (!target_file) return 1;

    char *dir_path = strdup(target_file);
    char *dir = dirname(dir_path);

    struct dirent **namelist;
    int n = scandir(dir, &namelist, video_filter, alphasort);
    if (n < 0) {
        free(target_file);
        free(dir_path);
        return 1;
    }

    char **vlc_args = malloc((n + 4) * sizeof(char *));
    int arg_count = 0;
    vlc_args[arg_count++] = "vlc";
    vlc_args[arg_count++] = "--no-auto-preparse";
    vlc_args[arg_count++] = "--no-playlist-tree";
    vlc_args[arg_count++] = target_file;

    for (int i = 0; i < n; i++) {
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, namelist[i]->d_name);
        char *resolved_path = realpath(full_path, NULL);
        
        if (resolved_path && strcmp(resolved_path, target_file) != 0) {
            vlc_args[arg_count++] = resolved_path;
        } else if (resolved_path) {
            free(resolved_path);
        }
        free(namelist[i]);
    }
    free(namelist);
    vlc_args[arg_count] = NULL;

    execvp("vlc", vlc_args);

    free(target_file);
    free(dir_path);
    return 0;
}
