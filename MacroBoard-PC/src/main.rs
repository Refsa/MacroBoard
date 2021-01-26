#![feature(toowned_clone_into)]

mod button_payload;
mod packet;
mod rot_enc_payload;
mod slide_pot_payload;
mod string_payload;
mod client;

use crate::client::Void;
use crate::client::ClientHandle;
use crate::client::Client;
use crate::client::Event;
use button_payload::*;
use packet::*;
use rot_enc_payload::*;
use slide_pot_payload::*;
use string_payload::*;

use num_enum::FromPrimitive;
use num_enum::IntoPrimitive;
use serde::{Deserialize, Serialize};
use std::{error::Error, io::BufReader};
use tokio::io::AsyncReadExt;
use tokio::io::AsyncWriteExt;
use tokio::net::TcpStream;

#[allow(non_camel_case_types)]
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
async fn main() {
    let mut client: ClientHandle = Client::start("192.168.1.108:8080");

    if let Some(Event::Connected) = client.receiver.recv().await {
        println!("Connected to server");
    }

    loop {
        match client.receiver.recv().await {
            Some(event) => {
                handle_event(event, &mut client).await;
            }
            None => {}
        }
    }

    let _ = client.shutdown.send(Void);


    // let auth_pkt = make_pkt(
    //     rmp_serde::to_vec(&StringPayload::new("AUTH 1337").msg).unwrap(),
    //     PacketID::STRING_PACKET_ID,
    // );
    // stream.write_all(&auth_pkt).await?;

    // let mut buf = vec![0u8; 1024];

    // println!("Starting TCP Client");
    // loop {
    //     let rec = stream.read(&mut buf).await?;
    //     println!("Received Data:");
    //     // println!("{:?}", &buf[..rec]);

    //     let mut reader = BufReader::new(&buf[..rec]);

    //     let pkt = Packet {
    //         id: rmp::decode::read_u8(&mut reader).unwrap(),
    //         uid: rmp::decode::read_u32(&mut reader).unwrap(),
    //     };

    //     println!("\t{:?}", pkt);

    //     handle_pkt(pkt, &mut stream, reader).await;
    // }

    // Ok(())
}

async fn handle_event(event: Event, client_handle: &mut ClientHandle) {
    match event {
        Event::Packet(pkt) => {
            let pkt = &pkt[..];
            let mut reader = BufReader::new(pkt);
            
            let pkt = Packet {
                id: rmp::decode::read_u8(&mut reader).unwrap(),
                uid: rmp::decode::read_u32(&mut reader).unwrap(),
            };

            handle_pkt(pkt, client_handle, reader).await;
        }
        Event::Disconnected => {
            println!("Disconnected from server");
        }
        _ => {},
    }
}

async fn handle_pkt(pkt: Packet, client_handle: &mut ClientHandle, mut reader: BufReader<&[u8]>) {
    match PacketID::from(pkt.id) {
        PacketID::BUTTON_PACKET_ID => {
            let btn_pld = ButtonPayload {
                button: rmp::decode::read_u8(&mut reader).unwrap(),
                pattern: rmp::decode::read_u8(&mut reader).unwrap(),
            };

            println!("\t{:?}", btn_pld);

            send_ack(pkt.uid, client_handle).await;
        }
        PacketID::STRING_PACKET_ID => {
            let pld: StringPayload = reader.into();
            println!("\t{}", pld.msg);

            send_ack(pkt.uid, client_handle).await;
        }
        PacketID::SLIDE_POT_PACKET_ID => {
            let pld: SlidePotPayload = reader.into();
            println!("\t{}", pld.value);

            send_ack(pkt.uid, client_handle).await;
        }
        PacketID::ROT_ENC_PACKET_ID => {
            let pld: RotaryEncPayload = reader.into();
            println!("\t{:?}", pld);
            send_ack(pkt.uid, client_handle).await;
        }
        _ => {}
    }
}

async fn send_ack(uid: u32, client_handle: &mut ClientHandle) {
    // std::thread::sleep(std::time::Duration::from_millis(500));

    let ack_pkt = make_pkt(rmp_serde::to_vec(&uid).unwrap(), PacketID::ACK_PACKET_ID);
    let _ = client_handle.sender.send(ack_pkt).await;

    // let _ = stream.write_all(&ack_pkt).await;
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
#[derive(Serialize, Deserialize, Debug)]
struct BitmapPayload {
    width: i16,
    height: i16,
    data: Vec<u8>,
}
