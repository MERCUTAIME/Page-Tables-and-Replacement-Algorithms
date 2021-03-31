
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int indexOf(char *lst, char types)
{
    for (int i = 0; i < 4; i++)
    {
        if (lst[i] == types)
        {
            return i;
        }
    }
    return -1;
}
typedef struct pagelst
{
    int traces[2];
    char page[16];
    struct pagelst *next;
} pagelst;

// Free the page list.
void freeNode(pagelst *root, pagelst *node)
{
    while (root->next != NULL)
    {
        node = root->next;
        free(root);
        root = node;
    }
    free(root);
}
/* Print all the instruction address*/
void print_instr(pagelst *node, bool index)
{

    while (node != NULL)
    {
        if (node->traces[index] != 0)
        {
            int c = 0;
            while (c < 16)
            {
                if (node->page[c] != 0)
                {
                    printf("0x%s,%d\n", (node->page) + c, node->traces[index]);
                    break;
                }
                c++;
            }
        }
        node = node->next;
    }
}
void print_data(pagelst *root, int *output)
{
    printf("Counts:\n");
    printf("  Instructions %d\n", output[0]);
    printf("  Loads %d\n", output[2]);
    printf("  Stores %d\n", output[1]);
    printf("  Modifies %d\n\n", output[3]);
    printf("Instructions:\n");
    pagelst *node = root;
    print_instr(node, 0);
    node = root;
    printf("Data:\n");
    print_instr(node, 1);
    freeNode(root, node);
}

pagelst *create_page_node()
{
    pagelst *node = (struct pagelst *)malloc(sizeof(struct pagelst));
    node->page[0] = '\0';
    node->traces[0] = 0;
    node->traces[1] = 0;
    node->next = NULL;
    return node;
}
void update_trace(bool isInstruction, pagelst *node)
{
    if (isInstruction)
        node->traces[0] += 1;
    else
        node->traces[1] += 1;
}

void store_page(pagelst *node, int *output, char *content, char *bit_addr)
{
    bool isInstruction = false;
    char ISLM[4] = {'I', 'S', 'L', 'M'};
    for (int d = 0; d < 100; ++d)
    {
        bool complete = false;
        int inde = indexOf(ISLM, content[d]);
        if (inde != -1)
        {
            isInstruction = content[d] == 'I';
            output[inde] += 1;
        }
        else if (content[d] != ' ')
        {
            int j;
            for (j = 0; j < 16; ++j)
            {
                bit_addr[j] = content[d];
                ++d;
                if (content[d] == ',')
                {
                    complete = true;
                    bit_addr[j + 1] = '\0';
                    break;
                }
            }
            for (int b = 0; b < 3; b++)
            {
                bit_addr[j - b] = '0';
            }

            pagelst *temp = node;
            pagelst *prev = NULL;
            while (temp != NULL)
            {
                if (!strcmp(temp->page, bit_addr))
                {
                    update_trace(isInstruction, temp);
                    break;
                }
                prev = temp;
                temp = temp->next;
            }

            if (temp == NULL)
            {
                temp = create_page_node();
                prev->next = temp;
                strcpy(temp->page, bit_addr);
                update_trace(isInstruction, temp);
            }
        }

        if (complete)
            break;
    }
}

int main(int argc, char **argv)
{
    int output[4] = {0, 0, 0, 0};
    char ISLM[4] = {'I', 'S', 'L', 'M'};
    FILE *fp = fopen(argv[1], "r");
    char content[100];
    char bit_addr[16];

    if (!fp)
    {
        perror("Error opening the given file:");
        exit(1);
    }

    pagelst *root = create_page_node();
    while (!feof(fp))
    {
        memset(content, '\0', 100);
        int k = 0;
        // Check if we reach the end of the file
        while (k < 100 && !feof(fp))
        {
            char c = fgetc(fp);
            char value = '\0';
            if (c != '\r' || c != '\n')
            {
                value = c;
            }
            content[k] = value;
            if (c == '\n')
            {

                break;
            }

            k++;
        }

        store_page(root, output, content, bit_addr);
    }

    fclose(fp);

    print_data(root, output);
}
