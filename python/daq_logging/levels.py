import logging
from typing import Literal

log_levels = {
    "CRITICAL": logging.CRITICAL,
    "ERROR": logging.ERROR,
    "WARNING": logging.WARNING,
    "INFO": logging.INFO,
    "DEBUG": logging.DEBUG,
    "NOTSET": logging.NOTSET,
}


def get_level_name_from_int(level_no: int) -> str | None:
    """Get the logging level name from the integer value."""
    level_name = next((k for k, v in log_levels.items() if v == level_no), None)
    if not level_name:
        err_msg = f"Level number {level_no} is not a valid logging level."
        raise ValueError(err_msg)
    return level_name


log_level_keys = list(log_levels.keys())
log_level_values = list(log_levels.values())
logging_methods = Literal["debug", "info", "warning", "error", "critical", "exception"]
