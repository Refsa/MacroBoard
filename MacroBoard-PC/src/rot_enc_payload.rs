use std::io::BufReader;
use serde::{Deserialize, Serialize};

#[derive(Debug, Serialize, Deserialize)]
pub enum RotaryEncState {
    NONE,
    LEFT(u8),
    RIGHT(u8),
}

impl From<i8> for RotaryEncState {
    fn from(val: i8) -> Self {
        match val {
            x if x < 0 => RotaryEncState::LEFT(val.abs() as u8),
            x if x > 0 => RotaryEncState::RIGHT(val as u8),
            _ => RotaryEncState::NONE,
        }
    }
}

impl From<RotaryEncState> for i8 {
    fn from(state: RotaryEncState) -> Self {
        match state {
            RotaryEncState::NONE => 0,
            RotaryEncState::LEFT(val) => -(val as i8),
            RotaryEncState::RIGHT(val) => val as i8,
        }
    }
}

#[repr(C)]
#[derive(Serialize, Deserialize, Debug)]
pub struct RotaryEncPayload {
    pub value: RotaryEncState,
}

impl RotaryEncPayload {
    pub fn new(value: RotaryEncState) -> Self {
        RotaryEncPayload { value: value }
    }
}

impl From<BufReader<&[u8]>> for RotaryEncPayload {
    fn from(data: BufReader<&[u8]>) -> Self {
        let des: i8 = rmp_serde::from_read(data).unwrap();
        RotaryEncPayload { value: des.into() }
    }
}
