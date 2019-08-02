
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// TODO: Change (Position) { a, i } TO position(a, i)

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

typedef struct Position {
    char alpha;     // A, B, ..., H
    int numeral;    // 1, 2, ..., 8
} Position;

typedef struct Piece {
    Player owner;
    PieceType type;   
    Color color;
    Position positionition;
} Piece;

const unsigned int BOARD_SIZE = 8;

char *piece_type_map[] = {
    "_", "P", "H", "B", "R", "Q", "K"
};

#define MALLOC_CHECK(object, string) \
    if ((object) == NULL) { \
        fprintf(stderr, "@ %d | Unable to allocate %s!", __LINE__, (string)); \
        exit(EXIT_FAILURE); \
    }

Piece **
board_get (Piece ***board, Position position) {
    Piece **piece = &(*board)[64 - (position.numeral * 8) + (position.alpha - 'A')];
    return piece;
}

Piece *
board_place (Piece ***board, Position position, Piece *replace) {
    Piece **obtain = board_get(board, position);
    
    Piece *temp = *obtain;
    *obtain = replace;

    return temp;
}


Position
position (char alpha, int numeral) {
    return (Position) { alpha, numeral };
}


Piece *
piece_create (Player owner, PieceType type, Color color, Position position) {
    Piece *piece = (Piece *) malloc(sizeof(Piece));
    MALLOC_CHECK(piece, "piece")

    *piece = (Piece) {
        .owner = owner,
        .type = type,
        .color = color,
        .positionition = position
    };

    return piece;
}

#define board_at(alpha, numeral) \
    board[64 - (numeral) * 8 + ((alpha) - 'A')]

void
board_initialize (Piece **board, Color human) {
    Color computer = (human == WHITE)? BLACK : WHITE;

    board_at('A', 8) = piece_create(COMPUTER, ROOK,   computer, position('A', 8)); 
    board_at('B', 8) = piece_create(COMPUTER, KNIGHT, computer, position('B', 8));
    board_at('C', 8) = piece_create(COMPUTER, BISHOP, computer, position('C', 8));
    board_at('D', 8) = piece_create(COMPUTER, KING,   computer, position('D', 8));
    board_at('E', 8) = piece_create(COMPUTER, QUEEN,  computer, position('E', 8));
    board_at('F', 8) = piece_create(COMPUTER, BISHOP, computer, position('F', 8));
    board_at('G', 8) = piece_create(COMPUTER, KNIGHT, computer, position('G', 8));
    board_at('H', 8) = piece_create(COMPUTER, ROOK,   computer, position('H', 8));

    for (int i = 0; i < BOARD_SIZE; i++) {
        board_at('A' + i, 7) = piece_create(
            COMPUTER, PAWN, computer, position('A' + i, 7)
        );
    }

    for (int i = 6; i > 2; i--) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board_at('A' + j, i) = piece_create(
                GAMEMASTER, NONE, NEITHER, position('A' + j, i) 
            );
        }
    }

    for (int i = 0; i < BOARD_SIZE; i++) {
        board_at('A' + i, 2) = piece_create(
            HUMAN, PAWN, computer, position('A' + i, 2) 
        );
    }

    board_at('A', 1) = piece_create(HUMAN, ROOK,   human, position('A', 1)); 
    board_at('B', 1) = piece_create(HUMAN, KNIGHT, human, position('B', 1));
    board_at('C', 1) = piece_create(HUMAN, BISHOP, human, position('C', 1));
    board_at('D', 1) = piece_create(HUMAN, KING,   human, position('D', 1));
    board_at('E', 1) = piece_create(HUMAN, QUEEN,  human, position('E', 1));
    board_at('F', 1) = piece_create(HUMAN, BISHOP, human, position('F', 1));
    board_at('G', 1) = piece_create(HUMAN, KNIGHT, human, position('G', 1));
    board_at('H', 1) = piece_create(HUMAN, ROOK,   human, position('H', 1));
}


