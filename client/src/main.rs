use std::alloc::handle_alloc_error;
use std::net::TcpStream;
use std::io::{Read, Write};

fn main() {
    let tcp_stream = TcpStream::connect("127.0.0.1:8787");
    match tcp_stream {
        Ok(stream) => {
            handle_connect(stream);
        },
        Err(e) => {
            println!("Failed to connect: {}", e);
        }
    }
}

fn handle_connect(mut stream: TcpStream) {
    println!("Successfully connected to server");

    let msg = b"Hello!";

    stream.write_all(msg).unwrap();
    stream.flush().unwrap();
    println!("Sent Hello, awaiting reply...");
    
    stream.shutdown(std::net::Shutdown::Write).unwrap();

    let mut data: String = String::new(); // using 6 byte buffer
    match stream.read_to_string(&mut data) {
        Ok(_) => {
            println!("Reply: {}", data);
        },
        Err(e) => {
            println!("Failed to receive data: {}", e);
        }
    }
}