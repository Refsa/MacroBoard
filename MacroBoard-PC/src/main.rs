use tokio::io::AsyncReadExt;
use std::error::Error;
use tokio::io::AsyncWriteExt;
use tokio::net::TcpStream;

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    let addr = "192.168.1.108:8080";
    let mut stream = TcpStream::connect(addr).await?;

    stream.write_all("Hello World".as_bytes()).await?;

    let mut buf = vec![0u8; 1024];

    loop {
        let rec = stream.read(&mut buf).await?;
        println!("{:?}", &buf[..rec]);
    }

    // loop {
    //     let rec = stream.read(&mut buf).await?;

    //     println!("{}", String::from_utf8(buf[..rec].to_vec()).unwrap());

    //     stream.write_all(&buf[..rec]).await?;

    //     std::thread::sleep(std::time::Duration::from_millis(20));
    // }

    Ok(())
}   

#[repr(C)]
struct Packet {
    id: u32,
    payload: Vec<u8>,
}