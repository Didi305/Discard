#!/usr/bin/env python3
"""Apply only after protocol and role tests pass. Run with sudo on Discard-M2."""
import pathlib,subprocess
base=pathlib.Path('/home/discard/Discard')
def pg(sql,db='chatdb'):
    return subprocess.check_output(['runuser','-u','postgres','--','psql','-X','-v','ON_ERROR_STOP=1','-At','-d',db],input=sql.encode()).decode().strip()
assert 'PASS server remains available' in (base/'protocol-test.log').read_text()
assert 'PASS real modification/deletion' in (base/'permissions-test.log').read_text()
assert pathlib.Path('/home/discard/m2-backups/pre-server-migration-20261012/chatdb.dump').exists()
assert pg("SELECT count(*) FROM pg_roles WHERE rolname='discard_owner'")=='0','Owner role already exists; inspect before retry'
sql="""BEGIN;
CREATE ROLE discard_owner NOLOGIN NOSUPERUSER NOCREATEDB NOCREATEROLE NOREPLICATION NOBYPASSRLS;
REASSIGN OWNED BY chatapp TO discard_owner;
REVOKE ALL ON DATABASE chatdb FROM PUBLIC;
GRANT CONNECT ON DATABASE chatdb TO discard_server;
"""+(base/'server/permissions.sql').read_text()+"""
REVOKE ALL ON ALL TABLES IN SCHEMA public FROM chatapp;
REVOKE ALL ON ALL SEQUENCES IN SCHEMA public FROM chatapp;
REVOKE ALL ON SCHEMA public FROM chatapp;
REVOKE ALL ON DATABASE chatdb FROM chatapp;
ALTER ROLE chatapp NOLOGIN PASSWORD NULL;
COMMIT;
"""
pg(sql)
pg("SELECT pg_terminate_backend(pid) FROM pg_stat_activity WHERE usename='chatapp' AND pid<>pg_backend_pid();")
pg("ALTER SYSTEM SET listen_addresses='127.0.0.1';")
subprocess.run(['pg_ctlcluster','18','main','restart'],check=True)
print('Activated M2: separate owner, restricted runtime role, old shared login disabled, PostgreSQL listens only on VM loopback.')
