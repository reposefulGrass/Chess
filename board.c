
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


#define board_at(alpha, numeral) \
    board[64 - numeral * 8 + (alpha - 'A')]


Piece *
piece_create (Player owner, PieceType type, Color color) {
    Piece *piece = (Piece *) malloc(sizeof(Piece));
    if (piece == NULL) {
        fprintf(stderr, "@ %d | Unable to allocate piece!", __LINE__);
        exit(EXIT_FAILURE);
    }

    *piece = (Piece) {
        .owner = owner,
        .type = type,
        .color = color
    };

    return piece;
}


void
board_initialize (Piece **board, Color human) {
    Color computer = (human == WHITE)? BLACK : WHITE;

    board_at('A', 8) = piece_create(COMPUTER, ROOK,   computer); 
    board_at('B', 8) = piece_create(COMPUTER, KNIGHT, computer);
    board_at('C', 8) = piece_create(COMPUTER, BISHOP, computer);
    board_at('D', 8) = piece_create(COMPUTER, KING,   computer);
    board_at('E', 8) = piece_create(COMPUTER, QUEEN,  computer);
    board_at('F', 8) = piece_create(COMPUTER, BISHOP, computer);
    board_at('G', 8) = piece_create(COMPUTER, KNIGHT, computer);
    board_at('H', 8) = piece_create(COMPUTER, ROOK,   computer);

    for (int i = 0; i < BOARD_SIZE; i++) {
        board_at('A' + i, 7) = piece_create(COMPUTER, PAWN, computer);
    }

    for (int i = 6; i > 2; i--) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board_at('A' + j, i) = piece_create(GAMEMASTER, NONE, NEITHER);
        }
    }

    board_at('A', 1) = piece_create(HUMAN, ROOK,   human); 
    board_at('B', 1) = piece_create(HUMAN, KNIGHT, human);
    board_at('C', 1) = piece_create(HUMAN, BISHOP, human);
    board_at('D', 1) = piece_create(HUMAN, KING,   human);
    board_at('E', 1) = piece_create(HUMAN, QUEEN,  human);
    board_at('F', 1) = piece_create(HUMAN, BISHOP, human);
    board_at('G', 1) = piece_create(HUMAN, KNIGHT, human);
    board_at('H', 1) = piece_create(HUMAN, ROOK,   human);

    for (int i = 0; i < BOARD_SIZE; i++) {
        board_at('A' + i, 2) = piece_create(COMPUTER, PAWN, computer);
    }
}


Piece **
board_create () {
    Piece **board = (Piece **) malloc(sizeof(Piece *) * BOARD_SIZE * BOARD_SIZE);
    if (board == NULL) {
        fprintf(stderr, "@ %d | Unable to allocate board!", __LINE__);
        exit(EXIT_FAILURE);
    }

    board_initialize(board, WHITE); // WHITE is only temp, outsource to board_create()

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

    board_print(board);

    board_destroy(board);
    return EXIT_SUCCESS;
}


