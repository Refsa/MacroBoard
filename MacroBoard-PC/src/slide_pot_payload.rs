use std::borrow::Cow;
use std::io::BufReader;

use serde::{Deserialize, Serialize};
#[repr(C)]
#[derive(Serialize, Deserialize, Debug)]
pub struct SlidePotPayload {
    pub value: f32,
}

impl SlidePotPayload {
    pub fn new(value: f32) -> Self {
        SlidePotPayload { value: value }
    }
}

impl From<BufReader<&[u8]>> for SlidePotPayload {
    fn from(data: BufReader<&[u8]>) -> Self {
        let des: f32 = rmp_serde::from_read(data).unwrap();
        SlidePotPayload { value: des }
    }
}
