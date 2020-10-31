#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "gol.h"

//  reads in the file from the file pointer infile and stores the universe in the structure pointed to by u.
void read_in_file(FILE *infile, struct universe *u)
{
    // Checks if infile is valid.
    if (infile == NULL)
        {
            fprintf(stderr, "Error: Invalid input file.\n");
            exit(1);
        }

    // Init Row, Column, Index & Alive to zero.
    int row = 0, column = 0, index = 0, bufferLength = 0, alive = 0;

    // Initilises the universe array
    char **p = malloc(1 * sizeof(char *));

    // Checks if malloc failed.
    if (p==NULL) {
        fprintf(stderr, "Error: Malloc Failed.\n");
        exit(1);
    }

    // Rewinds to begin of file.
    rewind(infile);

    // Max Character Buffer Length
    char buffer[513];

    // First run flag (To check width.)
    int hasRun = 0, width = 0;

    // Scans in each line.
    while (EOF != fscanf(infile, "%513[^\n]\n", buffer))
    {
        // Length of Buffer
        bufferLength = strlen(buffer);

        // If not run sets length of buffer to width. Else checks the length is consistent. 
        if (hasRun) {
            if (width != bufferLength) {
                fprintf(stderr, "Error: Input array size inconsistent. (Needs to be a rectangle).\n");
                exit(1);
            }
        } else {

            // Sets width and hasrun flags.
            width = bufferLength;
            hasRun = 1;

            // Checks if the file is under 512 column characters.
            if (512 < bufferLength) {
                fprintf(stderr, "Error: Input array size too large. (Needs to be < 512 valid chars).\n");
                exit(1);
            }
        }

        // Reallocate more space.
        p = realloc(p, sizeof(char *) * (row+1) * bufferLength);

        // Checks if realloc failed.
        if (p==NULL) {
        fprintf(stderr, "Error: realloc Failed.\n");
        exit(1);
        }

        // Add buffer to array.
        for (index = 0; index < bufferLength; index++)
        {
            // Checks if current character is valid
            if (buffer[index] == '*' || buffer[index] == '.')
            {   

                // Adds to buffer charater to universe array.
                *p[row * bufferLength + index] = buffer[index];

                // Checks if current character is alive.
                if (buffer[index] == '*')
                {
                    alive++;
                }
                
            } else {
                fprintf(stderr, "Error: Invalid character entered (Only '.' or '*' allowed).\n");
                exit(1);
            }

        }
        row++;

    }

    // Sets column to bufferLength.
    column = bufferLength;

    // Stores the universe values into universe.
    u->height = row;
    u->width = column;
    u->array = p;
    u->current_alive = alive;
    u->total_alive = alive;
    u->generation = 1;
}

// writes the content of the universe pointed to by u into the file from the file pointer outfile.
void write_out_file(FILE *outfile, struct universe *u)
{
    // Rewinds to begin of file.
    rewind(outfile);

    int fileline = 0;

    // Prints out the universe to the out file.
    for (int row = 0; row < u->height; row++)
    {
        for (int column = 0; column < u->width; column++)
        {
            fprintf(outfile, "%c", (char)u->array[row * u->width + column]);

            // Adds newline except if in the last row.
            if (column == (u->width - 1) && (row + 1) != u->height)
            {
                fprintf(outfile, "\n");
            }

            fileline++;
        }
    }

    fprintf(outfile, "\n");
}

// returns 1 if the cell in that column and row is alive and 0 otherwise.
int is_alive(struct universe *u, int column, int row)
{
    return(u->array[row * (u->width) + column] == (char *)'*' ? 1 : 0);
}

// returns 1 if the cell in that column and row will be alive in the next generation and 0 otherwise, assuming that cells outside the universe are always dead.
int will_be_alive(struct universe *u, int column, int row)
{   
    // Alive Neighbours
    int aliveNeighbours = 0;

    /*
        0 1 2
        3 * 4
        5 6 7
    */
    // Add all Neighbours
    struct point neighbours[8];
    
    // Check Top
    if (row == 0) {
        neighbours[0].y = -1;
        neighbours[1].y = -1;
        neighbours[2].y = -1;
    } else {
        neighbours[0].y = row - 1;
        neighbours[1].y = row - 1;
        neighbours[2].y = row - 1;
    }

    // Check Left
    if (column == 0) {
        neighbours[0].x = -1;
        neighbours[3].x = -1;
        neighbours[5].x = -1;
    } else {
        neighbours[0].x = (column - 1);
        neighbours[3].x = (column - 1);
        neighbours[5].x = (column - 1);
    }

    // Check Right
    if (column == (u->width - 1)) {
        neighbours[2].x = -1;
        neighbours[4].x = -1;
        neighbours[7].x = -1;
    } else {
        neighbours[2].x = (column + 1);
        neighbours[4].x = (column + 1);
        neighbours[7].x = (column + 1);
    }

    // Check Bottom
    if (row == (u->height - 1)) {
        neighbours[5].y = -1;
        neighbours[6].y = -1;
        neighbours[7].y = -1;
    } else {
        neighbours[5].y = (row + 1);
        neighbours[6].y = (row + 1);
        neighbours[7].y = (row + 1);
    }

    // Stragglers
    neighbours[1].x = column;
    neighbours[3].y = row;
    neighbours[4].y = row;
    neighbours[6].x = column;

    // // Check Neighbours
    for (int i = 0; i < 8; i++) {
        if (neighbours[i].y != -1 && neighbours[i].x != -1) {
            if (is_alive(u,neighbours[i].x,neighbours[i].y) == 1) {
            aliveNeighbours++;
            }
        }
    }

    // Applying Rules. If alive do x, else if dead do y.
    if (u->array[row * u->width + column] == (char *)'*') {
        if (aliveNeighbours == 2 || aliveNeighbours == 3) {
            return 1;
        }
    } else {
        if (aliveNeighbours == 3) {
            return 1;
        }
    }

    // Returns 0 if no rules are true.
    return 0;
}

