
#include "mp3.h"
#include <stdint.h>

#define MAX_TAG_SIZE 10000000

static uint32_t decode_size(const unsigned char *size)
{
    return ((uint32_t)size[0] << 21) |
           ((uint32_t)size[1] << 14) |
           ((uint32_t)size[2] << 7) |
           (uint32_t)size[3];
}

static void encode_size(unsigned char *size, uint32_t value)
{
    size[0] = (value >> 21) & 0x7F;
    size[1] = (value >> 14) & 0x7F;
    size[2] = (value >> 7) & 0x7F;
    size[3] = value & 0x7F;
}

void edit_mp3(const char *filename,
              const char *option,
              const char *new_value)
{
    FILE *src = fopen(filename, "rb");

    if (src == NULL)
    {
        perror("Cannot open MP3 file");
        return;
    }

    unsigned char header[10];

    if (fread(header, 1, 10, src) != 10 ||
        memcmp(header, "ID3", 3) != 0 || header[3] != 3)
    {
        printf("Error: Only ID3v2.3 files are supported.\n");
        fclose(src);
        return;
    }

    uint32_t old_tag_size = decode_size(header + 6);

    if (old_tag_size == 0 || old_tag_size > MAX_TAG_SIZE)
    {
        printf("Invalid ID3 tag size.\n");
        fclose(src);
        return;
    }

    unsigned char *tag = malloc(old_tag_size);

    if (tag == NULL)
    {
        printf("Memory allocation failed.\n");
        fclose(src);
        return;
    }

    if (fread(tag, 1, old_tag_size, src) != old_tag_size)
    {
        printf("Error reading ID3 tag.\n");
        free(tag);
        fclose(src);
        return;
    }

    const char *frame_id = NULL;

    if (strcmp(option, "-t") == 0)
        frame_id = "TIT2";
    else if (strcmp(option, "-a") == 0)
        frame_id = "TPE1";
    else if (strcmp(option, "-A") == 0)
        frame_id = "TALB";
    else if (strcmp(option, "-y") == 0)
        frame_id = "TYER";
    else if (strcmp(option, "-m") == 0)
        frame_id = "TCON";
    else if (strcmp(option, "-c") == 0)
        frame_id = "COMM";
    else
    {
        printf("Invalid edit option.\n");
        free(tag);
        fclose(src);
        return;
    }

    if (strcmp(frame_id, "COMM") == 0)
    {
        printf("Comment editing is not supported yet.\n");
        free(tag);
        fclose(src);
        return;
    }

    size_t value_length = strlen(new_value);

    if (value_length > UINT32_MAX - 1)
    {
        printf("New value is too large.\n");
        free(tag);
        fclose(src);
        return;
    }

    uint32_t new_frame_size = (uint32_t)value_length + 1;

    unsigned char *new_tag =
        malloc((size_t)old_tag_size + new_frame_size + 10);

    if (new_tag == NULL)
    {
        printf("Memory allocation failed.\n");
        free(tag);
        fclose(src);
        return;
    }

    uint32_t pos = 0;
    uint32_t new_pos = 0;
    int found = 0;
    uint32_t padding = 0;

    while (pos + 10 <= old_tag_size)
    {
        if (tag[pos] == 0)
            break;

        uint32_t frame_size =
            ((uint32_t)tag[pos + 4] << 24) |
            ((uint32_t)tag[pos + 5] << 16) |
            ((uint32_t)tag[pos + 6] << 8) |
            (uint32_t)tag[pos + 7];

        if (frame_size > old_tag_size - pos - 10)
        {
            printf("Invalid ID3 frame size.\n");
            free(new_tag);
            free(tag);
            fclose(src);
            return;
        }

        if (memcmp(tag + pos, frame_id, 4) == 0 && !found)
        {
            found = 1;

            /* Write frame ID */
            memcpy(new_tag + new_pos, frame_id, 4);

            /* Write new frame size in big-endian format */
            new_tag[new_pos + 4] = (new_frame_size >> 24) & 0xFF;
            new_tag[new_pos + 5] = (new_frame_size >> 16) & 0xFF;
            new_tag[new_pos + 6] = (new_frame_size >> 8) & 0xFF;
            new_tag[new_pos + 7] = new_frame_size & 0xFF;

            /* Preserve frame flags */
            new_tag[new_pos + 8] = tag[pos + 8];
            new_tag[new_pos + 9] = tag[pos + 9];

            /* Encoding byte: ISO-8859-1 */
            new_tag[new_pos + 10] = 0;

            /* Write new metadata */
            memcpy(new_tag + new_pos + 11,
                   new_value, value_length);

            new_pos += 10 + new_frame_size;
        }
        else
        {
            /* Copy existing frame unchanged */
            memcpy(new_tag + new_pos,
                   tag + pos, 10 + frame_size);

            new_pos += 10 + frame_size;
        }

        pos += 10 + frame_size;
    }

    if (!found)
    {
        printf("Frame not found. Adding new frames is not supported yet.\n");
        free(new_tag);
        free(tag);
        fclose(src);
        return;
    }

    /* Preserve the original padding */
    padding = old_tag_size - pos;

    uint32_t new_tag_size = new_pos + padding;

    if (new_tag_size > 0x0FFFFFFF)
    {
        printf("ID3 tag is too large.\n");
        free(new_tag);
        free(tag);
        fclose(src);
        return;
    }

    memset(new_tag + new_pos, 0, padding);

    /* Update ID3 header with the new tag size */
    encode_size(header + 6, new_tag_size);

    FILE *tmp = fopen("temp.mp3", "wb");

    if (tmp == NULL)
    {
        perror("Cannot create temporary file");
        free(new_tag);
        free(tag);
        fclose(src);
        return;
    }

    int error = 0;

    if (fwrite(header, 1, 10, tmp) != 10 ||
        fwrite(new_tag, 1, new_tag_size, tmp) != new_tag_size)
    {
        error = 1;
    }

    /* Copy original MP3 audio data */
    unsigned char buffer[4096];
    size_t bytes_read;

    while (!error &&
           (bytes_read = fread(buffer, 1, sizeof(buffer), src)) > 0)
    {
        if (fwrite(buffer, 1, bytes_read, tmp) != bytes_read)
            error = 1;
    }

    if (ferror(src))
        error = 1;

    if (fclose(src) != 0)
        error = 1;

    if (fclose(tmp) != 0)
        error = 1;

    if (error)
    {
        printf("Error writing temporary MP3 file.\n");
        remove("temp.mp3");
    }
    else if (rename("temp.mp3", filename) == 0)
    {
        printf("MP3 tag updated successfully!\n");
    }
    else
    {
        perror("Cannot replace original file");
        printf("Updated data is available in temp.mp3.\n");
    }

    free(new_tag);
    free(tag);
}

