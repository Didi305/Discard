-- Run as PostgreSQL administrator in the selected M2 database.
-- The login role must already exist with a separately supplied random password.
REVOKE ALL ON SCHEMA public FROM PUBLIC;
GRANT USAGE ON SCHEMA public TO discard_server;
REVOKE ALL ON ALL TABLES IN SCHEMA public FROM discard_server;
REVOKE ALL ON ALL SEQUENCES IN SCHEMA public FROM discard_server;
GRANT SELECT ON public.users, public.friendship, public.conversation, public.message, public.attachement TO discard_server;
GRANT INSERT (username,password_hash,nickname,created_at,avatar) ON public.users TO discard_server;
GRANT UPDATE (avatar,avatar_data,avatar_filename) ON public.users TO discard_server;
GRANT INSERT ON public.friendship, public.conversation, public.message, public.attachement TO discard_server;
GRANT UPDATE (status) ON public.friendship TO discard_server;
GRANT DELETE ON public.friendship TO discard_server;
GRANT USAGE ON ALL SEQUENCES IN SCHEMA public TO discard_server;
ALTER ROLE discard_server SET statement_timeout = '5s';
ALTER ROLE discard_server SET search_path = pg_catalog, public;
