use serde::{Deserialize, Serialize};

#[repr(C)]
#[derive(Debug)]
pub struct Packet {
    pub id: u8,
    pub uid: u32,
}

#[repr(C)]
#[derive(Serialize, Deserialize, Debug)]
pub struct AckPayload {}
