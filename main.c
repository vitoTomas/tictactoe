#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#ifndef MAX_DEPTH
#define MAX_DEPTH 2
#endif

#ifndef USEC
#define USEC 250e3
#endif

#ifndef DIM
#define DIM 3
#endif

#define POSITIVE 4
#define NEGATIVE -1

static char game_array[DIM][DIM];

static void *bp;

inline static void initialize_area()
{
  memset(game_array, '_', DIM  * DIM);
}
 
static void draw_screen()
{
  int i, j;

  printf("\033[2J\033[1;1H");

  for (i = 0; i < DIM; i++) {
    for (j = 0; j < DIM; j++) {
      printf("%c ", game_array[j][i]);
    }
    printf("\n");
  }
}

static void debug_draw(const char array[DIM][DIM])
{
  int i, j;
  for (i = 0; i < DIM; i++) {
    for (j = 0; j < DIM; j++) {
      printf("%c ", array[j][i]);
    }
    printf("\n");
  }
}

static int test(int col, int row)
{
  if (col > DIM || col < 1) return 1;
  if (row > DIM || row < 1) return 2;
  if (game_array[col - 1][row - 1] != '_') return 3;
  return 0;
}

static void player_move(char player)
{
  int col = 0, row = 0; 
  
  do {
    printf("\nChoose area (eg. '1 2' for column 1, row 2): ");
    scanf("%d %d", &col, &row);
  } while (test(col, row));

  game_array[col - 1][row - 1] = player;
}

static int check_win_condition(const char array[DIM][DIM])
{
  int i, j;
  int condition = 0;
  char player;
  
  /* Vertical scan */
  for (i = 0; i < DIM; i++) {
    player = 0;
    
    for (j = 0; j < DIM; j++) {
      condition = 0;
      if (j == 0) player = array[i][j];
      if (array[i][j] == '_' || player != array[i][j]) break;
      condition = 1;
    }
    
    if (condition) return player;
  }
  
  /* Horizontal scan */
  for (i = 0; i < DIM; i++) {
    player = 0;
    
    for (j = 0; j < DIM; j++) {
      condition = 0;
      if (j == 0) player = array[j][i];
      if (array[j][i] == '_' || player != array[j][i]) break;
      condition = 1;
    }
    
    if (condition) return player;
  }

  /* Diagonal scan top-left to bottom-right */
  for (i = 0; i < DIM; i++) {
    condition = 0;
    if (i == 0) player = array[i][i];
    if (array[i][i] == '_' || player != array[i][i]) break;
    condition = 1;
  }
  
  if (condition) return player;
  
  /* Diagonal scan top-right to bottom-left */
  for (i = 0; i < DIM; i++) {
    condition = 0;
    if (i == 0) player = game_array[i][DIM - 1 - i];
    if (array[i][DIM - 1 - i] == '_' || player != array[i][DIM - 1 - i]) break;
    condition = 1;
  }
  
  if (condition) return player;  
  return 0;
}

static void propagation_state(const char game_test_array[DIM][DIM],
                              char curr_player,
                              int i,
                              int j,
                              int result)
{
  void *sp;
  __asm__("mov %%rsp, %0" : "=r" (sp));

  printf("\033[2J\033[1;1H");
  draw_screen();
  printf("\n*************************\n");
  printf("Player %c makes move: %d %d\n", curr_player, i + 1, j + 1);
  printf("Result: %d\n", result);
  printf("Stack memory usage: %ld B\n", bp - sp);
  debug_draw(game_test_array);
  usleep(USEC);
}

static int propagate_move(const char game_array[DIM][DIM],
                          int depth,
                          char next_player,
                          char root_player) {
  int i, j, result = 0;
  char curr_player, player, game_test_array[DIM][DIM];

  if (depth <= 0) return 0;
  depth--;

  curr_player = next_player;
  if (curr_player == 'X') next_player = 'O';
  else next_player = 'X';

  memcpy(game_test_array, game_array, sizeof(char) * (DIM * DIM));

  player = check_win_condition(game_test_array);
  if (player == root_player) return POSITIVE;
  else if (player != 0) return NEGATIVE;

  for (i = 0; i < DIM; i++) {
    for (j = 0; j < DIM; j++) {
      if (game_test_array[i][j] == '_') {
        game_test_array[i][j] = curr_player;
        result += propagate_move(game_test_array,
                                 depth,
                                 next_player,
                                 root_player);
#ifdef DEBUG
        propagation_state(game_test_array, curr_player, i, j, result);
#endif
        game_test_array[i][j] = '_';
      }
    }
  }
  
  return result;  
}

static void make_random_move(char player)
{
  int i, j;

  do {
    i = rand() % DIM;
    j = rand() % DIM;
  } while (test(i + 1, j + 1));

  game_array[i][j] = player;
}

static void computer_move(char player, int move_counter)
{
  int i, j, result, resolved = 0;
  int maxres = 0, maxi = -1, maxj = -1;
  char game_test_array[DIM][DIM], next_player;

  if (((DIM * DIM) - move_counter) <= 1) {
    make_random_move(player);
    return;
  }

  memcpy(game_test_array, game_array, sizeof(char) * (DIM * DIM));

  for (i = 0; i < DIM; i++) {
    for (j = 0; j < DIM; j++) {
      if (game_test_array[i][j] == '_') {
        game_test_array[i][j] = player;
        if (player == 'O') next_player = 'X';
        else next_player = 'O';
        //player == 'O' ? (next_player = 'X') : (next_player = 'O');
        result = propagate_move(game_test_array,
                                MAX_DEPTH,
                                next_player,
                                player);
        
        if (maxi == -1) {
          maxi = i;
          maxj = j;
          maxres = result;
        }
        
        if (result > maxres) {
          maxres = result;
          maxi = i;
          maxj = j;
        }

        game_test_array[i][j] = '_';
      }
    }
  }

  game_array[maxi][maxj] = player;
}

static void game_loop(int game_mode)
{
  char player;
  int move_counter = DIM * DIM;

  do {
    switch (game_mode) {
      case 1:
        draw_screen();
        player_move('X');
        computer_move('O', move_counter);
        draw_screen();
        break;
      case 2:
        computer_move('X', move_counter);
        draw_screen();
        player_move('O');
        break;
      case 3:
        computer_move('X', move_counter);
        draw_screen();
        usleep(500e3);
        computer_move('O', move_counter);
        draw_screen();
        usleep(500e3);
    }

    player = check_win_condition(game_array);
    if (player != 0) {
      printf("Player %c wins!\n", player);
      return;
    }

    move_counter -= 2;
  } while (move_counter > 0);

  printf("Tie!\n");
}

int main()
{
  int game_mode;
  /* Get the base pointer of the program. */
  __asm__("mov %%rsp, %0" : "=r"(bp));

  printf("Choose game mode:"
         "\n* 1 - player v computer"
         "\n* 2 - computer v player"
         "\n* 3 - computer v computer\n");
  scanf("%d", &game_mode);
  if (game_mode < 1 || game_mode > 3) {
    printf("Incorrect game mode!\n");
    exit(1);
  }

  initialize_area();
  srand(time(NULL));
  //draw_screen();
  game_loop(game_mode);
  exit(0);
}
