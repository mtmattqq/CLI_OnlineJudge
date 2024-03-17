use std::fs;
use std::net::TcpStream;
use std::io::{Read, Write};
use rsa::pkcs1::DecodeRsaPublicKey;
use rsa::{Pkcs1v15Encrypt, RsaPublicKey};
use rand::thread_rng;

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

    let mut pub_key: [u8; 426] = [0; 426];

    match stream.read_exact(&mut pub_key) {
        Ok(_) => println!("Successfully read public key."),
        Err(e) => eprintln!("Failed to read public key: {}", e),
    }

    let pub_key = String::from_utf8(pub_key.to_vec()).unwrap();
    let pub_key = RsaPublicKey::from_pkcs1_pem(pub_key.as_str()).unwrap();

    let mut rng = thread_rng();
    let msg = fs::read_to_string("main.c").unwrap();
    let enc_data = pub_key.encrypt(&mut rng, Pkcs1v15Encrypt, &msg.as_bytes()).expect("Encrypt Failed");

    stream.write_all(&enc_data).unwrap();
    stream.flush().unwrap();
    stream.shutdown(std::net::Shutdown::Write).unwrap();
    println!("Sent Hello, awaiting reply...");

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