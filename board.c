
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum Player {
    GAMEMASTER, COMPUTER, HUMAN
} Player;

typedef enum PieceType {
    NONE = 0,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
} PieceType;

typedef enum Color {
    NEITHER, WHITE, BLACK
} Color;


typedef struct Piece {
    Player owner;
    PieceType type;   
    Color color;
} Piece;

const unsigned int BOARD_SIZE = 8;
const Piece DEFAULT_PIECE = {
    .owner = GAMEMASTER,
    .type = NONE,
    .color = NEITHER
};

char *piece_type_map[] = {
    "_", "P", "H", "B", "R", "Q", "K"
};


Piece **
board_create () {
    Piece **board = (Piece **) malloc(sizeof(Piece *) * BOARD_SIZE * BOARD_SIZE);
    if (board == NULL) {
        fprintf(stderr, "@ %d | Unable to initialize board!", __LINE__);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
        board[i] = (Piece *) malloc(sizeof(Piece));
        if (board[i] == NULL) {
            fprintf(stderr, "@ %d | Unable to initialize piece!", __LINE__);
            exit(EXIT_FAILURE);
        }
    }

    return board;
}


void
board_destroy (Piece **board) {
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
        free(board[i]);
    }

    free(board);
}


void
board_print (Piece **board) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece *piece = board[i * BOARD_SIZE + j];
            printf("%s ", piece_type_map[piece->type]);
        }
        printf("\n");
    }
}


int
main () {
    Piece **board = board_create();

    Piece *temp = (Piece *) malloc(sizeof(Piece));
    *temp = (Piece) {
        .owner = HUMAN,
        .type = KING,
        .color = BLACK
    };

    board[3 * BOARD_SIZE + 3] = temp;

    board_print(board);

    board_destroy(board);
    return EXIT_SUCCESS;
}


