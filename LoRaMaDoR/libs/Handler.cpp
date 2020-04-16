// LoRaMaDoR (LoRa-based mesh network for hams) project
// Copyright (c) 2019 PU5EPX

#include <string.h>
#include "Handler.h"

Ptr<Packet> Ping::handle(const Packet &pkt, const Callsign &me)
{
	if ((!pkt.to().isQ() || pkt.to().is_localhost()) && pkt.params().has("PING")) {
		Params pong = Params();
		pong.put("PONG", None);
		return new Packet(pkt.from(), me, pkt.ident(), pong, pkt.msg());
	}
	return 0;
}

Ptr<Packet> Rreq::handle(const Packet &pkt, const Callsign &me)
{
	if ((!pkt.to().isQ() || pkt.to().is_localhost()) && pkt.params().has("RREQ")) {
		Buffer msg = pkt.msg();
		msg.append("|");
		Params rrsp = Params();
		rrsp.put("RRSP", None);
		return new Packet(pkt.from(), me, pkt.ident(), rrsp, msg);
	}
	return 0;
}
