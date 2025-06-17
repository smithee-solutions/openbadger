//  call:

test-ecdh --peer

or test-ecdh --compute

assumes peer 
typedef struct
{
   EcDomainParameters params; ///<EC domain parameters
   EcPrivateKey da;           ///<One's own EC private key
   EcPublicKey qa;            ///<One's own EC public key
   EcPublicKey qb;            ///<Peer's EC public key
} EcdhContext;


//ECDH related functions
void ecdhInit(EcdhContext *context);
void ecdhFree(EcdhContext *context);

error_t ecdhGenerateKeyPair(EcdhContext *context, const PrngAlgo *prngAlgo,
   void *prngContext);

error_t ecdhCheckPublicKey(const EcDomainParameters *params, EcPoint *publicKey);

error_t ecdhComputeSharedSecret(EcdhContext *context,
   uint8_t *output, size_t outputSize, size_t *outputLen);


init domain parameters

generate key pair

load key pair into context

if peer
  encode public key as der
  write public key to peer-public-key.der
else
  load peer public key from file

compute shared secret

save shared secret

