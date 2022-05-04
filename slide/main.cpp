#include <stdexcept>

typedef char board_t[16];

int main(int argc, char** argv) {
    if (argc != 2)
        throw std::invalid_argument(
            "expected: input_file"
        );


    char cur[2];
    FILE *fp = fopen(argv[1], "r");
    while(fscanf(fp, "%s ", cur) > 0) {
        printf("%s\n", cur);
    }
}
