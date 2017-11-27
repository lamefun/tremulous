add_library( nettle STATIC
  nettle/bignum.c
  nettle/bignum-random.c
  nettle/bignum-random-prime.c
  nettle/buffer.c
  nettle/buffer-init.c
  nettle/gmp-glue.c
  nettle/mini-gmp.c
  nettle/pkcs1.c
  nettle/pkcs1-rsa-sha256.c
  nettle/realloc.c
  nettle/rsa.c
  nettle/rsa2sexp.c
  nettle/rsa-keygen.c
  nettle/rsa-sha256-sign.c
  nettle/rsa-sha256-verify.c
  nettle/rsa-sign.c
  nettle/rsa-verify.c
  nettle/sexp.c
  nettle/sexp-format.c
  nettle/sexp2bignum.c
  nettle/sexp2rsa.c
  nettle/sha256-compress.c
  nettle/sha256.c
  nettle/write-be32.c
)

target_compile_options( nettle PRIVATE -mfpmath=sse -ffast-math )

target_include_directories ( nettle PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" )
