use num_enum::FromPrimitive;
use num_enum::IntoPrimitive;
use serde::{de::value::StringDeserializer, Deserialize, Serialize};
use std::{ascii, convert::TryFrom, error::Error};
use tokio::io::AsyncReadExt;
use tokio::io::AsyncWriteExt;
use tokio::net::TcpStream;

#[repr(u8)]
#[derive(FromPrimitive, IntoPrimitive)]
enum PacketID {
    #[num_enum(default)]
    NIL = '0' as u8,
    BUTTON_PACKET_ID = '1' as u8,
    STRING_PACKET_ID = '2' as u8,
    BITMAP_PACKET_ID = '3' as u8,
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    let addr = "192.168.1.108:8080";
    let mut stream = TcpStream::connect(addr).await?;

    stream.write_all(&string_pkt("AUTH 1337")).await?;

    let mut buf = vec![0u8; 1024]; 

    println!("Starting TCP Client");
    loop {
        let rec = stream.read(&mut buf).await?;

        println!("{:?}", &buf[..rec]);

        let pkt = Packet {
            id: buf[0],
            payload: buf[1..rec].to_vec(),
        };

        match PacketID::from(pkt.id) {
            PacketID::BUTTON_PACKET_ID => {
                let btn_pld = ButtonPayload {
                    button: pkt.payload[0],
                    pattern: pkt.payload[1],
                };

                println!("{:?}", btn_pld);
            }
            PacketID::STRING_PACKET_ID => {
                let msg = String::from_utf8_lossy(&pkt.payload);
                println!("{}", msg);
            }
            _ => {}
        }
    }

    Ok(())
}

fn string_pkt(msg: &str) -> Vec<u8> {
    [&[PacketID::STRING_PACKET_ID.into()], msg.as_bytes()].concat()
}

#[repr(C)]
#[derive(Serialize, Deserialize, Debug)]
struct Packet {
    id: u8,
    payload: Vec<u8>,
}

#[repr(C)]
#[derive(Serialize, Deserialize, Debug)]
struct ButtonPayload {
    button: u8,
    pattern: u8,
}

#[repr(C)]
#[derive(Serialize, Deserialize, Debug)]
struct BitmapPayload {
    width: i16,
    height: i16,
    data: Vec<u8>,
}
