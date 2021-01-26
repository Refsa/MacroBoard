#![feature(toowned_clone_into)]

mod rot_enc_payload;
mod slide_pot_payload;
mod string_payload;

use decode::{read_u32, read_u8};
use encode::{write_i8, write_u8};
use rot_enc_payload::*;
use slide_pot_payload::*;
use string_payload::*;

use num_enum::FromPrimitive;
use num_enum::IntoPrimitive;
use rmp::*;
use serde::{Deserialize, Serialize, __private::de::StrDeserializer, de::value};
use std::{
    error::Error,
    io::{BufReader, BufWriter},
};
use tokio::io::AsyncReadExt;
use tokio::io::AsyncWriteExt;
use tokio::net::TcpStream;

#[repr(u8)]
#[derive(FromPrimitive, IntoPrimitive)]
enum PacketID {
    #[num_enum(default)]
    NIL = 0 as u8,
    BUTTON_PACKET_ID = '1' as u8,
    STRING_PACKET_ID = '2' as u8,
    BITMAP_PACKET_ID = '3' as u8,
    SLIDE_POT_PACKET_ID = '4' as u8,
    ROT_ENC_PACKET_ID = '5' as u8,
    ACK_PACKET_ID = 255 as u8,
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    let addr = "192.168.1.108:8080";
    let mut stream = TcpStream::connect(addr).await?;

    // let auth_pkt = make_pkt(
    //     rmp_serde::to_vec(&StringPayload::new("AUTH 1337").msg).unwrap(),
    //     PacketID::STRING_PACKET_ID,
    // );

    // stream.write_all(&auth_pkt).await?;

    let mut buf = vec![0u8; 1024];

    println!("Starting TCP Client");
    loop {
        let rec = stream.read(&mut buf).await?;
        println!("Received Data:");
        // println!("{:?}", &buf[..rec]);

        let mut reader = BufReader::new(&buf[..rec]);

        let pkt = Packet {
            id: rmp::decode::read_u8(&mut reader).unwrap(),
            uid: rmp::decode::read_u32(&mut reader).unwrap(),
        };

        println!("\t{:?}", pkt);

        handle_pkt(pkt, &mut stream, reader).await;
    }

    Ok(())
}

async fn handle_pkt(pkt: Packet, stream: &mut TcpStream, mut reader: BufReader<&[u8]>) {
    match PacketID::from(pkt.id) {
        PacketID::BUTTON_PACKET_ID => {
            let btn_pld = ButtonPayload {
                button: rmp::decode::read_u8(&mut reader).unwrap(),
                pattern: rmp::decode::read_u8(&mut reader).unwrap(),
            };

            println!("\t{:?}", btn_pld);

            send_ack(pkt.uid, stream).await;
        }
        PacketID::STRING_PACKET_ID => {
            let pld: StringPayload = reader.into();
            println!("\t{}", pld.msg);

            send_ack(pkt.uid, stream).await;
        }
        PacketID::SLIDE_POT_PACKET_ID => {
            let pld: SlidePotPayload = reader.into();
            println!("\t{}", pld.value);

            send_ack(pkt.uid, stream).await;
        }
        PacketID::ROT_ENC_PACKET_ID => {
            let pld: RotaryEncPayload = reader.into();
            println!("\t{:?}", pld);
            send_ack(pkt.uid, stream).await;
        }
        _ => {}
    }
}

async fn send_ack(uid: u32, stream: &mut TcpStream) {
    // std::thread::sleep(std::time::Duration::from_millis(500));

    let ack_pkt = make_pkt(rmp_serde::to_vec(&uid).unwrap(), PacketID::ACK_PACKET_ID);

    let _ = stream.write_all(&ack_pkt).await;
}

fn make_pkt(mut pld: Vec<u8>, packet_id: PacketID) -> Vec<u8> {
    let mut buf = Vec::new();

    let uid = gen_uid();

    let _ = rmp::encode::write_u8(&mut buf, packet_id.into());
    let _ = rmp::encode::write_u32(&mut buf, uid);

    buf.append(&mut pld);

    buf
}

fn gen_uid() -> u32 {
    use rand::Rng;

    let mut rng = rand::thread_rng();
    rng.gen()
}

#[repr(C)]
#[derive(Debug)]
struct Packet {
    id: u8,
    uid: u32,
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

#[repr(C)]
#[derive(Serialize, Deserialize, Debug)]
struct AckPayload {}
