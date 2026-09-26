"""Identity gate for the supported, stripped Buka retail control image."""
import hashlib
from pathlib import Path

RETAIL_SHA256 = "bc7e9c9320aa3e5c1ffca6d2bfa530ecedb5a3bca1b91c959501c15ad72c329a"


def verify_retail(path: Path) -> None:
    """Reject missing or reconstructed controls before generating evidence."""
    actual = hashlib.sha256(path.read_bytes()).hexdigest()
    if actual != RETAIL_SHA256:
        raise ValueError(
            f"{path}: not the supported Buka retail image: "
            f"SHA-256 {actual}; expected {RETAIL_SHA256}. "
            "Do not use a reconstructed/link output as the retail control."
        )
