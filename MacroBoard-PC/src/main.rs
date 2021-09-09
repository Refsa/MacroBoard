#![feature(toowned_clone_into)]

mod button_payload;
mod client;
mod packet;
mod rot_enc_payload;
mod slide_pot_payload;
mod string_payload;

use crate::client::Client;
use crate::client::ClientHandle;
use crate::client::Event;
use crate::client::Void;
use button_payload::*;
use packet::*;
use rot_enc_payload::*;
use slide_pot_payload::*;
use string_payload::*;

use serde::{Deserialize, Serialize};
use std::io::BufReader;
use tokio::net::TcpStream;

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
        Event::Connected => {
            println!("Connected to server");
        }
        Event::Disconnected => {
            println!("Disconnected from server");
        }
        _ => {}
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
        PacketID::NIL => {}
        PacketID::BITMAP_PACKET_ID => {}
        PacketID::KEEP_ALIVE_PACKET_ID => {
            println!("Keep Alive");
            send_ack(pkt.uid, client_handle).await;
        }
        PacketID::ACK_PACKET_ID => {}
    }
}

async fn send_ack(uid: u32, client_handle: &mut ClientHandle) {
    // std::thread::sleep(std::time::Duration::from_millis(500));

    let ack_pkt = make_pkt(rmp_serde::to_vec(&uid).unwrap(), PacketID::ACK_PACKET_ID);
    let _ = client_handle.sender.send(ack_pkt).await;

    // let _ = stream.write_all(&ack_pkt).await;
}

#[repr(C)]
#[derive(Serialize, Deserialize, Debug)]
struct BitmapPayload {
    width: i16,
    height: i16,
    data: Vec<u8>,
}
