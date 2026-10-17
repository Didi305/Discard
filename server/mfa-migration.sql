-- Apply once as the PostgreSQL administrator, before starting the MFA server.
-- Existing users are not enrolled until they enable 2FA inside Discard.
CREATE TABLE public.user_mfa (
    user_id INTEGER PRIMARY KEY REFERENCES public.users(id) ON DELETE CASCADE,
    secret TEXT NOT NULL CHECK (secret ~ '^[A-Z2-7]{32}$'),
    last_counter BIGINT NOT NULL DEFAULT -1,
    failed_attempts INTEGER NOT NULL DEFAULT 0 CHECK (failed_attempts >= 0),
    locked_until TIMESTAMPTZ
);

CREATE TABLE public.mfa_recovery_code (
    user_id INTEGER NOT NULL REFERENCES public.user_mfa(user_id) ON DELETE CASCADE,
    code_hash TEXT NOT NULL CHECK (code_hash ~ '^[0-9a-f]{64}$'),
    PRIMARY KEY (user_id, code_hash)
);

REVOKE ALL ON public.user_mfa, public.mfa_recovery_code FROM PUBLIC;
GRANT SELECT, INSERT, UPDATE, DELETE ON public.user_mfa, public.mfa_recovery_code TO discard_server;
