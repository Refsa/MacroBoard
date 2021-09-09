use serde::{Deserialize, Serialize};
use num_enum::FromPrimitive;
use num_enum::IntoPrimitive;

#[allow(non_camel_case_types)]
#[repr(u8)]
#[derive(FromPrimitive, IntoPrimitive)]
pub enum PacketID {
    #[num_enum(default)]
    NIL = 0 as u8,
    BUTTON_PACKET_ID = '1' as u8,
    STRING_PACKET_ID = '2' as u8,
    BITMAP_PACKET_ID = '3' as u8,
    SLIDE_POT_PACKET_ID = '4' as u8,
    ROT_ENC_PACKET_ID = '5' as u8,
    KEEP_ALIVE_PACKET_ID = 254 as u8,
    ACK_PACKET_ID = 255 as u8,
}

#[repr(C)]
#[derive(Debug)]
pub struct Packet {
    pub id: u8,
    pub uid: u32,
}

#[repr(C)]
#[derive(Serialize, Deserialize, Debug)]
pub struct AckPayload {}

pub fn make_pkt(mut pld: Vec<u8>, packet_id: PacketID) -> Vec<u8> {
    let mut buf = Vec::new();

    let uid = gen_uid();

    let _ = rmp::encode::write_u8(&mut buf, packet_id.into());
    let _ = rmp::encode::write_u32(&mut buf, uid);

    buf.append(&mut pld);

    buf
}

pub fn gen_uid() -> u32 {
    use rand::Rng;

    let mut rng = rand::thread_rng();
    rng.gen()
}