// returns 1 if the cell in that column and row will be alive in the next generation and 0 otherwise, assuming a torus topology.
int will_be_alive_torus(struct universe *u, int column, int row)
{
    // Alive Neighbours
    int aliveNeighbours = 0;

    /*
        0 1 2
        3 * 4
        5 6 7
    */
    // Add all Neighbours
    struct point neighbours[8];
    
    // Check Top
    if (row == 0) {
        neighbours[0].y = (u->height - 1);
        neighbours[1].y = (u->height - 1);
        neighbours[2].y = (u->height - 1);
        } else {
        neighbours[0].y = row - 1;
        neighbours[1].y = row - 1;
        neighbours[2].y = row - 1;
    }

    // Check Left
    if (column == 0) {
        neighbours[0].x = (u->width - 1);
        neighbours[3].x = (u->width - 1);
        neighbours[5].x = (u->width - 1);
    } else {
        neighbours[0].x = (column - 1);
        neighbours[3].x = (column - 1);
        neighbours[5].x = (column - 1);
    }

    // Check Right
    if (column == (u->width - 1)) {
        neighbours[2].x = 0;
        neighbours[4].x = 0;
        neighbours[7].x = 0;
    } else {
        neighbours[2].x = (column + 1);
        neighbours[4].x = (column + 1);
        neighbours[7].x = (column + 1);
    }

    // Check Bottom
    if (row == (u->height - 1)) {
        neighbours[5].y = 0;
        neighbours[6].y = 0;
        neighbours[7].y = 0;
    } else {
        neighbours[5].y = (row + 1);
        neighbours[6].y = (row + 1);
        neighbours[7].y = (row + 1);
    }

    // Stragglers
    neighbours[1].x = column;
    neighbours[3].y = row;
    neighbours[4].y = row;
    neighbours[6].x = column;

    // Check Neighbours
    for (int i = 0; i < 8; i++) {

        if (is_alive(u,neighbours[i].x,neighbours[i].y) == 1) {
            aliveNeighbours++;
        }
    }

    // Applying Rules. If alive do x, else if dead do y.
    if (u->array[row * u->width + column] == (char *)'*') {
        if (aliveNeighbours == 2 || aliveNeighbours == 3) {
            return 1;
        }
    } else {
        if (aliveNeighbours == 3) {
            return 1;
        }
    }

    // Returns 0 if no rules are true.
    return 0;

}

// changes the universe from the current generation to the next generation.
void evolve(struct universe *u, int (*rule)(struct universe *u, int column, int row))
{
    // Update Generation Number
    u->generation++;

    // Get Next Generation Init Variables
    int alive = 0;

    // Initilises the universe array
    char new[u->height * u->width];
    
    // Prints out the universe to the out file.
    for (int y = 0; y < u->height; y++)
    {
        for (int x = 0; x < u->width; x++)
        {   
            // Stores into element the new character deduced from rule u and column & row = x & y.
            int element = rule(u,x,y);

            // Adds new element to the new generation array.
            if (element == 1) {
                new[y * u->width + x] = '*';  
                alive++; 
            } else {
                new[y * u->width + x] = '.';
            }
        }
    }

    // Store Current_alive.
    u->current_alive = alive;

    // Calculate/Store Total_alive
    u->total_alive += alive;

    // Replaces old generation with new one in universe.
    for (int row = 0; row < u->height; row++)
    {
        for (int column = 0; column < u->width; column++)
        {
            u->array[row * u->width + column] = new[row * u->width + column];  
        }
    }

}

// should calculate the percentage of cells that are alive in the current generation and the average percentage that have been alive in all of the generations so far.
void print_statistics(struct universe *u)
{

    // Calculate Statistics
    float current_stat = (((float)u->current_alive * (float)100) / ((float)u->height * (float)u->width));
    float total_stat = (((float)u->total_alive * (float)100) / ((float)u->height * (float)u->width * (float)u->generation));

    // Print out Statistics
    printf("\n%.3f%% of cells currently alive\n", roundf(current_stat * 1000) / 1000);
    printf("%.3f%% of cells alive on average\n", roundf(total_stat * 1000) / 1000);
}
