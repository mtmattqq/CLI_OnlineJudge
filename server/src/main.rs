use server::ThreadPool;
use std::fs;
use std::io::prelude::*;
use std::net::TcpListener;
use std::net::TcpStream;
use rsa::{
    Pkcs1v15Encrypt, 
    RsaPrivateKey, 
    RsaPublicKey, 
    pkcs1::EncodeRsaPublicKey,
    pkcs1::LineEnding
};
use rand::thread_rng;

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
    let mut rng = thread_rng();
    let bits = 2048;
    let priv_key = RsaPrivateKey::new(&mut rng, bits).expect("Fail to Create Key");
    let pub_key = RsaPublicKey::from(&priv_key);

    stream.write_all(pub_key.to_pkcs1_pem(LineEnding::LF).unwrap().as_bytes()).unwrap();
    stream.flush().unwrap();

    let mut buffer:Vec<u8> = Vec::new();
    stream.read_to_end(&mut buffer).unwrap();

    let dec_data = priv_key.decrypt(Pkcs1v15Encrypt, &buffer).expect("Decrypt Failed");
    let dec_data = String::from_utf8(dec_data).unwrap();

    fs::write("Solve.c", dec_data).unwrap();

    let filename = "output.info";

    let contents = fs::read_to_string(filename).unwrap();

    stream.write_all(contents.as_bytes()).unwrap();
    stream.flush().unwrap();
}