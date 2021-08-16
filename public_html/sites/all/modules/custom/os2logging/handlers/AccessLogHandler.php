<?php

use Monolog\Handler\AbstractProcessingHandler;
use Monolog\Logger;

module_load_include('inc', 'os2logging', 'includes/os2logging.access_log');

/**
 * Saved access logs into database.
 */
class AccessLogHandler extends AbstractProcessingHandler {

  /**
   * @param int $level The minimum logging level at which this handler will be
   *   triggered
   */
  public function __construct($level = Logger::DEBUG) {
    parent::__construct($level, FALSE);
  }

  /**
   * {@inheritdoc}
   */
  protected function write(array $record) {
    // Create new record.
    os2logging_access_log_create($record);
  }

}
