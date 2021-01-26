use std::io::BufReader;
use serde::{Deserialize, Serialize};

#[repr(C)]
#[derive(Serialize, Deserialize, Debug)]
pub struct StringPayload {
    pub msg: String,
}

impl StringPayload {
    pub fn new(msg: &str) -> Self {
        StringPayload {
            msg: msg.to_string(),
        }
    }
}

impl From<BufReader<&[u8]>> for StringPayload {
    fn from(data: BufReader<&[u8]>) -> Self {
        let des: String = rmp_serde::from_read(data).unwrap();
        StringPayload { msg: des }
    }
}
