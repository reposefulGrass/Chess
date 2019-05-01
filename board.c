
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// TODO: Change the cordinate system, its fuck up

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
    Position position;
} Piece;

const unsigned int BOARD_SIZE = 8;

char *piece_type_map[] = {
    "_", "P", "H", "B", "R", "Q", "K"
};


#define board_at(alpha, numeral) \
    board[64 - (numeral) * 8 + ((alpha) - 'A')]

#define MALLOC_CHECK(object, string) \
    if ((object) == NULL) { \
        fprintf(stderr, "@ %d | Unable to allocate %s!", __LINE__, (string)); \
        exit(EXIT_FAILURE); \
    }


Piece *
piece_create (Player owner, PieceType type, Color color, Position pos) {
    Piece *piece = (Piece *) malloc(sizeof(Piece));
    MALLOC_CHECK(piece, "piece")

    *piece = (Piece) {
        .owner = owner,
        .type = type,
        .color = color,
        .position = pos
    };

    return piece;
}


void
board_initialize (Piece **board, Color human) {
    Color computer = (human == WHITE)? BLACK : WHITE;

    board_at('A', 8) = piece_create(COMPUTER, ROOK,   computer, (Position){'A', 8}); 
    board_at('B', 8) = piece_create(COMPUTER, KNIGHT, computer, (Position){'B', 8});
    board_at('C', 8) = piece_create(COMPUTER, BISHOP, computer, (Position){'C', 8});
    board_at('D', 8) = piece_create(COMPUTER, KING,   computer, (Position){'D', 8});
    board_at('E', 8) = piece_create(COMPUTER, QUEEN,  computer, (Position){'E', 8});
    board_at('F', 8) = piece_create(COMPUTER, BISHOP, computer, (Position){'F', 8});
    board_at('G', 8) = piece_create(COMPUTER, KNIGHT, computer, (Position){'G', 8});
    board_at('H', 8) = piece_create(COMPUTER, ROOK,   computer, (Position){'H', 8});

    for (int i = 0; i < BOARD_SIZE; i++) {
        board_at('A' + i, 7) = piece_create(
            COMPUTER, PAWN, computer, (Position){'A' + i, 7}
        );
    }

    for (int i = 6; i > 2; i--) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board_at('A' + j, i) = piece_create(
                GAMEMASTER, NONE, NEITHER, (Position){'A' + j, i}
            );
        }
    }

    for (int i = 0; i < BOARD_SIZE; i++) {
        board_at('A' + i, 2) = piece_create(
            HUMAN, PAWN, computer, (Position){'A' + i, 2}
        );
    }

    board_at('A', 1) = piece_create(HUMAN, ROOK,   human, (Position){'A', 1}); 
    board_at('B', 1) = piece_create(HUMAN, KNIGHT, human, (Position){'B', 1});
    board_at('C', 1) = piece_create(HUMAN, BISHOP, human, (Position){'C', 1});
    board_at('D', 1) = piece_create(HUMAN, KING,   human, (Position){'D', 1});
    board_at('E', 1) = piece_create(HUMAN, QUEEN,  human, (Position){'E', 1});
    board_at('F', 1) = piece_create(HUMAN, BISHOP, human, (Position){'F', 1});
    board_at('G', 1) = piece_create(HUMAN, KNIGHT, human, (Position){'G', 1});
    board_at('H', 1) = piece_create(HUMAN, ROOK,   human, (Position){'H', 1});
}


bool
position_is_in_board (Position pos) {
    if ((pos.alpha < 'A') || (pos.alpha > 'H')) {
        return false;
    } 

    if ((pos.numeral < 1) || (pos.numeral > 8)) {
        return false;
    }

    return true;
}


Position
position_from (Position position, int x_delta, int y_delta) {
    return (Position) {
        position.alpha + x_delta,
        position.numeral + y_delta 
    };
}


bool
valid_move (Piece **board, Position from, Position to) {
    if (position_is_in_board(to)) {
        Piece *piece_to   = board_at(to.alpha, to.numeral);
        Piece *piece_from = board_at(from.alpha, from.numeral);

        if (piece_to->owner != piece_from->owner) {
            return true; 
        }
    }

    return false;
}


