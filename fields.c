#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int starts_with(char *full, char *sub)
{
    int m = strlen(full);
    int n = strlen(sub);

    if (n > m)
        return 0;

    for (int i = 0; i < n; i++)
        if (full[i] != sub[i])
            return 0;

    return n;
}

int has_char(char *str, char c)
{
    int n = strlen(str);
    for (int i = 0; i < n; i++)
        if (str[i] == c)
            return i;
    return 0;
}

void get_line(char *data, int *index, char *out)
{
    int n = strlen(data);
    int i = 0;

    while (data[*index] != '\n' && *index < n)
    {
        out[i] = data[*index];
        *index += 1;
        i++;
    }

    out[i] = '\0';

    if (data[*index] == '\n')
        *index += 1;
}

void get_field(char *data, char *field, char *out)
{
    char buffer[100];
    int index = 0, n = 0;
    int size = strlen(data);
    while (index < size)
    {
        get_line(data, &index, buffer);
        if (!has_char(buffer, ':'))
        {
            *out = 0;
            return;
        }

        if (n = starts_with(buffer, field))
        {
            // +2 to skip the ": "
            strcpy(out, buffer + n + 2);
            return;
        }
    }
}