import logging
from typing import Literal

logging_log_levels = {
    "CRITICAL": logging.CRITICAL,
    "ERROR": logging.ERROR,
    "WARNING": logging.WARNING,
    "INFO": logging.INFO,
    "DEBUG": logging.DEBUG,
    "NOTSET": logging.NOTSET,
}


def get_level_name_from_int(level_no: int) -> str | None:
    """Get the logging level name from the integer value."""
    level_name = next((k for k, v in logging_log_levels.items() if v == level_no), None)
    if not level_name:
        err_msg = f"Level number {level_no} is not a valid logging level."
        raise ValueError(err_msg)
    return level_name


logging_log_level_keys = list(logging_log_levels.keys())
logging_log_level_values = list(logging_log_levels.values())
logging_methods = Literal["debug", "info", "warning", "error", "critical", "exception"]

oks_log_levels = {
    "kTopPriority": 0,
    "kEventDriven": 1073741824,
    "kDefault": 2147483648,
    "kLowestPriority": 4294967295,
}

oks_log_level_keys = list(oks_log_levels.keys())
oks_log_level_values = list(oks_log_levels.values())

oks_to_logging_map = {
    "kTopPriority": "ERROR",
    "kEventDriven": "WARNING",
    "kDefault": "INFO",
    "kLowestPriority": "DEBUG",
}

log_level_keys = logging_log_level_keys + oks_log_level_keys
log_level_values = logging_log_level_values + logging_log_level_values
