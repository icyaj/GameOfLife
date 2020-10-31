#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "gol.h"

int main(int argc, char *argv[])
{ 

    // Generates Universe
    struct universe v;

    // Input, Output, Stats flags. Init to 0
    int inputFlag, outputFlag, statsFlag = 0;

    // Number of Generations, defaults at 5.
    int generations = 5;

    // Rules, Defaults to will_be_alive
    int (*rule)(struct universe * u, int column, int row) = &will_be_alive;

    // Arguments for Files
    char *inputFile;
    char *outputFile;

    // File Name Declarations
    FILE *inFile;
    FILE *outFile;

    // Command line arguments
    int argIndex = 0;
    char arguments[5] = "";

    // For loop cycling through all valid command line options.
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {   
            // Checks if argument option is only one character after '-', i.e. -tt is rejected.
            if (argv[i][2] != '\0') {
                fprintf(stderr, "Error: Argument Options are only one character long.\n");
                exit(1);
            }
            // If repeated argument display error, else add to existing commands array (arguments).
            if (strchr(arguments, argv[i][1]) != NULL)
            {
                fprintf(stderr, "Error: Conflicting Argument.\n");
                exit(1);
                break;
            } else {
                arguments[argIndex] = argv[i][1];
                argIndex++;
            }

            // Switchcase determining the arguments.
            switch (argv[i][1])
            {
            // -i input_filename to specify that the initial generation of the universe should be read from a file.
            case 'i':
                inputFlag = 1;
                inputFile = argv[i + 1];
                if ((i + 1) >= argc || inputFile[0] == '-')
                {
                    fprintf(stderr, "Error: Please specify an input file.\n");
                    exit(1);
                    break;
                }
                break;
            // -o output_filename to specify a file into which the final generation should be output.
            case 'o':
                outputFlag = 1;
                outputFile = argv[i + 1];
                if ((i + 1) >= argc || outputFile[0] == '-')
                {
                    fprintf(stderr, "Error: Please specify an output file.\n");
                    exit(1);
                    break;
                }
                break;
            // -g number_of_generations to specify the number of new generations for which the game should be run.
            case 'g':

                if ((i + 1) >= argc || inputFile[0] == '-')
                {
                    fprintf(stderr, "Error: No generation integer specified.\n");
                    exit(1);
                    break;
                }
                
                //checking for negative numbers
                if (argv[i + 1][0] == '-') {
                    fprintf(stderr, "Error: Number of Generations must be a postive integer.\n");
                    exit(1);
                    break;
                };

                // checking for integer (all characters seem to return 0.)
                if ((atoi(argv[i + 1]) == 0)  && (argv[i + 1][0] != '0')) {
                    fprintf(stderr, "Error: Number of Generations must be an integer.\n");
                    exit(1);
                    break;
                }

                // Passes all checks.
                generations = atoi(argv[i + 1]);
                break;
            // -s to print statistics after the final generation has been output.
            case 's':
                statsFlag = 1;
                break;
            // -t to use the torus topology for the rule. If this is not specified, use the rule for cells outside the universe being permanently dead.
            case 't':
                rule = &will_be_alive_torus;
                break;
            // Catch all - Unknown option.
            default:
                fprintf(stderr, "Error: Unknown Option. \n");
                exit(1);
            }
        }
    }

    // Reads in input.
    if (inputFlag == 1) {
        inFile = fopen(inputFile, "r");
        if (inFile == NULL)
        {
            fprintf(stderr, "Error: Invalid input file.\n");
            exit(1);
        }
        read_in_file(inFile, &v);
        fclose(inFile);
    } else {
        read_in_file(stdin, &v);
    }

    // Performs evolutions generation amount of times.
    while (generations > 0) {
        evolve(&v, rule);
        generations--;
    }

    // Displays or stores the output.
    if (outputFlag == 1) {
        outFile = fopen(outputFile, "w");
        if (outFile == NULL)
        {
            fprintf(stderr, "Error: Invalid output file.\n");
            exit(1);
        }
        write_out_file(outFile, &v);
        fclose(outFile);
    } else {
        write_out_file(stdout, &v);
    }

    // Prints the statistics to screen if flag is true.
    if (statsFlag == 1) print_statistics(&v);

    return 0;
}
