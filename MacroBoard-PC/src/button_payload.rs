use serde::{Deserialize, Serialize};

#[repr(C)]
#[derive(Serialize, Deserialize, Debug)]
pub struct ButtonPayload {
    pub button: u8,
    pub pattern: u8,
}