bool
positionition_is_in_board (Position position) {
    if ((position.alpha < 'A') || (position.alpha > 'H')) {
        return false;
    } 

    if ((position.numeral < 1) || (position.numeral > 8)) {
        return false;
    }

    return true;
}


Position
positionition_from (Position positionition, int x_delta, int y_delta) {
    return (Position) {
        positionition.alpha + x_delta,
        positionition.numeral + y_delta 
    };
}


bool
valid_move (Piece **board, Position from, Position to) {
    if (positionition_is_in_board(to)) {
        Piece *piece_from = board_at(from.alpha, from.numeral);
        Piece *piece_to   = board_at(to.alpha, to.numeral);

        // includes any spot that isn't occupied by a (piece_from->owner)'s piece.
        if (piece_from->owner != piece_to->owner) {
            return true; 
        }
    }

    return false;
}


Position *
piece_deltas (Piece **board, Piece *piece, int *size) {
    Position *positionitions;
            
    Position current_position = piece->positionition;
    char alpha = current_position.alpha;
    int numeral = current_position.numeral;
    Player owner = piece->owner;

    switch (piece->type) {
        // A Pawn has 2 positionsible moves (forward +2) if he has yet to move,
        // and 1 positionsible move (forward +1) if he has.
        case PAWN: {
            *size = 0;
            int y_delta = (owner == HUMAN)? 1 : -1;

            // the pawn's first move
            if ((numeral == 2 && owner == HUMAN) || (numeral == 7 && COMPUTER)) {
                positionitions = (Position *) malloc(sizeof(Position) * 2);
                MALLOC_CHECK(positionitions, "Position Array")

                if (valid_move(board, current_position, position(alpha, numeral + y_delta))) {
                    positionitions[0] = (Position) {alpha, numeral + y_delta};
                    if (valid_move(board, current_position, position(alpha, numeral + 2 * y_delta))) {
                        *size = 2;
                        positionitions[1] = position(alpha, numeral + 2 * y_delta);
                    }
                    else {
                        *size = 1;
                    }
                }
            } 
            else {
                positionitions = (Position *) malloc(sizeof(Position) * 1);
                MALLOC_CHECK(positionitions, "Position Array")

                if (valid_move(board, current_position, position(alpha, numeral + y_delta))) {
                    *size = 1;
                    positionitions[0] = positionition_from(current_position, 0, y_delta);
                }
            }
            break;
        } 


        // A Rook has 14 positionsible moves, 7 in each direction, regardless of 
        // his positionition on the board. However, a rook cannot pass through other
        // pieces, so it's path must be stopped once encountered.
        case ROOK: {
            positionitions = (Position *) malloc(sizeof(Position) * (7 * 2));
            MALLOC_CHECK(positionitions, "Position Array")

            *size = 0;

            // alt is used to alternate the offset "i"
            for (int alt = -1; alt <= 1; alt += 2) {
                for (int i = 1; i < 8; i++) {
                    Position cursor = (Position) {alpha + (i * alt), numeral};
                    if (positionition_is_in_board(cursor)) {
                        Piece *piece = board_at(alpha + (i * alt), numeral);
                        // no piece
                        if (piece->type == NONE) {
                            positionitions[(*size)++] = cursor;
                        }
                        // allied piece
                        else if (piece->owner == owner) {
                            break;
                        }
                        // enemy piece
                        else {
                            positionitions[(*size)++] = cursor;    
                            break;
                        }
                    }
                }
            }
           
            for (int alt = -1; alt < 2; alt += 2) {
                for (int i = 1; i < 8; i++) {
                    Position cursor = (Position) {alpha, numeral + (i * alt)};
                    if (positionition_is_in_board(cursor)) {
                        Piece *piece = board_at(alpha, numeral + (i * alt));
                        // no piece
                        if (piece->type == NONE) {
                            positionitions[(*size)++] = cursor;
                        }
                        // allied piece
                        else if (piece->owner == owner) {
                            break;
                        }
                        // enemy piece
                        else {
                            positionitions[(*size)++] = cursor;
                            break;
                        }
                    }
                }
            }

            positionitions = realloc(positionitions, sizeof(Position) * (*size)); 
            MALLOC_CHECK(positionitions, "Position Array")
            break;
        }


        // A knight has 8 different spots he can move onto.
        // It can also jump over any other piece onto that spot.
        case KNIGHT:
            positionitions = (Position *) malloc(sizeof(Position) * (4 * 2));
            MALLOC_CHECK(positionitions, "Position Array")

            *size = 0;

            Position knight_positionsible_positionitions[8] = {
                (Position) {alpha - 1, numeral + 2},
                (Position) {alpha + 1, numeral + 2},
                (Position) {alpha + 2, numeral + 1},
                (Position) {alpha + 2, numeral - 1},
                (Position) {alpha + 1, numeral - 2},
                (Position) {alpha - 1, numeral - 2},
                (Position) {alpha - 2, numeral - 1},
                (Position) {alpha - 2, numeral + 1},
            };

            for (int i = 0; i < 8; i++) {
                Position position = knight_positionsible_positionitions[i];
                if (valid_move(board, current_position, position)) {
                    positionitions[(*size)++] = position;
                }
            }

            positionitions = (Position *) realloc(positionitions, sizeof(Position) * (*size));
            MALLOC_CHECK(positionitions, "Position Array")
            break;


        case BISHOP:

            break;


        // A King can move once in every direction, totaling 8 positionsible moves.
        case KING:
            positionitions = (Position *) malloc(sizeof(Position) * 8);
            MALLOC_CHECK(positionitions, "Position Array")

            *size = 0;

            Position king_positionsible_positionitions[8] = {
                (Position) {alpha - 1, numeral + 1},
                (Position) {alpha    , numeral + 1},
                (Position) {alpha + 1, numeral + 1},
                (Position) {alpha + 1, numeral    },
                (Position) {alpha + 1, numeral - 1},
                (Position) {alpha    , numeral - 1},
                (Position) {alpha - 1, numeral - 1},
                (Position) {alpha - 1, numeral    },
            };

            for (int i = 0; i < 8; i++) {
                Position position = king_positionsible_positionitions[i];
                if (valid_move(board, current_position, position)) {
                    positionitions[(*size)++] = position;
                }
            }

            positionitions = (Position *) realloc(positionitions, sizeof(Position) * (*size));
            MALLOC_CHECK(positionitions, "Position Array")
            break;


        case QUEEN:

            break;
    }

    return positionitions;
}


