#!/usr/bin/env python3
import json, os, subprocess, pathlib
cfg=json.loads((pathlib.Path.home()/'.config/discard-server/validation.json').read_text())
env=dict(os.environ,PGPASSWORD=cfg['password'])
cmd=['psql','-X','-h','127.0.0.1','-U',cfg['user'],'-d',cfg['database'],'-At','-v','ON_ERROR_STOP=1']
query="""SELECT current_user,
has_table_privilege(current_user,'public.message','SELECT'),
has_table_privilege(current_user,'public.message','INSERT'),
has_table_privilege(current_user,'public.message','UPDATE'),
has_table_privilege(current_user,'public.message','DELETE'),
has_table_privilege(current_user,'public.message','TRUNCATE'),
has_schema_privilege(current_user,'public','CREATE');"""
out=subprocess.check_output(cmd+['-c',query],env=env,text=True).strip()
assert out=='discard_server|t|t|f|f|f|f',out
print('PASS restricted database role: message SELECT/INSERT only; no UPDATE/DELETE/TRUNCATE/schema CREATE')
for operation in ["UPDATE public.message SET body='test' WHERE false",'DELETE FROM public.message WHERE false']:
    result=subprocess.run(cmd+['-c','BEGIN; '+operation+'; ROLLBACK;'],env=env,capture_output=True,text=True)
    assert result.returncode!=0 and ('permission denied' in result.stderr.lower() or 'keine berechtigung' in result.stderr.lower()),result.stderr
print('PASS real modification/deletion attempts denied by PostgreSQL')
