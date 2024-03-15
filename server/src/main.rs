use server::ThreadPool;
// use std::arch::x86_64::_mm_bitshuffle_epi64_mask;
use std::fs;
use std::io::prelude::*;
use std::net::TcpListener;
use std::net::TcpStream;
use std::thread;
use std::time::Duration;

fn main() {
    let listener = TcpListener::bind("127.0.0.1:8787").unwrap();
    let pool = ThreadPool::new(4);

    for stream in listener.incoming() {
        let stream = stream.unwrap();

        pool.execute(|| {
            handle_connection(stream);
        });
    }

    println!("Shutting down.");
}

fn handle_connection(mut stream: TcpStream) {
    let mut buffer:Vec<u8> = Vec::new();
    stream.read_to_end(&mut buffer).unwrap();

    let buffer = String::from_utf8(buffer).unwrap();
    println!("{}", buffer);

    let filename = "output.info";

    let contents = fs::read_to_string(filename).unwrap();

    stream.write_all(contents.as_bytes()).unwrap();
    stream.flush().unwrap();
}