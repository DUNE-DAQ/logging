import io
import logging
import os
import sys
from datetime import datetime

from rich.console import Console
from rich.logging import RichHandler
from rich.text import Text

from daq_logging.exceptions import LoggerHandlerError
from daq_logging.formatter import (
    CONSOLE_THEME,
    DATE_TIME_FORMAT,
    LOG_RECORD_PADDING,
    TIME_ZONE,
    LoggingFormatter,
)
from daq_logging.levels import get_level_name_from_int


def check_parent_handlers(
    log: logging.Logger,
    use_parent_handlers: bool,
    handler_type: type[logging.Handler],
    target_stream: io.IOBase | None = None,
) -> bool:
    """Check all parent loggers for an instance of the given logging type."""
    if not use_parent_handlers:
        return False
    logger = log
    while logger.parent:
        handler_checking = [
            isinstance(handler, handler_type) for handler in logger.handlers
        ]
        stream_handler_checking = [
            handler.stream is target_stream if target_stream else False
            for handler in logger.handlers
            if isinstance(handler, logging.StreamHandler)
        ]
        if any(handler_checking + stream_handler_checking):
            raise LoggerHandlerError(logger.name, handler_type)
        logger = logger.parent
    return None


def add_rich_handler(log: logging.Logger, use_parent_handlers: bool) -> None:
    """Add a rich handler to the root logger."""
    check_parent_handlers(log, use_parent_handlers, RichHandler)
    width: int = 0
    try:
        width = os.get_terminal_size()[0]
    except OSError:
        width = 100
    handler: RichHandler = FormattedRichHandler(width=width)
    log.addHandler(handler)
    return


def add_stdout_handler(log: logging.Logger, use_parent_handlers: bool) -> None:
    """Add a stdout handler to the root logger."""
    check_parent_handlers(
        log, use_parent_handlers, RichHandler, target_stream=sys.stdout
    )
    stdout_handler = logging.StreamHandler(sys.stdout)
    stdout_handler.setFormatter(LoggingFormatter())
    log.addHandler(stdout_handler)
    return


def add_stderr_handler(log: logging.Logger, use_parent_handlers: bool) -> None:
    """Add a stderr handler to the root logger."""
    check_parent_handlers(
        log, use_parent_handlers, RichHandler, target_stream=sys.stderr
    )
    stdout_handler = logging.StreamHandler(sys.stderr)
    stdout_handler.setFormatter(LoggingFormatter())
    log.addHandler(stdout_handler)
    return


def add_file_handler(log: logging.Logger, use_parent_handlers: bool, path: str) -> None:
    """Add a file handler to the root logger."""
    check_parent_handlers(log, use_parent_handlers, logging.FileHandler)
    file_handler = logging.FileHandler(filename=path)
    file_handler.setFormatter(LoggingFormatter())
    log.addHandler(file_handler)
    return


class FormattedRichHandler(RichHandler):
    """RichHandler that formats log messages with time, aligned columns, and styles."""

    def __init__(self, **kwargs: str) -> None:
        """Initialize with custom console and style settings."""
        width: int = kwargs.pop("width", 100)
        console: Console = Console(
            force_terminal=True, width=width, theme=CONSOLE_THEME
        )
        super().__init__(
            console=console,
            omit_repeated_times=False,
            markup=True,
            rich_tracebacks=True,
            show_path=False,
            show_time=False,  # We format time ourselves
            **kwargs,
        )

    def render(
        self, *, record: logging.LogRecord, traceback: object, message_renderable: Text
    ) -> Text:
        """Render the log record into a rich Text object with custom formatting."""
        dt: datetime = datetime.fromtimestamp(record.created, tz=TIME_ZONE)
        padding: int = LOG_RECORD_PADDING.get("time", 25)
        time_str: str = dt.strftime(DATE_TIME_FORMAT).ljust(padding)[:padding]
        time_text: Text = Text(time_str, style="logging.time")

        padding = LOG_RECORD_PADDING.get("level", 10)
        level_text: Text = Text(
            record.levelname.ljust(padding)[:padding],
            style=self._get_level_style(record.levelno),
        )

        file_and_no: str = f"{record.filename}:{record.lineno}"
        padding = LOG_RECORD_PADDING.get("file_and_line", 40)
        file_and_no_text: Text = Text(
            file_and_no.ljust(padding)[:padding], style="logging.location"
        )

        padding = LOG_RECORD_PADDING.get("logger_name", 45)
        logger_name_text: Text = Text(
            f"{record.name}:".ljust(padding)[:padding], style="logging.logger_name"
        )

        components: list[Text] = [
            time_text,
            level_text,
            file_and_no_text,
            logger_name_text,
            message_renderable,
        ]

        return Text(" ").join(components)

    def _get_level_style(self, level_no: int) -> str:
        """Get the style string for the given log level number from the theme."""
        return CONSOLE_THEME.styles.get(
            f"logging.level.{get_level_name_from_int(level_no).lower()}", ""
        )
