use server::ThreadPool;
use std::{
    fs,
    io::prelude::*,
    net::TcpListener,
    net::TcpStream,
    process::Command,
};
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


    let problem_list = fs::read_to_string("./problems/problems.info").unwrap();
    stream.write_all(problem_list.as_bytes()).unwrap();
    stream.write(b"END").unwrap();
    stream.flush().unwrap();

    let mut problem_id_buf: [u8; 4] = [0; 4];
    stream.read_exact(&mut problem_id_buf).unwrap();
    let mut problem_id: u32 = 0;
    for i in problem_id_buf.iter().clone() {
        problem_id = (problem_id << 8) + u32::from(*i);
    }
    println!("Problem {}", problem_id);

    let mut buffer:Vec<u8> = Vec::new();
    stream.read_to_end(&mut buffer).unwrap();

    let dec_data = priv_key.decrypt(Pkcs1v15Encrypt, &buffer).expect("Decrypt Failed");
    let dec_data = String::from_utf8(dec_data).unwrap();

    let user_code_path = problem_list.lines().nth((problem_id - 1).try_into().unwrap()).unwrap();
    let mut path = String::from("./problems/");
    path.push_str(user_code_path);
    path.push_str("/Solve.cpp");
    fs::write(path, dec_data).unwrap();

    let mut run_path = String::from("./problems/");
    run_path.push_str(user_code_path);
    run_path.push_str("/Run");

    let output = Command::new(run_path).output().unwrap();
    let output = output.stdout;

    stream.write_all(&output[..]).unwrap();
    stream.flush().unwrap();
}