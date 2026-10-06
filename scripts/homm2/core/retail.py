"""Identity gate for the pinned retail control images (config/retail/targets.json)."""
from __future__ import annotations

from pathlib import Path

from homm2.core.inputs import InputError, read_verified, targets
from homm2.core.paths import REPO, image_key, retail_exe


def verify_retail(path: Path | None = None, image: str | None = None) -> Path:
    """Reject a missing or reconstructed control before generating evidence.

    `path` defaults to the selected image's staged executable. A link output
    is never a retail control, whatever its name."""
    key = image or image_key()
    pin = targets(REPO)[key]
    path = Path(path) if path is not None else retail_exe(key)
    try:
        read_verified(pin, path)
    except InputError as error:
        raise ValueError(f"{error}: not the pinned {key} image {pin.name}. "
                         "Do not use a reconstructed/link output as the retail control.") from None
    return path
