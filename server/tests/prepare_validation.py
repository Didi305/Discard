#!/usr/bin/env python3
# Run with sudo. All validation writes go to the explicit isolated database.
import json, os, pathlib, pwd, secrets, subprocess
base=pathlib.Path("/home/discard/Discard")
backup=pathlib.Path("/home/discard/m2-backups/pre-server-migration-20261012")
backup.mkdir(parents=True,exist_ok=False);os.chmod(backup,0o700)
def pg(sql,db="postgres"):
    return subprocess.check_output(["runuser","-u","postgres","--","psql","-X","-v","ON_ERROR_STOP=1","-At","-d",db],input=sql.encode()).decode().strip()
with open(backup/"chatdb.dump","wb") as f:
    subprocess.run(["runuser","-u","postgres","--","pg_dump","-Fc","chatdb"],stdout=f,check=True)
with open(backup/"roles.sql","wb") as f:
    subprocess.run(["runuser","-u","postgres","--","pg_dumpall","--roles-only"],stdout=f,check=True)
assert pg("SELECT count(*) FROM pg_roles WHERE rolname='discard_server'")=="0","Role exists; inspect before retry"
password=secrets.token_urlsafe(36)
pg("CREATE ROLE discard_server LOGIN NOSUPERUSER NOCREATEDB NOCREATEROLE NOREPLICATION NOBYPASSRLS PASSWORD '"+password+"';")
subprocess.run(["runuser","-u","postgres","--","createdb","chatdb_m2_validation"],check=True)
with open(backup/"chatdb.dump","rb") as f:
    subprocess.run(["runuser","-u","postgres","--","pg_restore","--exit-on-error","--no-owner","-d","chatdb_m2_validation"],stdin=f,check=True)
pg((base/"server/permissions.sql").read_text(),"chatdb_m2_validation")
pg("GRANT CONNECT ON DATABASE chatdb_m2_validation TO discard_server")
config=pathlib.Path("/home/discard/.config/discard-server")
config.mkdir(parents=True,exist_ok=True);os.chmod(config,0o700)
uid=pwd.getpwnam("discard").pw_uid;gid=pwd.getpwnam("discard").pw_gid
os.chown(config,uid,gid)
for name,db in [("validation.json","chatdb_m2_validation"),("database.json","chatdb")]:
    target=config/name
    assert not target.exists(),"Config already exists"
    fd=os.open(target,os.O_WRONLY|os.O_CREAT|os.O_EXCL,0o600)
    with os.fdopen(fd,"w") as f:json.dump(dict(host="127.0.0.1",port=5432,database=db,user="discard_server",password=password),f,indent=2)
    os.chown(target,uid,gid)
print("Backup and isolated validation database ready; private server configuration created.")
