Packet Descriptor:

Serializer: MessagePack

Format (ANBF):
	message     = packet-id unique-id packet-data

	packet-id   = 1OCTET 1OCTET   ; First Octet: MsgPack ID

	unqiue-id   = 1OCTET 4OCTET   ; First Octet: MsgPack ID

	packet-data = 1OCTET *[OCTET] ; First Octet: MsgPack ID
