#!/usr/bin/env python3
"""Integration tests against an isolated PostgreSQL-backed server, never a mock."""
import base64, json, socket, sys, time, uuid
host = "127.0.0.1"
port = int(sys.argv[1]) if len(sys.argv)>1 else 1235
checks = []
class Peer:
    def __init__(self):
        self.s=socket.create_connection((host,port),5); self.s.settimeout(10)
        self.reader=self.s.makefile("rb");self.seq=0;self.events=[]
    def call(self,op,args=None,ok=True,error=None,fragment=False):
        self.seq+=1; rid=str(self.seq)
        wire=(json.dumps(dict(type=op,request_id=rid,args=args or {}))+"\n").encode()
        if fragment:
            for i in range(0,len(wire),3): self.s.sendall(wire[i:i+3])
        else: self.s.sendall(wire)
        while True:
            raw=self.reader.readline()
            assert raw, "Unexpected disconnection"
            response=json.loads(raw)
            if response.get("type")!="response":
                self.events.append(response);continue
            assert response["request_id"]==rid
            assert response["ok"]==ok, (op,response)
            if error: assert response["error"]==error,(op,response)
            return response.get("result",{})
    def close(self): self.reader.close();self.s.close()
def mark(name): checks.append(name);print("PASS",name,flush=True)

anonymous=Peer()
anonymous.call("messages",{"conversation_id":52},False,"authentication_required")
anonymous.call("init",{"user_id":74},False,"authentication_required")
anonymous.call("login",{"username":"alice","password":"wrong"},False,"invalid_credentials")
anonymous.call("friends",{},False,"authentication_required")
mark("unauthenticated reads, legacy init and invalid login rejected")
anonymous.close()

suffix=uuid.uuid4().hex[:10]
accounts=[("m2_"+suffix+x,"M2-"+suffix+x) for x in ("a","b","c")]
password="M2-fixture-password-123"
peers=[]
for username,nickname in accounts:
    p=Peer()
    p.call("register",dict(username=username,nickname=nickname,password=password))
    p.call("available",dict(field="username",value=username))["available"] is False
    user=p.call("login",dict(username=username,password=password),fragment=True)["user"]
    peers.append((p,user))
a,ua=peers[0];b,ub=peers[1];c,uc=peers[2]
mark("registration and fragmented login frames")
a.call("login",dict(username=accounts[1][0],password=password),False,"already_authenticated")
a.call("friends",{"user_id":ub["id"]},False,"invalid_request")
a.call("init",{"user_id":ub["id"]},False,"invalid_request")
mark("authenticated identity cannot be switched or forged")

a.call("friend_request",{"nickname":accounts[1][1]})
assert b.call("requests",{"direction":"incoming"})["users"][0]["id"]==ua["id"]
c.call("friend_accept",{"peer_id":ua["id"]},False,"forbidden")
b.call("friend_cancel",{"peer_id":ua["id"]},False,"forbidden")
b.call("friend_accept",{"peer_id":ua["id"]})
assert a.call("friends")["users"][0]["id"]==ub["id"]
mark("friend requests, direction checks and acceptance")

content=b"M2 attachment round trip\x00\xff"
sent=a.call("send_message",dict(peer_id=ub["id"],conversation_id=-1,body="M2 integration message",
    attachments=[dict(filename="proof.txt",bytes=base64.b64encode(content).decode())]))
conv=sent["messageSentIn"]; attachment=sent["attachs"][0]["id"]
assert sent["messageSender"]==ua["id"]
rows=b.call("messages",dict(conversation_id=conv))["messages"]
assert rows[-1]["messageBody"]=="M2 integration message"
assert base64.b64decode(b.call("attachment",{"attachment_id":attachment})["bytes"])==content
assert b.call("conversations")["conversations"][0]["conversation_id"]==conv
assert any(e.get("type")=="message" and e.get("messageSender")==ua["id"] for e in b.events)
mark("message persistence, attachment round trip and server-generated notification")

c.call("messages",{"conversation_id":conv},False,"forbidden")
c.call("attachment",{"attachment_id":attachment},False,"forbidden")
assert c.call("conversations")["conversations"]==[]
a.call("send_message",dict(peer_id=ub["id"],conversation_id=conv,body="forged",attachments=[],sender_id=uc["id"]),False,"invalid_request")
a.call("send_message",dict(peer_id=uc["id"],conversation_id=conv,body="wrong recipient",attachments=[]),False,"forbidden")
a.call("delete_message",{"message_id":sent["message_id"]},False,"unknown_operation")
a.call("sql",{"query":"SELECT * FROM users"},False,"unknown_operation")
mark("cross-user reads, attachment guessing, sender spoofing and arbitrary SQL denied")

a.call("set_avatar",{"peer_id":ub["id"],"bytes":"YQ==","filename":"x.png"},False,"invalid_request")
png=base64.b64decode("iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mP8/x8AAwMCAO+aWQAAAABJRU5ErkJggg==")
a.call("set_avatar",{"bytes":base64.b64encode(png).decode(),"filename":"avatar.png"})
assert base64.b64decode(b.call("avatar",{"peer_id":ua["id"]})["bytes"])==png
c.call("avatar",{"peer_id":ua["id"]},False,"forbidden")
mark("own-avatar update and relationship-limited avatar access")

# Failed attachment validation must roll back the whole message.
before=len(a.call("messages",{"conversation_id":conv})["messages"])
a.call("send_message",dict(peer_id=ub["id"],conversation_id=conv,body="rollback",attachments=[dict(filename="bad.txt",bytes="%%%")]),False,"invalid_file")
after=len(a.call("messages",{"conversation_id":conv})["messages"])
assert before==after
mark("invalid attachment rolls back message transaction")

a.call("friend_request",{"nickname":accounts[2][1]})
c.call("friend_decline",{"peer_id":ua["id"]})
assert a.call("requests",{"direction":"outgoing"})["users"]==[]
a.call("friend_request",{"nickname":accounts[2][1]})
a.call("friend_cancel",{"peer_id":uc["id"]})
assert c.call("requests",{"direction":"incoming"})["users"]==[]
mark("request decline and cancellation")

new=Peer();new.call("messages",{"conversation_id":conv},False,"authentication_required");new.close()
mark("new connection does not inherit authentication")
for p,_ in peers:p.close()
malformed=socket.create_connection((host,port),5);malformed.settimeout(5);malformed.sendall(b"not json\n");assert malformed.recv(1)==b"";malformed.close()
mark("malformed input closes only offending connection")
healthy=Peer();healthy.call("login",{"username":"alice","password":"alice123"});healthy.close()
mark("server remains available and existing baseline login works")
print(json.dumps({"passed":len(checks),"checks":checks,"fixture_prefix":"m2_"+suffix},indent=2))
