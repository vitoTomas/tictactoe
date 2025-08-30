use core::str::SplitN;
use envparse::parse_env;
use std::io::{self, Write};
use std::{thread, time};

mod tictachelpers;

const SIZE: usize = parse_env!("FIELD_SIZE" as usize else 3);

static MENU: &str = "Choose game mode:\n \
                    * 1 - player v computer\n \
                    * 2 - computer v player\n \
                    * 3 - computer v computer";

fn draw_screen(array: &[[char; SIZE]; SIZE]) -> () {
    print!("\x1B[2J\x1B[1;1H");

    for i in 0..SIZE {
        for j in 0..SIZE {
            print!(" {}", array[j][i]);
        }
        println!();
    }
}

fn test(col: usize, row: usize, array: &[[char; SIZE]; SIZE]) -> bool {
    if col < 1 || col > SIZE {
        return false;
    }

    if row < 1 || row > SIZE {
        return false;
    }

    if array[col - 1][row - 1] != '_' {
        return false;
    }

    return true;
}

fn check_win_condition(array: [[char; SIZE]; SIZE]) -> char {
    let mut condition: bool = false;
    let mut player: char = 0 as char;

    // Vertical scan
    for i in 0..SIZE {
        player = 0 as char;

        for j in 0..SIZE {
            condition = false;
            if j == 0 {
                player = array[i][j];
            }
            if array[i][j] == '_' || player != array[i][j] {
                break;
            }
            condition = true;
        }

        if condition {
            return player;
        }
    }

    // Horizontal scan
    for i in 0..SIZE {
        player = 0 as char;

        for j in 0..SIZE {
            condition = false;
            if j == 0 {
                player = array[j][i];
            }
            if array[j][i] == '_' || player != array[j][i] {
                break;
            }
            condition = true;
        }

        if condition {
            return player;
        }
    }

    // Diagonal scan top-left to bottom-right
    for i in 0..SIZE {
        condition = false;
        if i == 0 {
            player = array[i][i];
        }
        if array[i][i] == '_' || player != array[i][i] {
            break;
        }
        condition = true;
    }

    if condition {
        return player;
    }

    // Diagonal scan top-right to bottom-left
    for i in 0..SIZE {
        condition = false;
        if i == 0 {
            player = array[i][SIZE - 1 - i];
        }
        if array[i][SIZE - 1 - i] == '_' || player != array[i][SIZE - 1 - i] {
            break;
        }
        condition = true;
    }

    if condition {
        return player;
    }
    return 0 as char;
}

fn propagate_move(
    game_array: [[char; SIZE]; SIZE],
    mut depth: i32,
    mut next_player: char,
    root_player: char,
) -> i32 {
    let mut result: i32 = 0;
    let curr_player: char;
    let player: char;
    let mut game_test_array: [[char; SIZE]; SIZE] = [[' '; SIZE]; SIZE];

    if depth <= 0 {
        return 0;
    }
    depth -= 1;

    curr_player = next_player;
    if curr_player == 'X' {
        next_player = 'O';
    } else {
        next_player = 'X';
    }

    game_test_array.copy_from_slice(&game_array);

    player = check_win_condition(game_test_array);
    if player == root_player {
        return 1;
    } else if player != 0 as char {
        return -1;
    }

    for i in 0..SIZE {
        for j in 0..SIZE {
            if game_test_array[i][j] == '_' {
                game_test_array[i][j] = curr_player;
                result += propagate_move(game_test_array, depth, next_player, root_player);

                game_test_array[i][j] = '_';
            }
        }
    }

    return result;
}

fn computer_move(player: char, field: &mut [[char; SIZE]; SIZE]) -> () {
    let mut game_test_array: [[char; SIZE]; SIZE] = [[' '; SIZE]; SIZE];
    let mut result: i32;
    let mut maxres: i32 = 0;
    let mut maxi: i32 = -1;
    let mut maxj: i32 = -1;
    let mut next_player: char;

    game_test_array.copy_from_slice(field);

    for i in 0..SIZE {
        for j in 0..SIZE {
            if game_test_array[i][j] == '_' {
                game_test_array[i][j] = player;
                if player == 'O' {
                    next_player = 'X';
                } else {
                    next_player = 'O';
                }

                result = propagate_move(game_test_array, 2, next_player, player);

                if maxi == -1 {
                    maxi = i as i32;
                    maxj = j as i32;
                    maxres = result;
                }

                if result > maxres {
                    maxres = result;
                    maxi = i as i32;
                    maxj = j as i32;
                }

                game_test_array[i][j] = '_';
            }
        }
    }

    if maxi != -1 {
        field[maxi as usize][maxj as usize] = player;
    }
}

fn player_move(player: char, field: &mut [[char; SIZE]; SIZE]) -> () {
    let mut col: usize;
    let mut row: usize;
    let mut ret: Result<_, _>;

    loop {
        print!("Choose area (eg. '1 2' for column 1, row 2): ");

        ret = io::stdout().flush();
        if ret.is_err() {
            panic!("Could not flush buffer!");
        }

        position_choice!(col, row);
        if test(col, row, field) {
            break;
        }
    }

    field[col - 1][row - 1] = player;
}

fn game_loop(game_mode: u32) -> () {
    let mut field: [[char; SIZE]; SIZE] = [['_'; SIZE]; SIZE];
    let mut player: char;
    let mut move_counter: i32 = (SIZE * SIZE) as i32;
    let duration = time::Duration::from_millis(500);

    while move_counter > 0 {
        match game_mode {
            1 => {
                draw_screen(&field);
                player_move('X', &mut field);
                computer_move('O', &mut field);
                draw_screen(&field);
            }
            2 => {
                computer_move('X', &mut field);
                draw_screen(&field);
                player_move('O', &mut field);
            }
            3 => {
                computer_move('X', &mut field);
                draw_screen(&field);
                thread::sleep(duration);

                computer_move('O', &mut field);
                draw_screen(&field);
                thread::sleep(duration);
            }
            _ => {
                println!("Unexpected game mode!");
                return;
            }
        }

        player = check_win_condition(field);
        if player != 0 as char {
            println!("Player {} wins!", player);
            return;
        }

        move_counter -= 2;
    }

    println!("Tie!");
}

fn main() -> () {
    let game_mode: u32;
    let mut buffer: String = String::new();
    let retres: Result<_, _>;

    println!("{}", MENU);

    readln!(&mut buffer);
    retres = tictachelpers::parse_to_i32(&buffer);
    if retres.is_err() {
        panic!("Failed to parse to i32!");
    }

    game_mode = retres.unwrap() as u32;

    if game_mode < 1 || game_mode > 3 {
        println!("Incorrect game mode!");
        return;
    }

    game_loop(game_mode);
}
