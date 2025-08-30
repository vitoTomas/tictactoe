#[macro_export]
macro_rules! readln {
    ($buff: expr) => {
        let retres: Result<_, _>;

        retres = io::stdin().read_line($buff);
        if retres.is_err() {
            panic!("Could not read from STDIN, ending program!");
        }
    };
}

#[macro_export]
macro_rules! position_choice {
    ($col: ident, $row: ident) => {
        let mut buffer: String = String::new();
        let mut parts: SplitN<_>;
        let mut retopt: Option<_>;
        let mut retres: Result<_, _>;

        readln!(&mut buffer);
        parts = buffer.splitn(2, ' ');

        retopt = parts.next();
        if retopt.is_none() {
            println!("Expected the first value!");
            continue;
        }

        retres = retopt.unwrap().trim().parse();
        if retres.is_err() {
            println!("Expected a number defining column!");
            continue;
        }

        $col = retres.unwrap();

        retopt = parts.next();
        if retopt.is_none() {
            println!("Expected the second value!");
            continue;
        }

        retres = retopt.unwrap().trim().parse();
        if retres.is_err() {
            println!("Expected a number defining row!");
            continue;
        }

        $row = retres.unwrap();
    };
}

pub fn parse_to_i32(buff: &String) -> Result<i32, ()> {
    let value: i32;
    let retres: Result<i32, _>;

    retres = buff.trim().parse();
    if retres.is_err() {
        return Err(());
    }

    value = buff.trim().parse().unwrap();

    return Ok(value);
}
