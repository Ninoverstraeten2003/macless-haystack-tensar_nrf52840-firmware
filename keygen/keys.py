#!/usr/bin/env python3
"""
Generate OpenHaystack advertisement key pairs.

Each key pair consists of:
  - a 224-bit SECP224R1 private key
  - the corresponding public key x-coordinate (advertisement key)

Output (written to output/<PREFIX>_*):
  <PREFIX>.keys          – human-readable key listing
  <PREFIX>_keyfile       – binary blob for the firmware (1-byte count + raw pub keys)
  <PREFIX>_devices.json  – OpenHaystack accessory JSON
  <PREFIX>_<name>.yaml   – (optional) YAML key list for the server
"""

import argparse
import base64
import hashlib
import json
import os
import random
import shutil
import string

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives.asymmetric import ec

OUTPUT_FOLDER = "output/"


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def sha256(data: bytes) -> bytes:
    return hashlib.sha256(data).digest()


def generate_keypair() -> tuple[str, str, str]:
    """
    Return (priv_b64, adv_b64, s256_b64) for a fresh SECP224R1 key pair.
    Retries automatically if the hashed-pubkey base64 starts with '/'.
    """
    while True:
        priv = random.getrandbits(224)
        adv = (
            ec.derive_private_key(priv, ec.SECP224R1(), default_backend())
            .public_key()
            .public_numbers()
            .x
        )
        priv_bytes = priv.to_bytes(28, "big")
        adv_bytes  = adv.to_bytes(28, "big")

        priv_b64  = base64.b64encode(priv_bytes).decode()
        adv_b64   = base64.b64encode(adv_bytes).decode()
        s256_b64  = base64.b64encode(sha256(adv_bytes)).decode()

        # OpenHaystack uses the first 7 chars of the hashed key as a file ID;
        # a leading '/' breaks the path — skip and retry.
        if "/" not in s256_b64[:7]:
            return priv_b64, adv_b64, s256_b64

        print("Key skipped (/ in hashed pubkey prefix), retrying…")


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    p.add_argument("-n", "--nkeys",  type=int, default=1,
                   help="number of keys to generate (default: 1)")
    p.add_argument("-p", "--prefix", default=None,
                   help="output file prefix (random 6-char string if omitted)")
    p.add_argument("-y", "--yaml",   default=None,
                   help="suffix for the optional YAML output file")
    p.add_argument("-v", "--verbose", action="store_true",
                   help="print keys as they are generated")

    p.add_argument("-tinfs", "--thisisnotforstalking",
                   help=argparse.SUPPRESS)
    return p


def main() -> None:
    args = build_parser().parse_args()

    max_keys = 50 if args.thisisnotforstalking == "i_agree" else 1
    if args.nkeys < 1:
        raise SystemExit("Number of keys must be at least 1.")
    if args.nkeys > max_keys:
        raise SystemExit(
            f"Generating more than 1 key requires acknowledging the anti-stalking agreement:\n"
            f"  python3 keys.py -n {args.nkeys} -tinfs i_agree"
        )

    # --- output directory ---
    if os.path.exists(OUTPUT_FOLDER):
        shutil.rmtree(OUTPUT_FOLDER)
    os.makedirs(OUTPUT_FOLDER)

    prefix = args.prefix or "".join(
        random.choices(string.ascii_uppercase + string.digits, k=6)
    )
    print(f"Output will be written to {OUTPUT_FOLDER}")

    # --- generate keys ---
    keypairs: list[tuple[str, str, str]] = []
    for _ in range(args.nkeys):
        priv_b64, adv_b64, s256_b64 = generate_keypair()
        keypairs.append((priv_b64, adv_b64, s256_b64))
        if args.verbose:
            print(f"\n{len(keypairs)})")
            print(f"  Private key:      {priv_b64}")
            print(f"  Advertisement key:{adv_b64}")
            print(f"  Hashed adv key:   {s256_b64}")

    # --- write <PREFIX>.keys ---
    keys_path = os.path.join(OUTPUT_FOLDER, f"{prefix}.keys")
    with open(keys_path, "w") as f:
        for priv_b64, adv_b64, s256_b64 in keypairs:
            f.write(f"Private key: {priv_b64}\n")
            f.write(f"Advertisement key: {adv_b64}\n")
            f.write(f"Hashed adv key: {s256_b64}\n")

    # --- write <PREFIX>_keyfile (binary: count byte + raw pub keys) ---
    keyfile_path = os.path.join(OUTPUT_FOLDER, f"{prefix}_keyfile")
    with open(keyfile_path, "wb") as f:
        f.write(bytes([args.nkeys]))
        for _, adv_b64, _ in keypairs:
            f.write(base64.b64decode(adv_b64))

    # --- write <PREFIX>_devices.json ---
    # The "leading" key is the last generated one; all others are additionalKeys.
    additional_priv = [priv for priv, _, _ in keypairs[:-1]]
    leading_priv    = keypairs[-1][0]

    device = {
        "id": random.randint(0, 9_999_999),
        "colorComponents": [0, 1, 0, 1],
        "name": prefix,
        "privateKey": leading_priv,
        "icon": "",
        "isActive": True,
        "additionalKeys": additional_priv,
    }
    devices_path = os.path.join(OUTPUT_FOLDER, f"{prefix}_devices.json")
    with open(devices_path, "w") as f:
        json.dump([device], f, indent=2)

    # --- write optional YAML ---
    if args.yaml:
        yaml_path = os.path.join(OUTPUT_FOLDER, f"{prefix}_{args.yaml}.yaml")
        with open(yaml_path, "w") as f:
            f.write("  keys:\n")
            for _, adv_b64, _ in keypairs:
                f.write(f'    - "{adv_b64}"\n')

    print(f"\nGenerated {args.nkeys} key(s) with prefix '{prefix}'.")
    print(f"  Advertisement key (base64): {keypairs[-1][1]}")
    print(f"  → Pass to firmware: make BASE64_KEY={keypairs[-1][1]} openhaystack")


if __name__ == "__main__":
    main()