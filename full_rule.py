import random
import copy
import math as np
import time

MAX_BOARD = 20
board = [[0 for i in range(MAX_BOARD)] for j in range(MAX_BOARD)]

# the full-rule based table
# here always assume AI is 1, the opponent is 2 and vacancy is 0
get_points = {
    # ------ 1 ------
    '00000': (1, 1, 1, 1, 1),
    '00001': (1, 2, 2, 2, 0),
    '00010': (2, 2, 2, 0, 2),
    '00011': (3, 4, 4, 0, 0),
    '00100': (2, 2, 0, 2, 2),
    '00101': (3, 4, 0, 4, 0),
    '00110': (4, 4, 0, 0, 4),
    '00111': (6, 7, 0, 0, 0),
    '01000': (2, 0, 2, 2, 2),
    '01001': (3, 0, 4, 4, 0),
    '01010': (4, 0, 4, 0, 4),
    '01011': (5, 0, 7, 0, 0),
    '01100': (4, 0, 0, 4, 4),
    '01101': (6, 0, 0, 7, 0),
    '01110': (7, 0, 0, 0, 7),
    '01111': (8, 0, 0, 0, 0),
    '10000': (0, 2, 2, 2, 1),
    '10001': (0, 3, 3, 3, 0),
    '10010': (0, 4, 4, 0, 3),
    '10011': (0, 5, 6, 0, 0),
    '10100': (0, 4, 0, 4, 3),
    '10101': (0, 6, 0, 6, 0),
    '10110': (0, 7, 0, 0, 6),
    '10111': (0, 8, 0, 0, 0),
    '11000': (0, 0, 4, 4, 3),
    '11001': (0, 0, 6, 5, 0),
    '11010': (0, 0, 7, 0, 5),
    '11011': (0, 0, 8, 0, 0),
    '11100': (0, 0, 0, 7, 6),
    '11101': (0, 0, 0, 8, 0),
    '11110': (0, 0, 0, 0, 8),
    '11111': (0, 0, 0, 0, 0),
    # ------ 2 -------
    '000002': (1, 1, 1, 1, 0),
    '000012': (1, 1, 1, 1, 0),
    '000102': (2, 2, 2, 0, 1),
    '000112': (3, 3, 3, 0, 0),
    '001002': (2, 2, 0, 2, 1),
    '001012': (3, 3, 0, 3, 0),
    '001102': (4, 4, 0, 0, 3),
    '001112': (5, 5, 0, 0, 0),
    '010002': (2, 0, 2, 2, 1),
    '010012': (3, 0, 3, 3, 0),
    '010102': (4, 0, 4, 0, 3),
    '010112': (5, 0, 5, 0, 0),
    '011002': (4, 0, 0, 4, 3),
    '011012': (6, 0, 0, 5, 0),
    '011102': (7, 0, 0, 0, 5),
    '011112': (8, 0, 0, 0, 0),
    '100002': (0, 2, 2, 2, 1),
    '100012': (0, 3, 3, 3, 0),
    '100102': (0, 4, 4, 0, 3),
    '100112': (0, 5, 5, 0, 0),
    '101002': (0, 4, 0, 4, 3),
    '101012': (0, 6, 0, 5, 0),
    '101102': (0, 7, 0, 0, 5),
    '101112': (0, 8, 0, 0, 0),
    '110002': (0, 0, 4, 4, 3),
    '110012': (0, 0, 6, 5, 0),
    '110102': (0, 0, 7, 0, 5),
    '110112': (0, 0, 8, 0, 0),
    '111002': (0, 0, 0, 7, 6),
    '111012': (0, 0, 0, 8, 0),
    '111102': (0, 0, 0, 0, 8),
    '111112': (0, 0, 0, 0, 0),
    # ------ 3 -------

    # ------ 4 -------
    '2000002': (0, 0, 0, 0, 0),
    '2000012': (1, 1, 1, 1, 0),
    '2000102': (1, 1, 1, 0, 1),
    '2000112': (3, 3, 3, 0, 0),
    '2001002': (1, 1, 0, 1, 1),
    '2001012': (3, 3, 0, 3, 0),
    '2001102': (3, 3, 0, 0, 3),
    '2001112': (5, 5, 0, 0, 0),
    '2010002': (1, 0, 1, 1, 1),
    '2010012': (3, 0, 3, 3, 0),
    '2010102': (3, 0, 3, 0, 3),
    '2010112': (5, 0, 5, 0, 0),
    '2011002': (3, 0, 0, 3, 3),
    '2011012': (5, 0, 0, 5, 0),
    '2011102': (5, 0, 0, 0, 5),
    '2011112': (8, 0, 0, 0, 0),
    '2100002': (0, 1, 1, 1, 1),
    '2100012': (0, 3, 3, 3, 0),
    '2100102': (0, 3, 3, 0, 3),
    '2100112': (0, 5, 5, 0, 0),
    '2101002': (0, 3, 0, 3, 3),
    '2101012': (0, 5, 0, 5, 0),
    '2101102': (0, 5, 0, 0, 5),
    '2101112': (0, 8, 0, 0, 0),
    '2110002': (0, 0, 3, 3, 3),
    '2110012': (0, 0, 5, 5, 0),
    '2110102': (0, 0, 5, 0, 5),
    '2110112': (0, 0, 8, 0, 0),
    '2111002': (0, 0, 0, 5, 5),
    '2111012': (0, 0, 0, 8, 0),
    '2111102': (0, 0, 0, 0, 8),
    '2111112': (0, 0, 0, 0, 0),
}


class PointBoard:

    def __init__(self, input_board):
        self.board = copy.deepcopy(input_board)
        pass

    def update_opponent(self):
        # TODO: get the opponent's move and update values
        pass

    def update_my(self, move):
        # TODO: update the values according to my move
        pass