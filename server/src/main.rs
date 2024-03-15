use server::ThreadPool;
use std::fs;
use std::io::prelude::*;
use std::net::TcpListener;
use std::net::TcpStream;
use rsa::{Pkcs1v15Encrypt, RsaPrivateKey, RsaPublicKey};
use rand::thread_rng;

fn main() {
    // let listener = TcpListener::bind("127.0.0.1:8787").unwrap();
    // let pool = ThreadPool::new(4);

    // for stream in listener.incoming() {
    //     let stream = stream.unwrap();

    //     pool.execute(|| {
    //         handle_connection(stream);
    //     });
    // }

    let mut rng = thread_rng();
    let bits = 2048;
    let priv_key = RsaPrivateKey::new(&mut rng, bits).expect("Fail to Create Key");
    let pub_key = RsaPublicKey::from(&priv_key);

    let data = b"hello world";

    let enc_data = pub_key.encrypt(&mut rng, Pkcs1v15Encrypt, &data[..]).expect("Encrypt Failed");
    assert_ne!(&data[..], &enc_data[..]);

    let dec_data = priv_key.decrypt(Pkcs1v15Encrypt, &enc_data).expect("Decrypt Failed");
    assert_eq!(&data[..], &dec_data[..]);

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