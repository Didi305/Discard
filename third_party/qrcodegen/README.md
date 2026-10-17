# QR encoder

Unmodified C++ sources from Project Nayuki's QR Code generator, v1.8.0.

- Upstream: https://github.com/nayuki/QR-Code-generator
- Commit: `720f62bddb7226106071d4728c292cb1df519ceb`
- Files: `cpp/qrcodegen.cpp` and `cpp/qrcodegen.hpp`
- License: MIT; the original notice is preserved in both source files and LICENSE.

SHA256:

```text
b779c3b156cf7a57ce789d6fee4fc991ccc2913774d26c909d22bb8f26b2a793  qrcodegen.hpp
1f3b3fcdac6954c32cf583ccd02ec9b5901f756a38c461acedc70be4a77d3757  qrcodegen.cpp
```

Discard compiles this encoder into the existing client executable. It generates
authenticator setup images locally, without a web service or runtime download.