Piece **
board_create () {
    Piece **board = (Piece **) malloc(sizeof(Piece *) * BOARD_SIZE * BOARD_SIZE);
    MALLOC_CHECK(board, "board")

    board_initialize(board, WHITE); // WHITE is only temp, outsource to board_create()

    return board;
}


void
board_destroy (Piece ***board) {
    for (int alpha = 'A'; alpha <= 'H'; alpha++) {
        for (int numeral = 1; numeral <= 8; numeral++) {
            Piece *piece = *board_get(board, position(alpha, numeral));
            free(piece);
        }
    }

    free(*board);
}


void
board_print (Piece **board) {
    for (int i = 0; i < 8; i++) {
        printf("%d | ", 8 - i);
        for (int j = 0; j < 8; j++) {
            Piece *piece = board[i * BOARD_SIZE + j];
            printf("%s ", piece_type_map[piece->type]);
        }
        printf("\n");
    }
    printf("   ----------------\n");
    printf("    A B C D E F G H\n");
}


int
main () {
    Piece **board = board_create();

    Piece *empty = board_place(
        &board, position('D', 3), 
        piece_create(HUMAN, PAWN, WHITE, position('D', 3))
    );    
    free(empty);

    board_print(board);

    int num_spots = 0;
    Position *spots = piece_deltas(board, board_at('D', 3), &num_spots);
    printf("%d spots | ", num_spots);
    for (int i = 0; i < num_spots; i++) {
        printf("%c%d ", spots[i].alpha, spots[i].numeral);
    }    
    printf("\n");
    free(spots);

    board_destroy(&board);
    return EXIT_SUCCESS;
}


