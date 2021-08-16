<?php

use Monolog\Formatter\JsonFormatter;
use Monolog\Handler\RotatingFileHandler;

/**
 * Stores to a JSON file
 */
class JsonRotatingFileHandler extends RotatingFileHandler {

  /**
   * Gets the default formatter.
   *
   * Overwrite this if the LineFormatter is not a good default for your handler.
   */
  protected function getDefaultFormatter() {
    return new JsonFormatter();
  }

}
