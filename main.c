#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef MAX_DEPTH
#define MAX_DEPTH 2
#endif

#ifndef USEC
#define USEC 250e3
#endif

static char game_array[][3] = {{'_', '_', '_'},
                               {'_', '_', '_'},
                               {'_', '_', '_'}};

static void *bp;
 
static void draw_screen()
{
  int i, j;

  printf("\033[2J\033[1;1H");

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      printf("%c ", game_array[j][i]);
    }
    printf("\n");
  }
}

static void debug_draw(const char array[3][3])
{
  int i, j;
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      printf("%c ", array[j][i]);
    }
    printf("\n");
  }
}

static int test(int col, int row)
{
  if (col > 3 || col < 1) return 1;
  if (row > 3 || row < 1) return 2;
  if (game_array[col - 1][row - 1] != '_') return 3;
  return 0;
}

static void player_move()
{
  int col = 0, row = 0; 
  
  do {
    printf("\nChoose area (eg. '1 2' for column 1, row 2): ");
    scanf("%d %d", &col, &row);
  } while (test(col, row));

  game_array[col - 1][row -1] = 'X';
}

static int check_win_condition(const char array[3][3])
{
  int i, j;
  int condition = 0;
  char player;
  
  /* Vertical scan */
  for (i = 0; i < 3; i++) {
    player = 0;
    
    for (j = 0; j < 3; j++) {
      condition = 0;
      if (j == 0) player = array[i][j];
      if (array[i][j] == '_' || player != array[i][j]) break;
      condition = 1;
    }
    
    if (condition) return player;
  }
  
  /* Horizontal scan */
  for (i = 0; i < 3; i++) {
    player = 0;
    
    for (j = 0; j < 3; j++) {
      condition = 0;
      if (j == 0) player = array[j][i];
      if (array[j][i] == '_' || player != array[j][i]) break;
      condition = 1;
    }
    
    if (condition) return player;
  }

  /* Diagonal scan top-left to bottom-right */
  for (i = 0; i < 3; i++) {
    condition = 0;
    if (i == 0) player = array[i][i];
    if (array[i][i] == '_' || player != array[i][i]) break;
    condition = 1;
  }
  
  if (condition) return player;
  
  /* Diagonal scan top-right to bottom-left */
  for (i = 0; i < 3; i++) {
    condition = 0;
    if (i == 0) player = game_array[i][2 - i];
    if (array[i][2 - i] == '_' || player != array[i][2 - i]) break;
    condition = 1;
  }
  
  if (condition) return player;  
  return 0;
}

static void propagation_state(const char game_test_array[3][3],
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

static int propagate_move(const char game_array[3][3],
                          int depth,
                          char next_player) {
  int i, j, result = 0;
  char curr_player, player, game_test_array[3][3];

  if (depth <= 0) return 0;
  depth--;

  curr_player = next_player;
  if (curr_player == 'X') next_player = 'O';
  else next_player = 'X';

  memcpy(game_test_array, game_array, sizeof(char) * 9);

  player = check_win_condition(game_test_array);
  if (player == 'O') return 2;
  else if (player == 'X') return -1;

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      if (game_test_array[i][j] == '_') {
        game_test_array[i][j] = curr_player;
        result += propagate_move(game_test_array, depth, next_player);
#ifdef DEBUG
        propagation_state(game_test_array, curr_player, i, j, result);
#endif
        game_test_array[i][j] = '_';
      }
    }
  }
  
  return result;  
}

static void computer_move()
{
  int i, j, result, resolved = 0;
  int maxres = 0, maxi = -1, maxj = -1;
  char game_test_array[3][3];

  memcpy(game_test_array, game_array, sizeof(char) * 9);

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      if (game_test_array[i][j] == '_') {
        game_test_array[i][j] = 'O';
        result = propagate_move(game_test_array, MAX_DEPTH, 'X');
        
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

  game_array[maxi][maxj] = 'O';
}

static void game_loop()
{
  char player;
  int move_counter = 9;

  do {
    player_move();
    computer_move();
    draw_screen();
    
    player = check_win_condition(game_array);
    if (player != 0) {
      printf("Player %c wins!\n", player);
      return;
    }

    move_counter-=2;
  } while (move_counter > 0);

  printf("Tie!\n");
}

int main()
{
  /* Get the base pointer of the program. */
  __asm__("mov %%rsp, %0" : "=r"(bp));
  
  draw_screen();
  game_loop();
  exit(0);
}