Position *
piece_deltas (Piece **board, Piece *piece, int *size) {
    Position *positions;
            
    Position current_pos = piece->position;
    char alpha = current_pos.alpha;
    int numeral = current_pos.numeral;
    Player owner = piece->owner;

    switch (piece->type) {
        case PAWN: {
            int y_delta = (owner == COMPUTER)? 1 : -1;

            // the pawn's first move
            if ((numeral == 2 && owner == COMPUTER) || (numeral == 7 && HUMAN)) {
                *size = 2;
                positions = (Position *) malloc(sizeof(Position) * 2);
                MALLOC_CHECK(positions, "Position Array")

                positions[0] = position_from(current_pos, 0, y_delta);
                positions[1] = position_from(current_pos, 0, 2 * y_delta);
            } 
            else {
                *size = 1;
                positions = (Position *) malloc(sizeof(Position) * 1);
                MALLOC_CHECK(positions, "Position Array")

                positions[0] = position_from(current_pos, 0, y_delta);
            }
            break;
        } 


        case ROOK: {
            // 14 = max rook moves
            positions = (Position *) malloc(sizeof(Position) * 14);
            MALLOC_CHECK(positions, "Position Array")

            *size = 0;

            // alt is used to alternate the offset "i"
            for (int alt = -1; alt < 2; alt += 2) {
                for (int i = 1; i < 8; i++) {
                    if (position_is_in_board((Position) {alpha + (i * alt), numeral})) {
                        Piece *piece = board_at(alpha + (i * alt), numeral);
                        if (piece->type == NONE) {
                            positions[(*size)++] = (Position) {alpha + (i * alt), numeral};

                            Player opposite = (owner == HUMAN)? COMPUTER : HUMAN; 
                            if (piece->owner == opposite) 
                                break;
                        }
                        else if (piece->owner == owner) {
                            break;
                        }
                    }
                }
            }
           
            for (int alt = -1; alt < 2; alt += 2) {
                for (int i = 1; i < 8; i++) {
                    if (position_is_in_board((Position) {alpha, numeral + (i * alt)})) {
                        Piece *piece = board_at(alpha, numeral + (i * alt));
                        if (piece->type == NONE) {
                            positions[(*size)++] = (Position) {alpha, numeral + (i * alt)}; 

                            Player opposite = (owner == HUMAN)? COMPUTER : HUMAN; 
                            if (piece->owner == opposite) 
                                break;
                        }
                        else if (piece->owner == owner) {
                            break;
                        }
                    }
                }
            }

            positions = realloc(positions, sizeof(Position) * (*size)); 
            break;
        }


        case KNIGHT:
            // 8 = max moves a knight can make
            positions = (Position *) malloc(sizeof(Position) * 8);
            MALLOC_CHECK(positions, "Position Array")

            *size = 0;

            Position possible_positions[8] = {
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
                Position pos = possible_positions[i];
                if (valid_move(board, current_pos, pos)) {
                    positions[(*size)++] = pos;
                }
            }

            positions = (Position *) realloc(positions, sizeof(Position) * (*size));
            break;


        case BISHOP:

            break;


        case KING:

            break;


        case QUEEN:

            break;
    }

    return positions;
}


Position *
piece_spots_available (Piece **board, Piece *piece, int *num_pos) {
    int num_deltas = 0;
    Position *deltas = piece_deltas(board, piece, &num_deltas); 

    Player current_player = piece->owner;

    *num_pos = 0;
    Position positions[16] = {0, 0}; // max number of spots available to a piece

    for (Position *cursor = deltas; num_deltas > 0; cursor++, num_deltas--) {
        if (position_is_in_board(*cursor)) {
            if (board_at(cursor->alpha, cursor->numeral)->owner != current_player) {
                positions[*num_pos] = *cursor; 
                *num_pos = *num_pos + 1;
            }
        }
    }

    Position *filtered_positions = (Position *) malloc(sizeof(Position) * *num_pos);
    MALLOC_CHECK(filtered_positions, "filtered_positions")

    for (int i = 0; i < *num_pos; i++) {
        filtered_positions[i] = positions[i];
    }

    free(deltas);
    return filtered_positions;
}


Piece **
board_create () {
    Piece **board = (Piece **) malloc(sizeof(Piece *) * BOARD_SIZE * BOARD_SIZE);
    MALLOC_CHECK(board, "board")

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

    board_at('D', 5) = piece_create(HUMAN, KNIGHT, WHITE, (Position) {'D', 5});

    board_print(board);

    int num_spots = 0;
    Position *spots = piece_spots_available(
        board, board_at('D', 5), &num_spots
    );

    for (int i = 0; i < num_spots; i++) {
        printf("%c%d ", spots[i].alpha, spots[i].numeral);
    }    
    printf("\n");

    free(spots);

    board_destroy(board);
    return EXIT_SUCCESS;
}


