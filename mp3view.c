
#include "mp3.h"

void view_mp3(const char *filename)
{
    FILE *fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        printf("ERROR: Unable to open file\n");
        return;
    }

    unsigned char header[10];

    // Read ID3 header
    if (fread(header, 1, 10, fp) != 10)
    {
        printf("ERROR: Unable to read MP3 header\n");
        fclose(fp);
        return;
    }

    // Check ID3 identifier
    if (memcmp(header, "ID3", 3) != 0)
    {
        printf("ERROR: ID3 tag not found\n");
        fclose(fp);
        return;
    }

    // Check ID3 version
    if (header[3] != 3)
    {
        printf("ERROR: Only ID3v2.3 is supported\n");
        fclose(fp);
        return;
    }

    printf("\n============== MP3 TAG DETAILS ==============\n");

    while (1)
    {
        unsigned char frame_header[10];

        // Read frame header
        if (fread(frame_header, 1, 10, fp) != 10)
            break;

        // Stop at padding
        if (frame_header[0] == 0)
            break;

        char frame_id[5] = {0};
        memcpy(frame_id, frame_header, 4);

        // Calculate frame size (big-endian)
        unsigned int frame_size =
            ((unsigned int)frame_header[4] << 24) |
            ((unsigned int)frame_header[5] << 16) |
            ((unsigned int)frame_header[6] << 8) |
            (unsigned int)frame_header[7];

        if (frame_size == 0)
            continue;

        unsigned char *data = malloc(frame_size + 1);

        if (data == NULL)
        {
            printf("Memory allocation failed\n");
            break;
        }

        // Read frame data
        if (fread(data, 1, frame_size, fp) != frame_size)
        {
            free(data);
            break;
        }

        data[frame_size] = '\0';

        // Display metadata
        if (strcmp(frame_id, "TIT2") == 0)
        {
            printf("\nTitle    : ");
        }
        else if (strcmp(frame_id, "TPE1") == 0)
        {
            printf("\nArtist   : ");
        }
        else if (strcmp(frame_id, "TALB") == 0)
        {
            printf("\nAlbum    : ");
        }
        else if (strcmp(frame_id, "TYER") == 0)
        {
            printf("\nYear     : ");
        }
        else if (strcmp(frame_id, "TCON") == 0)
        {
            printf("\nGenre    : ");
        }
        else if (strcmp(frame_id, "COMM") == 0)
        {
            printf("\nComment  : ");

            if (frame_size > 4)
            {
                int encoding = data[0];
                unsigned int pos = 4;

                // Skip comment description
                if (encoding == 0 || encoding == 3)
                {
                    while (pos < frame_size && data[pos] != '\0')
                    {
                        pos++;
                    }

                    if (pos < frame_size)
                        pos++;
                }
                else if (encoding == 1 || encoding == 2)
                {
                    while (pos + 1 < frame_size)
                    {
                        if (data[pos] == 0 && data[pos + 1] == 0)
                        {
                            pos += 2;
                            break;
                        }

                        pos += 2;
                    }
                }

                if (pos < frame_size)
                {
                    if (encoding == 0 || encoding == 3)
                    {
                        printf("%.*s\n",
                               (int)(frame_size - pos),
                               data + pos);
                    }
                    else
                    {
                        printf("[UTF-16 comment - decoding not implemented]\n");
                    }
                }
                else
                {
                    printf("No comment text available\n");
                }
            }
            else
            {
                printf("No comment text available\n");
            }

            free(data);
            continue;
        }
        else
        {
            free(data);
            continue;
        }

        // Display normal text frame content
        if (frame_size > 1)
        {
            if (data[0] == 0 || data[0] == 3)
            {
                printf("%s\n", data + 1);
            }
            else
            {
                printf("[Unicode text - decoding not implemented]\n");
            }
        }
        else
        {
            printf("No text available\n");
        }

        free(data);
    }

    printf("\n==================================================\n");

    fclose(fp);
}

