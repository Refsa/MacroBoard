use crate::TcpStream;
use std::net::{SocketAddr, SocketAddrV4};
use tokio::{
    io::{self, AsyncRead, AsyncReadExt, AsyncWriteExt, BufReader},
    net::tcp::{OwnedReadHalf, OwnedWriteHalf},
    select,
    sync::{broadcast, mpsc},
    time::{sleep, timeout},
};

use crate::packet::Packet;

#[derive(Clone, Copy)]
pub struct Void;

pub enum Event {
    Connected,
    Disconnected,
    Packet(Vec<u8>),
}

pub struct ClientHandle {
    pub shutdown: broadcast::Sender<Void>,
    pub receiver: mpsc::Receiver<Event>,
    pub sender: mpsc::Sender<Vec<u8>>,
}

pub struct Client {
    addr: SocketAddr,
    shutdown: broadcast::Sender<Void>,
    sender: mpsc::Sender<Event>,
    outbound: mpsc::Receiver<Vec<u8>>,
}

impl Client {
    pub fn start(addr: &str) -> ClientHandle {
        let (abort_tx, _) = broadcast::channel(1);
        let (packet_tx, packet_rx) = mpsc::channel(1024);
        let (socket_tx, socket_rx) = mpsc::channel(1024);

        let client = Client {
            addr: SocketAddr::V4(addr.parse().unwrap()),
            shutdown: abort_tx.clone(),
            sender: packet_tx,
            outbound: socket_rx,
        };

        std::thread::spawn(move || client.run());

        ClientHandle {
            shutdown: abort_tx,
            receiver: packet_rx,
            sender: socket_tx,
        }
    }

    #[tokio::main]
    async fn run(self) {
        let mut stream = TcpStream::connect(self.addr).await;
        while let Err(_) = stream {
            stream = TcpStream::connect(self.addr).await;
        }
        let stream = stream.unwrap();

        let (rx, tx) = stream.into_split();

        let _ = self.sender.try_send(Event::Connected);

        tokio::select! {
            _ = tokio::spawn(Self::sender(self.shutdown.subscribe(), self.outbound, tx)) => { },
            _ = tokio::spawn(Self::receiver(self.shutdown.subscribe(), self.sender.clone(), rx)) => { },
        }

        let _ = self.shutdown.send(Void);
        let _ = self.sender.try_send(Event::Disconnected);
    }

    async fn sender(
        mut shutdown: broadcast::Receiver<Void>,
        mut outbound: mpsc::Receiver<Vec<u8>>,
        mut rx: OwnedWriteHalf,
    ) -> io::Result<()> {
        loop {
            select! {
                pkt = outbound.recv() => if let Some(pkt) = pkt {
                    if let Err(err) = rx.write(&pkt).await {
                        println!("Error when sending packet: {:?}", err);
                    }
                },
                void = shutdown.recv() => match void {
                    Ok(_) => break,
                    Err(_) => break,
                },
                else => break
            }
        }

        println!("Sender Shutdown");
        sleep(std::time::Duration::from_millis(250)).await;

        Ok(())
    }

    async fn receiver(
        mut shutdown: broadcast::Receiver<Void>,
        sender: mpsc::Sender<Event>,
        mut rx: OwnedReadHalf,
    ) -> io::Result<()> {
        let mut buf = [0u8; 1024];
        // let mut reader = BufReader::new(rx);

        loop {
            let mut read_count: usize = 0;

            select! {
                msg = timeout(std::time::Duration::from_secs(30), rx.read(&mut buf)) => match msg {
                    Ok(len) => {
                        read_count = len.unwrap_or(0);
                        if read_count == 0 {
                            break;
                        }

                        let _ = sender.send(Event::Packet(buf[..read_count].to_vec())).await;
                    }
                    Err(err) => {
                        println!("Error when sending: {:?}", err);
                    }
                },
                void = shutdown.recv() => match void {
                    Ok(_) => break,
                    Err(_) => break,
                },
                else => break
            }

            if read_count == 0 {
                println!("Remote host most likely disconnected");
                break;
            }
        }

        println!("Receiver Shutdown");
        Ok(())
    }
}
