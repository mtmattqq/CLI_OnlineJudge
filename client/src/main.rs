use std::{
    fs,
    net::TcpStream,
    io::{Read, Write},
    env::args
};
use rsa::pkcs1::DecodeRsaPublicKey;
use rsa::{Pkcs1v15Encrypt, RsaPublicKey};
use rand::thread_rng;

fn main() {
    let args: Vec<String> = args().collect();
    
    if args.len() != 2 {
        panic!("Missing the filename");
    }

    let file_name = args[1].clone();

    println!("Connecting to server...");
    let tcp_stream = TcpStream::connect("127.0.0.1:8787");
    match tcp_stream {
        Ok(stream) => {
            handle_connect(stream, file_name);
        },
        Err(e) => {
            println!("Failed to connect: {}", e);
        }
    }
}

fn handle_connect(mut stream: TcpStream, file_name: String) {
    println!("Successfully connected to server");

    println!("Successfully read public key");
    let mut pub_key: [u8; 426] = [0; 426];

    match stream.read_exact(&mut pub_key) {
        Ok(_) => println!("Successfully read public key"),
        Err(e) => {
            eprintln!("Failed to read public key: {}", e);
            return
        },
    }

    println!("Geting the problem list...");
    let mut problem_list:Vec<u8> = Vec::new();
    let mut problem_buf: [u8; 1] = [0; 1];
    loop {
        stream.read_exact(&mut problem_buf).unwrap();
        problem_list.extend_from_slice(&problem_buf);
        if String::from_utf8(problem_list.to_vec()).unwrap().ends_with("END") {
            break;
        }
    }

    println!("Problem List\n");
    let problem_list = String::from_utf8(problem_list.to_vec()).unwrap();
    let mut line_num = 1;
    for line in problem_list.lines() {
        if line == "END" {
            break;
        }
        println!("    {}: {}", line_num, line);
        line_num += 1;
    }

    println!("Enter problem number");
    let mut problem_id = String::new();
    std::io::stdin().read_line(&mut problem_id)
        .expect("File to read problem id");
    let problem_id: u32 = problem_id
        .trim().parse()
        .expect("Problem id should be an integer");

    if problem_id <= 0 || problem_id >= problem_list.len().try_into().unwrap() {
        println!("Problem id not found");
    }

    println!("Sending problem id to server");
    stream.write_all(&problem_id.to_be_bytes()).unwrap();
    stream.flush().unwrap();

    println!("Encrypting data...");

    let pub_key = String::from_utf8(pub_key.to_vec()).unwrap();
    let pub_key = RsaPublicKey::from_pkcs1_pem(pub_key.as_str()).unwrap();

    let mut rng = thread_rng();
    let msg = fs::read_to_string(file_name).unwrap();
    let enc_data = pub_key.encrypt(&mut rng, Pkcs1v15Encrypt, &msg.as_bytes()).expect("Encrypt Failed");

    stream.write_all(&enc_data).unwrap();
    stream.flush().unwrap();
    stream.shutdown(std::net::Shutdown::Write).unwrap();
    println!("Sent the code and problem id, awaiting reply...");

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