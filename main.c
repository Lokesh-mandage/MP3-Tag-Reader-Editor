#include "mp3.h"

/* Display usage instructions */
void print_usage(void)
{
    printf("\nERROR: INVALID ARGUMENTS\n");

    printf("\nUSAGE:\n");

    printf("To view MP3 file:\n");

    printf("./mp3tag -v filename.mp3\n");

    printf("\nTo edit MP3 file:\n");
    printf("./mp3tag --help\n");
}

/* Display help menu */
void print_help(void)
{
    printf("\n---------------- HELP MENU ----------------\n");

    printf("1. -v  -> To view MP3 file contents\n");

    printf("2. -e  -> To edit MP3 file contents\n");

    printf("    -t -> To edit song title\n");
    printf("    -a -> To edit artist name\n");
    printf("    -A -> To edit album name\n");
    printf("    -y -> To edit year\n");
    printf("    -m -> To edit music/genre\n");
    printf("    -c -> To edit comment\n");

    printf("-------------------------------------------\n");
}

/* Validate edit option */
int is_valid_edit_option(const char *option)
{
    if (strcmp(option, "-t") == 0 ||
        strcmp(option, "-a") == 0 ||
        strcmp(option, "-A") == 0 ||
        strcmp(option, "-y") == 0 ||
        strcmp(option, "-m") == 0 ||
        strcmp(option, "-c") == 0)
    {
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    /* Check help option */
    if (argc == 2 && strcmp(argv[1], "--help") == 0)
    {
        print_help();
        return 0;
    }

    /* Check view option */
    if (argc == 3 && strcmp(argv[1], "-v") == 0)
    {
        view_mp3(argv[2]);
        return 0;
    }

    /* Check edit option */
    if (argc == 5 && strcmp(argv[1], "-e") == 0)
    {
        if (is_valid_edit_option(argv[2]))
        {
            edit_mp3(argv[4], argv[2], argv[3]);
            return 0;
        }
    }

    /* Invalid arguments */
    print_usage();

    return 1;
}